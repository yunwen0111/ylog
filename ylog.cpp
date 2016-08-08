#include "ylog.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#if defined(WIN32)

#include <windows.h>

typedef HANDLE mutex_handle_t;
#define MUTEX_CREATE(handle) handle = CreateMutex(NULL, FALSE, NULL)
#define MUTEX_LOCK(handle) WaitForSingleObject(handle, INFINITE)
#define MUTEX_UNLOCK(handle) ReleaseMutex(handle)
#define MUTEX_DESTROY(handle) CloseHandle(handle)

#else /* Use pthread library */

#include <pthread.h>

typedef pthread_mutex_t mutex_handle_t;

#define MUTEX_CREATE(handle) pthread_mutex_init(&(handle), NULL)
#define MUTEX_LOCK(handle) pthread_mutex_lock(&(handle))
#define MUTEX_UNLOCK(handle) pthread_mutex_unlock(&(handle))
#define MUTEX_DESTROY(handle) pthread_mutex_destroy(&(handle))

#endif


struct ylog_s {
    int level;
    int position;
    ylog_callback_t cb;

    mutex_handle_t cb_mutex;
};


extern "C" ylog_t *ylog_open(int level, int position, ylog_callback_t cb)
{
    ylog_t *ylog = (ylog_t *)malloc(sizeof(ylog_t));
    ylog->level = level;
    ylog->position = position;
    ylog->cb = cb;
    MUTEX_CREATE(ylog->cb_mutex);

    return ylog;
}


extern "C" void ylog_close(ylog_t *ylog)
{
    MUTEX_DESTROY(ylog->cb_mutex);
    free(ylog);
}


extern "C" void ylog_log(ylog_t *ylog, int level, const char *file, int line, const char *func, const char *fmt, ...)
{
    MUTEX_LOCK(ylog->cb_mutex);

    if (level > ylog->level) {
        MUTEX_UNLOCK(ylog->cb_mutex);
        return;
    }

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

    MUTEX_UNLOCK(ylog->cb_mutex);
}
