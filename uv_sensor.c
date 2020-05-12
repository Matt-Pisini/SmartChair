#include <avr/io.h>
#include "uv_sensor.h"
#include <math.h>
#include <string.h>

#define CLK 7372800				            // clock rate
#define BDIV (CLK / 100000 - 16) / 2 + 	1	// TWBR value, prescaler 1
#define VEML6075_ADDR 0x20                  // I2C slave address; 7bit address 0x10
#define UVA_REG 0x07        // UVA internal address, command code 7
#define UVB_REG 0x09        // UVB internal address, command code 9
#define UVCOMP1_REG 0x0A    // UV_COMP1 internal address, command code 10
#define UVCOMP2_REG 0x0B    // UV_COMP2 internal address, command code 11

#define uva_a_coeff 2.22    // UV coefficients are defined in VEML_6075 application sheet
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

// I2C initialization code referenced by Professor Weber
void i2c_init(void)
{
    TWSR = 0; //prescaler is 1
    TWBR = BDIV;
}

// I2C communication code referenced by Professor Weber
// Edited to remove *wp and wn, as we are not writing anything other than the addresses to the VEML_6075
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

/* Function to communicate with VEML_6075 and read UVA, UVB, UVCOMP1, and UVCOMP2 values
 Input argument: ptr to internal address data; output: float UV value in decimal
 */
float find_uv_value(uint8_t *uv_address)
{
    int decimal_val = 0, base = 1, div;     // Variables to convert binary to decimal
    uint8_t rbuf[2];                        // Buffer to store 2 bytes of data
    uint8_t check_status = i2c_io(VEML6075_ADDR, uv_address, 2, rbuf, 2);
    if (check_status == 0) {                // If status is OK
        for (int i=0; i<2; i++) {           // Convert bytes to decimal, starting with LSB then MSB
            while (rbuf[i] > 0)             // See bibliography for bin->dec reference code
            {
                div = rbuf[i] % 10;
                decimal_val = decimal_val + div * base;
                rbuf[i] = rbuf[i] / 10 ;
                base = base * 2;
            }
        }
    }
    return decimal_val;
}

/* Function to obtain UV index (from 0 to 11+)
 This is displayed on the LCD in State 2
 */
int get_UVI(void)
{
    uint8_t uva_address = UVA_REG;              // Initialize ptrs to internal addresses
    uint8_t *uva_ptr = &uva_address;
    uint8_t uvb_address = UVB_REG;
    uint8_t *uvb_ptr = &uvb_address;
    uint8_t uvcomp1_address = UVCOMP1_REG;
    uint8_t *uv_comp1_ptr = &uvcomp1_address;
    uint8_t uvcomp2_address = UVCOMP2_REG;
    uint8_t *uv_comp2_ptr = &uvcomp2_address;
    float uva_value = find_uv_value(uva_ptr);   // Find float UVA, UVB, UVCOMP1, UVCOMP2 values
    float uvb_value = find_uv_value(uvb_ptr);
    float uv_comp1 = find_uv_value(uv_comp1_ptr);
    float uv_comp2 = find_uv_value(uv_comp2_ptr);
    float uva_calc = uva_value - (uva_a_coeff*uv_comp1) - (uva_b_coeff*uv_comp2);   // Calculate UVA_calc, UVB_calc, UVIA, and UVIB
    float uvb_calc = uvb_value - (uvb_c_coeff*uv_comp1) - (uvb_d_coeff*uv_comp2);   // Formulas provided in VEML_6075 application sheet
    float uvia = uva_calc * uva_resp;
    float uvib = uvb_calc * uvb_resp;
    float uvi = (uvia + uvib) / 2;              // Calculate UV irradiance value
    uvi = round(40*uvi);                        // Multiply by 40 to obtain corresponding UV index
    uvi = (int)uvi;
    return uvi;
}

/* Function to obtain recommended tanning timer based on input arguments UV index and user's skin type
 This is displayed on the LCD in State 4
 */
int getTanningTime(int UVI, char *skinType)
{
    char fair_skin[] = "Fair";              // Create char arrays to match skin type options
    char med_skin[] = "Medium";
    char dark_skin[] = "Dark";
    int tanningTime = 0;                    // Initialize tanningTime variable
    if (0 < UVI < 2) {                      // If UV index is at low exposure level
        if (strncmp(skinType, fair_skin, 4) == 0)
            tanningTime = 40;
        else if (strncmp(skinType, med_skin, 6) == 0)
            tanningTime = 50;
        else if (strncmp(skinType, dark_skin, 4) == 0)
            tanningTime = 60;
    }
    else if (3 < UVI < 5) {                 // If UV index is at moderate exposure level
        if (strncmp(skinType, fair_skin, 4) == 0)
            tanningTime = 20;
        else if (strncmp(skinType, med_skin, 6) == 0)
            tanningTime = 30;
        else if (strncmp(skinType, dark_skin, 4) == 0)
            tanningTime = 45;
    }
    else if (6 < UVI < 7) {                 // If UV index is at high exposure level
        if (strncmp(skinType, fair_skin, 4) == 0)
            tanningTime = 10;
        else if (strncmp(skinType, med_skin, 6) == 0)
            tanningTime = 15;
        else if (strncmp(skinType, dark_skin, 4) == 0)
            tanningTime = 25;
    }
    else if (8 < UVI < 10) {                // If UV index is at very high exposure level
        if (strncmp(skinType, fair_skin, 4) == 0)
            tanningTime = 6;
        else if (strncmp(skinType, med_skin, 6) == 0)
            tanningTime = 10;
        else if (strncmp(skinType, dark_skin, 4) == 0)
            tanningTime = 15;
    }
    else if (11 < UVI < 15) {               // If UV index is at extreme exposure level
        if (strncmp(skinType, fair_skin, 4) == 0)
            tanningTime = 3;
        else if (strncmp(skinType, med_skin, 6) == 0)
            tanningTime = 6;
        else if (strncmp(skinType, dark_skin, 4) == 0)
            tanningTime = 10;
    }
    return tanningTime;
}
