#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

int main (int argc, char* argv[]) {
    
    if (argc != 2) {
        printf("Запуск: %s <path>\n", argv[0]);
        return 1;
    }

    char* rights[8] = {
        "---", "--x", "-w-","-wx",
        "r--", "r-x", "rw-", "rwx"
    };
    
    for (int i = 1; i != argc; i++) {

        char* filename = strrchr(argv[i], '/'); // последний каталог (/<path>)
        if (filename == NULL) {
            filename = argv[i];
        }
        else 
            filename++; //уходим от первого символа (/)
    
        struct stat st;
        if (stat(argv[i], &st) == -1) {
            perror("stat");
            return 1;
        }
        else {
    
            int own_r   = (st.st_mode >> 6) & 0b111;
            int group_r = (st.st_mode >> 3) & 0b111;
            int user_r  = st.st_mode & 0b111;

            // первый символ 'd' - директория (каталог), '-' - простой файл, иначе '?'
            char fst_bit = S_ISDIR(st.st_mode) ? 'd' : ( S_ISREG(st.st_mode) ? '-' : '?'); // d - каталог, - - файл, иначе - ?
    
            struct passwd* pswd = getpwuid(st.st_uid);

            struct group* grp = getgrgid(st.st_gid);
    
            char time_buf[13];
            struct tm* timeinfo = localtime(&st.st_mtime);
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", timeinfo);
    
            char* display_name = (strcmp(filename, ".") == 0) ? "." : filename;

            printf("%c%s%s%s %3ld %-12s %-12s %7ld %12s %s\n",
                fst_bit,                                        // тип
                rights[own_r],
                rights[group_r],
                rights[user_r],                                 // права
                (long)st.st_nlink,
                pswd->pw_name,                                  // владелец
                grp->gr_name,                                   // группа
                st.st_size,
                time_buf,
                display_name
            );
        }
    }
    return 0;
}