#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>

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


float calculateSummCost(const tPhoneCall *call) {
    int code = call->code_service;
    switch (code) {
        case 1:
            return 0.30 / 60 * call->duration_in_seconds;
        case 3:
            return 0.50 / 60 * call->duration_in_seconds;
        case 4:
            return 10.0 / 60 * call->duration_in_seconds;
        case 5:
            return 50.0 / 60 * call->duration_in_seconds;
        default:
        return 0.0;
    }
}


int main(void) {

    setlocale(LC_ALL, "ru");
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

            if (call.code_service != 2) {
                if (isTimeInInterval(&call.call_time, &start_time, &end_time)) {
                    result_summ += calculateSummCost(&call);
                    count_calls++;
                    
                    printf("Phone Number: %lld, Code Service: %d, Duration: %d seconds\n",
                           call.phone_number, call.code_service, call.duration_in_seconds);
                }
            }
        }
        fprintf(report, "%d, %2.2f, %02d.%02d.%02d %02d:%02d:%02d, %02d.%02d.%02d %02d:%02d:%02d\n", count_calls, result_summ,
                start_time.day, start_time.month, start_time.year, start_time.hour, start_time.minute, start_time.seconds,
                end_time.day, end_time.month, end_time.year, end_time.hour, end_time.minute, end_time.seconds);
        fclose(info_services);
    }


    fclose(params);
    fclose(report);


    return 0;
}
