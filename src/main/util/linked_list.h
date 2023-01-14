#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>

struct LinkedList;

struct ListIterator;

struct LinkedList* createLinkedList();
struct LinkedList* createClearableLinkedList(void(*freeFunction)(void* ptr));
void freeLinkedList(struct LinkedList* list);
void clearList(struct LinkedList* list);
void addNode(struct LinkedList* list, void* data);
void* removeNode(struct LinkedList* list, size_t index);
void* getNode(struct LinkedList* list, size_t index);
size_t getListSize(struct LinkedList* list);

struct ListIterator* createListIterator(struct LinkedList* list);
void freeListIterator(struct ListIterator* iterator);
bool iteratorHasNext(struct ListIterator* iterator);
void* iteratorNext(struct ListIterator* iterator);

#endif