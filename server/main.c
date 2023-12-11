#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "logger.h"
#include "structs.h"
#include "handler.h"

#define MAX_LOGGER_PATH_LENGTH 100
#define DEFAULT_LOGGER_PATH "./logs/"
#define DEFAULT_LOGGER_LEVEL LEVEL_DEBUG

//To Do: fix names if needed and delete this ToDo
struct logger_init_data
{
    int level;
    char path[MAX_LOGGER_PATH_LENGTH];
};

int init(int* msg_queue_id,struct logger_init_data* logger_data);
int work_cycle(int msg_queue_id);
int deinit(int msg_queue_id);
int args_handling(int argc, char* argv[],struct logger_init_data* logger_data);

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    struct logger_init_data logger_data;
    if(args_handling(argc,argv, &logger_data)){
        return -1;
    }

    int msg_queue_id = 0;
    if(init(&msg_queue_id,&logger_data)){
        return -1;
    }

    work_cycle(msg_queue_id);
    deinit(msg_queue_id);

    return 0;
}

int init(int* msg_queue_id,struct logger_init_data* logger_data)
{
    //IPC message queue creating
    key_t key = ftok(PATH_NAME_FOR_FTOK,PROJECT_ID_FOR_FTOK);
    if (key == -1) {
        perror("Ошибка вызова функции ftok()");
        return -1;
    }
    *msg_queue_id = msgget(key, 0666|IPC_CREAT);
    if (*msg_queue_id == -1) {
        perror("Ошибка вызова функции msgget()");
        return -1;
    }
    //To Do: Logger initialisation 
    
    return 0;
}

int deinit(int msg_queue_id)
{
    msgctl(msg_queue_id, 0, NULL);
    //To Do: add logger deinit(if needed) or delete this todo
    return 0;
}

int work_cycle(int msg_queue_id)
{
    while (true) {
        struct message request_msg;
        //get request messages from queue( messages with type equal to 1)
        msgrcv(msg_queue_id,*request_msg, sizeof(request_msg), 1, 0);

        //if msg_type is no a MESSAGE_REQUEST, then abort msg processing
        if(request_msg.msg_type != MESSAGE_REQUEST){
            //To Do: add error log
            continue; 
        }
        //if source of request is server, then abort msg processing
        if (request_msg.pid_from == 1) {
            //To Do: add error log
            continue; 
        }

        struct message reply_msg;
        reply_msg.type = request_msg.pid_from;
        reply_msg.pid_from = 1;//To Do: fix this magic number(it's a type of server procces in msg queue) here and in msgrcv invoke
        reply_msg.msg_type = MESSAGE_REPLY;

        handle_request(request_msg.payload.req_type, *reply_msg);

        msgsnd(msg_queue_id, *reply_msg, sizeof(reply_msg), 0);
    }
}

int args_handling(int argc, char* argv[],struct logger_init_data* logger_data)
{
    if(argc == 1){
        logger_data->level = DEFAULT_LOGGER_LEVEL;
        strcat(logger_data->path,DEFAULT_LOGGER_PATH);
        printf("Аргументы командной строки отсуствуют, используются значения по-умолчанию.\n");
        return 0;
    }
    else if (argc == 2) {
        //To Do: add check level 
        int tmp = 0;
        if (sscanf(argv[1], "%i",&tmp) == 0) {
            fprintf(stderr,"Ведено некорректное значение\n");
            return -1;
        }
        logger_data->level = tmp;
        strcat(logger_data->path,DEFAULT_LOGGER_PATH);
        return 0;
    }
    else if (argc == 3) {
        int tmp = 0;
        char tmp_string[MAX_LOGGER_PATH_LENGTH];

        if (sscanf(argv[1], "%i",&tmp) == 0) {
            fprintf(stderr,"Ведено некорректное значение уровня\n");
            return -1;
        }

        if (sscanf(argv[2], "%s",&tmp_string) == 0) {
            fprintf(stderr,"Ведено некорректное значение пути файлов логов \n");
            return -1;
        }

        logger_data->level = tmp;
        strcat(logger_data->path,tmp_string);
        return 0;
    }
    else if (argc >= 4) {
        fprintf(stderr,"Ведено слишком много аргументов\n");
        return -1;
    }
}