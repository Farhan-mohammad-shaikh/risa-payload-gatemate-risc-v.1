#include <backward.hpp>

#include <outpost/base/testing_assert.h>

// noinline so that the offset of (interesting) stack pointers is always the same
void __attribute__((noinline)) __attribute__((noreturn))
testingAssertFail(const char* expression, const char* errorMessage)
{
    if (expression)
    {
        fprintf(stderr, "[ERROR] Assumption '%s' did not hold", expression);
        if (errorMessage)
        {
            fprintf(stderr, ": %s\n", errorMessage);
        }
        else
        {
            fprintf(stderr, "\n");
        }
    }
    backward::StackTrace st;
    st.load_here(32);     // Max 32 frames
    st.skip_n_firsts(3);  // skip 3 frames (call to this function)
    backward::Printer p;
    p.object = true;
    p.snippet = true;
    p.trace_context_size = 1;
    p.reverse = false;
    p.color_mode = backward::ColorMode::automatic;
    p.address = true;
    p.print(st, stderr);
    abort();
}
