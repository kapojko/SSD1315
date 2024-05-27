#include <string.h>
#include "SSD1315.h"

// Command/data control byte
#define SSD1315_CMD  0x80
#define SSD1315_DATA 0x40

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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CHECK_ROWS(y8x, height8x) (y8x >= 0 && height8x > 0 && y8x + height8x <= SSD1315_PAGE_COUNT * SSD1315_BITS_PER_PAGE && y8x % 8 == 0 && height8x % 8 == 0)
#define CHECK_COLUMNS(x, width) (x >= 0 && width > 0 && x + width <= SSD1315_COL_COUNT)
#define DATA_SIZE(width, height8x) (height8x / SSD1315_BITS_PER_PAGE * width)

static struct SSD1315_Platform *platform;

// NOTE: if array is partly initialized, all non-mentioned elements will be set to 0
static uint8_t buffer[SSD1315_BUFF_SIZE + 1] = { SSD1315_DATA, 0 };

static void writeToLcd(uint8_t data[], int len) {
    int err;

    err = platform->i2cWrite(platform->i2cAddr, data, len, 1, 1);
    if (err) {
        platform->debugPrint("I2C write error: %d\r\n", -err);
    }
}

static void writeDataFromBuffer(int pageStart, int pageEnd, int colStart, int colEnd) {
    uint8_t *bufPtr = buffer + 1;

    // Write data in chunks not exceeding SSD1315_I2C_BUFFER_SIZE (DATA byte prefix + data portion)
    int chunkSize = SSD1315_I2C_BUFFER_SIZE - 1;

    for (int page = pageStart; page <= pageEnd; page++) {
        // Write page address
        SSD1315_PageStartAddressSet(page);

        // Write data for columns
        for (int col = colStart; col <= colEnd; ) {
            // Write column address
            SSD1315_ColumnStartAddressSet(col);

            // Write chunk of data
            int chunk = MIN(chunkSize, colEnd - col + 1);
            *(bufPtr - 1) = SSD1315_DATA;
            writeToLcd(bufPtr - 1, chunk + 1);

            // Increase column
            bufPtr += chunk;
            col += chunk;
        }
    }
}

void SSD1315_Init(struct SSD1315_Platform *platformPtr) {
    platform = platformPtr;
}

void SSD1315_DefInit(bool xyFlip) {
    SSD1315_ChargePumpModeSet(SSD1315_CHARGE_PUMP_MODE7V);
    SSD1315_DisplayOn(false);
    SSD1315_MemoryModeSet(SSD1315_MEM_MODE_PAGE);
    SSD1315_DisplayStartLineSet(0);
    SSD1315_ComScanInvert(xyFlip);
    SSD1315_SegmentRemap(xyFlip);
    SSD1315_DisplayInvert(false);
    SSD1315_DisplayOn(true);
    SSD1315_EntireDisplayOn(false);
    
    // DelayMs(100);

    SSD1315_ClearScreen();
}

void SSD1315_DisplayOn(bool enable) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_DON | (uint8_t)enable
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_EntireDisplayOn(bool enable) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_EDON | (uint8_t)enable
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ColumnStartAddressSet(uint8_t addr) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_LCS | (addr & SSD1315_CMD_LCS_MASK))
    };
    writeToLcd(cmd, sizeof(cmd));
    cmd[1] = (SSD1315_CMD_HCS | ((addr >> 4) & SSD1315_CMD_HCS_MASK));
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_DisplayStartLineSet(uint8_t line) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_DSL | (line & SSD1315_CMD_DSL_MASK))
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_MemoryModeSet(SSD1315_MemMode mode) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_MAM,
        ((uint8_t)mode & SSD1315_CMD_MAM_MASK)
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_PageStartAddressSet(uint8_t addr) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_PSA | (addr & SSD1315_CMD_PSA_MASK))
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ContrastSet(uint8_t contrast) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CC,
        contrast
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_SegmentRemap(bool remap) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_SR | (uint8_t)remap
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_MuxRatioSet(uint8_t mux) {
    mux = (mux < SSD1315_MUX_MIN) ? SSD1315_MUX_MIN : mux;
    mux = (mux > SSD1315_MUX_MAX) ? SSD1315_MUX_MAX : mux;
    uint8_t cmd[] = { SSD1315_CMD, SSD1315_CMD_MR, mux };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ComScanInvert(bool invert) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_COSD | ((uint8_t)invert << SSD1315_CMD_COSD_POS)
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_DisplayOffsetSet(uint8_t offset) {
    uint8_t cmd[] = { SSD1315_CMD, SSD1315_CMD_DO, offset };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ClockSet(uint8_t freq, uint8_t divider) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_DCDR,
        (divider & SSD1315_CMD_DCDR_DIV_MASK) |
        ((freq << SSD1315_CMD_DCDR_CLK_POS) & SSD1315_CMD_DCDR_CLK_MASK)
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_PrechargePeriodSet(uint8_t period) {
    uint8_t cmd[] = { SSD1315_CMD, SSD1315_CMD_PCP, period };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ComPinsSet(bool alternative, bool left_right_remap) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CPHC,
        SSD1315_CMD_CPHC_VALUE
        | (uint8_t)alternative << SSD1315_CMD_CPHC_CONF_POS
        | (uint8_t)left_right_remap << SSD1315_CMD_CPHC_MAP_POS
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_VcomhLevelSet(SSD1315_VcomLevel level) {
    uint8_t cmd[] = { SSD1315_CMD, SSD1315_CMD_VCSL, (uint8_t)level };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ChargePumpModeSet(SSD1315_ChargePumpMode mode) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CPS,
        (uint8_t)mode
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_DisplayInvert(bool invert) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_DINV | (uint8_t)invert)
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_ColumnAddressSet(uint8_t begin, uint8_t end) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CA,
        begin & SSD1315_CMD_CA_MASK,
        end & SSD1315_CMD_CA_MASK
    };
    writeToLcd(cmd, sizeof(cmd));
}

void SSD1315_PageAddressSet(uint8_t begin, uint8_t end) {
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_PA,
        begin & SSD1315_CMD_PA_MASK,
        end & SSD1315_CMD_PA_MASK
    };
    writeToLcd(cmd, sizeof(cmd));
}

int SSD1315_ClearScreen(void) {
    // write zero data to LCD
    memset(buffer + 1, 0, sizeof(buffer) - 1);
    writeDataFromBuffer(0, SSD1315_PAGE_COUNT - 1, 0, SSD1315_COL_COUNT - 1);
    return 0;
}

bool SSD1315_OutputPreparedBitmap(int x, int y8x, int width, int height8x, const uint8_t *data, int dataSize) {
    // check rows
    if (!CHECK_ROWS(y8x, height8x)) {
        platform->debugPrint("Invalid y8x or height8x: %d, %d\r\n", y8x, height8x);
        return false;
    }

    // check columns
    if (!CHECK_COLUMNS(x, width)) {
        platform->debugPrint("Invalid x or width: %d, %d\r\n", x, width);
        return false;
    }

    // check length
    int calcDataSize = DATA_SIZE(width, height8x);
    if (calcDataSize != dataSize) {
        platform->debugPrint("Invalid dataSize: %d, expected %d\r\n", dataSize, calcDataSize);
        return false;
    }

    // copy data to buffer
    if (dataSize > sizeof(buffer) - 1) {
        // Unexpected error
        platform->debugPrint("Unexpected dataSize: %d\r\n", dataSize);
        return false;
    }

    memcpy(buffer + 1, data, dataSize);

    // write data to LCD
    writeDataFromBuffer(y8x / 8, (y8x + height8x) / 8 - 1, x, x + width - 1);
    
    return true;
}

bool SSD1315_FillArea(int x, int y8x, int width, int height8x, int data) {
    // check rows
    if (!CHECK_ROWS(y8x, height8x)) {
        platform->debugPrint("Invalid y8x or height8x: %d, %d\r\n", y8x, height8x);
        return false;
    }

    // check columns
    if (!CHECK_COLUMNS(x, width)) {
        platform->debugPrint("Invalid x or width: %d, %d\r\n", x, width);
        return false;
    }

    // fill buffer
    int dataSize = DATA_SIZE(width, height8x);
    memset(buffer + 1, data, dataSize);

    // write data to LCD
    writeDataFromBuffer(y8x / 8, (y8x + height8x) / 8 - 1, x, x + width - 1);

    return true;
}
