#include <stdio.h>
#include <stdlib.h>

#define TEST_DEFINE_BSWAP32(W, DEST)                            \
static unsigned long test_bswap32_##W(unsigned long val)        \
{                                                               \
    __asm__ volatile (                                          \
        "bswap %0\n\t"                                          \
        : #DEST (val)                                           \
        : /* no input */                                        \
        : /* no clobbing */                                     \
    );                                                          \
    return val;                                                 \
}

#define TEST_VAL1       0xAA00FFBBUL
#define TEST_VAL1_RES   0xBBFF00AAUL

#define TEST_VAL2       0xDEADBEEFUL
#define TEST_VAL2_RES   0xEFBEADDEUL

TEST_DEFINE_BSWAP32(ax, +a);
TEST_DEFINE_BSWAP32(bx, +b);
TEST_DEFINE_BSWAP32(cx, +c);
TEST_DEFINE_BSWAP32(dx, +d);
TEST_DEFINE_BSWAP32(si, +S);
TEST_DEFINE_BSWAP32(di, +D);


int main(int argc, char **argv)
{
    volatile unsigned long val;

    (void) argc;
    (void) argv;

    val = test_bswap32_ax(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_ax(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    val = test_bswap32_bx(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_bx(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    val = test_bswap32_cx(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_cx(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    val = test_bswap32_dx(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_dx(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    val = test_bswap32_si(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_si(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    val = test_bswap32_di(TEST_VAL1);
    printf("%s!\n", val == TEST_VAL1_RES ? "PASS" : "FAIL");

    val = test_bswap32_di(TEST_VAL2);
    printf("%s!\n", val == TEST_VAL2_RES ? "PASS" : "FAIL");

    return EXIT_SUCCESS;
}