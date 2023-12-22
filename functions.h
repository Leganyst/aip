#include <stdlib.h>
#include <stdio.h>
#include "functions.c"

#ifndef H_FUNCTIONS
#define H_FUNCTIONS

int isTimeInInterval(const tDateTime *time, const tDateTime *start, const tDateTime *end);
float calculateSummCost(const tPhoneCall *call, const tRates *rate);
void readRates(tRates *rate, FILE *rates);
int checkParams(char* buffer, FILE* report);

#endif