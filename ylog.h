#ifndef __YLOG_H__
#define __YLOG_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


typedef enum {
    YLOG_OUTPUT_LEVEL_NONE      = -1,
    YLOG_OUTPUT_LEVEL_ERROR     =  0,
    YLOG_OUTPUT_LEVEL_INFO      =  1,
    YLOG_OUTPUT_LEVEL_ALL       =  2,
} ylog_output_level_t;


typedef enum {
    YLOG_OUTPUT_POS_NO          =  0,
    YLOG_OUTPUT_POS_YES         =  1,
} ylog_output_position_t;


typedef enum {
    YLOG_OUTPUT_TIME_NO         =  0,
    YLOG_OUTPUT_TIME_YES        =  1,
} ylog_output_time_t;


typedef enum {
    YLOG_OUTPUT_FOLD_REPEAT_NO  =  0,
    YLOG_OUTPUT_FOLD_REPEAT_YES =  1,
} ylog_output_fold_t;


typedef void (*ylog_callback_t)(void *caller, ylog_output_level_t level, uint64_t millisecond_start, uint64_t millisecond, const char *msg);

typedef struct ylog_s ylog_t;


/**
 * \param caller    Pointer to caller data.
 * \param level     Log level to output.
 * \param pos       Output code position or not.
 * \param timer     Output millisecond or not.
 * \param fold      Fold repeat logs or not.
 */
ylog_t *ylog_open(void *caller, ylog_output_level_t level, ylog_output_position_t pos, ylog_output_time_t timer, ylog_output_fold_t fold, ylog_callback_t cb);

void ylog_close(ylog_t *ylog);

/* do not use directly */
void ylog_log(ylog_t *ylog, ylog_output_level_t level, const char *file, int line, const char *func, const char *fmt, ...);

#define ylog_error(ylog, fmt, ...) \
        ylog_log(ylog, YLOG_OUTPUT_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ylog_info(ylog, fmt, ...) \
        ylog_log(ylog, YLOG_OUTPUT_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ylog_debug(ylog, fmt, ...) \
        ylog_log(ylog, YLOG_OUTPUT_LEVEL_ALL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* __YLOG_H__ */
