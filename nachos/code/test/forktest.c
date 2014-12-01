/*
 * forktest.c
 *
 *         Simple program to test fork system call
 *
 */

#include "syscall.h"

void func()
{
    int a=0; 
    Exit(0); 
}
int
main(){
     Fork(func);
     int b=0;
     Exit(1);
}
