#include "logger.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

#define SIZE_OF_MESSAGE_BUFFER 512
#define SIZE_OF_FORMATING_BUFFER SIZE_OF_MESSAGE_BUFFER + 256

enum LOG_LEVEL
{
    LEVEL_OFF = 0,
    LEVEL_ERROR,
    LEVEL_WARN,
    LEVEL_INFO,
    LEVEL_DEBUG
};

typedef struct Entry
{
    unsigned id;
    unsigned level;
    time_t time_stamp;
    const char *name_file;
    const char *name_function;
    unsigned number_line;
    char *message;
} entry_t;

typedef struct Logger
{
    bool is_enable;
    unsigned level;
    FILE *stream;
} logger_t;

static logger_t instance;

entry_t *create_entry(const unsigned level, const unsigned number_line,
                      const char *name_file, const char *name_function, const char *format, ...)
{
    static unsigned counter = 0;
    entry_t *entry = (entry_t *)malloc(sizeof(entry_t));
    entry->id = counter++;
    entry->level = level;
    time(&entry->time_stamp);
    entry->name_file = name_file;
    entry->name_function = name_function;
    entry->number_line = number_line;
    va_list args;
    va_start(args, format);
    entry->message = (char *)malloc(SIZE_OF_MESSAGE_BUFFER * sizeof(char));
    vsprintf(entry->message, format, args);
    va_end(args);
    return entry;
}

void delete_entry(entry_t *entry)
{
    if (entry)
    {
        if (entry->message)
            free(entry->message);
        free(entry);
    }
}

void logger_init(const bool is_enable, const unsigned level, FILE *stream)
{
    instance.level = level;
    instance.is_enable = is_enable;
    instance.stream = stream;
}

void logger_init_file(const bool is_enable, const unsigned level, const char *path)
{
    char buffer[SIZE_OF_FORMATING_BUFFER];
    time_t tmp = time(NULL);
    struct tm *now = localtime(&tmp);
    sprintf(buffer, "%s\\%d-%2d-%2d_%2d:%2d:%2d.log",
            path,
            now->tm_year + 1900,
            now->tm_mon + 1,
            now->tm_mday,
            now->tm_hour,
            now->tm_min,
            now->tm_sec);
    FILE* file_stream = open(path, O_CREAT | O_RDWR);
    logger_init(is_enable, level, file_stream);
    LOG_INFO("Инициализация логгера");
}

void logger_enable()
{
    instance.is_enable = true;
}

void logger_disable()
{
    instance.is_enable = false;
}

void logger_set_level(const unsigned level)
{
    instance.level = level;
}

void logger_set_stream(FILE *stream)
{
    instance.stream = stream;
}

const char *level_to_string(const unsigned level)
{
    switch (level)
    {
    case LEVEL_ERROR:
        return "error";
    case LEVEL_WARN:
        return "warn ";
    case LEVEL_INFO:
        return "info ";
    case LEVEL_DEBUG:
        return "debug";
    }
    return "";
}

void logging(entry_t *entry)
{
    if (entry == NULL || entry->level > instance.level || instance.is_enable == false)
        return;
    char buffer[SIZE_OF_FORMATING_BUFFER];
    struct tm *t = localtime(&entry->time_stamp);
    sprintf(buffer, "#%d [%s] [%02d-%02d-%4d %02d:%02d:%02d] %s : %s\n",
            entry->id,
            level_to_string(entry->level),
            t->tm_mday,
            t->tm_mon + 1,
            t->tm_year + 1900,
            t->tm_hour,
            t->tm_min,
            t->tm_sec,
            entry->name_function,
            entry->message);
    fputs(buffer, instance.stream);
    delete_entry(entry);
}