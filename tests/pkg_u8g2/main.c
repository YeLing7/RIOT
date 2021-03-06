/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the U8g2 package.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#define TEST_U8G2_OUTPUT_STDOUT 1
#define TEST_U8G2_OUTPUT_SDL 2
#define TEST_U8G2_OUTPUT_SPI 3
#define TEST_U8G2_OUTPUT_I2C 4

#ifndef TEST_U8G2_OUTPUT
#error "TEST_U8G2_OUTPUT not defined"
#endif
#ifndef TEST_U8G2_DISPLAY
#error "TEST_U8G2_DISPLAY not defined"
#endif

#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_I2C
#ifndef TEST_U8G2_I2C
#error "TEST_U8G2_I2C not defined"
#endif
#ifndef TEST_U8G2_ADDR
#error "TEST_U8G2_ADDR not defined"
#endif
#ifndef TEST_U8G2_PIN_RESET
#error "TEST_U8G2_PIN_RESET not defined"
#endif
#endif

#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_SPI
#ifndef TEST_U8G2_SPI
#error "TEST_U8G2_SPI not defined"
#endif
#ifndef TEST_U8G2_PIN_CS
#error "TEST_U8G2_PIN_CS not defined"
#endif
#ifndef TEST_U8G2_PIN_DC
#error "TEST_U8G2_PIN_DC not defined"
#endif
#ifndef TEST_U8G2_PIN_RESET
#error "TEST_U8G2_PIN_RESET not defined"
#endif
#endif

#include <stdio.h>

#include "periph/gpio.h"

#include "xtimer.h"
#include "u8g2.h"

/**
 * @brief   RIOT-OS logo, 64x32 pixels at 8 pixels per byte.
 */
static const uint8_t logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x3C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF0, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0xF8,
    0x30, 0x3C, 0x3F, 0xC0, 0x00, 0x0C, 0x77, 0xF0, 0x38, 0x7E, 0x3F, 0xC0,
    0x00, 0x7E, 0x73, 0xC0, 0x38, 0xE7, 0x06, 0x00, 0x00, 0xFC, 0x71, 0x00,
    0x38, 0xE3, 0x06, 0x00, 0x01, 0xF0, 0x70, 0x00, 0x38, 0xE3, 0x06, 0x00,
    0x01, 0xC0, 0x70, 0x00, 0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0xC0,
    0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x71, 0xE0, 0x38, 0xE3, 0x06, 0x00,
    0x03, 0x80, 0x70, 0xE0, 0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0xF0,
    0x38, 0xE3, 0x06, 0x00, 0x03, 0x80, 0x70, 0x70, 0x38, 0xE3, 0x06, 0x00,
    0x03, 0x80, 0xF0, 0x78, 0x38, 0xE3, 0x06, 0x00, 0x03, 0xC1, 0xE0, 0x3C,
    0x38, 0xE7, 0x06, 0x00, 0x01, 0xE3, 0xE0, 0x3C, 0x38, 0x7E, 0x06, 0x00,
    0x01, 0xFF, 0xC0, 0x1C, 0x30, 0x3C, 0x06, 0x00, 0x00, 0x7F, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

#if (TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_I2C) || (TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_SPI)
/**
 * @brief   RIOT-OS pin maping of U8g2 pin numbers to RIOT-OS GPIO pins.
 * @note    To minimize the overhead, you can implement an alternative for
 *          u8x8_gpio_and_delay_riotos.
 */
static gpio_t pins[] = {
    [U8X8_PIN_CS] = TEST_U8G2_PIN_CS,
    [U8X8_PIN_DC] = TEST_U8G2_PIN_DC,
    [U8X8_PIN_RESET] = TEST_U8G2_PIN_RESET
};

/**
 * @brief   Bit mapping to indicate which pins are set.
 */
static uint32_t pins_enabled = (
    (1 << U8X8_PIN_CS) +
    (1 << U8X8_PIN_DC) +
    (1 << U8X8_PIN_RESET)
);
#endif

int main(void)
{
    uint32_t screen = 0;
    u8g2_t u8g2;

    /* initialize to stdout */
#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_STDOUT
    puts("initializing to stdout.");

    u8g2_SetupBuffer_Utf8(&u8g2, U8G2_R0);
#endif

    /* initialize to virtual SDL (native only) */
#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_SDL
    puts("initializing to SDL.");

    u8g2_SetupBuffer_SDL_128x64_4(&u8g2, U8G2_R0);
#endif

    /* initialize to SPI */
#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_SPI
    puts("initializing to SPI.");

    TEST_U8G2_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);

    u8g2_SetPins(&u8g2, pins, pins_enabled);
    u8g2_SetDevice(&u8g2, TEST_U8G2_SPI);
#endif

    /* initialize to I2C */
#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_I2C
    puts("initializing to I2C.");

    TEST_U8G2_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_i2c, u8x8_gpio_and_delay_riotos);

    u8g2_SetPins(&u8g2, pins, pins_enabled);
    u8g2_SetDevice(&u8g2, TEST_U8G2_I2C);
    u8g2_SetI2CAddress(&u8g2, TEST_U8G2_ADDR);
#endif

    /* initialize the display */
    puts("Initializing display.");

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    /* start drawing in a loop */
    puts("Drawing on screen.");

    while (true) {
        u8g2_FirstPage(&u8g2);

        do {
            u8g2_SetDrawColor(&u8g2, 1);
            u8g2_SetFont(&u8g2, u8g2_font_helvB12_tf);

            if (screen == 0) {
                u8g2_DrawStr(&u8g2, 12, 22, "THIS");
            } else if (screen == 1) {
                u8g2_DrawStr(&u8g2, 24, 22, "IS");
            } else if (screen == 2) {
                u8g2_DrawBitmap(&u8g2, 0, 0, 8, 32, logo);
            }
        } while (u8g2_NextPage(&u8g2));

#if TEST_U8G2_OUTPUT == TEST_U8G2_OUTPUT_STDOUT
        /* transfer screen buffer to stdout */
        utf8_show();
#endif

        /* show screen in next iteration */
        screen = (screen + 1) % 3;

        /* sleep a little */
        xtimer_sleep(1);
    }

    return 0;
}
