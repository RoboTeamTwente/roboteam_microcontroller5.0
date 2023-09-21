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

#include <stdint.h>
#include <math.h>

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

/* 7.5.1 A0 PIN states */
#define GND  0b1000000
#define VS   0b1000001
#define SDA  0b1000010
#define SCL  0b1000011


/* 7.6.1.1 Configuration Register (Address = 0h) [reset = 4127h] */
#define PM_CONFIGURATION_RST 15         // Set this bit to '1' to generate a system reset that is the same as power-on reset
#define PM_CONFIGURATION_ADCRANGE 12    // Enables the selection of the shunt full scale input across IN+ and IN–. 0=81.92mV, 1=20,48mV
#define PM_CONFIGURATION_AVG 9          // Sets the number of ADC conversion results to be averaged
#define PM_CONFIGURATION_VBUSCT 6       // Sets the conversion time of the VBUS measurement
#define PM_CONFIGURATION_VSHCT 3        // Sets the conversion time of the SHUNT measurement
#define PM_CONFIGURATION_MODE 0         // Operating mode, modes can be selected to operate the device either in Shutdown mode, continuous mode or triggered mode
// PM_CONFIGURATION_RST
#define PM_CONFIGURATION_RST_NORMAL_OPERATION 0
#define PM_CONFIGURATION_RST_SYSTEM_RESET     1
// PM_CONFIGURATION_ADCRANGE
#define PM_CONFIGURATION_ADCRANGE_8192 0
#define PM_CONFIGURATION_ADCRANGE_2048 1

// TODO naming. Calling something _ENUM is lame but "PM_CONFIGURATION_AVG" is already taken
// PM_CONFIGURATION_AVG
typedef enum {
    PM_CONFIGURATION_AVG_1    = 0b000,
    PM_CONFIGURATION_AVG_4    = 0b001,
    PM_CONFIGURATION_AVG_16   = 0b010,
    PM_CONFIGURATION_AVG_64   = 0b011,
    PM_CONFIGURATION_AVG_128  = 0b100,
    PM_CONFIGURATION_AVG_256  = 0b101,
    PM_CONFIGURATION_AVG_512  = 0b110,
    PM_CONFIGURATION_AVG_1024 = 0b111,
} PM_CONFIGURATION_AVG_ENUM;

// PM_CONFIGURATION_VBUSCT and PM_CONFIGURATION_VSHCT
#define PM_CONFIGURATION_VBUSCT_VSHCT_140  0b000
#define PM_CONFIGURATION_VBUSCT_VSHCT_204  0b001
#define PM_CONFIGURATION_VBUSCT_VSHCT_332  0b010
#define PM_CONFIGURATION_VBUSCT_VSHCT_588  0b011
#define PM_CONFIGURATION_VBUSCT_VSHCT_1100 0b100
#define PM_CONFIGURATION_VBUSCT_VSHCT_2116 0b101
#define PM_CONFIGURATION_VBUSCT_VSHCT_4156 0b110
#define PM_CONFIGURATION_VBUSCT_VSHCT_8244 0b111
// PM_CONFIGURATION_MODE
#define PM_CONFIGURATION_MODE_SHUTDOWN                        0b000
#define PM_CONFIGURATION_MODE_SHUNT_VOLTAGE_TRIGGERED         0b001
#define PM_CONFIGURATION_MODE_BUS_VOLTAGE_TRIGGERED           0b010
#define PM_CONFIGURATION_MODE_SHUNT_AND_BUS_VOLTAGE_TRIGGERED 0b011
// #define PM_CONFIGURATION_MODE_SHUTDOWN                     0b100
#define PM_CONFIGURATION_MODE_CONTINUOUS_SHUNT_VOLTAGE        0b101
#define PM_CONFIGURATION_MODE_CONTINUOUS_BUS_VOLTAGE          0b110
#define PM_CONFIGURATION_MODE_SHUNT_AND_BUS_VOLTAGE           0b111



/* 7.6.1.2 Shunt Voltage Register (Address = 1h) [reset = 0000h] */
#define PM_SHUNT_VOLTAGE_VSHUNT 4       // Differential voltage measured across the shunt output. Two's complement value

/* 7.6.1.3 Bus Voltage Register (Address = 2h) [reset = 0000h] */
#define PM_BUS_VOLTAGE_VBUS 4           // These bits readout the bus voltage of the system

/* 7.6.1.4 POWER Register (Address = 3h) [reset = 0000h] */
#define PM_POWER_POWER 0                // This bit returns a calculated value of power in the system. This is an unsigned result

/* 7.6.1.5 CURRENT Register (Address = 4h) [reset = 0000h] */
#define PM_CURRENT_CURRENT 4            // Calculated current output in Amperes. Two's complement value

/* 7.6.1.6 Calibration Register (Address = 5h) [reset = 0000h] */
#define PM_CALIBRATION_SHUNT_CAL 0      // Programmed value needed for doing the shunt voltage to current conversion

/* 7.6.1.7 Mask/Enable Register (Address = 6h) [reset = 0000h] */
#define PM_MASK_ENABLE_SOL 15           // ALERT pin is asserted on Shunt Over-limit
#define PM_MASK_ENABLE_SUL 14           // ALERT pin is asserted on Shunt Under-limit, given that bits above are not set
#define PM_MASK_ENABLE_BOL 13           // ALERT pin is asserted on Bus   Over-limit,  given that bits above are not set
#define PM_MASK_ENABLE_BUL 12           // ALERT pin is asserted on Bus   Under-limit, given that bits above are not set
#define PM_MASK_ENABLE_POL 11           // ALERT pin is asserted on Power Over-limit,  given that bits above are not set
#define PM_MASK_ENABLE_CNVR 10          // Setting this bit high configures the ALERT pin to be asserted when the Conversion Ready Flag, Bit 3, is asserted indicating that the device is ready for the next conversion
#define PM_MASK_ENABLE_MemError 5       // CRC or ECC error
#define PM_MASK_ENABLE_AFF 4            // When the Alert Latch Enable bit is set to Latch mode, the Alert Function Flag bit clears only when the Mask/Enable Register is read. When the Alert Latch Enablebit is set to Transparent mode, the Alert Function Flag bit is cleared following the next conversion that does not result in an Alert condition
#define PM_MASK_ENABLE_CVRF 3           // The Conversion Ready Flag bit is set after all conversions, averaging, and multiplications are complete
#define PM_MASK_ENABLE_OVF 2            // This bit is set to '1' if an arithmetic operation resulted in an overflow error. It indicates that current and power data may be invalid
#define PM_MASK_ENABLE_APOL 1           // Alert Polarity bit sets the Alert pin polarity. 0b = Normal (Active-low open drain). 1b= Inverted (active-high )
#define PM_MASK_ENABLE_LEN 0            // When the Alert Latch Enable bit is set to Latch mode, the Alert pin and AFF bit remains active following a fault until this register flag has been read. This bit must be set to use the I2C Alert Response function.

/* 7.6.1.8 Alert Limit Register (Address = 7h) [reset = 0000h] */
#define PM_ALERT_LIMIT_LIMIT 0          // The Alert Limit Register contains the value used to compare to the register selected in the Mask/Enable Register

/* 7.6.1.9 Manufacturer ID Register (Address = 3Eh) [reset = 5449h] */
#define PM_MANUFACTURER_ID_MANUFACTURE_ID 0 // Reads back TI in ASCII

/* 7.6.1.10 Device ID Register (Address = 3Fh) [reset = A480h] */
#define PM_DEVICE_ID_DIEID 3            // Stores the device identification bits





/* 8.1.2 Current and Power Calculations
*   Equation 1 : SHUNT_CAL        = 0.08192 / ( CURRENT_LSB * R_SHUNT )   If ADCRANGE = 1, SHUNT_CAL /= 4
*   Equation 2 : CURRENT_LSB(min) = MAX_EXPECTED_CURRENT / 2^11
* After programming the SHUNT_CAL register with the calculated value, the measured current in Amperes can be read from the CURRENT register. Use Equation 3 to calculate the final value scaled by the CURRENT_LSB
*   Equation 3 : CURRENT [A]      = CURRENT_LSB * CURRENT
* The power value can be read from the POWER register as a 16-bit value. Use Equation 4 to convert the power to Watts
*   Equation 4 : POWER [W]        = 32 * CURRENT_LSB * POWER
*/

/* 8.2.1 Design parameters */
// struct Design_Parameters_ {
//     float power_supply_voltage;
//     float bus_supply_rail;
//     float average_current;
//     float overcurrent_fault_threshold;
//     float maximum_current_monitored;
//     float ADC_range_selection;
// } Design_Parameters = {
//     3.3,
//     12,
//     6,
//     9,
//     10,
//     81.92
// };


/* 8.2.3.5 Calculate Returned Values by LSB Value 
*  The received contents of shunt voltage, current & bus voltage need to be right shifted by 4 bits
*  After shifting the contents convert it to decimals before multiplying it by the correct LSB value
*/
#define PM_LSB_ALERT_LIMIT     = 40 * 10^-6    //40 microvolt
#define PM_LSB_SHUNT_VOLTAGE   = 40 * 10^-6    //40 microvolt
#define PM_LSB_BUS_VOLTAGE     = 25.6 * 10^-3  //25.6 millivolt
#define PM_LSB_CURRENT         = 5 * 10^-3     //5 milliampere



#endif /* __POWERMONITOR__H_ */