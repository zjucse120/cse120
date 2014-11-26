#include "copyright.h"
#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"
#include "system.h"

class SynchConsole{
pubilc:
    SynchConsole(char* readFile, char* writeFile);
    
    ~SynchConsole();
    
    void PutChar(char ch);
    
    char GetChar();
    
    void WriteDone();
    
    void ReadDone();

private:
    Console* console;
    Lock* W_lock;
    Lock* R_lock;
    Semphore* readAvial;
    Semphore* writeDone;
};
#endif //SYNCHCONSOLE_H
