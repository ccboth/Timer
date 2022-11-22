#include "keypad.h"
#include "macro.h"
#include <util/delay.h>
#include <avr/io.h>


uint8_t keypadGetKey(void) {
    uint8_t outState = 0;
    PORTB = 0b110;
    outState = keypadOutState();
    if (outState != 0) return outState + 6;

    PORTB = 0b101;
    outState = keypadOutState();
    if (outState != 0) return outState + 3;

    PORTB = 0b011;
    outState = keypadOutState();
    if (outState != 0) return outState;
    return outState;
}

uint8_t keypadOutState(void) {
    uint8_t bit;
    // Получаем первый бит, сравниваем с нулем
    bit = readBit(PINB, 3);
    // Если равен, то соответвенно возвращаем номер вывода
    if (bit == 0) return 1;
    // Если нет, то проделываем с 2мя оставшимися ту же операцию
    bit = readBit(PINB, 4);
    if (bit == 0) return 2;
    bit = readBit(PINB, 5);
    if (bit == 0) return 3;
    // Если же ни на одном выходе нет 0 - то просто вернем 0
    return 0;
}