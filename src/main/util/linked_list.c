#include <stdlib.h>
#include <stdbool.h>

struct Node {
    void* data;
    struct Node* next;
};

struct LinkedList {
    struct Node* head;
    struct Node* tail;
    size_t size;
    void(*freeFunction)(void* ptr);
};

static void defaultFreeNodeDataFunction(void* ptr) {
    free(ptr);
}

struct LinkedList* createLinkedList() {
    struct LinkedList* list = malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->freeFunction = defaultFreeNodeDataFunction;
    list->size = 0;
    return list;
}

struct LinkedList* createClearableLinkedList(void(*freeFunction)(void* ptr)) {
    struct LinkedList* list = createLinkedList();
    list->freeFunction = freeFunction;
    return list;
}

void clearList(struct LinkedList* list) {
    if (list == NULL) {
        return;
    }
    struct Node* node = list->head;
    while (node != NULL) {
        struct Node* next = node->next;
        list->freeFunction(node->data);
        free(node);
        node = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void freeLinkedList(struct LinkedList* list) {
    clearList(list);
    free(list);
}

void addNode(struct LinkedList* list, void* data) {
    if (list == NULL) {
        return;
    }
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
    if (list == NULL || index >= list->size) {
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
    if (list == NULL || index >= list->size) {
        return NULL;
    }
    struct Node* node = list->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    return node->data;
}

size_t getListSize(struct LinkedList* list) {
    return list->size;
}

struct ListIterator {
    struct LinkedList* list;
    struct Node* next;
};

struct ListIterator* createListIterator(struct LinkedList* list) {
    struct ListIterator* iterator = malloc(sizeof(struct ListIterator));
    iterator->list = list;
    if (list == NULL) {
        iterator->next = NULL;
    } else {
        iterator->next = list->head;
    }
    return iterator;
}

void freeListIterator(struct ListIterator* iterator) {
    free(iterator);
}

bool iteratorHasNext(struct ListIterator* iterator) {
    return iterator->next != NULL;
}

void* iteratorNext(struct ListIterator* iterator) {
    if (!iteratorHasNext(iterator)) {
        return NULL;
    }
    void* data = iterator->next->data;
    iterator->next = iterator->next->next;
    return data;
}
