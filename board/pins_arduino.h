#ifndef Pins_Arduino_h
#define Pins_Arduino_h

/**
 * Variant: WT32-SC01
 * Vendor: Wireless-Tag
 * Url: https://en.wireless-tag.com/product-item-25.html
 */

#include <stdint.h>

// general purpose IO pins
static const uint8_t IO0 = 0;
static const uint8_t IO1 = 1; // TXD0 / TX0 pin
static const uint8_t IO2 = 2;
static const uint8_t IO3 = 3; // RXD0 / RX0 pin
static const uint8_t IO4 = 4;
static const uint8_t IO5 = 5; // RXD2 / RXD pin
static const uint8_t IO12 = 12;
static const uint8_t IO14 = 14;
static const uint8_t IO15 = 15;
static const uint8_t IO17 = 17; // TXD2 / TXD pin
static const uint8_t IO32 = 32; // CFG pin
static const uint8_t IO33 = 33; // 485_EN pin

// input-only pins
static const uint8_t IO35 = 35;
static const uint8_t IO36 = 36;
static const uint8_t IO39 = 39;

// UART interfaces
static const uint8_t TXD0 = 1, TX0 = 1;
static const uint8_t RXD0 = 3, RX0 = 3;
static const uint8_t TXD2 = 17, TXD = 17;
static const uint8_t RXD2 = 5, RXD = 5;
static const uint8_t TX = 1;
static const uint8_t RX = 3;

//SPI VSPI default pins
static const uint8_t SS    = -1;
static const uint8_t MOSI  = 14;
static const uint8_t MISO  = 15;
static const uint8_t SCK   = 12;

//I2C default pins
static const uint8_t SDA = 33;
static const uint8_t SCL = 32;

#endif /* Pins_Arduino_h */
