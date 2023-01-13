#include <stdlib.h>

struct Node {
    void* data;
    struct Node* next;
};

struct LinkedList {
    struct Node* head;
    struct Node* tail;
    size_t size;
};

struct LinkedList* createLinkedList() {
    struct LinkedList* list = malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void freeLinkedList(struct LinkedList* list) {
    struct Node* node = list->head;
    while (node != NULL) {
        struct Node* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

void addNode(struct LinkedList* list, void* data) {
    struct Node* node = malloc(sizeof(struct Node));
    node->data = data;
    node->next = NULL;
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void* removeNode(struct LinkedList* list, size_t index) {
    if (index >= list->size) {
        return NULL;
    }
    struct Node* node = list->head;
    struct Node* prev = NULL;
    for (size_t i = 0; i < index; i++) {
        prev = node;
        node = node->next;
    }
    if (prev == NULL) {
        list->head = node->next;
    } else {
        prev->next = node->next;
    }
    if (node == list->tail) {
        list->tail = prev;
    }
    list->size--;
    void* data = node->data;
    free(node);
    return data;
}

void* getNode(struct LinkedList* list, size_t index) {
    if (index >= list->size) {
        return NULL;
    }
    struct Node* node = list->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    return node->data;
}

size_t getSize(struct LinkedList* list) {
    return list->size;
}