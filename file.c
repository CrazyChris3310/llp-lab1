#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

struct Animal {
    char* name;
    void(*sayWord)(void*);
};

void __animalSay(void* this) {
    struct Animal* animal = (struct Animal*)this;
    printf("%s: I am an animal\n", animal->name);
}

struct Animal* createAnimal(char* name) {
    struct Animal* animal = malloc(sizeof(struct Animal));
    animal->name = name;
    animal->sayWord = __animalSay;
    return animal;
}

void destroyAnimal(void* animal) {
    free(animal);
}

struct Cat {
    struct Animal animal;
};

void __catSay(void* this) {
    struct Animal* animal = (struct Animal*)this;
    printf("%s: I am a cat\n", animal->name);
}

struct Cat* createCat(char* name) {
    struct Cat* cat = malloc(sizeof(struct Cat));
    cat->animal.name = name;
    cat->animal.sayWord = __catSay;
    return cat;
}

struct Dog {
    struct Animal animal;
};

void __dogSay(void* this) {
    struct Animal* animal = (struct Animal*)this;
    printf("%s: I am a dog\n", animal->name);
}

struct Dog* createDog(char* name) {
    struct Dog* dog = malloc(sizeof(struct Dog));
    dog->animal.name = name;
    dog->animal.sayWord = __dogSay;
    return dog;
}

void animalSay(struct Animal* animal) {
    animal->sayWord(animal);
}

struct TempStruct {
    int a;
    int b;
    int c;
};

struct Animal* createRandomAnimal(char* name) {
    return (struct Animal*)createDog(name);
};

int main() {

    struct Animal* animal = createRandomAnimal("Margaret");
    animalSay(animal);

    // struct Animal* animal = createAnimal("Vasya");
    // struct Cat* cat = createCat("Gav");
    // struct Dog* dog = createDog("Sharik");

    // animalSay(animal);
    // animalSay((struct Animal*)cat);
    // animalSay((struct Animal*)dog);

    // destroyAnimal(animal);
    // destroyAnimal(cat);
    // destroyAnimal(dog);

    destroyAnimal(animal);

    printf("%d\n", sizeof(bool));
    printf("%d\n", sizeof(int32_t));

    return 0;
}