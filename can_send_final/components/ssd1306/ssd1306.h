
#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stdint.h>
#include "esp_err.h"

void ssd1306_init(void);
void ssd1306_clear_screen(void);
void ssd1306_refresh_gram(void);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str, uint8_t size, uint8_t mode);

#endif
