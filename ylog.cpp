#include "ylog.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


#if defined(WIN32)

#include <windows.h>

typedef HANDLE mutex_handle_t;
#define MUTEX_CREATE(handle) handle = CreateMutex(NULL, FALSE, NULL)
#define MUTEX_LOCK(handle) WaitForSingleObject(handle, INFINITE)
#define MUTEX_UNLOCK(handle) ReleaseMutex(handle)
#define MUTEX_DESTROY(handle) CloseHandle(handle)

#define GetTimeMS() (uint64_t)timeGetTime()

#else /* Use pthread library */

#include <pthread.h>
#include <sys/time.h>

typedef pthread_mutex_t mutex_handle_t;

#define MUTEX_CREATE(handle) pthread_mutex_init(&(handle), NULL)
#define MUTEX_LOCK(handle) pthread_mutex_lock(&(handle))
#define MUTEX_UNLOCK(handle) pthread_mutex_unlock(&(handle))
#define MUTEX_DESTROY(handle) pthread_mutex_destroy(&(handle))

static inline uint64_t GetTimeMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

#endif


struct ylog_s {
    char *caller;
    int level;
    bool position;
    bool timer;
    ylog_callback_t cb;

    uint64_t start_millisecond;
    mutex_handle_t cb_mutex;
};


extern "C" ylog_t *ylog_open(const char *caller, int level, int position, int timer, ylog_callback_t cb)
{
    if (!cb)
        return NULL;

    ylog_t *ylog = (ylog_t *)malloc(sizeof(ylog_t));
    ylog->caller = (char *)malloc(strlen(caller)+1);
    strcpy(ylog->caller, caller);
    ylog->level = level;
    ylog->position = (position != 0);
    ylog->timer = (timer != 0);
    ylog->cb = cb;
    ylog->start_millisecond = GetTimeMS();
    MUTEX_CREATE(ylog->cb_mutex);

    return ylog;
}


extern "C" void ylog_close(ylog_t *ylog)
{
    MUTEX_DESTROY(ylog->cb_mutex);
    free(ylog->caller);
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

    ylog->cb(ylog->caller,
            ylog->timer ? GetTimeMS()-ylog->start_millisecond : 0, buf);

    MUTEX_UNLOCK(ylog->cb_mutex);
}
