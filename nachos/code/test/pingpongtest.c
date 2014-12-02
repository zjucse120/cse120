/*
 * pingpongtest.c
 *
 * Simple user program to ping-pong between threads using fork() and yield() system call.
 * 
 *
 */

#include "syscall.h"

void ping(){
  
     Exit(1);     
}

void pong(){

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
