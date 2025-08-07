#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "appdemo.h"

int main(int argc, char const *argv[])
{
    int c = 1;
    printf("Advanced Application Hello : %d\n", demoComputation(c));
    if (demoComputation(c) == 2 * c)
    {
        return 0;
    }
    return -1;
}
