#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef struct linkedlist{
    char *string;
    struct linkedlist *next;
}LinkedList;
void addNode(LinkedList* ptr,char *string){
    while(ptr->next!=NULL){
        ptr=ptr->next;
    }
    ptr->string=malloc(strlen(string)+1);
    strcpy(ptr->string,string);
    ptr->next=malloc(sizeof(LinkedList));
    ptr->next->next=NULL;
    ptr->next->string=NULL;
}
int main(void){
    char buf[1024];
    LinkedList first;
    first.next=NULL;
    first.string=NULL;
    do{
        fgets(buf,1024,stdin);
        addNode(&first,buf);
    }while(*buf!='.');
    LinkedList *ptr=&first;
    while(ptr->string!= NULL && ptr->string[0] != '.'){
        printf("%s",ptr->string);
        ptr=ptr->next;
    }
    return 0;
}