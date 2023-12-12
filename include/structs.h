#ifndef __STRUCTS_H__
#define __STRUCTS_H__

/*
* Этот файл должен создать сервер при инициализации (и папку)
* При прекращении работы сервера - удалить 
*/
#define PATH_NAME_FOR_FTOK "/tmp/MSG"
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

typedef struct 
{
    int   reply_code;
    int   req_type;
    float result;
} reply;

enum MESSAGE_TYPE
{
    MESSAGE_BAD = -1,
    MESSAGE_REQUEST,
    MESSAGE_REPLY,
    MESSAGE_N
};


/*
* Если сообщение message имеет msg_type = MESSAGE_REQUEST читаем/заполняем из union.req_type
* Если msg_type = MESSAGE_REPLY, то заполняем/читаем union.reply_msg
* Если msg_type = MESSAGE_BAD, то union можно не читать и не заполнять 
*/
typedef union 
{
    int   req_type;     // Тип запроса
    reply reply_msg;    // Структура для сообщения ответа 
} payload_t;


/*
* Структура для передачи в msgrcv и msgsnd
* Предлагаю считать что type = 1 - есть сервер
* Ответ клиент ждет по своему pid 
*/
typedef struct
{
    long      type;     // Тип для адресации в очереди сообщений
    long      pid_from; // pid автора сообщения/клиента (для сервера выставить 1 или 0)
    int       msg_type; // Тип переданного сообщения (enum MESSAGE_TYPE)
    payload_t payload;  // Полезная нагрузка
} message;

#endif // !__STRUCTS_H__
