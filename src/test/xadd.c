#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

static uint32_t flags;
static uint32_t src;
static uint32_t dest;

#define TEST_DEFINE_XADD(W, LOCK_PREFIX, DEST, SRC) \
static void test_xadd_##W (void) {                  \
    __asm__ volatile (                              \
    LOCK_PREFIX " xaddl %1, %0\n\t"                 \
        "pushfl\n\t"                                \
        "popl %2\n\t"                               \
    : #DEST (dest), #SRC (src), "=m" (flags)        \
    : /* no input */                                \
    : "cc"                                          \
    );                                              \
}

#define TEST_CALL_XADD(W) do {                      \
        test_xadd_##W();                            \
    } while(0)

#define TEST_CHECK_RESULTS_XADD(W, DEST_VAL, SRC_VAL, ZF) do {              \
    printf(# W " a flags: 0x%" PRIx32 "\n", flags);                         \
    printf(# W " a dest: %" PRIu32 "\n", dest);                             \
    printf(# W " a src: %" PRIu32 "\n", src);                               \
    log_result(# W " dest and src", dest == (DEST_VAL) && src == (SRC_VAL));\
    log_result(# W " zero flag", ((flags >> 6) & 1) == (ZF));               \
} while(0)

TEST_DEFINE_XADD(regreg, "", +r, +r);
TEST_DEFINE_XADD(memreg, "", +m, +r);
TEST_DEFINE_XADD(lockmemreg, "lock", +m, +r);

static void preset_test_values_to_zero(void)
{
    flags = 0;
    src = 0;
    dest = 0;
}

static void preset_test_values(void)
{
    flags = UINT32_MAX;
    src = 666;
    dest = 111;
}

static void log_result(char *what, bool expr)
{
    printf("%s: %d - ", what, !!(expr));
    if (!!(expr)) {
        printf("PASS\n");
        return;
    }

    printf("FAIL\n");
}

int main(int argc, char **argv)
{

    (void) argc;
    (void) argv;

    preset_test_values();
    TEST_CALL_XADD(regreg);
    TEST_CHECK_RESULTS_XADD(regreg, 777, 111, 0);

    preset_test_values_to_zero();
    TEST_CALL_XADD(regreg);
    TEST_CHECK_RESULTS_XADD(regreg, 0, 0, 1);

    preset_test_values();
    TEST_CALL_XADD(memreg);
    TEST_CHECK_RESULTS_XADD(memreg, 777, 111, 0);

    preset_test_values_to_zero();
    TEST_CALL_XADD(memreg);
    TEST_CHECK_RESULTS_XADD(memreg, 0, 0, 1);

    /*
     * LOCK PREFIX
     */
    preset_test_values();
    TEST_CALL_XADD(lockmemreg);
    TEST_CHECK_RESULTS_XADD(lockmemreg, 777, 111, 0);

    preset_test_values_to_zero();
    TEST_CALL_XADD(lockmemreg);
    TEST_CHECK_RESULTS_XADD(lockmemreg, 0, 0, 1);

    return EXIT_SUCCESS;
}
