#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


extern char *tzname[];

int main() {

    time_t now;
    struct tm *sp;

    // возврат текущего времени
    (void)time(&now);

    // "Pacific Standard Time" + "8" (20 - 12) + "Pacific Daylight Time" (летнее время)
    setenv("TZ", "PST8PDT", 1);
    tzset();

    printf("\nTime in California...\n%s", ctime(&now));

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
}