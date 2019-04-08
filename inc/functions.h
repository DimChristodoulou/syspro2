#ifndef FUNCTIONS__H
#define FUNCTIONS__H

    typedef struct listNode{
        char clientId[256];
        int checked;
        struct listNode *next;
    } checkListNode;

    int childProcess();
    void pushToCheckList(checkListNode **root, char clientId[256]);
    void printList(checkListNode *root);

#endif