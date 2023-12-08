#include "logger.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdbool.h>

typedef struct Entry
{
	unsigned id;
	unsigned level;
	time_t time_stamp;
	const char* name_file;
	const char* name_function;
	unsigned number_line;
	char* message;
} entry_t;

typedef struct Logger
{
    bool is_enable;
	unsigned level;
	FILE* stream;
} logger_t;

entry_t *create_entry(const unsigned level, const unsigned number_line,
	const char *name_file, const char *name_function, const char *format, ...)
{
	static unsigned counter = 0;
	entry_t* entry = (entry_t*)malloc(sizeof(entry_t));
	entry->id = counter++;
	entry->level = level;
	time(&entry->time_stamp);
	entry->name_file = name_file;
    entry->name_function = name_function;
    entry->number_line = number_line;
	va_list args;
	va_start(args, format);
	// TODO почему не видит директиву LOGGING_SIZE_OF_MESSAGE_BUFFER
	entry->message = (char*)malloc(512 * sizeof(char));
	vsprintf(entry->message, format, args);
	va_end(args);
	return entry;
}

entry_t *create_entry(const unsigned level, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	entry_t* entry = create_entry(level, 0, NULL, NULL, format, args);
	va_end(args);
	return entry;
}

void delete_entry(entry_t* entry)
{
	if (entry) {
		if (entry->message)
			free(entry->message);
		free(entry);
	}
}
logger_t *create_logger(const bool is_enable, const unsigned level, FILE *stream)
{
	logger_t* logger = (logger_t*)malloc(sizeof(logger_t));
	logger->level = level;
	logger->is_enable = is_enable;
	logger->stream = stream;
	return logger;
}

void delete_logger(logger_t* logger)
{
    if (logger != NULL)
	    free(logger);
}

const char* level_to_string(unsigned level)
{
	return "";
}

void logging(logger_t *logger, entry_t *entry)
{
    if (logger == NULL || entry == NULL || entry->level > logger->level)
        return;
	char buffer[768];
    struct tm *t = localtime(&entry->time_stamp);
	sprintf(buffer, "#%d [%s] [%02d-%02d-%4d %02d:%02d:%02d] %s : %s",
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
	fputs(buffer, logger->stream);
    delete_entry(entry);
}