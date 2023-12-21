#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <wchar.h>

#include "functions.h"
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

int main(void)
{
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    FILE *params = fopen("Param.ini", "r");
    FILE *report = fopen("Report.txt", "wt");

    if (params == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char buffer[100];

    while (fgets(buffer, 100, params)) {
        if (checkParams(buffer, report) == -1) {
            return -1;
        }
    }

    fclose(params);
    fclose(report);

    return 0;
}


