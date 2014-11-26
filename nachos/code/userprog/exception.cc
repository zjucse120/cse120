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
#include "synch.h"

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
//	are in machine.h.
//----------------------------------------------------------------------

void ExitHandler()
{
          Lock* exitlock;
          exitlock = new Lock("exitlock");
          AddrSpace *space;
          exitlock->Acquire();
          space = currentThread->space;
          int value = machine->ReadRegister(4);
          printf("%d\n", value); 
          space->~AddrSpace();
          currentThread->Finish();
          exitlock->Release();
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    //Thread *thread;
    //typedef void (*function)();
    //function func;
    if (which == SyscallException){
     switch(type){
           case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
           case SC_Exit:
                 ExitHandler();    
                 break;
  //       case SC_Fork:
    //          AddrSpace *space;
      //        Thread *thread = new Thread("newThread");
        //      int arg = machine->ReadRegister(4);
        //      VoidFunctionPtr func = arg;
      //        space = currentThread->space;
      //        thread->Fork(func, 0);   
      //        currentThread->space = space;


           case SC_Yield:
                  currentThread->Yield();
                  break;
                  }
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

