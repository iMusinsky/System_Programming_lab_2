/*!
	\brief Файл, содержащий реализацию логгера
	\author Шпагин Д.
	\date 5 дек. 2023 г.
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

# define LOGGER_INIT(is_enable, level, stream) logger_enable(is_enable, level, stream)
# define LOGGER_ENABLE() logger_enable()
# define LOGGER_DISABLE() logger_disable()
# define LOGGER_SET_LEVEL(level) logger_set_level(level)
# define LOGGER_SET_STREAM(stream) logger_set_stream(stream)
# define LOG(level, format, ...) logging(create_entry(level, __LINE__, __FILE__, LOGGING_FUNCION_NAME, format, ##__VA_ARGS__));
# define LOG_ERROR(format, ...) LOG(LEVEL_ERROR, format, ##__VA_ARGS__)
# define LOG_WARN(format, ...) LOG(LEVEL_WARN,  format, ##__VA_ARGS__)
# define LOG_INFO(format, ...) LOG(LEVEL_INFO,  format, ##__VA_ARGS__)
# define LOG_DEBUG(format, ...) LOG(LEVEL_DEBUG, format, ##__VA_ARGS__)
# define GET_NAME_VAR(variable) #variable

# if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
# 	define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
# elif defined(__DMC__) && (__DMC__ >= 0x810)
# 	define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
# elif defined(__FUNCSIG__)
# 	define LOGGING_FUNCION_NAME __FUNCSIG__
# elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
# 	define LOGGING_FUNCION_NAME __FUNCTION__
# elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
# 	define LOGGING_FUNCION_NAME __FUNC__
# elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
# 	define LOGGING_FUNCION_NAME __func__
# else
# 	define LOGGING_FUNCION_NAME ""
# endif

#include <stdio.h>
#include <stdbool.h>

enum LOG_LEVEL
{
    LEVEL_OFF  = 0,
    LEVEL_ERROR,
    LEVEL_WARN,
    LEVEL_INFO,
    LEVEL_DEBUG
};
struct entry_t;
struct logger_t;

extern entry_t* create_entry(const unsigned level, const unsigned number_line,
	const char* name_file, const char* name_function, const char* format, ...);
extern void delete_entry(entry_t* entry);

extern void logger_init(const bool is_enable, const unsigned level, FILE* stream);
extern void logger_enable();
extern void logger_disable();
extern void logger_set_level(const unsigned level);
extern void logger_set_stream(FILE* stream);
extern void logging(entry_t* entry);

#endif // __LOGGER_H__