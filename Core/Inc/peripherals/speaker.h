/**
 * @addtogroup Peripherals 
 * @{ 
 * 
 * @addtogroup Speaker
 * @{
 * 
 * @file speaker.h
 * @author Emiel
 * @brief The speaker that is located on the robot
 * 
 * @date 06-09-2022
 * 
 */
#ifndef __SPEAKER_H
#define __SPEAKER_H

#include "REM_RobotMusicCommand.h"
#include "stdint.h"


void speaker_Play();
void speaker_Pause();
void speaker_NextSong();
void speaker_PreviousSong();
void speaker_VolumeUp();
void speaker_VolumeDown();
void speaker_Stop();

void speaker_SelectSong(uint8_t folder, uint8_t song);
void speaker_Setvolume(uint8_t level);
void speaker_HandleCommand(REM_RobotMusicCommand* rmc);

#endif /* __SPEAKER_H */