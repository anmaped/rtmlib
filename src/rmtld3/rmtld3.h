#ifndef _RMTLD3_H_
#define _RMTLD3_H_

#include <assert.h>

typedef unsigned int proposition;
typedef float realnumber;
typedef std::pair<realnumber, bool> duration;

enum three_valued_type { T_TRUE, T_FALSE, T_UNKNOWN };
enum four_valued_type { FV_TRUE, FV_FALSE, FV_UNKNOWN, FV_SYMBOL };

#define make_duration(r, b) std::make_pair((realnumber)r, b)

inline duration sum_dur(const duration &lhs, const duration &rhs) {
  return make_duration(lhs.first + rhs.first, lhs.second || rhs.second);
}

inline duration mult_dur(const duration &lhs, const duration &rhs) {
  return make_duration(lhs.first * rhs.first, lhs.second || rhs.second);
}

// type conversion from three_valued_type to four_valued_type
#define b3_to_b4(b3)                                                           \
  (b3 == T_TRUE) ? FV_TRUE : ((b3 == T_FALSE) ? FV_FALSE : FV_UNKNOWN)

// convert four_valued_type into three_valued type
#define b4_to_b3(b4)                                                           \
  (b4 == FV_TRUE) ? T_TRUE : ((b4 == FV_FALSE) ? T_FALSE : T_UNKNOWN)

/** OR */
#define b3_or(b31, b32)                                                        \
  (b31 == T_TRUE || b32 == T_TRUE)                                             \
      ? T_TRUE                                                                 \
      : ((b31 == T_FALSE && b32 == T_FALSE) ? T_FALSE : T_UNKNOWN)

/** NOT */
#define b3_not(b3)                                                             \
  ((b3 == T_TRUE) ? T_FALSE : ((b3 == T_FALSE) ? T_TRUE : T_UNKNOWN))

/** Relation operator < */
#define b3_lessthan(n1, n2)                                                    \
  ((std::get<1>(n1) || std::get<1>(n2))                                        \
       ? T_UNKNOWN                                                             \
       : ((std::get<0>(n1) < std::get<0>(n2)) ? T_TRUE : T_FALSE))

#define ASSERT_RMTLD3(l) assert(l)

#ifndef USE_DEBUGV_RMTLD3
#define DEBUGV_RMTLD3(...)
#else
#include <algorithm>
#include <string.h>

static int ident = 0;
static int r = 0;

#define DEBUGV_RMTLD3(cformat, ...)                                            \
  {                                                                            \
    std::string str(cformat);                                                  \
    const char *x, *y, *z = NULL;                                              \
                                                                               \
    x = strchr(cformat, '$');                                                  \
    if (x != NULL) {                                                           \
      str.erase(std::remove(str.begin(), str.end(), '$'), str.end());          \
      for (int i = 0; i < ident; i++)                                          \
        printf("\t");                                                          \
      ident++;                                                                 \
    }                                                                          \
                                                                               \
    y = strchr(cformat, '@');                                                  \
    if (y != NULL) {                                                           \
      str.erase(std::remove(str.begin(), str.end(), '@'), str.end());          \
      ident--;                                                                 \
      for (int i = 0; i < ident; i++)                                          \
        printf("\t");                                                          \
    }                                                                          \
                                                                               \
    z = strchr(cformat, '\n');                                                 \
    if (z != NULL) {                                                           \
      r = 0;                                                                   \
    }                                                                          \
                                                                               \
    if (x == NULL && y == NULL && r == 0) {                                    \
      if (z == NULL)                                                           \
        r++;                                                                   \
      for (int i = 0; i < ident; i++)                                          \
        printf("\t");                                                          \
    }                                                                          \
    ::printf(str.c_str(), ##__VA_ARGS__);                                      \
  };
#endif

#ifndef USE_DEBUG_RMTLD3
#define DEBUG_RTMLD3(...)
#else
#define DEBUG_RTMLD3(args...) ::printf(args)
#endif

#define out_p(res)                                                             \
  (res == T_TRUE) ? "true" : ((res == T_FALSE) ? "false" : "unknown")

#define out_fv(fv)                                                             \
  (fv == FV_TRUE)                                                              \
      ? "true"                                                                 \
      : ((fv == FV_FALSE) ? "false"                                            \
                          : ((fv == FV_UNKNOWN) ? "unknown" : "symbol"))

extern int count_until_iterations;

#endif //_RMTLD3_H_
