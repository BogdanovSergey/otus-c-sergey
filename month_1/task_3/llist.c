#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct node_ {
    int64_t value;
    struct node_ *next;
};
typedef struct node_ Node;

void print_elements(Node *listPtr);
void print_int(long x);
//void add_element();
// https://yandex.ru/video/preview/8097089350866890152
// Шаг №0 - связный список
//    v Создание списка
//    v Добавление элемента в начало
//    v Печать элементов
//    Добавление элемента в конец
//    Добавление элемента в середину (после опр значения)
//    Ф-я поиска (можно с доп текст данными)
//    Ф-я является ли связный список циклическим (круговым)*

//

Node* add_element(int64_t value, Node *listPtr) {
    Node *new_node = malloc(sizeof(Node));
    if(!new_node) return listPtr;

    new_node->value = value;
    new_node->next = listPtr;

    return new_node;
}

int main() {

    Node *listPtr = NULL;

    //print_int(5);

    listPtr = add_element(123, listPtr);
    listPtr = add_element(456, listPtr);
    listPtr = add_element(777, listPtr);

    //printf(int_format, listPtr->value);
    print_elements(listPtr);

    //printf(int_format, my_node.next->value);

    // struct Node *new_node

    return EXIT_SUCCESS;

}

void print_elements(Node *listPtr) {
    Node *el = listPtr;

    while (el != NULL) {
        printf("> %ld ", el->value);
        el = el->next;
    }

 }