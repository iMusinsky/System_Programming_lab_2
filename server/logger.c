#include "logger.h"

int init_logger(int log_lvl, const char *path)
{
    if (!path) {
        return -1;
    }
    return 0;
}

int log_msg(int log_lvl, const char* format, ...)
{
    (void)log_lvl;
    (const void)format;
    return 0;
}

int deinit_logger()
{
    return 0;
}