#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


// массив строк с названием временных зон
extern char *tzname[];

int main() {


    time_t now;
    struct tm *sp;

    // возврат текущего времени с 1января 1970 года
    (void)time(&now);

    setenv("TZ", "PST8PDT", 1);

    // PST - Pacific Standard Time (зимнее время)
    // 8 - смещение от UTC в часах для зимнего времени
    // PDT - Pacific Daylight Time (летнее время)
    // 1 - флаг перезаписи (если переменная уже существует, она будет заменена)

    tzset();

    printf("\nTime in California...\n%s", ctime(&now));

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
}