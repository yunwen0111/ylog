#ifndef __YLOG_H__
#define __YLOG_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


typedef void (*ylog_callback_t)(uint64_t millisecond, const char *msg);

typedef struct ylog_s ylog_t;


ylog_t *ylog_open(int level, int position, int timer, ylog_callback_t cb);

void ylog_close(ylog_t *ylog);

/* do not use directly */
void ylog_log(ylog_t *ylog, int level, const char *file, int line, const char *func, const char *fmt, ...);

#define ylog_info(ylog, fmt, ...) \
        ylog_log(ylog, 0, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ylog_debug(ylog, fmt, ...) \
        ylog_log(ylog, 1, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* __YLOG_H__ */
