#ifndef __LOGGER_H__
#define __LOGGER_H__

enum LOG_LEVEL
{
    LOG_OFF  = 0,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN ,
    LOG_INFO ,
    LOG_DEBUG,
    LOG_TRACE
};

int init_logger(int log_lvl, const char *path);
int log_msg(int log_lvl, const char* format, ...);
int deinit_logger();

#endif // !__LOGGER_H__