#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];

int main(){
    time_t now;
    struct tm *sp;
    // летом pdt зимой pst
    setenv("TZ", "PST8", 1);
    tzset();
    time(&now);

    printf("%s", ctime(&now));

    sp = localtime(&now);
    printf("%02d/%02d/%d %02d:%02d %s\n",
        sp->tm_mon + 1,
        sp->tm_mday,
        sp->tm_year + 1900, // добавляем 1900 тк счет годов идет с 1900 года
        sp->tm_hour,
        sp->tm_min,
        tzname[sp->tm_isdst]);

    return 0;
}
