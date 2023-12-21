#ifndef H_TYPES
#define H_TYPES

typedef struct
{
    int day, month, year, hour, minute, seconds;
} tDateTime;

typedef struct
{
    long long int phone_number;
    int code_service, duration_in_seconds;
    tDateTime call_time;
} tPhoneCall;

typedef struct
{
    wchar_t name[300];
    int code;
    float price;
    wchar_t time[300];
} tRates;

#endif