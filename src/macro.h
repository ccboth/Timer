#define readBit(byte, bit) (byte >> bit) & 1
#define setBit(byte, bit) byte |= 1 << bit
#define clearBit(byte, bit) byte &= ~(1 << bit)
#define setBitTo(byte, bit, value) byte ^= (-(!!value) ^ byte) & (1 << bit)
#define getOldTetrade(byte) ((byte >> 4) & 0x0f)
#define getJunTetrade(byte) ((byte) & 0x0f)

#define false 0
#define true  1