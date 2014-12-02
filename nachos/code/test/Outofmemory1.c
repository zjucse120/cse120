/*
 * Outofmemory.c
 *
 * Test the case when memory is enough to load for the 'main', BUT fail to exec the new processes(../test/exittest) because of not enough memory
 */

#include "syscall.h"

int
main()
{   int i =1;
    int result = 1000;
    while(i<10){
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);
    result = Exec("../test/exittest", 0, 0, 0);

    i++;
}
    Exit(result);
}
