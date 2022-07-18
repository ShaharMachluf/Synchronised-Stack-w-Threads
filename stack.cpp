#include "part8.cpp"
#include "stack.hpp"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
// #include <stdlib.h>

pnode new_node(char* data){//create new node to put in the stack
    pnode curr = (pnode)malloc(sizeof(node));
    memset(curr->data, 0, 1024);
    strcpy(curr->data, data);
    curr->next = NULL;
    return curr;
}

void push(char* d, pnode *head){//put the new node in the begining of the linked list
    pnode curr = new_node(d);
    curr->next = *head;
    *head = curr;
}

void pop(pnode *head){//remove the head from the linked list
    pnode temp;
    temp = (*head);
    *head = (*head)->next;
    free(temp);
}

char* top(pnode *head){
    return((*head)->data);
}
