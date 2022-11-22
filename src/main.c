#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "lcd-interface.h"
#include "keypad.h"
#include "macro.h"

#define TIMER_HIGHT 253
#define TIME_BOUDN	5999999ULL
#define USER_ADDR	0x0


#define getSeconds(seconds) (time)seconds*1000
#define getMinutes(minutes)	getSeconds(minutes)*60ULL

typedef unsigned char 			bool;
typedef unsigned long long		time;

/// @brief Итого, сколько сейчас милисекунд
time totalMilliseconds = 0;
/// @brief Настройки пользователя
time userSettingMilliseconds = 0;

// Если TRUE - Считаем обратно(Timer)
// Если FALSE - Считаем прямо(Secondsmer)
bool 			returnInvoice 	= 0;
/// @brief Работает ли счет секунд или нет
bool			isRunner		= false;
/// @brief Пропикивать ли
bool 			soundEnable 	= true;

/// @brief Вывести время на дисплей
void outTimeOnDisplay(void);
/// @brief Настройка таймера.
void timerConfigure(void);
/// @brief Обработка кодов клавиатуры
/// @param state Состояние на клавиатуре
void keypadHandler(uint8_t state);
/// @brief Проверка времени
void timeCheker(void);


ISR(TIMER0_OVF_vect) {
	// Сброс таймера
	TCNT0H = TIMER_HIGHT;
	// Если у нас обратный режим - убавляем милисекунду
	if (returnInvoice) {
		totalMilliseconds--;
		// Если время вышло
		if (!totalMilliseconds) {
			// Выключим прерывания
			clearBit(TCCR0B, CS00);
			// Отключим состояния
			isRunner = false;
			// Обновим время на дисплее
			outTimeOnDisplay();
			lcdCommand(lCmdEnableCur);

			// Пропикаем чем можно
			if (soundEnable) {
				setBit(PORTA, 7);
				_delay_ms(333);
				clearBit(PORTA, 7);
				_delay_ms(333);
				setBit(PORTA, 7);
				_delay_ms(333);
				clearBit(PORTA, 7);
			}
			return;
		}	
	}
	// Если нет - прибавляем
	else {
		totalMilliseconds++;
		// Если время достигло своей границы
		if (totalMilliseconds == TIME_BOUDN) {
			totalMilliseconds = 0;
			clearBit(TCCR0B, CS00);
			isRunner = false;
			outTimeOnDisplay();
			return;
		}
	};
}

int main(void)
{
	
	// Выделим память под состояние кнопки
	uint8_t keypadKeyState;
	// Место для записи последней позиции
	uint8_t cursorLastPosition;
	// Последний режим
	bool lastMode = !returnInvoice;
	// Порта "А" Полностью на выход
	DDRA = 255;
	// Порта "В" на выход только первые 3 вывода
	DDRB = 0b00000111;		
	// Включить прерывания
	sei();

	// Настройка таймера
	// Включаем нормальный режим
	TCCR0B = 0;
	TCCR0A = 0;
	// Устанавливаем предделитель
	setBit(TCCR0B, CS00);

	lcdInit();
	lcdSetPosition(0, 1);
	lcdPrintStr("-Time: ");

	lcdCommand(lCmdEnableCur);
	outTimeOnDisplay();

	// eeprom_read_block(&userSettingMilliseconds, USER_ADDR, sizeof(time));

	while (1) {
		if (lastMode != returnInvoice) {
			// Режим сменился
			// запомним позицию
			cursorLastPosition = lcdGetCursorPositionX();
			// Запомнили что режим теперь другой
			lastMode = returnInvoice;
			lcdSetPosition(0, 0);
			lcdPrintStr("--Mode:");
			if (returnInvoice == true) lcdPrintStr("reverse--");
			else lcdPrintStr("straigh--");
			// Вернули позицию
			lcdSetPosition(cursorLastPosition, -1);
		}
		// Обрабатываем кнопки
		keypadKeyState = keypadGetKey();
		// Если ничего не нажато
		if (keypadKeyState == 0) {
			// Если таймер запущен, обновим время
			if (isRunner == true) outTimeOnDisplay();
			continue;
		}
		// Включим пьезоизлучатель
		if (soundEnable) setBit(PORTA, 7);
		if (isRunner == true) {
			// Отключим прерывания по таймеру
			clearBit(TIMSK, TOIE0);
			// Обновим время на дисплее
			outTimeOnDisplay();
			// Защита от дребезга
			_delay_ms(100);
			// Выключим состояние
			isRunner = false;
			// выключим пьезоизлучатель
			clearBit(PORTA, 7);
			continue;
		}
		// Обработка кнопки
		keypadHandler(keypadKeyState);
		// Защита от дребезга
		_delay_ms(50);
		// выключим пьезоизлучатель
		clearBit(PORTA, 7);
	}
}

void keypadHandler(uint8_t state) {
	uint8_t cursorPosition = lcdGetCursorPositionX();

	if (state == KB_SwitchMode) returnInvoice = !returnInvoice;
	else if (state == KB_START) {
		// Сохраним настройки пользователя
		userSettingMilliseconds = totalMilliseconds;
		// настройка таймера
		TCNT0H = TIMER_HIGHT;
		// включение прерываний таймера
		setBit(TIMSK, TOIE0);
		// включение глобального состояния счета
		isRunner = true;
		// отключение курсора
		lcdCommand(lCmdDisableCur);
	} else if (state == KB_CursorLeft) {
		// Проверяем, в конце ли курсор
		if (cursorPosition == 6) return;
		// Проверяем границы курсора
		if (cursorPosition == 9 || cursorPosition == 12) lcdMoveCursorByX(-1);
		// Если нет, то двигаем курсор
		lcdMoveCursorByX(-1);

	} else if (state == KB_CursorRight) {
		// В конце ли курсор ?
		if (cursorPosition == 14) return;
		// Перед точкой или перед двоеточием ?
		if (cursorPosition == 10 || cursorPosition == 7) lcdMoveCursorByX(1);
		lcdMoveCursorByX(1);
	} else if (state == KB_IncCurrentNum || state == KB_DecCurrentNum) {
		// Место, куда время запишем
		time delta;
		// Думаем на какое значение будем прибавлять
		if (cursorPosition == 14) 		delta = 1;
		else if (cursorPosition == 13) 	delta = 10;
		else if (cursorPosition == 12) 	delta = 100;
		else if (cursorPosition == 10) 	delta = getSeconds(1);
		else if (cursorPosition == 9) 	delta = getSeconds(10);
		else if (cursorPosition == 7)	delta = getMinutes(1);
		else if (cursorPosition == 6)	delta = getMinutes(10);
		else 							delta = 0;
		// Проверка на безопасность
		// Если сумма больше границы, (Только для прибавки)
		if (((delta + totalMilliseconds) > TIME_BOUDN) && state == KB_IncCurrentNum) 
			// То разница будет просто до конца
			delta = TIME_BOUDN - totalMilliseconds;
		// Если же разница больше чем глобальное время
		else if ((delta > totalMilliseconds) && state == KB_DecCurrentNum)
			// то приравниваем их
			delta = totalMilliseconds;

		// Теперь уже решим что делать
		if (state == KB_IncCurrentNum) totalMilliseconds = totalMilliseconds + delta;
		else totalMilliseconds -= delta;
		// Выключить курсор, пока обновляем время
		lcdCommand(lCmdDisableCur);
		outTimeOnDisplay();
		// Включим обратно
		lcdCommand(lCmdEnableCur);
		userSettingMilliseconds = totalMilliseconds;
	}
	else if (state == KB_CLEAR) {
		totalMilliseconds = 0;
		lcdCommand(lCmdDisableCur);
		outTimeOnDisplay();
		lcdCommand(lCmdEnableCur);
	}
	else if (state == KB_RETURN) {
		totalMilliseconds = userSettingMilliseconds;
		lcdCommand(lCmdDisableCur);
		outTimeOnDisplay();
		lcdCommand(lCmdEnableCur);
	}

	else if (state == KB_SWITCH_ECHO) soundEnable = !soundEnable;
	
}

void outTimeOnDisplay(void) {
	uint8_t cursorLastPosition = lcdGetCursorPositionX();
	char buffer[10];
	uint32_t milliseconds = totalMilliseconds;
	uint32_t seconds = (milliseconds / 1000);
	uint32_t minutes = seconds / 60;
	// Вычитаем из милисекунд секунды
	milliseconds -= seconds * 1000;
	// Вычитаем из секунд минуты
	seconds -= minutes * 60;
	lcdSetPosition(6, 1);

	// Преобразуем минуты в строку
	sprintf(buffer, "%u", minutes);
	if (buffer[1] == '\0') {
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[2] = '\0';
	}
	lcdPrintStr(buffer);
	
	// Преобразуем секунды в строку
	sprintf(buffer, "%u", seconds);
	if (buffer[1] == '\0') {
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[2] = '\0';
	}
	lcdPrintChar(':');
	lcdPrintStr(buffer);

	// Преобразуем милисекунды в строку
	sprintf(buffer, "%u", milliseconds);
	if (buffer[1] == '\0') {
		buffer[1] = '0';
		buffer[2] = buffer[0];
		buffer[3] = '\0';
		buffer[0] = '0';
	} 
	else if (buffer[2] == '\0') {
		buffer[2] = buffer[1];
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[3] = '\0';
	}
	lcdPrintChar('.');
	lcdPrintStr(buffer);

	lcdPrintChar('-');
	lcdSetPosition(cursorLastPosition, 1);
}

void timerConfigure(void) {
	
}