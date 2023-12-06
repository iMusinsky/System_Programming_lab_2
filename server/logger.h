/*!
    \brief Файл, содержащий реализацию логгера
    \author Шпагин Д.
    \date 5 дек. 2023 г.
    Перед использование логгер необходимо инициализировать фунцией logger_init, logger_init_file или соответствующими макросами.
    Желательно, использовать не функции, а именно макросы, т.к. в случае чего с помощью лишь одной директивы можно будет заставить компилятор игнорировать эту часть кода.
    Создать запись лога можно функцией logging или макросами LOG с припиской соответствующего уровня записи.
    Уровни важности записей лога выстроены по приоритету от меньшего к большему: LEVEL_DEBUG < LEVEL_INFO < LEVEL_WARN < LEVEL_ERROR.
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

// В случае, если нам совсем не нужен логгер, мы можем его выкинуть из сборки
#if !defined(USING_LOGGING)

#define LOGGER_INIT(...)
#define LOGGER_INIT_FILE(...)
#define LOGGER_ENABLE()
#define LOGGER_DISABLE()
#define LOGGER_SET_LEVEL(...)
#define LOGGER_SET_STREAM(...)
#define LOG(...)
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_DEBUG(...)
#define GET_NAME_VAR(...)

#else

/// Макрос инициализации статического внутреннего логгера
#define LOGGER_INIT(is_enable, level, stream) logger_init(is_enable, level, stream)
/// Макрос инициализации статического внутреннего логгера. Создается файл по указанному пути с именем год-месяц-день_часы:минуты:секунды.log
#define LOGGER_INIT_FILE(is_enable, level, path) logger_init_file(is_enable, level, path)
/// Макрос включения логгирования
#define LOGGER_ENABLE() logger_enable()
/// Макрос отключения логгирования
#define LOGGER_DISABLE() logger_disable()
/// Макрос установки уровня важности сообщений
#define LOGGER_SET_LEVEL(level) logger_set_level(level)
/// Макрос установки потока вывода логов
#define LOGGER_SET_STREAM(stream) logger_set_stream(stream)
/// Макрос создания записи лога с заданным уровнем
#define LOG(level, format, ...) logging(create_entry(level, __LINE__, __FILE__, LOGGING_FUNCION_NAME, format, ##__VA_ARGS__))
/// Макрос создания записи лога с уровнем ошибки
#define LOG_ERROR(format, ...) LOG(LEVEL_ERROR, format, ##__VA_ARGS__)
/// Макрос создания записи лога с уровнем предупреждения
#define LOG_WARN(format, ...) LOG(LEVEL_WARN, format, ##__VA_ARGS__)
/// Макрос создания записи лога с уровнем информации о системе
#define LOG_INFO(format, ...) LOG(LEVEL_INFO, format, ##__VA_ARGS__)
/// Макрос создания записи лога с уровнем отладки
#define LOG_DEBUG(format, ...) LOG(LEVEL_DEBUG, format, ##__VA_ARGS__)
/// Макрос получения строки имени переменной. Например, int counter; GET_NAME_VAR(counter) вернет "counter"
#define GET_NAME_VAR(variable) #variable

// Подбор доступной в данном компиляторе предопределенной директивы имени/сигнатуры функции
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define LOGGING_FUNCION_NAME __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define LOGGING_FUNCION_NAME __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define LOGGING_FUNCION_NAME __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define LOGGING_FUNCION_NAME __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define LOGGING_FUNCION_NAME __func__
#else
#define LOGGING_FUNCION_NAME ""
#endif

#include <stdio.h>
#include <stdbool.h>

/// @brief Перечисление с уровням важности лога. LEVEL_OFF - максимальный приоритет, LEVEL_DEBUG - минимальный
enum LOG_LEVEL
{
    LEVEL_ERROR = 1,
    LEVEL_WARN,
    LEVEL_INFO,
    LEVEL_DEBUG
};
typedef struct Entry entry_t;

/// @brief Создание записи лога с выделением памяти и форматированием сообщения
/// @param level Уроваень важности записи лога
/// @param number_line Номер строки, в который было вызвано создание записи (директива __LINE__)
/// @param name_file Имя файла, в котором было вызвано создание записи (директива __FILE__)
/// @param name_function Имя функции, в которой было вызвано создание записи (директива LOGGING_FUNCION_NAME)
/// @param format Строка форматирования
/// @param ... Параметры для форматирования
/// @return Указатель на новую запись лога
extern entry_t *create_entry(const unsigned level, const unsigned number_line,
    const char *name_file, const char *name_function, const char *format, ...);

/// @brief Освобождение памяти, занимаемой записью лога
/// @param entry Указатель на запись для удаления
extern void delete_entry(entry_t *entry);

/// @brief Инициализация статического внутреннего логгера
/// @param is_enable Должен ли работать логгер после иницализации
/// @param level Уровень важности выводимых записей
/// @param stream Указатель на поток вывода записей
extern void logger_init(const bool is_enable, const unsigned level, FILE *stream);

/// @brief Инициализация статического внетреннего логгера. Создается файл по указанному пути с именем год-месяц-день_часы:минуты:секунды.log
/// @param is_enable Должен ли работать логгер после иницализации
/// @param level Уровень важности выводимых записей
/// @param path Путь, по которому будет создан файл, в который будет писаться лог
extern void logger_init_file(const bool is_enable, const unsigned level, const char *path);

/// @brief Включить логгирование
extern void logger_enable();

/// @brief Выключить логгирование
extern void logger_disable();

/// @brief Установить уровень важности выводимых записей
/// @param level Уровень важности
extern void logger_set_level(const unsigned level);

/// @brief Установить поток вывода записей
/// @param stream Указатель на поток вывода записей
extern void logger_set_stream(FILE *stream);

/// @brief Вывести запись лога в поток вывода. Функция потокобезопасна
/// @param entry Указатель на запись
extern void logging(entry_t *entry);

#endif // !defined(USING_LOGGING)
#endif // __LOGGER_H__