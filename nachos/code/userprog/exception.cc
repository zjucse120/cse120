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
void AdjustPC();
void ExecHandler(char *filename);
void ProcessStart(int a);
void ExitHandler()
{
          //Lock* exitlock;
          //exitlock = new Lock("exitlock");
          AddrSpace *space;
          //exitlock->Acquire();
          space = currentThread->space;
          int value = machine->ReadRegister(4);
          printf("Exit value is %d\n", value); 
          space->~AddrSpace();
          currentThread->Finish();
          //exitlock->Release();
          AdjustPC();
}


void
ExceptionHandler(ExceptionType which)
{    
    int type = machine->ReadRegister(2);
    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int arg4 = machine->ReadRegister(7);
    int syscallfail = 0;
   if (which == SyscallException) {
        switch(type){
        case SC_Halt:
       {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
        break;}
        case SC_Exit:{
        ExitHandler();
        printf("Unexpected user mode exception %d %d\n", which, type);
        break;
        }
        case SC_Exec:
        {
        int position = 0;
        char* fileName = new char[128];
        int value;
        while (value != NULL) {
        machine->ReadMem(arg1, 1, &value);
        fileName[position] = (char) value;
        position++;
        arg1++;
        }
        ExecHandler(fileName);
        break;
        }
        case SC_Join:
        break;
       
        case SC_Create:
        break;
       
        case SC_Open:
        break;
       
        case SC_Read:
        break;
       
        case SC_Write:
        break;
       
        case SC_Close:
        break;
       
        case SC_Fork:
        break;
       
        case SC_Yield:
        break;
        
        default:
        break;
         } 
      if(syscallfail){
         machine->WriteRegister(2,-1);
           }
}
    else {
    if  ((which == SyscallException) && (type == SC_Halt)) {
       DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
   else if((which == SyscallException) && (type == SC_Exit)) {

        ExitHandler();    
    } 
   else if((which == SyscallException) && (type == SC_Yield)) {
        currentThread->Yield();

   }
   else if(which == NoException)
  {
        printf("Everything is ok!");
   }
   else if(which == PageFaultException)
   {
        printf("No valid translation found.\n");
        ExitHandler();
   }
   else if(which == ReadOnlyException)
   {
        printf("Write attempted to page marked 'read-only'.\n");
        ExitHandler();
   }
   else if(which == BusErrorException)
   {
        printf("Translation resulted in an invalid physical address.\n");
        ExitHandler();
    }
   else if(which == AddressErrorException)
   {
        printf("Unaligned reference or one that was beyond the end of the address space.\n");
        ExitHandler();
    }
    else if(which == OverflowException)
   {
        printf("Integer overflow in add or sub.\n");
        ExitHandler();   
   }
   else if(which == IllegalInstrException)
   {
         printf("Unimplemented or reserved instr.\n");
         ExitHandler();
    }
   else{
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
         }

}
}

void 
ExecHandler(char *filename){
    SpaceId pid;
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        machine->WriteRegister(2,-1);
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
    else   
    machine->WriteRegister(2,-1);
    AdjustPC();
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



