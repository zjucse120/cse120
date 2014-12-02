/*
 * pingpongtest.c
 *
 * Simple user program to ping-pong between threads using fork() and yield() system call.
 * 
 *
 */

#include "syscall.h"

void ping(){
  
     //char prompt[1] = "p";
     //Write(prompt, 1, ConsoleOutput);
     //Yield();
     //int a=0;
     Exit(1);     
}

void pong(){

     //char prompt[1] = "b";
     //Write(prompt, 1, ConsoleOutput);
     //int b=0;
     //Yield();
     Exit(2);
}

int main(){
       Fork(ping);
       Yield();
       Fork(pong);
       Yield();
       Fork(ping);
       Yield();
       Fork(pong);
       Yield();
       Exit(0);
}
