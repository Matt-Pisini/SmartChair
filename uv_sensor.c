#include <avr/io.h>
#include "uv_sensor.h"

#define CLK 7372800				// clock rate
#define BDIV (CLK / 100000 - 16) / 2 + 	1	// TWBR value, prescaler 1
#define VEML6075_ADDR 0x20      // I2C slave address; 7bit address (0x10) left shifted with 0 in LSB
#define UVA_REG 0x07
#define UVB_REG 0x09
#define UVCOMP1_REG 0x0A
#define UVCOMP2_REG 0x0B

#define uva_a_coeff 2.22
#define uva_b_coeff 1.33
#define uvb_c_coeff 2.95
#define uvb_d_coeff 1.74
#define uva_resp 0.001461
#define uvb_resp 0.002591

uint16_t VEML6075;
uint16_t uva_data;
uint16_t uvb_data;
uint16_t uvcomp1_data;
uint16_t uvcomp2_data;

void i2c_init(void)
{
    TWSR = 0; //prescaler is 1
    TWBR = BDIV;
}

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, uint8_t *rp, uint16_t rn)
{
    uint8_t status, send_stop, wrote, start_stat;
    
    status = 0;
    wrote = 0;
    send_stop = 0;
    
    if (an > 0) {
        wrote = 1;
        send_stop = 1;
        
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  // Send start condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x08)                 // Check that START was sent OK
            return(status);
        
        TWDR = device_addr & 0xfe;          // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18) {               // Check that SLA+W was sent OK
            if (status == 0x20)             // Check for NAK
                goto nakstop;               // Send STOP condition
            return(status);                 // Otherwise just return the status
        }
        
        // Write "an" data bytes to the slave device
        while (an-- > 0) {
            TWDR = *ap++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }
        status = 0;                         // Set status value to successful
    }
    
    if (rn > 0) {
        send_stop = 1;
        
        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;
        
        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
        // Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat)           // Check that START or repeated START sent OK
            return(status);
        
        TWDR = device_addr  | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Send address+r
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40) {               // Check that SLA+R was sent OK
            if (status == 0x48)             // Check for NAK
                goto nakstop;
            return(status);
        }
        
        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50)             // Check that data received OK
                return(status);
            *rp++ = TWDR;                   // Read the data
        }
        
        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN);  // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58)                 // Check that data received OK
            return(status);
        *rp++ = TWDR;                       // Read the data
        
        status = 0;                         // Set status value to successful
    }
    
nakstop:                                    // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Send STOP condition
    
    return(status);
}

float find_uv_value(uint8_t *uv_address)
{
    float decimal_val = 0;
    int base = 1, rem;
    uint8_t rbuf[2];
    uint8_t check_status = i2c_io(VEML6075_ADDR, uv_address, 2, rbuf, 2);
    if (check_status == 0) {
        while (*uv_address > 0)
        {
            rem = *uv_address % 10;
            decimal_val = decimal_val + rem * base;
            *uv_address = *uv_address / 10 ;
            base = base * 2;
        }
    }
    return decimal_val;
}

int get_UVI(void)
{
    uint8_t uva_address = UVA_REG;
    uint8_t *uva_ptr = &uva_address;
    uint8_t uvb_address = UVB_REG;
    uint8_t *uvb_ptr = &uvb_address;
    uint8_t uvcomp1_address = UVCOMP1_REG;
    uint8_t *uv_comp1_ptr = &uvcomp1_address;
    uint8_t uvcomp2_address = UVCOMP2_REG;
    uint8_t *uv_comp2_ptr = &uvcomp2_address;
    float uva_value = find_uv_value(uva_ptr);
    float uvb_value = find_uv_value(uvb_ptr);
    float uv_comp1 = find_uv_value(uv_comp1_ptr);
    float uv_comp2 = find_uv_value(uv_comp2_ptr);
    float uva_calc = uva_value - (uva_a_coeff*uv_comp1) - (uva_b_coeff*uv_comp2);
    float uvb_calc = uvb_value - (uvb_c_coeff*uv_comp1) - (uvb_d_coeff*uv_comp2);
    float uvia = uva_calc * uva_resp;
    float uvib = uvb_calc * uvb_resp;
    float uvi = (uvia + uvib) / 2;
    uvi = round(40*uvi);
    uvi = (int)uvi;
    return uvi;
}
}
