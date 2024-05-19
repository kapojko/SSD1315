#ifndef SSD1315_H
#define SSD1315_H

// based on https://github.com/kefir/ssd1315/blob/master/ssd1315.c

#include <stdint.h>
#include <stdbool.h>

// I2C address
#define SSD1315_I2C_ADDR_DEF 0x3C
#define SSD1315_I2C_ADDR_ALTERNATE 0x3D // depends on D/C# pin

#define SSD1315_I2C_BUFFER_SIZE 32

// Command/data control byte
#define SSD1315_CMD  0x80
#define SSD1315_DATA 0x40

// Display size
#define SSD1315_BITS_PER_PAGE 8
#define SSD1315_PAGE_COUNT  8
#define SSD1315_COL_COUNT 128

#define SSD1315_BUFF_SIZE (SSD1315_COL_COUNT * SSD1315_PAGE_COUNT)

// Commands
#define SSD1315_CMD_CC            0x81 /** Set Contrast Control */
#define SSD1315_CMD_SR            0xA0 /** Set Segment Remap */
#define SSD1315_CMD_EDON          0xA4 /** Entire Display On */
#define SSD1315_CMD_DINV          0xA6 /** Set Normal/Inverse Display */
#define SSD1315_CMD_IREF          0xAD /** Internal Iref Setting */
#define SSD1315_CMD_DON           0xAE /** Set Display On/Off */
#define SSD1315_CMD_DO            0xD3 /** Set Display Offset */
#define SSD1315_CMD_PCP           0xD9 /** Set Pre-charge Period */
#define SSD1315_CMD_VCSL          0xDB /** Set Vcomh Select Level */
#define SSD1315_CMD_NOP           0xE3 /** NOP */
#define SSD1315_CMD_CPS           0x8D /** Charge Pump Setting */
#define SSD1315_CMD_CHSS          0x26 /** Continuous Horizontal Scroll Setup */
#define SSD1315_CMD_CVHSS         0x29 /** Continuous Vertical and Horizontal Scroll Setup */
#define SSD1315_CMD_DS            0x2E /** Deactivate Scroll */
#define SSD1315_CMD_AS            0x2F /** Activate Scroll */
#define SSD1315_CMD_VSA           0xA3 /** Set Vertical Scroll Area */
#define SSD1315_CMD_CSS           0x2C /** Content Scroll Setup */
#define SSD1315_CMD_FOB           0x23 /** Set Fade Out and Blinking */
#define SSD1315_CMD_ZI            0xD6 /** Set Zoom In */

#define SSD1315_CMD_LCS           0x00 /** Set Lower Column Start Address for Page Addressing Mode */
#define SSD1315_CMD_LCS_MASK      0x0F

#define SSD1315_CMD_HCS           0x10 /** Set Higher Column Start Address for Page Addressing Mode */
#define SSD1315_CMD_HCS_MASK      0x07

#define SSD1315_CMD_MAM           0x20 /** Set Memory Addressing Mode */
#define SSD1315_CMD_MAM_MASK      0x03

#define SSD1315_CMD_CA            0x21 /** Set Column Address */
#define SSD1315_CMD_CA_MASK       0x7F

#define SSD1315_CMD_PA            0x22 /** Set Page Address */
#define SSD1315_CMD_PA_MASK       0x07

#define SSD1315_CMD_DSL           0x40 /** Set Display Start Line */
#define SSD1315_CMD_DSL_MASK      0x3F

#define SSD1315_CMD_MR            0xA8 /** Set Multiplex Ratio */
#define SSD1315_MUX_MIN           15
#define SSD1315_MUX_MAX           63

#define SSD1315_CMD_PSA           0xB0 /** Set Page Start Address for Page Addressing Mode */
#define SSD1315_CMD_PSA_MASK      0x07

#define SSD1315_CMD_COSD          0xC0 /** Set COM Output Scan Direction */
#define SSD1315_CMD_COSD_POS      3

#define SSD1315_CMD_DCDR          0xD5 /** Set Display Clock Divide Ratio/Oscillator Frequency */
#define SSD1315_CMD_DCDR_CLK_POS  4
#define SSD1315_CMD_DCDR_CLK_MASK 0xF0
#define SSD1315_CMD_DCDR_DIV_MASK 0x0F

#define SSD1315_CMD_CPHC          0xDA /** Set COM Pins Hardware Configuration */
#define SSD1315_CMD_CPHC_VALUE    0x02
#define SSD1315_CMD_CPHC_CONF_POS 4
#define SSD1315_CMD_CPHC_MAP_POS  5

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
 * @param dataSize data length, including SSD1315_DATA byte (if OUTPUT_FLAG_DATA_BYTE_PREFIXED flag is set)
 * @return 0 on success, SSD1315_ERR_... on error
*/
int SSD1315_OutputPreparedBitmap(int x, int y8x, int width, int height8x, const uint8_t *data, int dataSize);

int SSD1315_FillArea(int x, int y8x, int width, int height8x, uint8_t data);


#endif // SSD1315_H
