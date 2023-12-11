#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "logger.h"
#include "structs.h"
#include "handler.h"

int init(int* msg_queue_id);
int work_cycle();
int deinit(int msg_queue_id);

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    int msg_queue_id = 0;
    init(&msg_queue_id);
    work_cycle();
    deinit(msg_queue_id);

    return 0;
}

int init(int* msg_queue_id)
{
    //IPC message queue creating
    key_t key = ftok(PATH_NAME_FOR_FTOK,PROJECT_ID_FOR_FTOK);
    *msg_queue_id = msgget(key, 0666|IPC_CREAT);
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

        struct message reply_msg;
        reply_msg.type = request_msg.pid_from;
        reply_msg.pid_from = 1;//To Do: fix this magic number(it's a type of server procces in msg queue) here and in msgrcv invoke

        handle_request(request_msg.msg_type, *reply_msg);

        msgsnd(msg_queue_id, *reply_msg, sizeof(reply_msg), 0);
    }
}