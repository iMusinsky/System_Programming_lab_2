#include "handler.h"

#include "logger.h"

#include <pthread.h> // Для потоков
#include <string.h> // Для memset

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Для usleep

#include <sys/stat.h>

#include <time.h>

#include <errno.h>

struct req_1
{
    unsigned idx;    //!< Индекс потока
    unsigned offset; //!< Сдвиг вектора, с которым работает поток
    float    *v1;    //!< Вектор №1
    float    *v2;    //!< Вектор №2
    float    res;    //!< Результат умножения потока
};
 
//функция потока
void *calc_multi_vectors(void *args)
{
    struct req_1 *inst = ((struct req_1*)args);
    unsigned offset = inst->offset * inst->idx;
    pthread_t thr_id = pthread_self();

    for(unsigned i = offset; i < offset + inst->offset; i++) {
        float local_res = inst->v1[i] * inst->v2[i];
        LOG(LEVEL_DEBUG, "pid: %ld, idx: %d, vec1[%d] * vec2[%d] = %1.2f * %1.2f = %1.2f", 
            thr_id, inst->idx, i, i, inst->v1[i], inst->v2[i], local_res);
        inst->res += local_res;
        usleep(10);
    }
    LOG(LEVEL_INFO, "pid: %ld, offset: %d, idx: %d, res: %1.2f", 
        thr_id, inst->offset, inst->idx, inst->res);

    return NULL;
}

int handle_req_1(struct reply *answer)
{
    const unsigned vector_len = 100;
    const unsigned thread_count = 4;

    float v1[vector_len];
    float v2[vector_len];
    for (unsigned i = 0; i < vector_len; ++i) {
        v1[i] = i;
        v2[i] = vector_len - i;
    }
 
    pthread_t thrs[thread_count];
    struct req_1 args[thread_count];
    memset(&thrs, 0, sizeof(thrs));
    memset(&args, 0, sizeof(args));


    for (unsigned i = 0; i < thread_count; ++i) {
        args[i].idx    = i;
        args[i].offset = vector_len / thread_count;
        args[i].v1     = v1;
        args[i].v2     = v2;
        args[i].res    = 0.f;
    }

    pthread_attr_t attrs;
    if(pthread_attr_init(&attrs) != 0) {
        perror("Cannot initialize attributes");
        return -1;
    };

    if(pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE) != 0) {
        perror("Error in setting attributes");
        return -1;
    }

    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_create(&thrs[i], &attrs, calc_multi_vectors, &args[i]) != 0) {
            perror("Cannot create a thread");
            return -1;
        }
    }

    pthread_attr_destroy(&attrs);
    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_join(thrs[i], NULL) != 0) {
            perror("Cannot join a thread");
            return -1;
        }
    }

    answer->req_type = REPLY_CHECK_RESULT;
    answer->result   = 0.f;
    for (unsigned i = 0; i < thread_count; ++i) {
        answer->result += args[i].res;
    }
    LOG(LEVEL_INFO, "Result: %g", answer->result);
 
    return 0;
}

struct req_2
{
    FILE           *fp;           //!< Файл куда нужно будет записать данные
    char           *data_to_rec;  //!< Строка которую будет писать поток
    unsigned        record_count; //!< Кол-во записей в файл
    pthread_mutex_t mutex;        //!< Общий мьютекс
};

void *write_to_file(void *args)
{
    struct req_2 *inst = (struct req_2*)args;

    pthread_t thr_id = pthread_self();
    LOG(LEVEL_INFO, "pid: %ld start", thr_id);

    for (size_t i = 0; i < inst->record_count; i++) {
        pthread_mutex_lock(&inst->mutex); 
        LOG(LEVEL_DEBUG, "pid: %ld start write to file, data: %s", thr_id, inst->data_to_rec);
        fwrite(inst->data_to_rec, strlen(inst->data_to_rec), 1, inst->fp);
        pthread_mutex_unlock(&inst->mutex);
        usleep(10);
    }
    
    return NULL;
}


int handle_req_2(struct reply *answer)
{
    const unsigned max_dir_len = 256;
    const unsigned thread_count = 10;

    char root_dir[max_dir_len];
    memset(root_dir, 0, max_dir_len);
    getcwd(root_dir, max_dir_len);

    strcat(root_dir, "/tmp");
    if(mkdir(root_dir, S_IRWXU) != 0) {
        if (errno != EEXIST) {
            LOG(LEVEL_ERROR, "Can not create dir %s", root_dir);
            return -1;
        }
    }

    strcat(root_dir, "/req_2");

    FILE *fp = fopen(root_dir, "w");
    if (fp == NULL) {
        LOG(LEVEL_ERROR, "Can not open file %s", root_dir);
        perror("fopen");
        return -1;
    }

    pthread_t thrs[thread_count];
    struct req_2 args[thread_count];
    memset(&thrs, 0, sizeof(thrs));
    memset(&args, 0, sizeof(args));

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    char temp[56] = {0};
    for (unsigned i = 0; i < thread_count; ++i) {
        args[i].fp           = fp;
        snprintf(temp, sizeof(temp), "tr(%d)", i);
        args[i].data_to_rec  = strdup(temp);
        args[i].record_count = thread_count;
        args[i].mutex        = mutex;
    }

    pthread_attr_t attrs;
    if(pthread_attr_init(&attrs) != 0) {
        perror("Cannot initialize attributes");
        return -1;
    };

    if(pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE) != 0) {
        perror("Error in setting attributes");
        return -1;
    }

    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_create(&thrs[i], &attrs, write_to_file, &args[i]) != 0) {
            perror("Cannot create a thread");
            return -1;
        }
    }

    pthread_attr_destroy(&attrs);
    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_join(thrs[i], NULL) != 0) {
            perror("Cannot join a thread");
            return -1;
        }
    }

    fclose(fp);
    for (unsigned i = 0; i < thread_count; ++i) {
        if (args[i].data_to_rec) {
            free(args[i].data_to_rec);
        }
    }

    answer->req_type = REPLY_SUCCESS;
    answer->result   = 0.f;


    return 0;
}

struct req_3
{
    FILE           *from;  //!< Файл откуда брать данные
    FILE           *to;    //!< Файл куда нужно будет записать данные
    pthread_mutex_t mutex; //!< Общий мьютекс для записи
};

void *read_and_write(void *args)
{
    struct req_3 *inst = (struct req_3*)args;

    pthread_t thr_id = pthread_self();
    LOG(LEVEL_INFO, "pid: %ld start", thr_id);

    const unsigned max_string_size = 256;

    while (!feof(inst->from)) {
        char tmp[max_string_size];
        memset(tmp, 0, max_string_size);
        fgets(tmp, max_string_size, inst->from);
        LOG(LEVEL_DEBUG, "pid: %ld get data from file: %s", thr_id, tmp);
        usleep(10);

        LOG(LEVEL_DEBUG, "pid: %ld Handle data", thr_id);

        pthread_mutex_lock(&inst->mutex); 
        LOG(LEVEL_DEBUG, "pid: %ld start write to file, write %s", thr_id, tmp);
        fputs(tmp, inst->to);
        pthread_mutex_unlock(&inst->mutex);
        usleep(100);
    }
    return NULL;
}

int handle_req_3(struct reply *answer)
{
    const unsigned max_dir_len = 256;
    const unsigned thread_count = 10;

    char root_dir[max_dir_len];
    memset(root_dir, 0, max_dir_len);
    getcwd(root_dir, max_dir_len);

    strcat(root_dir, "/tmp");
    if(mkdir(root_dir, S_IRWXU) != 0) {
        if (errno != EEXIST) {
            LOG(LEVEL_ERROR, "Can not create dir %s", root_dir);
            return -1;
        }
    }

    char file_to[max_dir_len];
    memset(file_to, 0, max_dir_len);
    strcat(file_to, root_dir);
    strcat(file_to, "/req_3_to");

    FILE *fp = fopen(file_to, "w");
    if (fp == NULL) {
        LOG(LEVEL_ERROR, "Can not open file %s", file_to);
        perror("Fopen");
        return -1;
    }
    LOG(LEVEL_INFO, "Threads record data to file %s", file_to);

    pthread_t thrs[thread_count];
    struct req_3 args[thread_count];
    memset(&thrs, 0, sizeof(thrs));
    memset(&args, 0, sizeof(args));

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);


    char file_from[max_dir_len][thread_count];
    for (size_t i = 0; i < thread_count; i++) {
        memset(file_from[i], 0, sizeof(file_from[i]));
    }
    
    FILE *ffrom[thread_count];
    for (unsigned i = 0; i < thread_count; i++) {
        char tmp[max_dir_len];
        memset(tmp, 0, max_dir_len);
        strcat(tmp, root_dir);
        strcat(tmp, "/req_3_data");
        snprintf(file_from[i], max_dir_len, "%s_thr_%d", tmp, i);
        ffrom[i] = fopen(file_from[i], "w");
        if (ffrom[i] == NULL) {
            LOG(LEVEL_ERROR, "Can not open file %s", file_from[i]);
            return -1;
        }
        LOG(LEVEL_DEBUG, "Thread %d use data from file %s", i, file_from[i]);

        int string_data_count = rand() % thread_count + 3;
        for (int j = 0; j < string_data_count; ++j) {
            char tmp[max_dir_len];
            memset(tmp, 0, max_dir_len);
            snprintf(tmp, max_dir_len, "%d) Data from thread %d\n", j, i);
            fputs(tmp, ffrom[i]);
        }
        fclose(ffrom[i]);
        ffrom[i] = fopen(file_from[i], "r");
        if (ffrom[i] == NULL) {
            LOG(LEVEL_ERROR, "Can not open file %s", file_from[i]);
            return -1;
        }
    }

    for (unsigned i = 0; i < thread_count; ++i) {
        args[i].to    = fp;
        args[i].from  = ffrom[i];
        args[i].mutex = mutex;
    }

    pthread_attr_t attrs;
    if(pthread_attr_init(&attrs) != 0) {
        perror("Cannot initialize attributes");
        return -1;
    };

    if(pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE) != 0) {
        perror("Error in setting attributes");
        return -1;
    }

    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_create(&thrs[i], &attrs, read_and_write, &args[i]) != 0) {
            perror("Cannot create a thread");
            return -1;
        }
    }

    pthread_attr_destroy(&attrs);
    for(unsigned i = 0; i < thread_count; ++i) {
        if(pthread_join(thrs[i], NULL) != 0) {
            perror("Cannot join a thread");
            return -1;
        }
    }

    for (unsigned i = 0; i < thread_count; ++i) {
        if (ffrom[i]) {
            fclose(ffrom[i]);
        }
    }
    if (fp) {
        fclose(fp);
    }

    answer->req_type = REPLY_SUCCESS;
    answer->result   = 0.f;


    return 0;
}


int handle_request(int req_type, struct reply *answer)
{
    if (!answer) {
        return -1;
    }
    if (req_type < REQUEST_CALC_1 || req_type >= REQUEST_N) {
        return -1;
    }

    int err = 0;

    LOG(LEVEL_INFO, "Start handle request %d", req_type);
    switch (req_type)
    {
    case REQUEST_CALC_1:
        err = handle_req_1(answer);
        break;
    case REQUEST_CALC_2:
        err = handle_req_2(answer);
        break;
    case REQUEST_CALC_3:
        err = handle_req_3(answer);
        break;
    
    default:
        break;
    }
    LOG(LEVEL_INFO, "End handle request %d", req_type);

    if (err) {
        answer->req_type = REPLY_BAD;
    }
    answer->req_type = req_type;

    return err;
}
