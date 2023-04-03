#include <avr/io.h>

// Переключение режима счета (с таймера на секундомер и наоборот)
#define KB_SwitchMode           1
// Уменьшить число, на котором сейчас курсор
#define KB_DecCurrentNum        2
// Увеличить число, на котором сейчас курсор
#define KB_IncCurrentNum        3
// Курсор влево
#define KB_CursorLeft           4
// Курсор в право
#define KB_CursorRight          5
// Старт
#define KB_START                6
// Очистить
#define KB_CLEAR                7
// Вернутся к последнему числу
#define KB_RETURN               8
// Звуковое сопровождение при пропикивании/окончании отсчета
#define KB_SWITCH_ECHO          9

/// @brief Получает номер нажатой кнопки
/// @return Номер нажатой кнопки
uint8_t keypadGetKey(void);
uint8_t keypadOutState(void);
