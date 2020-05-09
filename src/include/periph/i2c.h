#pragma once

#include "aux.h"

#define I2C_MODE_READ 1
#define I2C_MODE_WRITE 0
#define I2C_ADDRESS(addr, mode) ((addr<<1) | mode)

#define SSD1306_Address             I2C_ADDRESS(0x3C, I2C_MODE_WRITE)
#define SSD1306_DataStream          0x40
#define SSD1306_CommandStream       0x00
#define SSD1306_SingleByte          0xC0
#define SSD1306_SingleCommand       0x80

#define SSD1306_BLACK               0
#define SSD1306_WHITE               0xFF
#define SSD1306_PATTERN_R           0x55
#define SSD1306_PATTERN_L           0xAA

#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64
//#define SSD1306_MIRROR_VERT
//#define SSD1306_MIRROR_HORIZ
//#define SSD1306_INVERSE_COLOR

#define SSD1306_MEMORYMODE          0x20 //2< Автоматическая адресация
#define SSD1306_COLUMNADDR          0x21 //3< See datasheet
#define SSD1306_PAGEADDR            0x22 //3< See datasheet
#define SSD1306_SETCONTRAST         0x81 //2< Контрастность
#define SSD1306_CHARGEPUMP          0x8D //2< Умножитель напряжения
#define SSD1306_SEGREMAP_MIRROR     0xA0 ///< Развёртка справа/налево
#define SSD1306_SEGREMAP            0xA1 ///< Развёртка слева/направо
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< отображение содержимого RAM
#define SSD1306_DISPLAYALLON        0xA5 ///<  Игнорирует содержимое RAM.
#define SSD1306_NORMALDISPLAY       0xA6 ///< Нет инверсии
#define SSD1306_INVERTDISPLAY       0xA7 ///< Инверсия
#define SSD1306_SETMULTIPLEX        0xA8 //2< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< Выключить дисплей
#define SSD1306_DISPLAYON           0xAF ///< Дисплей включен
#define SSD1306_COMSCANINC          0xC0 ///< Развёртка снизу/верх
#define SSD1306_COMSCANDEC          0xC8 ///< Развёртка сверху/вниз
#define SSD1306_SETDISPLAYOFFSET    0xD3 //2< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 //2< Частота обновления
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA //2< Разрешение 0x02-128x32, 0x12-128x64
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range

void I2C_init();

void ssd1306_send(uint8_t control_byte, uint8_t data);
void ssd1306_Init();
void ssd1306_SendCommand(uint8_t command);
void ssd1306_SendData(uint8_t mode, uint8_t *data, uint8_t count);
void ssd1306_Fill(uint8_t color);
void ssd1306_UpdateScreen(void);