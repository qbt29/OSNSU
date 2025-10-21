#include <sys/types.h>
#include <stdio.h>
#include <envz.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];
 
int main()
{
    time_t now;
    struct tm *sp;
    (void) time( &now );
    printf("%s",ctime(&now));

    sp = localtime(&now);
    printf("Current timezone:\n");
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    
    putenv("TZ=America/Los_Angeles");
    tzset();
    // (void) time( &now );
    // printf("%s",ctime(&now));
    sp = localtime(&now);
    printf("California timezone:\n");
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);
    printf("Timezone: %s\n", *tzname);
    return 0;
}
