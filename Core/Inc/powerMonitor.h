#ifndef __POWERMONITOR__H_
#define __POWERMONITOR__H_

/**
 * @brief Implementation to control the INA234
 * 
 * The INA234 is a digital current-sense amplifier with an I2C- and SMBus-compatible interface. The device reports
 * the sensed current and features programmable out-of-range limits to issue alerts when the current is outside
 * the normal range of operation. The integrated analog-to-digital converter (ADC) can be set to different averaging
 * modes and configured for continuous-versus-triggered operation.
 */

/* 7.6.1 Device Registers */
#define PM_CONFIGURATION_REGISTER 0x0
#define PM_SHUNT_VOLTAGE_REGISTER 0x1
#define PM_BUS_VOLTAGE_REGISTER 0x2
#define PM_POWER_REGISTER 0x3
#define PM_CURRENT_REGISTER 0x4
#define PM_CALIBRATION_REGISTER 0x5
#define PM_MASK_ENABLE_REGISTER 0x6
#define PM_ALERT_LIMIT_REGISTER 0x7
#define PM_MANUFACTURER_ID_REGISTER 0x3E
#define PM_DEVICE_ID_REGISTER 0x3F

/* 7.6.1.1 Configuration Register (Address = 0h) [reset = 4127h] */
#define PM_CONFIGURATION_RST 15         // Set this bit to '1' to generate a system reset that is the same as power-on reset
#define PM_CONFIGURATION_ADCRANGE 12    // Enables the selection of the shunt full scale input across IN+ and IN–
#define PM_CONFIGURATION_AVG 9          // Sets the number of ADC conversion results to be averaged
#define PM_CONFIGURATION_VBUSCT 6       // Sets the conversion time of the VBUS measurement
#define PM_CONFIGURATION_VSHCT 3        // Sets the conversion time of the SHUNT measurement
#define PM_CONFIGURATION_MODE 0         // Operating mode, modes can be selected to operate the device either in Shutdown mode, continuous mode or triggered mode

/* 7.6.1.2 Shunt Voltage Register (Address = 1h) [reset = 0000h] */
#define PM_SHUNT_VOLTAGE_VSHUNT 4       // Differential voltage measured across the shunt output. Two's complement value

/* 7.6.1.3 Bus Voltage Register (Address = 2h) [reset = 0000h] */
#define PM_BUS_VOLTAGE_VBUS 4           // These bits readout the bus voltage of the system

/* 7.6.1.4 POWER Register (Address = 3h) [reset = 0000h] */
#define PM_POWER_POWER 0                // This bit returns a calculated value of power in the system. This is an unsigned result



#endif /* __POWERMONITOR__H_ */