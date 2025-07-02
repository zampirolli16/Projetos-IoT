/// as5600.h
#ifndef AS5600_H
#define AS5600_H

#include <stdint.h>

#define AS5600_I2C_ADDR 0x36

void as5600_init();
uint16_t as5600_get_raw_angle();
float as5600_get_angle_degrees();

#endif // AS5600_H

