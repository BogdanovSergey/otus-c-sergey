    bits 64
    ; Объявляем внешние функции из libc, которые мы будем вызывать
    extern malloc, puts, printf, fflush, abort
    ; Точка входа для линковщика
    global main

    section   .data
empty_str: db 0x0                 ; Байт 0x00 — пустая C-строка (просто символ '\0')
int_format: db "%ld ", 0x0        ; Форматная строка: "%ld " + терминатор нуля
data: dq 4, 8, 15, 16, 23, 42     ; dq = define quadword (8 байт). Массив из 6 long'ов
data_length: equ ($-data) / 8     ; equ = константа времени ассемблирования.
                                  ; $ = адрес текущей позиции. ($-data) = размер в байтах.
                                  ; Делим на 8, получаем количество элементов = 6

    section   .text

;;; print_int proc
;;; void print_int(long value)
;;; Печатает число в stdout и сбрасывает буфер
print_int:
    push rbp                      ; Сохраняем старое значение rbp (кадр вызывающей функции)
    mov rbp, rsp                  ; rbp теперь указывает на верх нашего стекового кадра
    sub rsp, 16                   ; Выделяем 16 байт локального места.
                                  ; Зачем 16? ABI требует, чтобы перед call rsp был
                                  ; кратен 16. push rbp съел 8 байт, значит нужно ещё 8,
                                  ; но для выравнивания часто берут 16. Здесь они не
                                  ; используются — просто для выравнивания.

    mov rsi, rdi                  ; По ABI: rdi = 1-й аргумент (наше число).
                                  ; Копируем его в rsi — 2-й аргумент printf.
    mov rdi, int_format           ; rdi = 1-й аргумент printf — указатель на формат "%ld "
    xor rax, rax                  ; rax = 0. Для variadic-функций (printf) в rax передаётся
                                  ; количество аргументов, переданных через векторные
                                  ; регистры (xmm0-xmm7). У нас их нет — ставим 0.
    call printf                   ; Вызов printf(format="%ld ", value=число)

    xor rdi, rdi                  ; rdi = 0 — файловый дескриптор stdout
    call fflush                   ; Сбрасываем буфер stdout, чтобы вывод появился сразу

    mov rsp, rbp                  ; Восстанавливаем rsp — уничтожаем локальную область
    pop rbp                       ; Восстанавливаем rbp вызывающей функции
    ret                           ; Возврат. Результат (void) не важен.

;;; p proc
;;; int p(long value)
;;; Предикат: возвращает 1, если число нечетное, и 0, если четное
p:
    mov rax, rdi                  ; rdi = аргумент (число). Кладём его в rax — регистр
                                  ; возврата по ABI.
    and rax, 1                    ; Побитовое И с 1. Остаётся только младший бит.
                                  ; Если число нечётное — бит = 1, чётное — 0.
                                  ; Результат уже в rax — готов к возврату.
    ret                           ; Возврат. rax = 0 или 1.

;;; add_element proc
;;; Node* add_element(long value, Node* next)
;;; Создаёт новый узел списка и добавляет его в голову (prepend)
add_element:
    push rbp                      ; Сохраняем rbp вызывающего
    push rbx                      ; rbx — callee-saved регистр. Если мы его используем,
    push r14                      ; обязаны сохранить и восстановить. r14 — тоже.
    mov rbp, rsp                  ; Настраиваем кадр стека
    sub rsp, 16                   ; Выделяем 16 байт (для выравнивания стека)

    mov r14, rdi                  ; Сохраняем 1-й аргумент (value) в r14,
                                  ; т.к. дальше будем вызывать malloc и rdi затрётся.
    mov rbx, rsi                  ; Сохраняем 2-й аргумент (next) в rbx по той же причине.

    mov rdi, 16                   ; 1-й аргумент malloc — размер в байтах.
                                  ; sizeof(Node) = 8 (value) + 8 (next) = 16.
    call malloc                   ; Вызов malloc(16). Результат (указатель) в rax.
    test rax, rax                 ; Проверяем rax на 0 (NULL).
                                  ; test делает rax & rax, флаги ставит, но rax не меняет.
                                  ; Это быстрее и компактнее, чем cmp rax, 0.
    jz abort                      ; jump if zero — если malloc вернул NULL, крашим программу.

    mov [rax], r14                ; Записываем value по адресу, который вернул malloc.
                                  ; [rax] = разыменование указателя. Это node->value.
    mov [rax + 8], rbx            ; Записываем next со смещением 8 байт. Это node->next.

    mov rsp, rbp                  ; Эпилог: восстанавливаем rsp
    pop r14                       ; Восстанавливаем callee-saved регистры
    pop rbx                       ; ВАЖНО: порядок pop обратен порядку push!
    pop rbp
    ret                           ; Возврат. rax уже содержит указатель на новый узел.

;;; m proc
;;; void m(Node* list, void (*func)(long))
;;; Обходит список и применяет func к каждому значению (аналог map/foreach)
m:
    push rbp
    mov rbp, rsp
    sub rsp, 16                   ; Пролог + выравнивание стека

    test rdi, rdi                 ; rdi = 1-й аргумент (указатель на узел).
                                  ; Проверяем: список пуст? (NULL == 0)
    jz outm                       ; Если да — прыгаем на выход, ничего не делаем.

    push rbp                      ; Сохраняем регистры, которые будем использовать
    push rbx                      ; для хранения промежуточных значений между вызовами.

    mov rbx, rdi                  ; rbx = текущий узел (сохраняем, т.к. rdi затрётся)
    mov rbp, rsi                  ; rbp = указатель на функцию (тоже сохраняем)

    mov rdi, [rdi]                ; [rdi] = значение по адресу узла = node->value.
                                  ; Кладём его в rdi — 1-й аргумент для вызова func.
    call rsi                      ; Вызов функции по адресу в rsi.
                                  ; Да, в x86-64 можно вызывать по регистру!
                                  ; Это эквивалент func(node->value).

    mov rdi, [rbx + 8]            ; [rbx + 8] = node->next.
                                  ; Кладём в rdi — 1-й аргумент следующего вызова m.
    mov rsi, rbp                  ; rsi = указатель на функцию (2-й аргумент m).
    call m                        ; Рекурсивный вызов m(next, func).

    pop rbx                       ; Эпилог: восстановление регистров
    pop rbp

outm:
    mov rsp, rbp                  ; Восстановление кадра
    pop rbp
    ret                           ; Возврат (void)

;;; f proc
;;; Node* f(Node* list, Node* acc, int (*pred)(long))
;;; Фильтрует список. Если pred(value) == true, добавляет value в acc (в начало)
;;; Возвращает новый отфильтрованный список
f:
    mov rax, rsi                  ; rax = rsi = acc (аккумулятор, 2-й аргумент).
                                  ; Сразу кладём его в rax — он же будет результатом,
                                  ; если список пуст.

    test rdi, rdi                 ; rdi = list. Проверяем на NULL.
    jz outf                       ; Если список кончился — выходим. В rax уже acc.

    push rbx                      ; Сохраняем callee-saved регистры
    push r12                      ; Их много, т.к. нужно сохранить 3 аргумента
    push r13                      ; между рекурсивными вызовами.

    mov rbx, rdi                  ; rbx = current_node (list)
    mov r12, rsi                  ; r12 = acc
    mov r13, rdx                  ; r13 = pred (указатель на предикат)

    mov rdi, [rdi]                ; rdi = node->value (1-й аргумент предиката)
    call rdx                      ; Вызов pred(value). Результат в rax.
    test rax, rax                 ; Проверяем результат: 0 или не 0?
    jz z                          ; Если 0 (false) — прыгаем на метку z (false branch)

    ; === TRUE BRANCH: предикат вернул истину ===
    mov rdi, [rbx]                ; rdi = node->value (1-й аргумент add_element)
    mov rsi, r12                  ; rsi = acc (2-й аргумент add_element)
    call add_element              ; Вызов add_element(value, acc). Результат в rax.
    mov rsi, rax                  ; rsi = новый узел. Это новый аккумулятор.
                                  ; ВАЖНО: мы перезаписываем rsi, потому что дальше
                                  ; нам нужен обновлённый acc для рекурсивного вызова.
    jmp ff                        ; Прыгаем на общий рекурсивный шаг

z:  ; === FALSE BRANCH: предикат вернул ложь ===
    mov rsi, r12                  ; rsi = acc (не меняем его)

ff: ; === РЕКУРСИВНЫЙ ШАГ ===
    mov rdi, [rbx + 8]            ; rdi = node->next (1-й аргумент f)
    mov rdx, r13                  ; rdx = pred (3-й аргумент f)
    call f                        ; Рекурсивный вызов f(next, acc, pred).
                                  ; Результат (новая голова списка) в rax.

    pop r13                       ; Восстановление callee-saved регистров
    pop r12
    pop rbx

outf:
    ret                           ; Возврат. rax содержит голову отфильтрованного списка.

;;; main proc
;;; int main()
main:
    mov rbp, rsp                  ; Сохраняем rsp в rbp — удобно для отладчика (GDB)
    push rbx                      ; Сохраняем rbx (callee-saved)

    xor rax, rax                  ; rax = 0 = NULL. Это будущая голова списка.
                                  ; Список растится "справа налево" — добавляем в начало.
    mov rbx, data_length          ; rbx = 6 (счётчик обратного цикла)

adding_loop:
    ; Хитрая адресация: читаем массив С КОНЦА.
    ; rbx идёт от 6 до 1.
    ; Когда rbx=6: [data - 8 + 6*8] = [data + 40] = data[5] = 42
    ; Когда rbx=1: [data - 8 + 1*8] = [data + 0]  = data[0] = 4
    ; Зачем -8? Чтобы при rbx=1 попасть в data[0], а не в data[-1].
    ; Альтернатива: dec rbx ПЕРЕД чтением, но тогда усложняется условие выхода.
    mov rdi, [data - 8 + rbx * 8] ; rdi = data[rbx-1]
    mov rsi, rax                  ; rsi = текущая голова списка
    call add_element              ; Новая голова = add_element(значение, старая голова)
    dec rbx                       ; rbx--
    jnz adding_loop               ; jump if not zero — если rbx != 0, повторяем.
                                  ; После 6 итераций rbx=0, выходим.
                                  ; Итог: список в прямом порядке 4->8->15->16->23->42->NULL

    mov rbx, rax                  ; rbx = голова списка (сохраняем, т.к. rax затрётся)

    ; === ПЕЧАТЬ ИСХОДНОГО СПИСКА ===
    mov rdi, rax                  ; rdi = список (1-й аргумент m)
    mov rsi, print_int            ; rsi = адрес функции print_int (2-й аргумент m)
    call m                        ; m(list, print_int) — печатаем все элементы

    mov rdi, empty_str            ; rdi = указатель на пустую строку "\0"
    call puts                     ; puts выводит строку и добавляет '\n'.
                                  ; Т.к. строка пустая — выведется просто перевод строки.

    ; === ФИЛЬТРАЦИЯ СПИСКА ===
    mov rdx, p                    ; rdx = адрес функции p (3-й аргумент f)
    xor rsi, rsi                  ; rsi = 0 = NULL (2-й аргумент f — пустой аккумулятор)
    mov rdi, rbx                  ; rdi = голова списка (1-й аргумент f)
    call f                        ; f(list, NULL, p) — фильтруем. Результат в rax.
                                  ; ВАЖНО: filter добавляет в начало, поэтому порядок
                                  ; нечетных чисел будет ОБРАТНЫМ: 23->15->NULL

    ; === ПЕЧАТЬ ОТФИЛЬТРОВАННОГО СПИСКА ===
    mov rdi, rax                  ; rdi = отфильтрованный список
    mov rsi, print_int            ; rsi = print_int
    call m                        ; m(filtered_list, print_int)

    mov rdi, empty_str
    call puts                     ; Перевод строки в конце

    pop rbx                       ; Восстановление rbx
    xor rax, rax                  ; rax = 0 — код возврата main (EXIT_SUCCESS)
    ret                           ; Возврат в libc startup, который вызовет exit(rax)