//#ifndef _SYNCHCONSOLE_H_
//#define _SYNCHCONSOLE_H_
#include "synchconsole.h"
//#endif


static void SynchConsoleReadPoll(int c) { 
	SynchConsole *sconsole = (SynchConsole *)c; sconsole->ReadAvail(); 
}
static void SynchConsoleWriteDone(int c) { 
	SynchConsole *sconsole = (SynchConsole *)c; sconsole->WriteDone(); 
}



SynchConsole::SynchConsole(char *readFile, char *writeFile){
         readAvail = new Semaphore("console read sem", 0);
         writeDone = new Semaphore("console write sem", 0);
         W_lock = new Lock("console write lock");
         R_lock = new Lock("console read lock");
         console = new Console(readFile, writeFile, (VoidFunctionPtr) SynchConsoleReadPoll,(VoidFunctionPtr) SynchConsoleWriteDone, (int) this);
         	
}

SynchConsole::~SynchConsole(){
     delete W_lock;
     delete R_lock;
     delete readAvail;
     delete writeDone;
     delete console;
}
void
SynchConsole::PutChar(char c){
     W_lock->Acquire();
     console->PutChar(c);
     writeDone->P();
     W_lock->Release();
}

char
SynchConsole::GetChar(){
     char c;
     R_lock->Acquire();
     readAvail->P();
     c = console->GetChar();
    // if(c == EOF)
     R_lock->Release();
     return c;
}


void
SynchConsole::WriteDone(){
     writeDone->V();
}

void
SynchConsole::ReadAvail(){
     readAvail->V();
} 




