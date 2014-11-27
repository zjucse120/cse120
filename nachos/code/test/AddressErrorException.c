/*
 * AddressErrorException.c
 *
 *        Simple program to test address error exception.
 */

#include "syscall.h"

int
main()
{
        int* p;
        p = -1;
        *p = 42;
}
