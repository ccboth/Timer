#include <util/delay.h>
#include <avr/io.h>
#include "lcd-interface.h"
#include "macro.h"

#define swithE()        setBit(CONFIGURE_PORT, E); _delay_us(1); clearBit(CONFIGURE_PORT, E);

#define LINES           2
#define printChar       lcdPrintChar
#define printStr        lcdPrintStr

static char _positionX, _currentLine;
static void send(char message);

void lcdInit(void) {
    _positionX = 0;
    _currentLine = 0;
    // Включаем 4-битный режим
    lcdCommand(lCmd4bitMode);
    lcdCommand(lCmd2Commands);
    lcdCommand(lCmdClearDisp);
    lcdCommand(lCmdDisableCur);
    lcdCommand(lCmdAutoInc);
    lcdCommand(lCmdFirstLine);
}

static void send(char message) {
    uint8_t rsbuff = readBit(CONFIGURE_PORT, RS);
    
    CONFIGURE_PORT = getOldTetrade(message);
    setBitTo(CONFIGURE_PORT, RS, rsbuff);
    swithE();
    _delay_ms(1);

    CONFIGURE_PORT = getJunTetrade(message);
    setBitTo(CONFIGURE_PORT, RS, rsbuff);
    swithE();
    _delay_ms(1);
}

void lcdPrintChar(char symbol) {
    if (_positionX == 16 || symbol == '\n') 
    {
        _positionX = 0;
        _currentLine++;
        if (_currentLine == LINES) 
        {
            _currentLine = 0;
            lcdCommand(lCmdFirstLine);
        } 
        else 
        {
            lcdCommand(lCmdNextString);
        }
    }
    setBit(CONFIGURE_PORT, RS);
    if (symbol == '\n') return;
    send(symbol);
    _positionX++;
}

void lcdCommand(char cmd) {
    clearBit(CONFIGURE_PORT, RS);
    send(cmd);
    _delay_ms(3);
}

void lcdPrintStr(const char* str) {
    for (uint8_t i = 0;; i++)
    {
        if (str[i] == '\0') return;
        printChar(str[i]);
    }
}

void lcdSetPosition(char x, char line) {
    char cmd = 0;
    
    if (line == -1) line = _currentLine;

    if (line == 0) cmd = 0x80 | getJunTetrade(x);
    else cmd = 0xc0 | getJunTetrade(x);
    lcdCommand(cmd);
    _positionX = x;
    _currentLine = line;
}

void lcdClear(void) {
    lcdCommand(lCmdClearDisp);
    lcdSetPosition(0, 0);
}

// void lcdInit() 
// {
//     lcdCommand(lCmd4bitMode);
//     lcdCommand(lCmd2Commands);
//     lcdCommand(lCmdClearDisp);
//     lcdCommand(lCmdDisableCur);
//     lcdCommand(lCmdAutoInc);
//     lcdCommand(lCmdFirstLine);
// }

// void lcdSetSubsquense(char subsquesne) 
// {
//     for (char i = 0; i < 4; i++) 
//     {
//         setBitTo(DATAPORT, 0, readBit(subsquesne, i+i));
//         setBitTo(DATAPORT, 1, readBit(subsquesne, i+i+1));
//     }
// }

// void lcdCommand(char cmd) 
// {
//     RS = 0;
//     lcdSend(cmd);
//     _delay_ms(3);
// }

// void lcdPrintChar(char symbol) 
// {
//     if (positionX == 16 || symbol == '\n') 
//     {
//         positionX = 0;
//         currentLine++;
//         if (currentLine == LINES) 
//         {
//             currentLine = 0;
//             lcdCommand(lCmdFirstLine);
//         } 
//         else 
//         {
//             lcdCommand(lCmdNextString);
//         }
//     }
//     RS = 1;
//     if (symbol == '\n') return;
//     lcdSend(symbol);
//     positionX++;
// }

// void lcdSend(char message)
// {
//     PORTA = getOldTetrade(message);
//     swithE();
//     __delay_ms(1);
//     PORTA = getJunTetrade(message);
//     swithE();
//     __delay_ms(3);
// }

// void lcdPrintStr(const char* string)
// {
//     for (char i = 0;; i++)
//     {
//         if (string[i] == 0) return;
//         lcdPrintChar(string[i]);
//     }
// }
// void lcdSetPosition(char x, char line)
// {
//     char cmd = 0;
    
//     if (line == -1) line = currentLine;

//     if (line == 0) cmd = 0x80 | getJunTetrade(x);
//     else cmd = 0xc0 | getJunTetrade(x);
//     lcdCommand(cmd);
//     positionX = x;
//     currentLine = line;
// }

// void lcdClearDisplay()
// {
//     lcdCommand(lCmdClearDisp);
//     lcdSetPosition(0, 0);
// }

void lcdMoveCursorByX(char dx)
{
    lcdSetPosition(_positionX + dx, -1);
}

char lcdGetCursorPositionX(void)
{
    return _positionX;
}

char lcdGetCursorLine(void)
{
    return _currentLine;
}