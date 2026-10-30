#include <stdio.h>
#include <stdlib.h>

// Структура узла связного списка (16 байт на x86_64)
typedef struct Node {
    long value;
    struct Node* next;
} Node;

// Прототипы
Node* add_element(long value, Node* next);
void m(Node* list, void (*func)(long));
int p(long value);
Node* f(Node* list, Node* acc, int (*pred)(long));
void print_int(long value);

// Исходные данные
long data[] = {4, 8, 15, 16, 23, 42};
#define DATA_LENGTH (sizeof(data) / sizeof(data[0]))

void print_int(long value) {
    printf("%ld ", value);
    fflush(stdout);
}

int p(long value) {
    return value & 1;
}

Node* add_element(long value, Node* next) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) abort();
    
    new_node->value = value;
    new_node->next = next;
    return new_node;
}

void m(Node* list, void (*func)(long)) {
    if (!list) return;
    
    func(list->value);
    m(list->next, func);
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
    Node* list = NULL;
    
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
    
    return 0;
}