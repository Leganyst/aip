#include <wchar.h>
#include <stdio.h>
#include "types.h"

int isTimeInInterval(const tDateTime *time, const tDateTime *start, const tDateTime *end)
{
    if (time->year < start->year || (time->year == start->year && time->month < start->month) ||
        (time->year == start->year && time->month == start->month && time->day < start->day) ||
        (time->year == start->year && time->month == start->month && time->day == start->day &&
         (time->hour < start->hour || (time->hour == start->hour && time->minute < start->minute) ||
          (time->hour == start->hour && time->minute == start->minute && time->seconds < start->seconds))))
    {
        return 0;
    }

    if (time->year > end->year || (time->year == end->year && time->month > end->month) ||
        (time->year == end->year && time->month == end->month && time->day > end->day) ||
        (time->year == end->year && time->month == end->month && time->day == end->day &&
         (time->hour > end->hour || (time->hour == end->hour && time->minute > end->minute) ||
          (time->hour == end->hour && time->minute == end->minute && time->seconds > end->seconds))))
    {
        return 0;
    }
    return 1;
}

float calculateSummCost(const tPhoneCall *call, const tRates *rate)
{
    int code = call->code_service;
    if (wcscmp(rate->time, L"мин") == 0)
    {
        return call->duration_in_seconds * (rate->price / 60);
    }
    else if (wcscmp(rate->time, L"сутки") == 0)
    {
        return call->duration_in_seconds * (rate->price / (24 * 3600));
    }
    else if (wcscmp(rate->time, L"месяц") == 0)
    {
        return call->duration_in_seconds * (rate->price / (30 * 24 * 3600));
    }
}

void readRates(tRates *rate, FILE *rates)
{
    wchar_t buffer_rate[300];
    while (fgetws(buffer_rate, 300, rates))
    {
        fwscanf(rates, L"%99[^,], %d, %f, %99[^\n]", rate->name, &rate->code, &rate->price, rate->time);
        if (wcscmp(rate->name, L"Междугородние звонки") == 0)
        {
            break;
        }
    }
}

int checkCall(float* resultSumm, int* countCalls, char* bufferCallInfo, tPhoneCall* call, tDateTime* startTime, tDateTime* endTime) {
    int isSearch = 0;
    sscanf(bufferCallInfo, "%lld, %d, %d.%d.%d %d:%d:%d, %d",
            &call->phone_number, &call->code_service,
            &call->call_time.day, &call->call_time.month, &call->call_time.year,
            &call->call_time.hour, &call->call_time.minute, &call->call_time.seconds,
            &call->duration_in_seconds);

    if (isTimeInInterval(&call->call_time, startTime, endTime))
    {
        FILE *rates = _wfopen(L"rates.txt", L"r, ccs=UTF-8");
        if (rates == NULL)
        {
            printf("Error opening file");
            return 1;
        }
        tRates rate;
        readRates(&rate, rates);

        if (call->code_service == rate.code)
        {
            *resultSumm += calculateSummCost(call, &rate);
            *countCalls++;
            isSearch = 0;
            // printf("Phone Number: %lld, Code Service: %d, Duration: %d seconds\n",
            // call.phone_number, call.code_service, call.duration_in_seconds);
        }
    }
}


int checkParams(char* buffer, FILE* report) {
        {
        tDateTime start_time;
        tDateTime end_time;
        tPhoneCall call;
        float result_summ = 0;
        int count_calls = 0;
        int is_search = 1;

        sscanf(buffer, "%d.%d.%d %d:%d:%d %d.%d.%d %d:%d:%d",
               &start_time.day, &start_time.month, &start_time.year,
               &start_time.hour, &start_time.minute, &start_time.seconds,
               &end_time.day, &end_time.month, &end_time.year,
               &end_time.hour, &end_time.minute, &end_time.seconds);

        FILE *info_services = fopen("info_services.txt", "r");
        if (info_services == NULL)
        {
            printf("Error opening file");
            return 1;
        }

        char buffer_call_info[100];
        while (fgets(buffer_call_info, 100, info_services))
        {
            is_search = checkCall(&result_summ, &count_calls, buffer_call_info, &call, &start_time, &end_time);
        }
        fprintf(report, "%d, %2.2f, %02d.%02d.%02d %02d:%02d:%02d, %02d.%02d.%02d %02d:%02d:%02d\n", count_calls, result_summ,
                start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        fclose(info_services);
        if (is_search)
        {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Нет данных\n",
                    start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                    end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        }
        else
        {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Данные проанализированы. Результаты выведены в файл Report.txt\n",
                    start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                    end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        }
    }
    return 0;
}