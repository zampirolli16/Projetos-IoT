
#include "ssd1306.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <string.h>

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

static const char *TAG = "SSD1306";

static uint8_t ssd1306_buffer[128 * 64 / 8];
static i2c_port_t i2c_num = I2C_MASTER_NUM;

static void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_num, &conf);
    i2c_driver_install(i2c_num, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
    ESP_LOGI(TAG, "I2C initialized");
}

static esp_err_t ssd1306_write_cmd(uint8_t cmd) {
    uint8_t write_buf[2] = {0x00, cmd}; // Control byte 0x00 = command
    return i2c_master_write_to_device(i2c_num, 0x3C, write_buf, 2, 1000 / portTICK_PERIOD_MS);
}

static esp_err_t ssd1306_write_data(uint8_t *data, size_t len) {
    uint8_t *buf = malloc(len + 1);
    if (!buf) return ESP_ERR_NO_MEM;
    buf[0] = 0x40; // Control byte 0x40 = data
    memcpy(buf + 1, data, len);
    esp_err_t ret = i2c_master_write_to_device(i2c_num, 0x3C, buf, len + 1, 1000 / portTICK_PERIOD_MS);
    free(buf);
    return ret;
}

void ssd1306_init(void) {
    i2c_master_init();

    ssd1306_write_cmd(0xAE); // Display OFF
    ssd1306_write_cmd(0x20); // Set Memory Addressing Mode
    ssd1306_write_cmd(0x10); // Page addressing mode
    ssd1306_write_cmd(0xB0); // Set Page Start Address for Page Addressing Mode
    ssd1306_write_cmd(0xC8); // COM Output Scan Direction
    ssd1306_write_cmd(0x00); // Low column address
    ssd1306_write_cmd(0x10); // High column address
    ssd1306_write_cmd(0x40); // Start line address
    ssd1306_write_cmd(0x81); // Contrast control
    ssd1306_write_cmd(0xFF);
    ssd1306_write_cmd(0xA1); // Segment remap
    ssd1306_write_cmd(0xA6); // Normal display
    ssd1306_write_cmd(0xA8); // Multiplex ratio
    ssd1306_write_cmd(0x3F);
    ssd1306_write_cmd(0xA4); // Output RAM to Display
    ssd1306_write_cmd(0xD3); // Display offset
    ssd1306_write_cmd(0x00);
    ssd1306_write_cmd(0xD5); // Display clock divide ratio/oscillator frequency
    ssd1306_write_cmd(0xF0);
    ssd1306_write_cmd(0xD9); // Pre-charge period
    ssd1306_write_cmd(0x22);
    ssd1306_write_cmd(0xDA); // COM pins hardware configuration
    ssd1306_write_cmd(0x12);
    ssd1306_write_cmd(0xDB); // VCOMH deselect level
    ssd1306_write_cmd(0x20);
    ssd1306_write_cmd(0x8D); // Charge pump setting
    ssd1306_write_cmd(0x14);
    ssd1306_write_cmd(0xAF); // Display ON

    memset(ssd1306_buffer, 0, sizeof(ssd1306_buffer));
}

void ssd1306_clear_screen(void) {
    memset(ssd1306_buffer, 0, sizeof(ssd1306_buffer));
}

void ssd1306_refresh_gram(void) {
    for (uint8_t i = 0; i < 8; i++) {
        ssd1306_write_cmd(0xB0 + i);
        ssd1306_write_cmd(0x00);
        ssd1306_write_cmd(0x10);
        ssd1306_write_data(&ssd1306_buffer[128 * i], 128);
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str, uint8_t size, uint8_t mode) {
    printf("Draw string: %s at (%d,%d)\n", str, x, y);
}
