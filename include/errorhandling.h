#pragma once

#include "logging.h"
#include <errno.h>

#define TRY(func)                   \
{                                   \
    if (func == -1)                 \
    {                               \
        if (errno != 0)             \
            log_perror();           \
        else                        \
            log_error();            \
                                    \
        errno = 0;                  \
        return -1;                  \
    }                               \
}

#define TRYEXPR(func, expr)         \
{                                   \
    if (func == -1)                 \
    {                               \
        if (errno != 0)             \
            log_perror();           \
        else                        \
            log_error(expr);        \
                                    \
        errno = 0;                  \
        return -1;                  \
    }                               \
}
