#include <time.h>
#include <stdio.h>
#include <string>

#include "date.h"

int chartimer;

//This returns the date as a c string
char *GetDate(int Format)
{
    char *Ret = new char[24];

    time_t Raw;
    time(&Raw);
    tm *Time = localtime(&Raw);

    switch(Format)
    {
        case FORMAT_YDM:
        {
            sprintf(Ret, "%04d-%02d-%02d_%02d-%02d", Time->tm_year + 1900, Time->tm_mday, Time->tm_mon + 1, Time->tm_hour, Time->tm_min);
            break;
        }
        case FORMAT_YMD:
        {
            sprintf(Ret, "%04d-%02d-%02d_%02d-%02d", Time->tm_year + 1900, Time->tm_mon + 1, Time->tm_mday, Time->tm_hour, Time->tm_min);
            break;
        }
    }

    return Ret;
}

//this returns the time for the top bar
std::string RetTime()
{
    std::string Ret;

    time_t Raw;
    time(&Raw);
    tm *Time = localtime(&Raw);

    char Tmp[32];
	
	chartimer++;
	if (chartimer >= 120*2)
		chartimer = 0;

	if (chartimer >= 120)
		sprintf(Tmp, "%02d %02d", Time->tm_hour, Time->tm_min);
	else
		sprintf(Tmp, "%02d:%02d", Time->tm_hour, Time->tm_min);

    Ret = Tmp;

    return Ret;
}
