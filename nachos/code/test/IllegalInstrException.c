/*
 * IllegalInstrException.c
 *
 *        Simple program to test illegal instruction exception.
 */ 

#include "syscall.h"

int
main()
{
      int a;
      a = 5/0;
}
