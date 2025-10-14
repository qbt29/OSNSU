#include <stdio.h>
#include <unistd.h>

void print_id_and_open_file(){

    printf("ID = %d\nUID = %d\n",getuid(),geteuid());

    FILE *file = fopen("newfile.txt","r");

    if(file==NULL)
    {
        perror("Error open file");
    }
    else
    {
        fclose(file);
    }
}


int main(){

    print_id_and_open_file();    

    setuid(getuid());

    print_id_and_open_file();

    return 0;
}
