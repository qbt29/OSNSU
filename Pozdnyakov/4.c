#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void remove_function_key_sequences(char *str) {
    char *src = str;
    char *dst = str;
    
    while (*src) {
        if (*src == 27 || (*src == '^' && *(src + 1) == '[')) {
            if (*src == 27) {
                src++;
            } else {
                src += 2;
            }
            
            while (*src && 
                   ((*src >= 'A' && *src <= 'Z') ||
                    (*src >= '0' && *src <= '9') ||
                    *src == '[' || *src == '~' || *src == ';' ||
                    *src == 'O' || *src == 'P' || *src == 'Q' || 
                    *src == 'R' || *src == 'S')) {
                src++;
            }
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}


int main(){

    char** arr=NULL;
    char str[1000];
    int count=0;

    while(fgets(str,1000,stdin))
    {

        remove_function_key_sequences(str);

        if(str[0]!='.')
        {
            char **temp = (char**)realloc(arr,(count+1)*sizeof(char*));

            if(temp==NULL){
                perror("Error realloc");
                break;
            }

            temp[count++]=strdup(str);

            arr=temp;
        }
        else
        {
            break;
        }
    }

    printf("\nВведённые строки:\n");

    for(int i=0;i<count;i++)
    {
        printf("%s",arr[i]);
        free(arr[i]);
    }

    free(arr);

    return 0;
}
