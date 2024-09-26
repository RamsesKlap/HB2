#include "vl53l7cx.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../inc/tm4c123gh6pm.h"
#include "../../inc/hw_i2c.h"
#include "../../inc/hw_memmap.h"
#include "../../inc/hw_types.h"
#include "../../driverlib/gpio.h"
#include "../../driverlib/i2c.h"
#include "../../driverlib/pin_map.h"
#include "../../driverlib/sysctl.h"

uint8_t _I2CBuffer[256];
#define DELAY_1MS 1350
#define ERR 1
#define ALL_GOOD 0

/*
    \brief Subroutine to delay 20 milliseconds
    \param None
    \return None
    \note Assumes 16 MHz clock
*/
void Delay(uint32_t delay) {
    for (volatile uint32_t i = DELAY_1MS * delay; i > 0; i--) {}
}

void I2CInit(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PCTL_PA7_I2C1SDA);
    GPIOPinConfigure(GPIO_PCTL_PA6_I2C1SCL);

    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);
}

uint8_t _I2CWrite(uint16_t dev, uint32_t base, uint8_t address, uint8_t *pData, uint32_t count) {
    (void) (dev);
    I2CMasterSlaveAddrSet(base, address, false);
    for (int i = 0; i < count; i++) {
        I2CMasterDataPut(base, (pData + i));
        while(I2CMasterbusy(base)) {}
    }
    return ALL_GOOD;
}

uint8_t _I2CRead(uint16_t dev, uint32_t base, uint8_t address, uint8_t *pData, uint32_t count) {
    (void) (dev);
    I2CMasterSlaveAddrSet(base, address, true);

    for (int i = 0; i < count; i++) {
        I2CMasterControl(base, I2C_MASTER_CMD_SINGLE_RECEIVE);
        pData[i] = I2CMasterDataGet(base);
        Delay(2);
    }
    return ALL_GOOD;
}