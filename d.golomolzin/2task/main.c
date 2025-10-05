#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>


// массив строк с названием временных зон
extern char *tzname[];

int main() {

    // до tzset. Значения по умолчанию
    // tzname = [GMT][GMT]
    
    // хранит время в секундах с 1 января 1970 года
    time_t now;

    // объявление структуры
    struct tm *sp;
    /*
    struct tm {
        int tm_sec;    // Секунды [0-59]
        int tm_min;    // Минуты [0-59]
        int tm_hour;   // Часы [0-23]
        int tm_mday;   // День месяца [1-31]
        int tm_mon;    // Месяц [0-11]
        int tm_year;   // Год с 1900
        int tm_wday;   // День недели [0-6]
        int tm_yday;   // День года [0-365]
        int tm_isdst;  // Флаг летнего времени
    };
    */
    
    // возврат текущего времени в секундах
    (void)time(&now);
    
    // установка значение переменной окружения
    setenv("TZ", "PST8PDT", 1);
    // TZ  - переменная окружения
    // PST - Pacific Standard Time (стандартное время)
    // 8   - смещение западнее от UTC в часах (UTC-8)
    // PDT - Pacific Dailight Time (название летнего времени)
    // 1   - флаг перезаписи (если переменная уже существует, она будет заменена)
    
    // парсит TZ и изменение времени
    tzset();

    // после tzset
    // tzname = [PST][PDT]
    
    printf("\nTime in California...\n%s", ctime(&now));

    // преобразует время в локальную временную зону
    sp = localtime(&now);

    printf("%d/%d/%02d %d:%02d %s\n\n",
        sp->tm_mon + 1,         // месяц: +1 потому что в структуре месяц от 0 до 11
        sp->tm_mday,            // день месяца [1-31]
        sp->tm_year,            // год начиная с 1900 (2024 год = 1900 + 124)
        sp->tm_hour,            // часы [0-23]
        sp->tm_min,             // минуты [0-59]
        tzname[sp->tm_isdst]);  // название временной зоны

    exit(0);
}