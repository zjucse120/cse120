/*
 * releasetest.c
 *
 * Test the case when a exec is loaded and released then next exec can run
 */

#include "syscall.h"

int
main()
{   int i = 0;
    int result = 1000;
    int num;
    result = Exec("../test/exittest", 0, 0, 0);
    for(i=0;i<50000;i++){}
    num = 0;
    result = Exec("../test/exittest", 0, 0, 0);
    for(i=0;i<50000;i++){}
    num ++;
    result = Exec("../test/exittest", 0, 0, 0);
    for(i=0;i<50000;i++){}
    num ++;
    result = Exec("../test/exittest", 0, 0, 0);
    for(i=0;i<50000;i++){}
    num ++;
    result = Exec("../test/exittest", 0, 0, 0);
    for(i=0;i<50000;i++){}
    num ++;

    Exit(num);
}
