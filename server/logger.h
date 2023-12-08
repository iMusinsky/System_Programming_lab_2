/*!
	\brief Файл, содержащий реализацию логгера
	\author Шпагин Д.
	\date 5 дек. 2023 г.
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__
// Макросы замены в случае отсутсвия логгирования
#if !defined(USING_LOGGING)

#	define LOGGER_ENABLE()
#	define LOGGER_DISABLE()
#	define LOGGER_SET_STREAM(...)

#	define LOG(...) 
#	define LOG_DEBUG(...) 
#	define LOG_INFO(...)
#	define LOG_WARN(...)
#	define LOG_ERROR(...)
#	define LOG_FATAL(...)

#else

#	define LOGGING_SIZE_OF_MESSAGE_BUFFER 	512
#	define LOGGING_SIZE_OF_FORMATING_BUFFER LOGGING_SIZE_OF_MESSAGE_BUFFER + 256

#	define LOGGER_ENABLE()
#	define LOGGER_DISABLE()
#	define LOGGER_SET_STREAM(stream)

#	define LOG(level, format, ...) logging(instance, create_entry(level, __LINE__, __FILE__, LOGGING_FUNCION_NAME, format, ##__VA_ARGS__));
#	define LOG_DEBUG(format, ...)  LOG(LEVEL_DEBUG, format, ##__VA_ARGS__)
#	define LOG_INFO(format, ...)   LOG(LEVEL_INFO,  format, ##__VA_ARGS__)
#	define LOG_WARN(format, ...)   LOG(LEVEL_WARN,  format, ##__VA_ARGS__)
#	define LOG_ERROR(format, ...)  LOG(LEVEL_ERROR, format, ##__VA_ARGS__)

#	define GET_NAME_VAR(variable) #variable

#	if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#		define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
#	elif defined(__DMC__) && (__DMC__ >= 0x810)
#		define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
#	elif defined(__FUNCSIG__)
#		define LOGGING_FUNCION_NAME __FUNCSIG__
#	elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#		define LOGGING_FUNCION_NAME __FUNCTION__
#	elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#		define LOGGING_FUNCION_NAME __FUNC__
#	elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#		define LOGGING_FUNCION_NAME __func__
#	else
#		define LOGGING_FUNCION_NAME ""
#	endif

#include <stdio.h>

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

typedef struct Entry entry_t;
extern entry_t* create_entry(const unsigned level, const unsigned number_line,
	const char* name_file, const char* name_function, const char* format, ...);
extern entry_t* create_entry(const unsigned level, const char* format, ...);
extern void delete_entry(entry_t* entry);

typedef struct Entry logger_t;
extern void delete_entry(entry_t* entry);
extern logger_t* create_logger(const bool is_enable, const unsigned level, FILE* stream);
extern void delete_logger(logger_t* logger);
extern void logging(logger_t* logger, entry_t* entry);

static logger_t* instance;

#endif // defined(USING_LOGGING)
#endif // __LOGGER_H__
