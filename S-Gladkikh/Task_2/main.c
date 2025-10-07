#include <stdio.h>
#include <time.h>
extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;
    (void) time( &now );
    sp = gmtime(&now);
    sp->tm_hour-=8;
    now = mktime(sp);
    printf("%s",ctime(&now));
    return 0;
 }
