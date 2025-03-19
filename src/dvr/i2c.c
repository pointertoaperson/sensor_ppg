#include "i2c.h"

void i2c_init(void) {
    TWSR = 0;
    TWBR = 32; // 100kHz @ 16MHz CPU
}

void i2c_start(uint8_t address) {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Start condition
    while (!(TWCR & (1 << TWINT)));
    i2c_write(address << 1); // Send address with write operation
}

void i2c_write(uint8_t data) {
    TWDR = data; // Load data into the data register
    TWCR = (1 << TWEN) | (1 << TWINT); // Initiate transfer
    while (!(TWCR & (1 << TWINT))); // Wait until transfer is complete
}

void i2c_stop(void) {
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); // Stop condition
    while (TWCR & (1 << TWSTO)); // Wait until the stop condition is executed
}
