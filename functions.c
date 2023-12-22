#include <wchar.h>
#include <stdio.h>
#include "types.h"

/**
 * @brief Проверяет, входит ли время в заданный интервал
 * 
 * @param time - время в формате год, месяц, день, час, минута, секунда (структура времени)
 * @param start - начальный интервал (структура времени)
 * @param end - конечный интервал (структура времени)
 * @return int - 1 - входит, 0 - не входит
 */
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

/**
 * @brief Вычисляет сумму стоимости звонка
 * 
 * @param call - информация о звонке (структура) 
 * @param rate - информация о тарифе (структура)
 * @return float - сумма стоимости
 */
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

/**
 * @brief Читает информацию о тарифах. Останавливается в том случае, когда среди перечней находит тариф "Междугородние звонки"
 * Непосредственное изменение переменной rate.
 * 
 * @param rate - информация о тарифе (структура)
 * @param rates - файл с информацией о тарифах
 */
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

/**
 * @brief Проверяет информацию о звонке. Если найдено - возвращает 1.
 * 
 * @param resultSumm - сумма стоимости (изменяется непосредственно)
 * @par     am countCalls - количество звонков (изменяется непосредственно)
 * @param bufferCallInfo - информация о звонке (строка с неотформатированными данными)
 * @param call - информация о звонке (структура для записи данных)
 * @param startTime - начальный интервал времени
 * @param endTime - конечный интервал времени
 * @return int - 0 или 1
 */
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
            return 0;
        }
        tRates rate;
        readRates(&rate, rates);

        if (call->code_service == rate.code)
        {
            *resultSumm += calculateSummCost(call, &rate);
            *countCalls += 1;
            isSearch = 1;
            // printf("Phone Number: %lld, Code Service: %d, Duration: %d seconds\n",
            // call.phone_number, call.code_service, call.duration_in_seconds);
        }
    }
    return isSearch;
}

/**
 * @brief Функция которая вызывается для каждого входного параметра в файле Param.ini. Является точкой входа по подсчету
 * кол-ва звонков и их суммарной стоимости
 * 
 * @param buffer - информация о тесте (введенное неотформатированное время)
 * @param report - файл для записи результата
 * @return int - 0 или 1 (1 в случае проблемы или ошибки)
 */
int checkParams(char* buffer, FILE* report) {
        {
        tDateTime start_time;
        tDateTime end_time;
        tPhoneCall call;
        float result_summ = 0;
        int count_calls = 0;
        int isSearch = 0;

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
            if (checkCall(&result_summ, &count_calls, buffer_call_info, &call, &start_time, &end_time)) {
                isSearch = 1;
            }  
            // printf("%d", isSearch);
        }
        fprintf(report, "%d, %2.2f, %02d.%02d.%02d %02d:%02d:%02d, %02d.%02d.%02d %02d:%02d:%02d\n", count_calls, result_summ,
                start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        fclose(info_services);
        if (!isSearch)
        {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Нет данных\n",
                    start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                    end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        }
        else
        {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Данные проанализированы. Результаты выведены в файл Report.txt. Кол-во звонков: %d\n",
                    start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                    end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds, count_calls);
        }
    }
    return 0;
}