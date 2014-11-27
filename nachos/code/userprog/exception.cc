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
#include "synchconsole.h"
#include "filesys.h"
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
void Read_Handler();
void Write_Handler();

void Exec(char *filename);

void CopyToUser(char *FromKernelAddr, int NumBytes, int ToUserAddr);
void CopyToKernel(int FromUserAddr, int NumBytes, char *ToKernelAddr);
void ProcessStart(int a);


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
          pid = machine->ReadRegister(2);
          printf("Exit value is %d\n", value); 
          space->~AddrSpace();
          currentThread->Finish();
          pt->Release(pid);
          AdjustPC();
}
 
void Exec_Handler(){
        int arg1 = machine->ReadRegister(4);
        int position = 0;
        char* fileName = new char[128];
        int value;
        while (value != 0) {
            machine->ReadMem(arg1, 1, &value);
            fileName[position] = (char) value;
            position++;
            arg1++;
        }
        Exec(fileName);
}


  //       case SC_Fork:
    //          AddrSpace *space;
      //        Thread *thread = new Thread("newThread");
        //      int arg = machine->ReadRegister(4);
        //      VoidFunctionPtr func = arg;
      //        space = currentThread->space;
      //        thread->Fork(func, 0);   
      //        currentThread->space = space;


void 
Exec(char *filename){
    SpaceId pid;
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        machine->WriteRegister(2,0);
        AdjustPC();
        return ;
    }
      
    AddrSpace *space;
    space = new AddrSpace(executable);    
    
    if(space->Initialize(executable)){
       Thread *thread;
       thread = new Thread("1", 0 ,0);
       thread->space = space;
       pid = pt->Alloc(thread);
       printf("The thread with pid of %d is going to run\n", pid); 
       delete executable;	
       thread->Fork(ProcessStart,0);
       currentThread->Yield(); 
       machine->WriteRegister(2,pid);
     }
    else  { 
       delete executable;	
        machine->WriteRegister(2,0);
    }
    AdjustPC();
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
    machine->Run();			// jump to the user progam
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



