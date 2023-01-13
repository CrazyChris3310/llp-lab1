#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

struct LinkedList;

struct LinkedList* createLinkedList();
void freeLinkedList(struct LinkedList* list);
void addNode(struct LinkedList* list, void* data);
void* removeNode(struct LinkedList* list, size_t index);
void* getNode(struct LinkedList* list, size_t index);
size_t getSize(struct LinkedList* list);

#endif