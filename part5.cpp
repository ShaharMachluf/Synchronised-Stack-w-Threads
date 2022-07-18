#include <stdio.h>
#include <string.h>
#include "stack.cpp"
int main(){
    pnode head = NULL;
    int flag = 1, i=0;
    char command[1024];
    char *data;
    while(flag){
        memset(data, 0, 1024);
        memset(command, 0, 1024);
        fgets(data, 1024, stdin);
        i=0;
        while (data[i]!='\0' && data[i]!= ' ')
        {
            command[i] = data[i];
            i++;
        }
        if(strcmp(command, "PUSH")==0){
            data+=(i+1);//get to the data part
            push(data, &head);
            continue;
        }
        else if(strcmp(command, "POP\n")==0){
            if(head == NULL){
                printf("ERROR: stack is empty\n");
                continue;
            }
            pop(&head);
            continue;
        }
        else if(strcmp(command, "TOP\n")==0){
            if(head == NULL){
                printf("ERROR: stack is empty\n");
                continue;
            }
            memset(data, 0, 1024);
            data = top(&head);
            printf("OUTPUT: %s", data);
            continue;
        }
        else{
            flag = 0;
            break;
        }
    }
    return 0;
}

