#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>


#include "structs.h"

int num_of_tries = 0;

/*
* Инициализация клиента
* Невозможно подключиться к очереди - не существует очереди - ошибка - выход
*/
int init()
{
    key_t token = ftok(PATH_NAME_FOR_FTOK, PROJECT_ID_FOR_FTOK);
    if (token == -1)
    {
        perror("Ftok");
        printf("\nUnable to find common file to get token!\n");
        exit(-1);
    }

    int id = msgget(token, 0666);
    if (id == -1)
    {
        perror("MSGGET");
        printf("\nUnable to get queue id. Check if it is created and server is running and rerun the client app!\n");
        exit(-1);
    }
    return id; 
}

/*
* Парсинг ответа сервера
*/
void parse_reply(message* msg)
{
    printf("Got reply. Parsing...\n");
    reply* payload = &msg->payload.reply_msg;
    if (payload->reply_code == REPLY_BAD)
    {
        printf("\nError occured on a server side! Try again later\n");
    }
    else if (payload->reply_code == REPLY_SUCCESS || payload->reply_code == REPLY_CHECK_RESULT)
    {
        printf("\nTask successfully finished!\n");
        if(payload->reply_code == REPLY_CHECK_RESULT)
        {
            printf("The result of computing: %f\n", payload->result);
        }
    }
    else
    {
        printf("\nViolation of protocol by %ld\n", msg->pid_from);
    }
}

/*
* Парсинг ответа сервера
* ошибка в msgrcv приводит к закрытию, так как очередь закрыта
*/
void wait_for_response(int msg_id)
{
    printf("Waiting for reply...\n");
    message new = {0};
    int status = msgrcv(msg_id, &new, sizeof(message) - sizeof(long), getpid(), 0);
    if(status == -1)
    {
        perror("MSGRCV");
        printf("\nError occured while waiting for request... Shutting down client...\n");
        exit(-1);
    }
    
    switch (new.msg_type)
    {
        case MESSAGE_BAD:
            printf("\nSome error occured on server side! Try to send your request again!\n");
            break;
        case MESSAGE_REPLY:
            parse_reply(&new);
            break;
        default:
            printf("\nMSG queue was interupted by unexpected message. \n");
            wait_for_response(msg_id);
            break;
    }
}

/*
* Функция отправки сообщения
* Создает запрос, отправляет его. 
* Если произошла ошибка - повторяет еще 2 раза попытку отправить
* Если третья попытка неудачная - фатальная ошибка, клиент закрывается
*/
void send_msg(int msg_id, enum REQUEST_TYPE type)
{
    message new;
    new.type = 1;
    new.pid_from = getpid();
    new.msg_type = MESSAGE_REQUEST;
    new.payload.req_type = type;

    int status = msgsnd(msg_id, &new, sizeof(message) - sizeof(long), 0);
    if (status == -1)
    {
        perror("MSGSND");
        if(num_of_tries > 3)
        {
            printf("\nSome fatal error occured... Shutting down client...\n");
            exit(-1);
        }
        printf("\nError occured while sending. Try again...\n");
        num_of_tries++;
        return;
    }
    num_of_tries = 0;

    wait_for_response(msg_id);

}

/*
* Основной цикл работы и интерфейс
* Происходит выбор из доступных опций
*/
int work_cycle(int msg_id)
{
    int is_running = 1;
    while (is_running)
    {
        printf("Please choose number of option:\n");
        printf("Option 1: Example 1\n");
        printf("Option 2: Example 2\n");
        printf("Option 3: Example 3\n");
        printf("Option 4: Exit\n");

        int option = -1;
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            send_msg(msg_id, REQUEST_CALC_1);
            break;
        case 2:
            send_msg(msg_id, REQUEST_CALC_2);
            break;
        case 3:
            send_msg(msg_id, REQUEST_CALC_3);
            break;
        case 4:
            is_running = 0;
            (void) msg_id;
            break;
        default:
            printf("No such option! Input correct one:\n");
            break;
        }
    }
    return 0;
}



int main()
{
    printf("\nStarting client app...\n");
    int msg_id = init();
    printf("\nSuccessfully started the client app...\n");
    work_cycle(msg_id);
    printf("\nClosing client app...\n");
    return 0;
}
