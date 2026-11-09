#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char *empty_str = "";
int64_t data[] = {4, 8, 15, 16, 23, 42};
char *int_format = "%ld ";
size_t DATA_LENGTH = sizeof(data) / sizeof(data[0]);

struct node_ {
    int64_t value;
    struct node_ *next;
};
typedef struct node_ Node;

void m(Node *list, void (*func)(int64_t));
void print_int(long x);


Node* add_element(int64_t value, Node *listPtr) {
    Node *new_node = malloc(sizeof(Node));
    if(!new_node) abort();

    new_node->value = value;
    new_node->next = listPtr;

    return new_node;
}


void m(Node *list, void (*func)(int64_t)) {
    if(list == NULL) return;

    func(list->value);

    m(list->next, func);

 }

void print_int(long x) {
    printf(int_format, x);
    fflush(stdout);
}

int p(long value) {
    return value & 1;
}

Node* f(Node* list, Node* acc, int (*pred)(long)) {
    if (!list) return acc;
    
    if (pred(list->value)) {
        // Если предикат истинен, добавляем элемент в аккумулятор (в начало)
        acc = add_element(list->value, acc);
    }
    
    // Рекурсивно обрабатываем хвост
    return f(list->next, acc, pred);
}

int main() {
    Node *list = NULL;

    //print_int(5);

    // Собираем список. Идем по массиву с конца, 
    // так как add_element добавляет в голову (prepend).
    for (int i = DATA_LENGTH - 1; i >= 0; i--) {
        list = add_element(data[i], list);
    }

    // Печать исходного списка
    m(list, print_int);
    puts("");
    
    // Фильтрация: оставляем только нечетные
    Node* filtered_list = f(list, NULL, p);
    
    // Печать отфильтрованного списка
    m(filtered_list, print_int);
    puts("");

    return EXIT_SUCCESS;

}
