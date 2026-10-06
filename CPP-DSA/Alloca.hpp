#if defined(_MSC_VER)
/* Microsoft Visual Studio / MSVC */
#include <malloc.h>
#define alloca _alloca
#elif defined(__GNUC__) || defined(__clang__)
/* GCC or Clang */
#define alloca __builtin_alloca
#elif defined(__linux__) || defined(__sun)
/* Linux or Solaris */
#include <alloca.h>
#else
/* Fallback for other standard POSIX systems */
#include <stdlib.h>
#endif