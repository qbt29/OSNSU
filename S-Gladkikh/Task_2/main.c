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
    printf("%s",asctime(local));
    return 0;
 }
