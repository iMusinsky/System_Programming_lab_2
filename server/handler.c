#include "handler.h"

#include "logger.h"

int handle_request(int req_type, struct reply *answer)
{
    if (!answer) {
        return -1;
    }
    if (req_type < REQUEST_CALC_1 || req_type >= REQUEST_N) {
        return -1;
    }

    (void)req_type;
    (void)answer;
    return 0;
}
