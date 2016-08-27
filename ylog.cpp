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

#define BUF_LEN  4096


struct ylog_s {
    char *caller;
    int level;
    bool position;
    bool timer;
    bool fold;
    ylog_callback_t cb;

    uint64_t start_millisecond;
    mutex_handle_t cb_mutex;

    char *cache;
    int repeat_counter;
};


extern "C" ylog_t *ylog_open(const char *caller, int level, int position, int timer, int fold, ylog_callback_t cb)
{
    if (!cb)
        return NULL;

    ylog_t *ylog = (ylog_t *)malloc(sizeof(ylog_t));
    ylog->caller = (char *)malloc(strlen(caller)+1);
    strcpy(ylog->caller, caller);
    ylog->level = level;
    ylog->position = (position != 0);
    ylog->timer = (timer != 0);
    ylog->fold = (fold != 0);
    ylog->cb = cb;

    ylog->start_millisecond = GetTimeMS();
    MUTEX_CREATE(ylog->cb_mutex);

    ylog->cache = (char *)malloc(BUF_LEN);
    ylog->cache[0] = '\0';
    ylog->repeat_counter = 0;

    return ylog;
}


extern "C" void ylog_close(ylog_t *ylog)
{
    MUTEX_LOCK(ylog->cb_mutex);
    if (ylog->repeat_counter > 0) {
        char buf_repeat[64];
        snprintf(buf_repeat, sizeof(buf_repeat), "<repeat %d times>\n",
                ylog->repeat_counter + 1);
        ylog->cb(ylog->caller,
                ylog->timer ? GetTimeMS()-ylog->start_millisecond : 0,
                buf_repeat);
    }
    MUTEX_UNLOCK(ylog->cb_mutex);

    MUTEX_DESTROY(ylog->cb_mutex);
    free(ylog->caller);
    free(ylog->cache);
    free(ylog);
}


extern "C" void ylog_log(ylog_t *ylog, int level, const char *file, int line, const char *func, const char *fmt, ...)
{
    MUTEX_LOCK(ylog->cb_mutex);

    if (level > ylog->level) {
        MUTEX_UNLOCK(ylog->cb_mutex);
        return;
    }

    char buf[BUF_LEN];
    buf[sizeof(buf)-1] = '\0';

    va_list ap;
    va_start(ap, fmt);
    int offset = vsnprintf(buf, sizeof(buf)-129, fmt, ap);
    va_end(ap);

    if (ylog->position)
        snprintf(buf + offset, 128, "|  ..[%s:%d,%s]\n", file, line, func);
    else
        snprintf(buf + offset, 128, "\n");

    if (ylog->fold) {
        if (!strncmp(buf, ylog->cache, BUF_LEN)) {
            ylog->repeat_counter++;
        }
        else {
            if (ylog->repeat_counter > 0) {
                char buf_repeat[64];
                snprintf(buf_repeat, sizeof(buf_repeat), "<repeat %d times>\n",
                        ylog->repeat_counter + 1);
                ylog->cb(ylog->caller,
                        ylog->timer ? GetTimeMS()-ylog->start_millisecond : 0,
                        buf_repeat);
            }
            ylog->cb(ylog->caller,
                    ylog->timer ? GetTimeMS()-ylog->start_millisecond : 0, buf);
            strcpy(ylog->cache, buf);
            ylog->repeat_counter = 0;
        }
    }
    else {
        ylog->cb(ylog->caller,
                ylog->timer ? GetTimeMS()-ylog->start_millisecond : 0, buf);
    }

    MUTEX_UNLOCK(ylog->cb_mutex);
}
