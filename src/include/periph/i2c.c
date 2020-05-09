#include "i2c.h"
#include "xprintf/xprintf.h"

static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

void I2C_init() {
    // PB6 -> SCL, PB7 -> SDA

    // First enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* Enable GPIOB for SCL/SDA */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; /* Enable I2C1 on APB1 */

    // Enable alternative func AF4 for pins (currently all PB6/PB7 are in AFRL)
    GPIOB->AFR[0] |= (4 << 24); /* PB6 I2C alternate func */
    GPIOB->AFR[0] |= (4 << 28); /* PB7 I2C alternate func */

    // Choose alternative GPIO mode from
    // 00: Input (reset state)
    // 01: General purpose output mode
    // 10: Alternate function mode
    // 11: Analog mode
    GPIOB->MODER &= ~(3 << 12);	// clear bits 12 & 13 (PB6)
    GPIOB->MODER |=  (2 << 12);	// MODER6[1:0] = 10 bin
    GPIOB->MODER &= ~(3 << 14);	// clear bits 14 & 15 (PB7)
    GPIOB->MODER |=  (2 << 14);	// MODER7[1:0] = 10 bin

    // Enable OD mode for pins (0 by default)
    GPIOB->OTYPER |= (1 << 6);	//PB6 open drain
    GPIOB->OTYPER |= (1 << 7);	//PB9 open drain

    // Disable all pullups for SSD1306
    // 00: No pull-up, pull-down
    // 01: Pull-up
    // 10: Pull-down
    // 11: Reserved
    GPIOB->PUPDR &= ~(3 << 12);	// clear bits 12 & 13 (PB6)
    GPIOB->PUPDR |=  (1 << 12); // PUPDR6[1:0] = 01 bin
    GPIOB->PUPDR &= ~(3 << 14);	// clear bits 14 & 15 (PB9)
    GPIOB->PUPDR |=  (1 << 14); // PUPDR7[1:0] = 01 bin

    // Set port output speed register
    // 00: 2 MHz Low speed
    // 01: 25 MHz Medium speed
    // 10: 50 MHz Fast speed
    // 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
    // Reset values:  0x0000 00C0 for port B / 0x0000 0000 for other ports
    GPIOB->OSPEEDR &= ~(3 << 12); // clear bits 12 & 13 (PB6)
    GPIOB->OSPEEDR |=  (2 << 12); // OSPEEDR6[1:0] = 10 bin
    GPIOB->OSPEEDR &= ~(3 << 14); // clear bits 14 & 15 (PB7)
    GPIOB->OSPEEDR |=  (2 << 14); // OSPEEDR6[1:0] = 10 bin

    // Reset I2C
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 = 0;
    //I2C1->CR1 &= ~I2C_CR1_SWRST;
    // Switch it off
    //I2C1->CR1 &= ~I2C_CR1_PE; // Just 0?
    // Set up I2C speed from PCLK1 source (FREQR). Interrupts and DMA live here too.
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 42;// 84 / 2 = 42
    // Fast mode and clocks
    I2C1->CCR &= ~I2C_CCR_CCR;
    I2C1->CCR = 12 | I2C_CCR_FS; // 2500 ns (for 400KHz) / 3 (2 duty + 1 low) * 24 ns (42 MHz) = 34
    // TRISE
    I2C1->TRISE = 12; // (TrMAX = 300 ns / 24 ns) + 1 = 13
    // Switch it on
    I2C1->CR1 |= I2C_CR1_PE;
}

static const uint8_t ssd_1306_init_seq_2[] = {
        0xAE, // Off
        0x20, // Memory adressing mode,
        0x00, // 00b Horizontal Addressing Mode, 01b Vertical Addressing Mode;
              // 10b Page Addressing Mode (RESET), 11b Invalid
        0xB0, // Set Page Start Address for Page Addressing Mode,0-7
#ifdef SSD1306_MIRROR_VERT
        0xC0, // Mirror vertically
#else
        0xC8, //Set COM Output Scan Direction
#endif
        0x00, //---set low column address
        0x10, //---set high column address
        0x40, //--set start line address - CHECK
        0x81, //--set contrast control register - CHECK
        0xFF,
#ifdef SSD1306_MIRROR_HORIZ
        0xA0, // Mirror horizontally
#else
        0xA1, //--set segment re-map 0 to 127 - CHECK
#endif
#ifdef SSD1306_INVERSE_COLOR
        0xA7, //--set inverse color
#else
        0xA6, //--set normal color
#endif
// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
        0xFF, // Found in the Luma Python lib for SH1106.
#else
        0xA8, //--set multiplex ratio(1 to 64) - CHECK
#endif
#if (SSD1306_HEIGHT == 32)
        0x1F,
#elif (SSD1306_HEIGHT == 64)
        0x3F,
#elif (SSD1306_HEIGHT == 128)
        0x3F, // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
        0xA4, //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
        0xD3, //-set display offset - CHECK
        0x00, //-not offset
        0xD5, //--set display clock divide ratio/oscillator frequency
        0xF0, //--set divide ratio
        0xD9, //--set pre-charge period
        0x22,
        0xDA, //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
        0x02,
#elif (SSD1306_HEIGHT == 64)
        0x12,
#elif (SSD1306_HEIGHT == 128)
        0x12,
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
        0xDB, //--set vcomh
        0x20, //0x20,0.77xVcc

        0x8D, //--set DC-DC enable
        0x14, //
        0xAF, //--turn on SSD1306 panel
};

static const uint8_t ssd_1306_init_seq[] = {
        SSD1306_SETMULTIPLEX,
        0x3F,
        SSD1306_SETDISPLAYOFFSET,
        0x00,
        SSD1306_SETSTARTLINE,
        SSD1306_MEMORYMODE,
        0x00,
        SSD1306_COLUMNADDR,
        0x00,
        0x7F,
        SSD1306_PAGEADDR,
        0x00,
        0x07,
        SSD1306_SEGREMAP,
        SSD1306_COMSCANDEC,
        SSD1306_SETCOMPINS,
        0x12,
        SSD1306_SETCONTRAST,
        0x7F,
        SSD1306_NORMALDISPLAY,
        SSD1306_SETDISPLAYCLOCKDIV,
        0x20,
        SSD1306_CHARGEPUMP,
        0x14,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_DISPLAYON
};

void ssd1306_send(uint8_t control_byte, uint8_t data) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
    (void) I2C1->SR1;

    I2C1->DR = SSD1306_Address;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void) I2C1->SR1;
    (void) I2C1->SR2;

    I2C1->DR = control_byte;
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));

    I2C1->CR1 |= I2C_CR1_STOP;
}

void ssd1306_SendCommand(uint8_t command) {
    ssd1306_send(0x0/*SSD1306_Command_Mode*/, command);
}

void ssd1306_SendData(uint8_t *data, uint8_t count) {
    I2C1->CR1 |= I2C_CR1_START;

    while (!(I2C1->SR1 & I2C_SR1_SB));
    (void) I2C1->SR1;

    I2C1->DR = SSD1306_Address;

    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void) I2C1->SR1;
    (void) I2C1->SR2;

    I2C1->DR = SSD1306_Dats_Mode;
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    for(uint8_t index = 0; index < count; ++index) {
        I2C1->DR = *data++;
        while (!(I2C1->SR1 & I2C_SR1_TXE));
    }

    while (!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
}

/*
void ssd1306_Fill(uint8_t color) {
    for (uint8_t i = 0; i < 128; ++i) {
        I2C1->CR1 |= I2C_CR1_START;

        while (!(I2C1->SR1 & I2C_SR1_SB));
        (void) I2C1->SR1;

        I2C1->DR = SSD1306_Address;

        while (!(I2C1->SR1 & I2C_SR1_ADDR));
        (void) I2C1->SR1;
        (void) I2C1->SR2;

        I2C1->DR = SSD1306_Dats_Mode;
        while (!(I2C1->SR1 & I2C_SR1_TXE));

        for (uint8_t j = 0; j < 8; ++j) {
            I2C1->DR = color;
            while (!(I2C1->SR1 & I2C_SR1_TXE));
        }

        while (!(I2C1->SR1 & I2C_SR1_BTF));
        I2C1->CR1 |= I2C_CR1_STOP;
    }
}*/

void ssd1306_Init() {
    for (uint8_t i = 0; i < sizeof(ssd_1306_init_seq_2); ++i) {
        ssd1306_SendCommand(ssd_1306_init_seq_2[i]);
    }
}

// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
        ssd1306_SendCommand(0xB0 + i); // Set the current RAM page address.
        ssd1306_SendCommand(0x00);
        ssd1306_SendCommand(0x10);
        ssd1306_SendData(&SSD1306_Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
    }
}

// Fill the whole screen with the given color
void ssd1306_Fill(uint8_t color) {
    // Set memory
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = color;
    }
}