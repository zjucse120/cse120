// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"
#include "processmanager.h"
#include "thread.h"
#include "synchconsole.h"
#include "filesys.h"
#include "boundedbuffer.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.#include "synch.h"
//----------------------------------------------------------------------
extern ProcessTable *pt;
extern SynchConsole *synchCons;

void AdjustPC();

void SystemCall(int type, int which);
void Halt_Handler();
void Exit_Handler();
void Exec_Handler();
void Yield_Handler();
void Fork_Handler();
void Read_Handler();
void Write_Handler();
void Join_Handler();

SpaceId Exec(char *filename);
int ReadFile(char *buff, int address, int length, int numpage);

void CopyToUser(char *FromKernelAddr, int NumBytes, int ToUserAddr);
void CopyToKernel(int FromUserAddr, int NumBytes, char *ToKernelAddr);
void ProcessStart(int a);
void newThreadfunc(int a);


void
ExceptionHandler(ExceptionType which){
    int type = machine->ReadRegister(2);
    switch(which){
       case NoException:
            printf("Everything is ok!");
            break;
       case SyscallException: 
            SystemCall(type,which);
            break;
       case PageFaultException:
            printf("No valid translation found.\n");
            Exit_Handler();
            break;
       case ReadOnlyException: 
            printf("Write attempted to page marked 'read-only'.\n");
            Exit_Handler();
            break;
       case BusErrorException:   
            printf("Translation resulted in an invalid physical address.\n");
            Exit_Handler();		
            break;
       case AddressErrorException: 
            printf("Unaligned reference or one that was beyond the end of the address space.\n");
            Exit_Handler();		
            break;
       case OverflowException:  
            printf("Integer overflow in add or sub.\n");
            Exit_Handler();    		
            break;
       case IllegalInstrException:
            printf("Unimplemented or reserved instr.\n");
            Exit_Handler();
            break;
       case NumExceptionTypes:
            Exit_Handler();
            break;
       default:
            printf("Unexpected user mode exception %d %d\n", which, type);
            ASSERT(FALSE); 
            break;     

     }
    
}


void SystemCall(int type, int which) {
	switch (type) {
	       case SC_Halt:
                    Halt_Handler();
                    break;
               case SC_Exit:
                    Exit_Handler();
                    break;
               case SC_Exec:
                    Exec_Handler();
                    break;
               case SC_Join: 
                    printf("Unexpected user mode exception %d %d\n", which, type);
                    Join_Handler();
                    break; 
               case SC_Create:
                    break;
       
               case SC_Open:
                    break;
       
               case SC_Read:
                    Read_Handler();
                    break;
               case SC_Write:
                    Write_Handler();
                    break;
               case SC_Fork:
                    Fork_Handler();
                    break;
               case SC_Yield:
                    Yield_Handler(); 
                    break;
               default:
                    printf("Unexpected user mode exception %d %d\n", which, type);
                    ASSERT(FALSE); 
                    break;     

	}
}

void Halt_Handler() {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
}

void Exit_Handler(){
          AddrSpace *space;
          SpaceId pid;
          space = currentThread->space;
          int value = machine->ReadRegister(4);
          pid = currentThread->GetPid();
          printf("Exit value is %d\n", value); 
          currentThread->Finish();
          space->~AddrSpace();
          pt->Release(pid); 
          AdjustPC();
}
 
void Exec_Handler(){
        AddrSpace *space;
        space = currentThread->space;
        int numpage = space->numberPages();
        SpaceId pid;
        int arg1 = machine->ReadRegister(4);
        char* filename = new char[128];
        
        if(!ReadFile(filename,arg1,128,numpage)){
        printf("Exec fails to read the filename\n");
        machine->WriteRegister(2,0);
        return;
    } 
            
    pid = Exec(filename);
    machine->WriteRegister(2,pid);
    AdjustPC();
}



int ReadFile(char *buff, int address, int length, int numpage){
    int i;
    int value;

    for(i = 0;i < length;i++,address++){
        
        if(address < 0 || address >= numpage*length){
            printf("Invalid string filename\n");
            return false;
        }
        if(!machine->ReadMem(address,1,&value)){
            printf("Read memory fails\n");
            return false;
        }
       
        buff[i] = value;
        if(buff[i] == '\0')
            break;
        }
 
        if(i == length) {
            printf("The filename is too long to execute\n");
            return false;
        }
        
        if(machine->ReadRegister(4)%length + i > 128){
            printf("The virtual address across the boundary\n");
            return false;
        }
        return true;
}
 
SpaceId 
Exec(char *filename){
    SpaceId pid;
    int pipectrl = 0;
    int arg4 =  machine->ReadRegister(7);
    if ((arg4 & 0x6) == 0x2)
	pipectrl = 1;
    else if((arg4 & 0x6) == 0x6)
	pipectrl = 2;
    else if((arg4 = 0x6) == 0x4)
	pipectrl = 3;

    machine->WriteRegister(7,pipectrl);

    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return 0;
    }
      
    AddrSpace *space;
    space = new AddrSpace(executable);    
    
    if(space->Initialize(executable)){

       Thread *thread;
       thread = new Thread("1", arg4 & 0x1 ,0);
       thread->space = space;
       pid = pt->Alloc(thread);
      if(pid == 0){
           delete executable;
           delete thread;
           printf("sorry, I run out of process tables\n"); 
           return 0;  
         }
      else{
           thread->SetPid(pid);
           printf("The thread with pid of %d is going to run\n", pid); 
           delete executable;	
           thread->Fork(ProcessStart,0);
           currentThread->Yield(); 
           return pid;
           }

     }
    else  { 
        delete executable;	
        return 0;
    }
   }

void
Read_Handler(){
    int bufferVrtAddr, size, fd, numBytes;
    char * buffer;
    OpenFile * of;
    
    bufferVrtAddr = machine->ReadRegister(4);
    size = machine->ReadRegister(5);
    fd = machine->ReadRegister(6);//?
    
    /*if (size <= 0)
        return -1;*/
    
    buffer = new char[size];
    if (fd == ConsoleInput){ //how to judge whether the input is I/O and read-only?
        numBytes = 0;
        for (int i=0; i<size; i++)
            buffer[i] = synchCons->GetChar();
        CopyToUser(buffer, size, bufferVrtAddr);
        numBytes++ ;
        machine->WriteRegister(2, numBytes);
    }
    else {
        // how to judge whether the file exist
            numBytes = of->Read(buffer,size);
            CopyToUser(buffer, numBytes, bufferVrtAddr);
            machine->WriteRegister(2, numBytes);
            
    }
    
    AdjustPC();
    delete buffer;
}

void
Write_Handler(){
    int bufferVrtAddr, size, fd;
    char *buffer;
    OpenFile * of;
    int i;
    
    bufferVrtAddr = machine->ReadRegister(4);
    size = machine->ReadRegister(5);
    fd = machine->ReadRegister(6);//?
    
    buffer = new char[size];
    CopyToKernel(bufferVrtAddr, size, buffer);
    if (fd == ConsoleOutput){
        for (i=0; i<size; i++)
            synchCons->PutChar(buffer[i]);
    }
    else{
        of->Write(buffer,size);
    }
    
    AdjustPC();
    delete buffer;
}

void
Join_Handler() {
	SpaceId pid;
	Thread *t;
	pid = machine->ReadRegister(4);
	t =(Thread*)pt->Get(pid);
	t->Join();
 
        AdjustPC();
	
}

void Fork_Handler(){
              
              AddrSpace *space;
              space = currentThread->space;
              Thread *thread = new Thread("newThread");   
              thread->space = space;
              thread->Fork(newThreadfunc, 0);
              AdjustPC(); 
      
}

void Yield_Handler() {
        currentThread->SaveUserState();
        currentThread->Yield();
        currentThread->RestoreUserState();
        AdjustPC();
}

void CopyToUser(char *FromKernelAddr, int NumBytes, int ToUserAddr){
    int i;
    for (i=0; i<NumBytes; i++){
        machine->WriteMem(ToUserAddr, 1, (int)*FromKernelAddr);
        FromKernelAddr++;
        ToUserAddr++;
    }
}

void CopyToKernel(int FromUserAddr, int NumBytes, char *ToKernelAddr){
    int c;
    int i;
    for (i=0; i<NumBytes; i++){
        machine->ReadMem(FromUserAddr, 1, &c);
        FromUserAddr++;
        *ToKernelAddr++ = (char) c;
    }
}

void ProcessStart(int a){
    currentThread->space->InitRegisters();
    currentThread->space->SaveState();
    currentThread->space->RestoreState(); 
    machine->Run();			// jump to the user program
    ASSERT(FALSE);			

}       
   

void AdjustPC()
{
    int pc;

    pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

void newThreadfunc(int a){

   int pc = machine->ReadRegister(4);
   machine->WriteRegister(PCReg, pc);
   pc +=4;
   machine->WriteRegister(NextPCReg, pc);
   machine->WriteRegister(StackReg, (currentThread->space->numPages--) * PageSize - 16);
   machine->Run();

}



