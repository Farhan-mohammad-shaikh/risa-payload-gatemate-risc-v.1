#ifndef OUTPOST_BASE_TESTING_ASSERT_H
#define OUTPOST_BASE_TESTING_ASSERT_H

#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1
#define OUTPOST_ASSERT(expr, msg) testingAssert(expr, msg)
#if __cplusplus < 201402L
// in c++11, (runtime) assertions are not allowed in constexpr context
#define OUTPOST_CONSTEXPR_ASSERT(expr, msg)  // nothing
#else
// default case: Also use assertions in constexpr context.
#define OUTPOST_CONSTEXPR_ASSERT(expr, msg) OUTPOST_ASSERT(expr, msg)
#endif
#else
// Default case: No assertions shall be called
#define OUTPOST_ASSERT(expr, msg)            // nothing
#define OUTPOST_CONSTEXPR_ASSERT(expr, msg)  // nothing
#endif

/**
 * @brief runtime assert fail function.
 * It is not defined in production code, but only in test/unittest/testing_assert.cpp
 * This is to assure that any unintentional compilation flags are noticed during linking.
 */
extern void __attribute__((noreturn))
testingAssertFail(const char* expression, const char* errorMessage = nullptr);

// The following lines are inspired by <assert.h>.
#if defined __cplusplus
#define __OUTPOST_ASSERT_VOID_CAST static_cast<void>
#else
#define __OUTPOST_ASSERT_VOID_CAST (void)
#endif

/* When possible, define assert so that it does not add extra
   parentheses around EXPR. Otherwise, those added parentheses would
   suppress warnings we'd expect to be detected by gcc's -Wparentheses. */
#if defined __cplusplus
#define testingAssert(expr, msg) (static_cast<bool>(expr) ? void(0) : testingAssertFail(#expr, msg))
#elif !defined __GNUC__ || defined __STRICT_ANSI__
#define testingAssert(expr, msg) ((expr) ? __ASSERT_VOID_CAST(0) : testingAssertFail(#expr, msg))
#else
/* The first occurrence of EXPR is not evaluated due to the sizeof,
   but will trigger any pedantic warnings masked by the __extension__
   for the second occurrence. The ternary operator is required to
   support function pointers and bit fields in this context, and to
   suppress the evaluation of variable length arrays. */
#define testingAssert(expr, msg)                    \
    ((void) sizeof((expr) ? 1 : 0), __extension__({ \
         if (expr)                                  \
             ; /* empty */                          \
         else                                       \
             testingAssertFail(#expr, msg);         \
     }))
#endif

#endif  // OUTPOST_BASE_TESTING_ASSERT_H
