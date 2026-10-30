

int main(){
    int i = 8;
    int x = 0;
    if (i < 10) {
        x = 6;
    }
}

/*
push rbp                    ; Сохраняем указатель стека
mov rbp, rsp                ; Новый кадр
mov DWORD PTR [rbp-4], 8    ; i = 8
mov DWORD PTR [rbp-8], 0    ; x = 0
cmp DWORD PTR [rbp-4], 9    ; Сравниваем i с 9
jg .L2                      ; Если i > 9, прыгаем на метку L2
mov DWORD PTR [rbp-8], 6    ; x = 6 (если условие выполнилось)
.L2:                        ; Метка выхода из if
    mov eax, 0                  ; Возвращаем 0
    pop rbp                     ; Восстанавливаем стек
    ret                         ; Выход


; Условие i < 10 компилятор преобразует в i <= 9
; Используется инверсное условие для прыжка (jg - jump if greater)
; Если условие не выполняется - сразу прыгаем на выходa


; ZF (zero flag)     - если результат равен нулю
; SF (sign flag)     - если результат отрицательный
; OF (overflow flag) - если было переполнение
; CF (carry flag)    - если был перенос

; jg (jump if greater)           - прыжок если больше
; jge (jump if greater or equal) - прыжок если больше или равно
; jle (jump if less)             - прыжок если меньше или равно
; je (jump if equal)             - прыжок если равно
; jne (jump if not equal)        - прыжок если не равно
*/
