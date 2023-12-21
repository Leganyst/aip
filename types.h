#ifndef H_TYPES
#define H_TYPES

/**
 * @brief Структура даты
 * 
 * @field day - день
 * @field month - месяц
 * @field year - год
 * @field hour - час
 * @field minute - минута
 * @field seconds - секунда
 */
typedef struct
{
    int day, month, year, hour, minute, seconds;
} tDateTime;

/**
 * @brief Структура звонка
 * 
 * @field phone_number - номер телефона
 * @field code_service - код услуги
 * @field duration_in_seconds - длительность звонка в секундах
 * @field call_time - время (дата) звонка
 */
typedef struct
{
    long long int phone_number;
    int code_service, duration_in_seconds;
    tDateTime call_time;
} tPhoneCall;


/**
 * @brief Структура тарифа
 * 
 * @field name - название (широкие/русские символы)
 * @field code - код
 * @field price - цена
 * @field time - временная привязка (широкие/русские символы)
 */
typedef struct
{
    wchar_t name[300];
    int code;
    float price;
    wchar_t time[300];
} tRates;

#endif