#include "../inc/functions.h"
#include "../inc/global.h"

/*
 * Pushes a @param client id to the checklist list.
 */
void pushToCheckList(checkListNode **root, char clientId[256]){
    checkListNode* newClient = (checkListNode*)malloc(sizeof(checkListNode));
    newClient->checked = 0;
    strcpy(newClient->clientId, clientId);
    newClient->next = *root; 
    *root = newClient;
}

void printList(checkListNode *root){
    checkListNode *temp = root;
    while(temp!=NULL){
        printf("%s CHECKED %d\n",temp->clientId, temp->checked);
        if(temp->next == NULL)
            break;
        temp = temp->next;        
    }
    
}

int childProcess(){

}