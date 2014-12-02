#include "syscall.h"

void ping(){
  
     char prompt[1] = "p";
     Write(prompt, 1, ConsoleOutput);
     Yield(); 
}

void pong(){

     char prompt[1] = "b";
     Write(prompt, 1, ConsoleOutput);
     Yield();
}

int main(){
       Fork(ping);
       Yield();
       Fork(pong);
       Yield();
       Exit(0);
}
