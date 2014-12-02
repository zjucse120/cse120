#include "copyright.h"
#include "console.h"
#include "synch.h"
//#include "system.h"
//#ifdef USER_PROGRAM
#include "machine.h"
#include "addrspace.h"



class SynchConsole{
public:
    SynchConsole(char* readFile, char* writeFile);
    
    ~SynchConsole();
    
    void PutChar(char c);
    
    char GetChar();
    
    void WriteDone();
    
    void ReadAvail();

private:
    Console* console;
    Lock* W_lock;
    Lock* R_lock;
    Semaphore *readAvail;
    Semaphore *writeDone;
};
