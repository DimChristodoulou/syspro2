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

void deleteNode(checkListNode **head_ref, char *key){
    // Store head node 
    checkListNode *temp = *head_ref, *prev; 

    // If head node itself holds the key to be deleted 
    if (temp != NULL && !strcmp(temp->clientId, key)){
        *head_ref = temp->next;
        free(temp);
        return; 
    } 
  
    // Search for the key to be deleted, keep track of the 
    // previous node as we need to change 'prev->next' 
    while (temp != NULL && strcmp(temp->clientId, key)) {
        prev = temp; 
        temp = temp->next; 
    } 
  
    // If key was not present in linked list 
    if (temp == NULL) return; 
  
    // Unlink the node from linked list 
    prev->next = temp->next; 
    free(temp);  // Free memory 
} 

int countListItems(checkListNode *root){
    checkListNode *temp = root;
    int count = 0;
    while(temp!=NULL){
        count++;
        if(temp->next==NULL)
            break;
        temp = temp->next;
    }
    return count;
}