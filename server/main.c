#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "logger.h"
#include "structs.h"
#include "handler.h"

#define MAX_LOGGER_PATH_LENGTH 100
#define DEFAULT_LOGGER_PATH "logs"
#define DEFAULT_LOGGER_LEVEL LEVEL_DEBUG

//To Do: fix names if needed and delete this ToDo
struct logger_init_data
{
    int level;
    char path[MAX_LOGGER_PATH_LENGTH];
};

int init(int *msg_queue_id, struct logger_init_data *logger_data);
int work_cycle(int msg_queue_id);
int deinit(int msg_queue_id);
int args_handling(int argc, char *argv[], struct logger_init_data *logger_data);

void unlink_file(int sig)
{
    (void)sig;
    if (unlink(PATH_NAME_FOR_FTOK)) {
        perror("Can not create unlink");
    }
    exit (0);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, unlink_file);

    struct logger_init_data logger_data;
    memset(&logger_data, 0, sizeof(logger_data));
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

int init(int *msg_queue_id, struct logger_init_data *logger_data)
{
    if (!logger_data) {
        puts("Bad logger data struct");
        return -1;
    }
    if (!msg_queue_id) {
        puts("Bad msg_queue_id pointer");
        return -1;
    }

    FILE *fp = fopen(PATH_NAME_FOR_FTOK, "w");
    if (fp == NULL) {
        perror("Fail to open file for msg queue");
        return -1;
    }
    fclose(fp);
    puts("Create file to msg queue");

    //IPC message queue creating
    key_t key = ftok(PATH_NAME_FOR_FTOK, PROJECT_ID_FOR_FTOK);
    if (key == -1) {
        perror("Ошибка вызова функции ftok()");
        return -1;
    }
    *msg_queue_id = msgget(key, 0666 | IPC_CREAT);
    if (*msg_queue_id == -1) {
        perror("Ошибка вызова функции msgget()");
        return -1;
    }
    puts("Msg queue init success");


    unsigned work_dir_len = 256;
    char logger_dir[work_dir_len];
    memset(logger_dir, 0, work_dir_len);
    getcwd(logger_dir, work_dir_len);
    printf("Cur dir %s\n", logger_dir);
    if (logger_data->path[0] != '/') {
        strcat(logger_dir, "/");
    }
    strcat(logger_dir, logger_data->path);
    printf("Cur dir %s\n", logger_dir);
    if(mkdir(logger_dir, S_IRWXU) != 0) {
        if (errno != EEXIST) {
            LOG(LEVEL_ERROR, "Can not create dir %s", logger_dir);
            return -1;
        }
    }
    LOGGER_INIT_FILE(true, logger_data->level, logger_data->path);
    puts("logget init");
    
    return 0;
}

int deinit(int msg_queue_id)
{
    msgctl(msg_queue_id, 0, NULL);
    if (unlink(PATH_NAME_FOR_FTOK)) {
        perror("Can not create unlink");
    }
    //To Do: add logger deinit(if needed) or delete this todo
    return 0;
}

int work_cycle(int msg_queue_id)
{
    while (true) {
        message request_msg;
        puts("Wait msg");
        //get request messages from queue( messages with type equal to 1)
        int status = msgrcv(msg_queue_id, &request_msg, sizeof(request_msg) - sizeof(long), 1, 0);
        if (status == -1) {
            perror("MSGRCV");
            return (-1);
        }

        //if msg_type is no a MESSAGE_REQUEST, then abort msg processing
        if(request_msg.msg_type != MESSAGE_REQUEST){
            puts("Get not MESSAGE_REQUEST");
            continue; 
        }
        //if source of request is server, then abort msg processing
        if (request_msg.pid_from == 1) {
            puts("Get bad client type");
            continue; 
        }

        message reply_msg;
        reply_msg.type = request_msg.pid_from;
        reply_msg.pid_from = 1;//To Do: fix this magic number(it's a type of server procces in msg queue) here and in msgrcv invoke
        reply_msg.msg_type = MESSAGE_REPLY;

        if(handle_request(request_msg.payload.req_type, &reply_msg.payload.reply_msg) == -1) {
            printf("Ошибка функции обработчика\n");
            return -1;
        }
        if (msgsnd(msg_queue_id, &reply_msg, sizeof(reply_msg) - sizeof(long), IPC_NOWAIT) == -1) {
            perror("Ошибка отправки сообщения");
        }
    }     
}

int args_handling(int argc, char *argv[],struct logger_init_data *logger_data)
{
    if(argc == 1){
        logger_data->level = DEFAULT_LOGGER_LEVEL;
        strcat(logger_data->path, DEFAULT_LOGGER_PATH);
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
        strcat(logger_data->path, DEFAULT_LOGGER_PATH);
        return 0;
    }
    else if (argc == 3) {
        int tmp = 0;
        char tmp_string[MAX_LOGGER_PATH_LENGTH];
        memset(tmp_string, 0, sizeof(tmp_string));

        if (sscanf(argv[1], "%i",&tmp) == 0) {
            fprintf(stderr,"Ведено некорректное значение уровня\n");
            return -1;
        }

        if (sscanf(argv[2], "%s", tmp_string) == 0) {
            fprintf(stderr,"Ведено некорректное значение пути файлов логов \n");
            return -1;
        }

        logger_data->level = tmp;
        strcat(logger_data->path, tmp_string);
        return 0;
    }
    else if (argc >= 4) {
        fprintf(stderr,"Введено слишком много аргументов\n");
        return -1;
    }

    return 0;
}