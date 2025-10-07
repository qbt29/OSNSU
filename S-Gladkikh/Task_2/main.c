#include <stdio.h>
#include <time.h>
#include "stdlib.h"
int main()
{
    setenv("TZ","America/Tijuana",1);
    tzset();
    time_t now;
    time(&now);
    struct tm *local=localtime(&now);
    local->tm_hour-=local->tm_isdst;
    mktime(local);
    printf("%s",asctime(local));
    printf("%d",local->tm_isdst);
    return 0;
 }
