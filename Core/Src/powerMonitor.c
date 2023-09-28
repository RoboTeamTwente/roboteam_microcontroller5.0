#include "powerMonitor.h"
#include "peripheral_util.h"
#include "main.h"

uint8_t  ACKNOWLEDGEMENT;    // TO DO :: we only need 1 bit here to change if possible
uint8_t  I2C_return;         // for the return of the  errors
uint8_t  REGISTER_POINTER = 0b11111111;   // current address the INA234 is pointed 
uint16_t RECEIVED_DATA;      // supposed to be a buffer 
HAL_StatusTypeDef I2C_return; // Process the 


/*
    Master (controller), sends the first byte indicating the which slave it is talking too, 
    in this case the voltage meter.
        The slave replies with an acknowledgment bit == 0,
    After the master recives the acknowledgment bit, we then send the byte indicating which register it wants
        then antoher acknowledgment bit by the salve == 0,
    

    address << 1        for transmitting data
    address << 1 | 0x01 for reading data from slave 
*/
void readData(uint8_t registerToReadFrom){
    if (REGISTER_POINTER != registerToReadFrom)
        changeRegisterPointer(registerToReadFrom);
    
    if (I2C_return != HAL_OK){
        return;
    }

    uint8_t secondHalf;
    HAL_I2C_Master_Receive(BATT_I2C, VS << 1| 0x1, RECEIVED_DATA, sizeof(uint8_t), 10);
    HAL_I2C_Master_Receive(BATT_I2C, VS << 1| 0x1, secondHalf, sizeof(uint8_t), 10);

    RECEIVED_DATA = RECEIVED_DATA << 8;
    RECEIVED_DATA = RECEIVED_DATA | secondHalf;
    char buf[25];
    snprintf(buf, 25, "[POWERMONITOR] : %d \n", RECEIVED_DATA);
    LOG(buf);
    free(buf);
}

/*
    This method will request data from the INA234
    First we change the register pointer of the INA234
        - Done by setting the A0 PIN to SDA_0
        - We should recieve a 0 after the SDA implying an acnkowledge
    After we send a a byte indicating the register pointer
        - We should then recieve a0 indicating acknowledege
*/
void changeRegisterPointer(uint8_t registerToChange){
    I2C_return = HAL_I2C_Master_Transmit(BATT_I2C, SDA << 1 | 0x0, registerToChange, sizeof(registerToChange), 50);
    if (I2C_return != HAL_OK){ 
        LOG("[POWERMONITOR] : Failed to change pointer\n");
        return;
    }
    LOG("[POWERMONITOR] : Changed pointer register\n");
    REGISTER_POINTER = registerToChange;
}

/*
    This is a write operation
        - We first set the A0 pin to SDA_0, we then recieve a 0 indicating acnkowledge
        - We send the the register we want to write to, we then recieve a 0 indicating acnkowledge
        - We send the first byte, we then recieve a 0 indicating acnkowledge
        - Followed by the second byte, we then recieve a 0 indicating acnkowledge
*/
void writeData(uint16_t dataToBeWritten){
    
    uint8_t MSB_write = dataToBeWritten >> 8;
    uint8_t LSB_write = dataToBeWritten;

    HAL_I2C_Master_Transmit(BATT_I2C, SDA << 1, LSB_write, sizeof(uint8_t), 10);
    I2C_return = HAL_I2C_Master_Transmit(BATT_I2C, SDA << 1, MSB_write, sizeof(uint8_t), 10);
    if (I2C_return != HAL_OK){ 
        LOG("[POWERMONITOR] : Failed to write\n");
        return;
    }
    LOG("[POWERMONITOR] : Sucesful write\n");
}

void init_BattMeter(){
    RECEIVED_DATA = 0b0000000000000000;       
    I2C_return = BATT_I2C -> State;
    if (I2C_return == HAL_I2C_STATE_READY )
        LOG("[POWERMONITOR] : Checked state manually, it's ready\n");
    I2C_return = HAL_I2C_IsDeviceReady(BATT_I2C, SDA << 1 | 0x1, 10, 50);
    if (I2C_return  == HAL_OK )
        LOG("[POWERMONITOR] : isReady() returned True\n");
        changeRegisterPointer(PM_CONFIGURATION_REGISTER);
        writeData(CONFIGURATION_BYTES);
}
