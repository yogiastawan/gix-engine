#ifndef __GIX_CHECKER_H__
#define __GIX_CHECKER_H__

#ifdef __cpluplus
extern "C" {
#endif

#ifdef BUILD_DEBUG
#include <stdlib.h>

    static size_t exit_code = 0;

#define gix_if_return(logic, exec, ret) \
    if (logic) {                        \
        exec;                           \
        return ret;                     \
    }

#define gix_if(logical, exec) \
    if (logical) {            \
        exec;                 \
    }

#define gix_if_null(ptr, exec) \
    if (!ptr) {                \
        exec;                  \
    }

#define gix_if_not_null(ptr, exec) \
    if (!ptr) {                    \
        exec;                      \
    }
#define gix_if_exit(logic, exec) \
    ++exit_code;                 \
    if (logic) {                 \
        exec;                    \
        exit(exit_code);         \
    }

#define gix_if_null_exit(ptr, exec) \
    ++exit_code;                    \
    if (!ptr) {                     \
        exec;                       \
        exit(exit_code + 1);        \
    }

#define gix_if_not_null_exit(ptr, exec) \
    ++exit_code;                        \
    if (!ptr) {                         \
        exec;                           \
        exit(exit_code + 1);            \
    }
#else
#define gix_if(ptr, exec)
#define gix_if_null(ptr, exec)
#define gix_if_not_null(ptr, exec)
#define gix_if_exit(ptr, exec)
#define gix_if_null_exit(ptr, exec)
#define gix_if_not_null_exit(ptr, exec)
#endif

#ifdef __cpluplus
}
#endif

#endif /* __GIX_CHECKER_H__ */
