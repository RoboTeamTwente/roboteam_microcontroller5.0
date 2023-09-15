#include "stm32f7xx_hal.h"
#include "stdbool.h"
#include "fatfs.h"
#include "sdcard.h"
#include "gpio_util.h"
#include "tim_util.h"
#include "logging.h"

///////////////////////////////////////////////////// STRUCTS

///////////////////////////////////////////////////// PRIVATE VARIABLES

static bool sd_initialized = false;
static uint32_t total_bytes_written = 0;
static uint32_t time_last_sync = 0;
static FATFS fs;
static FIL file;

///////////////////////////////////////////////////// PRIVATE FUNCTION DECLARATIONS

///////////////////////////////////////////////////// PUBLIC FUNCTION IMPLEMENTATIONS

/**
 * @brief Mount the SD card, delete all empty log files, and create a new log file
 */
bool SDCard_Init(){
    // Don't initialize if already initialized
	if(sd_initialized) return true;

    FRESULT result;
    DIR directory;
    FILINFO file_info;

    // Mount the SD card immediately
	result = f_mount(&fs, "", 1);
    if(result != FR_OK) return false;

    // Open the root directory
    result = f_opendir(&directory, "");
    if(result != FR_OK) return false;

    // List all files on the sdcard, and count the number of log files, by checking if the filename ends with ".BIN"
    // Also immediately delete any empty log files
    uint32_t n_log_files = 0;
       
    for(;;){
        result = f_readdir(&directory, &file_info);                 // Read a directory item
        if(result != FR_OK || file_info.fname[0] == 0) break;       // Break on error or end of directory
        if(file_info.fname[0] == '.') continue;                     // Ignore root directory
        // Delete empty files while we're at it. 
        if(file_info.fsize == 0) {
            f_unlink(file_info.fname);                              // Delete the file
            // LOG_printf("[sdcard.c:"STRINGIZE(__LINE__)"] Deleted empty file: %s\n", file_info.fname);
        }else{
            if(strstr(file_info.fname, ".BIN") != NULL) n_log_files++;  // If the filename ends with ".BIN", increment the counter
        }
    }

    // Create a new log file name
    char logFileName[20];
    sprintf(logFileName, "LOG%04d.BIN", n_log_files);
    LOG_printf("[sdcard.c:"STRINGIZE(__LINE__)"] Creating new log file: %s\n", logFileName);
    
    // Open the log file, and overwrite it if it already exists (which it shouldn't)
    result = f_open(&file, logFileName, FA_CREATE_ALWAYS | FA_WRITE);
    if(result != FR_OK) return false;

    // Set the time of the last sync to now
    time_last_sync = HAL_GetTick();

    sd_initialized = true;
    return true;
}

bool SDCard_Write(uint8_t* data, uint32_t length, bool no_sync){
    if(!sd_initialized) return false;

    uint32_t bytes_written = 0;
    FRESULT result = f_write(&file, data, length, &bytes_written);
    
    // Increment the total number of bytes written
    total_bytes_written += bytes_written;

    // If the number of bytes written is not equal to the number of bytes we wanted to write,
    // return false. This indicates that the SD card is full.
    if(length != bytes_written) return false;
    if(result != FR_OK) return false;

    // If more than 30 seconds passed, sync the file to the SD card. This ensures that the data
    // is written to the SD card. However, we don't want to do this too often, because it takes
    // a lot of time, and it can cause the robot to stutter. Measurements indicated that it can
    // take up to 10ms to sync the file.
    if(!no_sync && HAL_GetTick() - time_last_sync > 15000){
        __HAL_TIM_SET_COUNTER(TIM_1us, 0);
        result = f_sync(&file);
        uint32_t sync_time = __HAL_TIM_GET_COUNTER(TIM_1us);
        // LOG_printf("[sdcard.c:"STRINGIZE(__LINE__)"] sync time: %luus\n", sync_time);
        if(result != FR_OK) return false;
        time_last_sync = HAL_GetTick();
    }
    
    return true;
}

bool SDCard_Present(){
    return read_Pin(SD_CD_pin) == 1;
}

bool SDCard_Initialized(){
    return sd_initialized;
}

uint32_t SDCard_GetTotalBytesWritten(){
    return total_bytes_written;
}

///////////////////////////////////////////////////// PRIVATE FUNCTION IMPLEMENTATIONS