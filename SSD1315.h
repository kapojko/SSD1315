#ifndef SSD1315_H
#define SSD1315_H

// based on https://github.com/kefir/ssd1315/blob/master/ssd1315.c

#include <stdint.h>
#include <stdbool.h>

// I2C address
#define SSD1315_I2C_ADDR_DEF 0x3C
#define SSD1315_I2C_ADDR_ALTERNATE 0x3D // depends on D/C# pin

#define SSD1315_I2C_BUFFER_SIZE 32

// Display size
#define SSD1315_BITS_PER_PAGE 8
#define SSD1315_PAGE_COUNT  8
#define SSD1315_COL_COUNT 128
#define SSD1315_ROW_COUNT (SSD1315_PAGE_COUNT * SSD1315_BITS_PER_PAGE)

#define SSD1315_BUFF_SIZE (SSD1315_COL_COUNT * SSD1315_PAGE_COUNT)

// Error codes
#define SSD1315_ERR_I2C -1
#define SSD1315_ERR_INVALID_ROW -2
#define SSD1315_ERR_INVALID_COL -3
#define SSD1315_ERR_INVALID_LENGTH -4

typedef enum {
    SSD1315_MEM_MODE_HORIZ = 0x00,
    SSD1315_MEM_MODE_VERT = 0x01,
    SSD1315_MEM_MODE_PAGE = 0x02,
} SSD1315_MemMode;

typedef enum {
    SSD1315_VCOM_LEVEL065 = 0x00,
    SSD1315_VCOM_LEVEL071 = 0x10,
    SSD1315_VCOM_LEVEL077 = 0x20,
    SSD1315_VCOM_LEVEL083 = 0x30,
} SSD1315_VcomLevel;

typedef enum {
    SSD1315_CHARGE_PUMP_DISABLE = 0x10,
    SSD1315_CHARGE_PUMP_MODE7V = 0x14,
    SSD1315_CHARGE_PUMP_MODE8_5V = 0x94,
    SSD1315_CHARGE_PUMP_MODE9V = 0x95,
} SSD1315_ChargePumpMode;

struct SSD1315_Platform {
    int (*i2cWrite)(uint8_t addr_7bit, const uint8_t *data, uint8_t length, uint8_t wait, uint8_t send_stop);
    void (*debugPrint)(const char *fmt, ...);
    
    uint8_t i2cAddr;
};

void SSD1315_Init(struct SSD1315_Platform *platform);

void SSD1315_DefInit(bool xyFlip);

void SSD1315_DisplayOn(bool enable);
void SSD1315_EntireDisplayOn(bool enable);
void SSD1315_ColumnStartAddressSet(uint8_t addr);
void SSD1315_DisplayStartLineSet(uint8_t line);
void SSD1315_MemoryModeSet(SSD1315_MemMode mode);
void SSD1315_PageStartAddressSet(uint8_t addr);
void SSD1315_ContrastSet(uint8_t contrast);
void SSD1315_SegmentRemap(bool remap);
void SSD1315_MuxRatioSet(uint8_t mux);
void SSD1315_ComScanInvert(bool invert);
void SSD1315_DisplayOffsetSet(uint8_t offset);
void SSD1315_ClockSet(uint8_t freq, uint8_t divider);
void SSD1315_PrechargePeriodSet(uint8_t period);
void SSD1315_ComPinsSet(bool alternative, bool left_right_remap);
void SSD1315_VcomhLevelSet(SSD1315_VcomLevel level);
void SSD1315_ChargePumpModeSet(SSD1315_ChargePumpMode mode);
void SSD1315_DisplayInvert(bool invert);
void SSD1315_ColumnAddressSet(uint8_t begin, uint8_t end);
void SSD1315_PageAddressSet(uint8_t begin, uint8_t end);

int SSD1315_ClearScreen(void);

/** Output prepared bitmap to LCD.
 * @param x start column
 * @param y8x start row (must be multiple of 8)
 * @param width width
 * @param height8x height (must be multiple of 8)
 * @param data data, encoding order: each byte denotes 8 pixels in a column (LSB on the top, MSB on the bottom),
 *     bytes go from left to right and byte sequences go from top to bottom
 * @param dataSize data length
 * @return 0 on success, SSD1315_ERR_... on error
*/
int SSD1315_OutputPreparedBitmap(int x, int y8x, int width, int height8x, const uint8_t *data, int dataSize);

int SSD1315_FillArea(int x, int y8x, int width, int height8x, uint8_t data);


#endif // SSD1315_H
