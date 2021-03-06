#ifndef uv_sensor_h
#define uv_sensor_h

void i2c_init(void);
uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, uint8_t *rp, uint16_t rn);
float find_uv_value(uint8_t *uv_address);
int get_UVI(void);
int getTanningTime(int UVI, char *skinType);

#endif /* uv_sensor_h */
