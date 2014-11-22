#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_
#include "synchconsole.h"
#endif
#include "copyright.h"

SynchConsole::SynchConsole(char *readFile, char *writeFile){
         readAvail = new Semaphore("console read sem", 0);
         writeDone = new Semaphore("console write sem", 0);
         W_lock = new Lock("console write lock");
         R_lokc = new Lock("console read lock");
         console = new Console(readFile, writeFile, readAvail, writeDone,int(this))
}

SynchConsole::~SynchConsole(){
     delete W_lock;
     delete R_lock;
     delete console;
     delete readAvail;
     delete writeDone;
}

char
SynchConsole::GetChar(){
     char ch;
     R_lock->Acquire();
     ch = console->GetChar();
     if(ch == EOF)
     readAvail->P();
     R_lock->Release();
     return ch;
}

void
SynchConsole::PutChar(char ch){
     W_lock->Acquire();
     console->PutChar(ch);
     writeDone->P();
     W_lock->Release();
}

void
SynchConsole::WriteDone{
     writeDone->V();
}
void
SynchConsole::ReadAvail{
     readAvail->V();
} 
