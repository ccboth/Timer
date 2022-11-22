
// LCD commads
/// Move next string
#define lCmdNextString  0xc0
/// Move firstLine
#define lCmdFirstLine   0x80
/// Enable/disable automove cursor
#define lCmdAutoInc     0x06
/// off Cursor
#define lCmdDisableCur  0x0c
/// Use 2 commands
#define lCmd2Commands   0x28
/// Use 4bit mode
#define lCmd4bitMode    0x02
/// clearCursor
#define lCmdClearDisp   0x01
/// enable cursor
#define lCmdEnableCur   0x0e

#define CONFIGURE_PORT  PORTA
#define RS              5
#define E               4

// class LcdInterface {
//     private: PortState _dataPort, _e, _rs;


//     /// @brief Инициализация дисплея
//     /// @param dataPort порт для вывода данных
//     /// @param E Ножка строба
//     /// @param RS Ножка переключения между командами и текстом
//     // public: LcdInterface(PortState dataPort, PortState Es, PortState RSs);
//     /// @brief Вывод символа на текущую позицию
//     /// @param symbol Выводимый символ
//     public: void printChar(char symbol);
//     /// @brief Отправка команды LCD дисплею
//     /// @param cmd Команда
//     public: void sendCommand(char cmd);
//     /// @brief Вывод строки с текущей позиции без задержки
//     /// @param string Строка
//     public: void printStr(const char* string);
//     /// @brief Установка курсора в позицию. Чтобы позицию не изменить следует передать параметром -1
//     /// @param x Позиция в строке
//     /// @param line Строка
//     public: void setPosition(char x, char line);
//     /// @brief Очистка дисплея и возврат в самое начальное положение
//     public: void clear();
// };

/// @brief Задержка в милисекундах
/// @param ms 
// void delayMs(unsigned int ms);
/// @brief Инициализация дисплея
void lcdInit(void);
/// @brief Вывод символа на текущую позицию
/// @param symbol Выводимый символ
void lcdPrintChar(char symbol);
/// @brief Отправка команды LCD дисплею
/// @param cmd Команда
void lcdCommand(char cmd);
/// @brief Вывод строки с текущей позиции без задержки
/// @param string Строка
void lcdPrintStr(const char* string);
/// @brief Установка курсора в позицию. Чтобы позицию не изменить следует передать параметром -1
/// @param x Позиция в строке
/// @param line Строка
void lcdSetPosition(char x, char line);
/// @brief Очистка дисплея и возврат в самое начальное положение
void lcdClearDisplay(void);
/// @brief Перемещение курсора по оси Х
/// @param dx Смещение
void lcdMoveCursorByX(char dx);
/// @brief Получить позицию курусора по оси х
/// @return Позиция курсора по оси х
char lcdGetCursorPositionX(void);
/// @brief Получить позицию курсора на линии
/// @return номер линии
char lcdGetCursorLine(void);