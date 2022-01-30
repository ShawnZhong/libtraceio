#include <cstdio>

#define GEN_FN(name)                                                           \
  namespace name##_internal {                                                  \
    void fn() { fprintf(stderr, "\t\thello from fn\n"); }                      \
    static void static_fn() { fprintf(stderr, "\t\thello from static_fn\n"); } \
  }                                                                            \
  void name() {                                                                \
    name##_internal::fn();                                                     \
    name##_internal::static_fn();                                              \
  }
