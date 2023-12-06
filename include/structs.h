#ifndef __STRUCTS_H__
#define __STRUCTS_H__

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

#endif // !__STRUCTS_H__
