#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <wchar.h>

/*
В файле clients.txt содержатся записи о клиентах. Первым идут ФИО клиента, через запятую номер, 
дата заключения договора, дата окончания, размер задолженности, допустимый кредит. Все разделены запятыми

В файле rates.txt содержится информация о тарифах. Включаются именование услуги, её код, тариф (в рублях), 
временной интервал измерения (минуты, сутки, месяцы. Если временной привязки нет - ставится шарп #).
Наименование     | код | цена | за которое время |
Cвязь внутри сети|   1 | 0.30 |     мин          |

В файле info_services.txt содержится информация о предоставленных услугах. 
Номер телефона | код услуги | дата | длительность использования в секундах |

*/
typedef struct {
    int day, month, year, hour, minute, seconds;
} tDateTime;

typedef struct {
    long long int phone_number;
    int code_service, duration_in_seconds;
    tDateTime call_time;
} tPhoneCall;


typedef struct {
    wchar_t name[300];
    int code;
    float price;
    wchar_t time[300];
} tRates;

int isTimeInInterval(const tDateTime *time, const tDateTime *start, const tDateTime *end) {
    if (time->year < start->year || (time->year == start->year && time->month < start->month) ||
        (time->year == start->year && time->month == start->month && time->day < start->day) ||
        (time->year == start->year && time->month == start->month && time->day == start->day &&
        (time->hour < start->hour || (time->hour == start->hour && time->minute < start->minute) ||
        (time->hour == start->hour && time->minute == start->minute && time->seconds < start->seconds)))) {
        return 0;
    }

    if (time->year > end->year || (time->year == end->year && time->month > end->month) ||
        (time->year == end->year && time->month == end->month && time->day > end->day) ||
        (time->year == end->year && time->month == end->month && time->day == end->day &&
        (time->hour > end->hour || (time->hour == end->hour && time->minute > end->minute) ||
        (time->hour == end->hour && time->minute == end->minute && time->seconds > end->seconds)))) {
        return 0;
    }

    return 1;
}


float calculateSummCost(const tPhoneCall *call, const tRates *rate) {
  int code = call->code_service;
  if (wcscmp(rate->time, L"мин") == 0){
    return call->duration_in_seconds * (rate->price / 60);
  } else if (wcscmp(rate->time, L"сутки") == 0) {
    return call->duration_in_seconds * (rate->price / (24 * 3600));
  } else if (wcscmp(rate->time, L"месяц") == 0) {
    return call->duration_in_seconds * (rate->price / (30 * 24 * 3600));
  }
}


void readRates(tRates *rate, FILE *rates) {
    wchar_t buffer_rate[300];
    while(fgetws(buffer_rate, 300, rates)) {
        fwscanf(rates, L"%99[^,], %d, %f, %99[^\n]", rate->name, &rate->code, &rate->price, rate->time);
        // fwscanf(rates, L"%ls, %d, %f, %ls", rate->name, &rate->code, &rate->price, rate->time);
        if (wcscmp(rate->name, L"Междугородние звонки") == 0) {
            break;
        }
    }
}


int main(void) {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    FILE *params = fopen("Param.ini", "r");
    FILE *report = fopen("Report.txt", "wt");

    if (params == NULL) {
        printf("Error opening file");
        return 1;
    }

    char buffer[100];
 
    while (fgets(buffer, 100, params)) {
        
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
        if (info_services == NULL) {
            printf("Error opening file");
            return 1;
        }

        char buffer_call_info[100];
        while (fgets(buffer_call_info, 100, info_services)) {
            sscanf(buffer_call_info, "%lld, %d, %d.%d.%d %d:%d:%d, %d",
                   &call.phone_number, &call.code_service,
                   &call.call_time.day, &call.call_time.month, &call.call_time.year,
                   &call.call_time.hour, &call.call_time.minute, &call.call_time.seconds,
                   &call.duration_in_seconds);

            if (isTimeInInterval(&call.call_time, &start_time, &end_time)) {
                FILE *rates = _wfopen(L"rates.txt", L"r, ccs=UTF-8");
                if (rates == NULL) {
                    printf("Error opening file");
                    return 1;
                }
                tRates rate;
                readRates(&rate, rates);

                if (call.code_service == rate.code) {
                    result_summ += calculateSummCost(&call, &rate);
                    count_calls++;
                    is_search = 0;
                    // printf("Phone Number: %lld, Code Service: %d, Duration: %d seconds\n",
                            // call.phone_number, call.code_service, call.duration_in_seconds);
                }
            }

        }
        fprintf(report, "%d, %2.2f, %02d.%02d.%02d %02d:%02d:%02d, %02d.%02d.%02d %02d:%02d:%02d\n", count_calls, result_summ,
                start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        fclose(info_services);
        if (is_search) {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Нет данных\n", 
            start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
            end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        } else {
            wprintf(L"%02d.%02d.%02d %02d:%02d:%02d - %02d.%02d.%02d %02d:%02d:%02d - Данные проанализированы. Результаты выведены в файл Report.txt\n",
            start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
            end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        }

    }


    fclose(params);
    fclose(report);


    return 0;
}
