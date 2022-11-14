#include <stdio.h>
#include <stdlib.h>

static void call_endbr32(void)\
{
    __asm__ volatile ("endbr32");
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    call_endbr32();
    /* Killed process othervise. */
    printf("PASS!\n");

    return EXIT_SUCCESS;
}