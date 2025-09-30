#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp, *gmt;

    // Устанавливаем часовой пояс: Калифорния (Pacific Time)
    setenv("TZ", "America/Los_Angeles", 1);
    tzset();

    time(&now);

    // Время в Pacific (локальное для заданного TZ)
    printf("ctime (Pacific): %s", ctime(&now));

    sp = localtime(&now);
    printf("localtime (Pacific): %d/%d/%02d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday,
           sp->tm_year + 1900, sp->tm_hour,
           sp->tm_min, tzname[sp->tm_isdst]);

    // Время в UTC (для сравнения)
    gmt = gmtime(&now);
    printf("gmtime (UTC): %d/%d/%02d %d:%02d\n",
           gmt->tm_mon + 1, gmt->tm_mday,
           gmt->tm_year + 1900, gmt->tm_hour, gmt->tm_min);

    return 0;
}
