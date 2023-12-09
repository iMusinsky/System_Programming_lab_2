// #include <stdio.h>

// #include "structs.h"
// #include "handler.h"

// int init();
// int work_cycle();
// int deinit();

// int main(int argc, char *argv[])
// {
//     (void)argc;
//     (void)argv;
//     return 0;
// }

#include "logger.h"
#include <stdlib.h>

int main()
{
    system("pwd");
    LOGGER_INIT_FILE(true, LEVEL_DEBUG, "/home/daniil/Desktop/System_Programming_lab_2/server");
    for (int i = 0; i < 100; i++)
        LOG_INFO("Hi, %d", i);
    getchar();
    return 0;
}