#ifndef __STRUCTS_H__
#define __STRUCTS_H__

// Этот файл должен находится в директории проекта
#define PATH_NAME_FOR_FTOK "MSG"
#define PROJECT_ID_FOR_FTOK 13

enum REQUEST_TYPE
{
    REQUEST_CALC_1,
    REQUEST_CALC_2,
    REQUEST_CALC_3,
    REQUEST_N,
};

enum REPLY_CODE
{
    REPLY_BAD = -1,
    REPLY_SUCCESS,
    REPLY_CHECK_RESULT,
    REPLY_N,
};

struct request
{
    int req_type;
};

struct reply
{
    int   reply_code;
    int   req_type;
    float result;
};

enum MESSAGE_TYPE
{
    MESSAGE_BAD = -1,
    MESSAGE_REQUEST,
    MESSAGE_REPLY,
    MESSAGE_N
};

struct msg_data
{
    long  pid;
    int   msg_type; // Тип переданного сообщения (enum MESSAGE_TYPE)
    void *data;     // Приведенная к void* структура запроса или ответа
};

struct message
{
    long type; // Тип для адресации в очереди сообщений
    struct msg_data payload;
};

#endif // !__STRUCTS_H__
