#include "ylog.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


struct ylog_s {
    int level;
    int position;
    ylog_callback_t cb;
};


extern "C" ylog_t *ylog_open(int level, int position, ylog_callback_t cb)
{
    ylog_t *ylog = (ylog_t *)malloc(sizeof(ylog_t));
    ylog->level = level;
    ylog->position = position;
    ylog->cb = cb;

    return ylog;
}


extern "C" void ylog_close(ylog_t *ylog)
{
    free(ylog);
}


extern "C" void ylog_log(ylog_t *ylog, int level, const char *file, int line, const char *func, const char *fmt, ...)
{
    if (level > ylog->level)
        return;

    char buf[4096];
    buf[sizeof(buf)-1] = '\0';

    va_list ap;
    va_start(ap, fmt);
    int offset = vsnprintf(buf, sizeof(buf)-129, fmt, ap);
    va_end(ap);

    if (ylog->position)
        snprintf(buf + offset, 128, "[...%s:%d|%s]", file, line, func);

    if (ylog->cb)
        ylog->cb(buf);
}
