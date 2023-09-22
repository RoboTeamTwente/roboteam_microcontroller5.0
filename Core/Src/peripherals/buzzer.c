#include <stdbool.h>
#include "buzzer.h"
#include "tim_util.h"
#include "buzzer_tunes.h"

uint32_t buzzer_Duration = 0;
bool buzzer_initialized = false;
song_struct* song;
song_struct song_single_note[] = {{buzz_Si, 0}, {0xFFFF, 0}};

///////////////////////////////////////////////////// PUBLIC FUNCTIONS IMPLEMENTATIONS

void buzzer_Init() {
	HAL_TIM_Base_Stop(PWM_Buzzer.TIM);
	buzzer_Duration = 0;
	// play a sound to inform that we are aliiiiiiiiive
	// song = quickBeepUp;
	// buzzer_Play(song);
	buzzer_initialized = true;
}

void buzzer_DeInit() {
	HAL_TIM_PWM_Stop(PWM_Buzzer.TIM, PWM_Buzzer.Channel);
	HAL_TIM_Base_Stop_IT(PWM_Buzzer.TIM);
}

bool buzzer_IsPlaying(){
	return song == NULL;
}

void buzzer_Callback() {
	if(!buzzer_initialized) return;

	if(song){
		if(buzzer_Duration != 0){
			buzzer_Duration--;
		} else {
			song++;
			HAL_TIM_PWM_Stop(PWM_Buzzer.TIM, PWM_Buzzer.Channel);
			HAL_TIM_Base_Stop_IT(PWM_Buzzer.TIM);
			if(song->period == 0xFFFF){
				__HAL_TIM_SET_COUNTER(PWM_Buzzer.TIM, 0);
				song = NULL;
				return;
			}
			buzzer_Play(song);
		}
	}
}

void buzzer_SetPWM_Duty(uint16_t duty) {
	set_PWM(PWM_Buzzer, duty);
}

void buzzer_SetPWM_Period(uint16_t period) {
	// HAL library macro to set timer PWM period in runtime
	__HAL_TIM_SET_AUTORELOAD(PWM_Buzzer.TIM, period);
}

void buzzer_Play_note(uint16_t period, float duration){
	song = song_single_note;
	song_single_note->period = period;
	song_single_note->duration = duration;
	buzzer_Play(song_single_note);
	return;
}

void buzzer_Play(song_struct* tone) {
	if(!buzzer_initialized) return;
	
	song = tone;

	uint16_t tone_period = (tone->period == 0) ? 0xFFFF : ((1e6 / tone->period)-1);
	buzzer_Duration = (0.9e6F / tone_period) * (tone->duration);

	__HAL_TIM_SET_COUNTER(PWM_Buzzer.TIM, 0);
	buzzer_SetPWM_Period(tone_period);
	buzzer_SetPWM_Duty(tone_period/2);
	HAL_TIM_Base_Start_IT(PWM_Buzzer.TIM);

	if(tone_period != 0xFFFF)
		HAL_TIM_PWM_Start(PWM_Buzzer.TIM, PWM_Buzzer.Channel);	
}

void buzzer_Play_QuickBeepUp(){
	song = quickBeepUp;
	buzzer_Play(song);
}

void buzzer_Play_QuickBeepDown(){
	song = quickBeepDown;
	buzzer_Play(song);
}

void buzzer_Play_Startup(){
	song = startup_song;
	buzzer_Play(song);
}

void buzzer_Play_Tetris(){
	song = tetris;
	buzzer_Play(song);
}

void buzzer_Play_Mario(){
	song = mario_victory;
	buzzer_Play(song);
}

void buzzer_Play_PinkPanther(){
	song = PinkPanther;
	buzzer_Play(song);
}

void buzzer_Play_BatteryDrainWarning(){
	song = batteryDrainWarning;
	buzzer_Play(song);
}

void buzzer_Play_WarningOne() {
	song = warningOne;
	buzzer_Play(song);
}

void buzzer_Play_WarningTwo() {
	song = warningTwo;
	buzzer_Play(song);
}

void buzzer_Play_WarningThree() {
	song = warningThree;
	buzzer_Play(song);
}

void buzzer_Play_WarningFour() {
	song = warningFour;
	buzzer_Play(song);
}

void buzzer_Play_WarningGit() {
	song = warningGit;
	buzzer_Play(song);
}

void buzzer_Play_BridgeBattle() {
	song = bridgeBattle;
	buzzer_Play(song);
}

void buzzer_Play_ImperialMarch() {
	song = imperialMarch;
	buzzer_Play(song);
}

void buzzer_Play_Flatline() {
	song = flatLine;
	buzzer_Play(song);
}


void buzzer_Play_HBD() {
	song = HBD;
	buzzer_Play(song);
}

/* If it's ugly and it works, is it still ugly? */
void buzzer_Play_ID(uint8_t id){
	if(id ==  0){ song = id0; }
	if(id ==  1){ song = id1; }
	if(id ==  2){ song = id2; }
	if(id ==  3){ song = id3; }
	if(id ==  4){ song = id4; }
	if(id ==  5){ song = id5; }
	if(id ==  6){ song = id6; }
	if(id ==  7){ song = id7; }
	if(id ==  8){ song = id8; }
	if(id ==  9){ song = id9; }
	if(id == 10){ song = id10;}
	if(id == 11){ song = id11;}
	if(id == 12){ song = id12;}
	if(id == 13){ song = id13;}
	if(id == 14){ song = id14;}
	if(id == 15){ song = id15;}
	buzzer_Play(song);
}

song_struct beep_blue[] = {{buzz_C4, 0.5F}, {0xFFFF, 0}};
song_struct beep_yellow[] = {{buzz_C4, 0.2F}, {buzz_Si,0.1F}, {buzz_C4, 0.2F}, {0xFFFF, 0}};

song_struct quickBeepUp[] = {{buzz_C4, 0.1F}, {buzz_C5, 0.1F}, {0xFFFF, 0}};
song_struct quickBeepDown[] = {{buzz_C5, 0.1F}, {buzz_C4, 0.1F}, {0xFFFF, 0}};

song_struct startup_song[] = {{buzz_C4, 0.1F}, {buzz_D4, 0.1F}, {buzz_E4, 0.1F}, {buzz_F4, 0.1F}, {buzz_G4, 0.1F}, {buzz_A4, 0.1F}, {buzz_B4, 0.1F}, {buzz_C5, 0.1F}, {0xFFFF, 0}};
song_struct tetris[] = {{buzz_E5, Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5, Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_A4, Tbeat}, {buzz_A4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_E5, Tbeat}, {buzz_D5, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, Tbeat}, {buzz_Si, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5, Tbeat}, {buzz_E5, Tbeat}, {buzz_C5, Tbeat}, {buzz_A4, Tbeat}, {buzz_A4, Tbeat}, {buzz_Si, 1.5F*Tbeat}, {buzz_D5, Tbeat}, {buzz_F5, 0.5F*Tbeat}, {buzz_A5, 0.5F*Tbeat}, {buzz_A5, 0.5F*Tbeat}, {buzz_G5, 0.5F*Tbeat}, {buzz_F5, 0.5F*Tbeat}, {buzz_E5, Tbeat}, {buzz_Si, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_E5, Tbeat}, {buzz_D5, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5,Tbeat}, {buzz_E5,Tbeat}, {buzz_C5,Tbeat}, {buzz_A4,Tbeat}, {buzz_A4,Tbeat},{buzz_E5,Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5,Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_A4,Tbeat}, {buzz_A4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_E5,Tbeat}, {buzz_D5, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, Tbeat}, {buzz_Si, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5,Tbeat}, {buzz_E5,Tbeat}, {buzz_C5,Tbeat}, {buzz_A4,Tbeat}, {buzz_A4,Tbeat}, {buzz_Si,1.5F*Tbeat}, {buzz_D5, Tbeat}, {buzz_F5, 0.5F*Tbeat}, {buzz_A5, 0.5F*Tbeat}, {buzz_A5, 0.5F*Tbeat}, {buzz_G5, 0.5F*Tbeat}, {buzz_F5, 0.5F*Tbeat}, {buzz_E5, Tbeat}, {buzz_Si, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_E5,Tbeat}, {buzz_D5, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_B4, Tbeat}, {buzz_B4, 0.5F*Tbeat}, {buzz_C5, 0.5F*Tbeat}, {buzz_D5,Tbeat}, {buzz_E5,Tbeat}, {buzz_C5,Tbeat}, {buzz_A4,Tbeat}, {buzz_A4,Tbeat}, {buzz_E4,2*Tbeat}, {buzz_C4,2*Tbeat}, {buzz_D4,2*Tbeat}, {buzz_B3,2*Tbeat}, {buzz_C4,2*Tbeat}, {buzz_A3,2*Tbeat}, {buzz_GS3,2*Tbeat}, {buzz_B3,Tbeat}, {buzz_Si,Tbeat}, {buzz_E4,2*Tbeat}, {buzz_C4,2*Tbeat}, {buzz_D4,2*Tbeat}, {buzz_B3,2*Tbeat}, {buzz_C4,Tbeat}, {buzz_E4,Tbeat}, {buzz_A4,2*Tbeat}, {buzz_GS4,3*Tbeat}, {buzz_Si,Tbeat}, {0xFFFF, 0}};
song_struct mario_victory[] = {{buzz_GS3,Mbeat/3.0F}, {buzz_CS4,Mbeat/3.0F}, {buzz_F4,Mbeat/3.0F}, {buzz_GS4,Mbeat/3.0F}, {buzz_CS5,Mbeat/3.0F}, {buzz_F5,Mbeat/3.0F}, {buzz_GS5,Mbeat}, {buzz_F5,Mbeat}, {buzz_A3,Mbeat/3.0F}, {buzz_CS4,Mbeat/3.0F}, {buzz_E4,Mbeat/3.0F}, {buzz_A4,Mbeat/3.0F}, {buzz_CS5,Mbeat/3.0F}, {buzz_E5,Mbeat/3.0F}, {buzz_A5,Mbeat}, {buzz_E5,Mbeat}, {buzz_B3,Mbeat/3.0F}, {buzz_DS4,Mbeat/3.0F}, {buzz_FS4,Mbeat/3.0F}, {buzz_B4,Mbeat/3.0F}, {buzz_DS5,Mbeat/3.0F}, {buzz_FS5,Mbeat/3.0F}, {buzz_B5,Mbeat}, {buzz_B5,Mbeat/3.0F}, {buzz_B5,Mbeat/3.0F}, {buzz_B5,Mbeat/3.0F}, {buzz_CS6,Mbeat*2.0F},{0xFFFF, 0}};
song_struct batteryDrainWarning[] = {{buzz_C5,fBeat*0.5F},{buzz_G4,fBeat},{buzz_C5,fBeat},{buzz_E5,fBeat},{buzz_G5,fBeat},{buzz_C6,fBeat},{buzz_G5,fBeat},{buzz_C5,fBeat*0.5F},{buzz_GS4,fBeat},{buzz_C5,fBeat},{buzz_DS5,fBeat},{buzz_GS5,fBeat},{buzz_DS5,fBeat},{buzz_GS5,fBeat},{buzz_C6,fBeat},{buzz_DS6,fBeat},{buzz_GS6,fBeat},{buzz_DS6,fBeat},{buzz_D5,0.5F*fBeat},{buzz_AS4,fBeat},{buzz_D5,fBeat},{buzz_F5,fBeat},{buzz_AS5,fBeat},{buzz_F5,fBeat},{buzz_AS5,fBeat},{buzz_D6,fBeat},{buzz_F6,fBeat},{buzz_AS6,fBeat},{buzz_F6,fBeat},{0xFFFF, 0}};
song_struct warningOne[] = {{buzz_A5,0.1F},{buzz_E5,0.2F},{buzz_Si,0.1F},{buzz_A5,0.1F},{buzz_E5,0.2F},{buzz_Si,0.1F},{buzz_A5,0.1F},{buzz_E5,0.2F},{buzz_Si,0.1F},{0xFFFF, 0}};
song_struct warningTwo[] = {{buzz_C5,0.1F},{buzz_FS5,0.1F},{buzz_B5,0.6F},{buzz_C5,0.1F},{buzz_FS5,0.1F},{buzz_B5,0.1F},{buzz_Si,0.5F},{buzz_C5,0.1F},{buzz_FS5,0.1F},{buzz_B5,0.1F},{buzz_Si,0.5F},{buzz_C5,0.1F},{buzz_FS5,0.1F},{buzz_B5,0.1F},{buzz_Si,0.5F},{0xFFFF, 0}};
song_struct warningThree[] = {{buzz_FS6,0.66F},{buzz_D6,0.66F},{buzz_B5,0.66F},{buzz_G5,0.66F},{buzz_AS5,1.33F},{buzz_Si,1.76F},{buzz_FS6,0.66F},{buzz_D6,0.66F},{buzz_B5,0.66F},{buzz_G5,0.66F},{buzz_AS5,1.33F},{buzz_Si,1.76F},{buzz_FS6,0.66F},{buzz_D6,0.66F},{buzz_B5,0.66F},{buzz_G5,0.66F},{buzz_AS5,1.33F},{buzz_Si,1.76F},{0xFFFF, 0}};
song_struct warningFour[] = {{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_Si,0.2F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_Si,0.2F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_Si,0.2F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_Si,0.2F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_Si,0.2F},{0xFFFF, 0}};
song_struct warningRunningTest[] = {{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{buzz_G5,0.05F},{buzz_GS5,0.05F},{0xFFFF, 0}};

song_struct warningGit[] = {{buzz_G5,0.1F},{buzz_GS5,0.1F},{buzz_G3,0.1F},{buzz_GS3,0.1F},{0xFFFF, 0}};


song_struct bridgeBattle[] = {{buzz_F4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,halfBeat},{buzz_F4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,halfBeat},{buzz_F4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_AS4,halfBeat},{buzz_B4,halfBeat},{buzz_AS4,0.6666F*halfBeat},{buzz_B4,0.6666F*halfBeat},{buzz_AS4,0.6666F*halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_DS4,halfBeat},{buzz_F4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,halfBeat},{buzz_F4,halfBeat},{buzz_GS4,halfBeat},{buzz_G4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,9*halfBeat},
								{buzz_F4,halfBeat},{buzz_G4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_AS4,halfBeat},{buzz_GS4,halfBeat},{buzz_G4,halfBeat},{buzz_F4,halfBeat},{buzz_C5,halfBeat},{buzz_F4,halfBeat},{buzz_CS5,halfBeat},{buzz_F4,halfBeat},{buzz_C5,halfBeat},{buzz_GS4,2*halfBeat},{buzz_GS4,halfBeat},{buzz_AS4,halfBeat},{buzz_E4,halfBeat},{buzz_C5,halfBeat},{buzz_E4,halfBeat},{buzz_AS4,halfBeat},{buzz_G4,2*halfBeat},{buzz_G4,halfBeat},{buzz_GS4,halfBeat},{buzz_G4,halfBeat},{buzz_F4,halfBeat},{buzz_G4,halfBeat},{buzz_GS4,halfBeat},{buzz_AS4,halfBeat},{buzz_GS4,halfBeat},{buzz_G4,halfBeat},{buzz_F4,halfBeat},{buzz_G4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_AS4,halfBeat},{buzz_GS4,halfBeat},{buzz_G4,halfBeat},{buzz_F4,halfBeat},{buzz_C5,halfBeat},{buzz_F4,halfBeat},{buzz_CS5,halfBeat},{buzz_F4,halfBeat},{buzz_C5,halfBeat},{buzz_GS4,2*halfBeat},{buzz_GS4,halfBeat},{buzz_AS4,halfBeat},{buzz_E4,halfBeat},{buzz_C5,halfBeat},{buzz_E4,halfBeat},{buzz_AS4,halfBeat},{buzz_G4,2*halfBeat},{buzz_G4,halfBeat},{buzz_GS4,halfBeat},{buzz_AS4,halfBeat},{buzz_C5,halfBeat},{buzz_CS5,halfBeat},{buzz_DS5,halfBeat},{buzz_CS5,halfBeat},{buzz_C5,halfBeat},{buzz_CS5,halfBeat},
								{buzz_DS5,10*halfBeat},{buzz_C5,halfBeat},{buzz_CS5,halfBeat},{buzz_DS5,2*halfBeat},{buzz_G5,2*halfBeat},{buzz_F5,14*halfBeat},{buzz_Si,2*halfBeat},{buzz_AS4,3*halfBeat},{buzz_F4,3*halfBeat},{buzz_DS4,2*halfBeat},{buzz_D4,3*halfBeat},{buzz_DS4,3*halfBeat},{buzz_D4,2*halfBeat},{buzz_C4,3*halfBeat},{buzz_F4,3*halfBeat},{buzz_G4,2*halfBeat},{buzz_A4,3*halfBeat},{buzz_C5,3*halfBeat},{buzz_D5,2*halfBeat},{buzz_DS5,2*halfBeat},{buzz_D5,halfBeat},{buzz_C5,2*halfBeat},{buzz_D5,halfBeat},{buzz_DS5,5*halfBeat},{buzz_Si,halfBeat},{buzz_DS5,2*halfBeat},{buzz_F5,2*halfBeat},{buzz_G5,2*halfBeat},{buzz_F5,halfBeat},{buzz_DS5,2*halfBeat},{buzz_F5,halfBeat},{buzz_G5,8*halfBeat},{buzz_Si,2*halfBeat},{buzz_A5,2*halfBeat},{buzz_FS5,halfBeat},{buzz_DS5,halfBeat},{buzz_A4,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_DS5,2*halfBeat},{buzz_DS5,2*halfBeat},{buzz_C5,halfBeat},{buzz_F5,2*halfBeat},{buzz_DS5,2*halfBeat},{buzz_D5,8*halfBeat},{buzz_Si,halfBeat},{buzz_G4,halfBeat},{buzz_B4,halfBeat},{buzz_D5,halfBeat},{buzz_G5,halfBeat},{buzz_F5,halfBeat},{buzz_D5,halfBeat},{buzz_B4,halfBeat},
								{buzz_GS5,2*halfBeat},{buzz_G5,halfBeat},{buzz_Si,halfBeat},{buzz_FS5,2*halfBeat},{buzz_G5,halfBeat},{buzz_Si,halfBeat},{buzz_GS5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_G5,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_B4,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_FS5,0.5F*halfBeat},{buzz_CS5,0.5F*halfBeat},{buzz_AS4,0.5F*halfBeat},{buzz_CS5,0.5F*halfBeat},{buzz_G5,0.5F*halfBeat},{buzz_Si,0.5F*halfBeat},
								{buzz_C6,0.5F*halfBeat},{buzz_G5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_G5,0.5F*halfBeat},{buzz_AS5,0.5F*halfBeat},{buzz_F5,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_F5,0.5F*halfBeat},{buzz_GS5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_G5,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_B4,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_F5,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_AS4,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_G4,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_DS5,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_FS4,0.5F*halfBeat},{buzz_C5,0.5F*halfBeat},{buzz_D5,0.5F*halfBeat},{buzz_B4,0.5F*halfBeat},{buzz_G4,0.5F*halfBeat},{buzz_D4,0.5F*halfBeat},
								{buzz_C5,halfBeat},{buzz_C5,halfBeat},{buzz_Si,halfBeat},{buzz_C5,halfBeat},{buzz_DS5,halfBeat},{buzz_C5,halfBeat},{buzz_Si,halfBeat},{buzz_C5,halfBeat},{buzz_DS5,halfBeat},{buzz_C5,halfBeat},{buzz_F5,halfBeat},{buzz_FS5,halfBeat},{buzz_F5,0.6666F*halfBeat},{buzz_FS5,0.6666F*halfBeat},{buzz_F5,0.6666F*halfBeat},{buzz_DS5,halfBeat},{buzz_C5,halfBeat},{buzz_AS4,halfBeat},
								{buzz_GS5,3*halfBeat},{buzz_G5,3*halfBeat},{buzz_FS5,2*halfBeat},{buzz_Si,2*halfBeat},{buzz_DS5,2*halfBeat},{buzz_D5,2*halfBeat},{buzz_CS5,2*halfBeat},
								{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},
								{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},
								{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},{buzz_C5,halfBeat},{buzz_D5,halfBeat},
								{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},{buzz_B4,halfBeat},{buzz_CS4,halfBeat},
								{buzz_F4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,halfBeat},{buzz_F4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_Si,halfBeat},{buzz_F4,halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_AS4,halfBeat},{buzz_B4,halfBeat},{buzz_AS4,0.6666F*halfBeat},{buzz_B4,0.6666F*halfBeat},{buzz_AS4,0.6666F*halfBeat},{buzz_GS4,halfBeat},{buzz_F4,halfBeat},{buzz_DS4,halfBeat},
								{0xFFFF,0}};
song_struct imperialMarch[] = {{buzz_A4, 0.50F},{buzz_Si, 0.20F},{buzz_A4, 0.50F},
		{buzz_Si, 0.20F}, {buzz_A4, 0.50F},{buzz_Si, 0.20F},{buzz_F4, 0.40F},{buzz_Si, 0.05F},{buzz_C5, 0.20F},{buzz_Si, 0.05F},

		{buzz_A4, 0.60F},{buzz_Si, 0.10F},{buzz_F4, 0.40F},{buzz_Si, 0.05F}, {buzz_C5, 0.20F},
		{buzz_Si, 0.05F},{buzz_A4, 0.60F},{buzz_Si, 0.80F},

		{buzz_E5, 0.50F},{buzz_Si, 0.20F},
		{buzz_E5, 0.50F},{buzz_Si, 0.20F},{buzz_E5, 0.50F},{buzz_Si, 0.20F},{buzz_F5, 0.40F},
		{buzz_Si, 0.05F},{buzz_C5, 0.20F},{buzz_Si, 0.05F},

		{buzz_A4, 0.60F},{buzz_Si, 0.10F},{buzz_F4, 0.40F},{buzz_Si, 0.05F}, {buzz_C5, 0.20F},
		{buzz_Si, 0.05F},{buzz_A4, 0.60F},{buzz_Si, 0.80F},{0xFFFF, 0}};
song_struct flatLine[] = {
		{buzz_A7,0.10F},{buzz_Si,1.00F},{buzz_A7,0.10F},{buzz_Si,1.00F},{buzz_A7,0.10F},{buzz_Si,1.00F},{buzz_A7,0.10F},{buzz_Si,1.00F},{buzz_A7,0.10F},{buzz_Si,1.00F},
		{buzz_A7,0.10F},{buzz_Si,0.10F},{buzz_A7,0.10F},{buzz_Si,0.10F},{buzz_A7,0.10F},{buzz_Si,0.10F},{buzz_A7,0.10F},{buzz_Si,0.10F},{buzz_A7,0.10F},{buzz_Si,0.10F},
		{buzz_A7,5.0F},{0xFFFF, 0}};

song_struct HBD[] = {{buzz_G4, 0.7F*tempo},{buzz_Si, 0.1F*tempo},{buzz_G4, 0.2F*tempo},{buzz_A4, 1*tempo},{buzz_G4, 1*tempo}, {buzz_C5,1*tempo},{buzz_B4, 2*tempo},
					 {buzz_G4, 0.7F*tempo},{buzz_Si, 0.1F*tempo},{buzz_G4, 0.2F*tempo},{buzz_A4, 1*tempo},{buzz_G4, 1*tempo}, {buzz_D5,1*tempo},{buzz_C5, 2*tempo},
					 {buzz_G4, 0.7F*tempo},{buzz_Si, 0.1F*tempo},{buzz_G4, 0.2F*tempo},{buzz_G5, 1*tempo},{buzz_E5, 1*tempo}, {buzz_C5,1*tempo},{buzz_B4, 1*tempo},
					 {buzz_A4, 1*tempo},{buzz_F5, 0.7F*tempo},{buzz_Si, 0.1F*tempo},{buzz_F5, 0.2F*tempo},{buzz_E5, 1*tempo},{buzz_C5, 1*tempo}, {buzz_D5,1*tempo},
					 {buzz_C5, 2*tempo},
					 {0xFFFF, 0}};

song_struct GoT[] = {{buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_E4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_E4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_E4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.3583F}, {buzz_C4, 0.3583F}, {buzz_E4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 1.0750F}, {buzz_C4, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.7167F}, {buzz_C4, 0.7167F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_D4, 4.3000F}, {buzz_F4, 1.0750F}, {buzz_AS3, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_D4, 0.1792F}, {buzz_F4, 0.7167F}, {buzz_AS3, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_D4, 0.1792F}, {buzz_C4, 4.3000F}, {buzz_G4, 1.0750F}, {buzz_C4, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.7167F}, {buzz_C4, 0.7167F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_D4, 4.3000F}, {buzz_F4, 1.0750F}, {buzz_AS3, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_D4, 0.1792F}, {buzz_F4, 0.7167F}, {buzz_AS3, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_D4, 0.1792F}, {buzz_C4, 4.3000F}, {buzz_G4, 1.0750F}, {buzz_C4, 1.0750F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_G4, 0.7167F}, {buzz_C4, 0.7167F}, {buzz_DS4, 0.1792F}, {buzz_F4, 0.1792F}, {buzz_D4, 2.1500F}, {buzz_F4, 1.0750F}, {buzz_AS3, 1.0750F}, {buzz_D4, 0.5375F}, {buzz_DS4, 0.5375F}, {buzz_D4, 0.5375F}, {buzz_AS3, 0.5375F}, {buzz_C4, 4.3000F}, {buzz_C5, 2.1500F}, {buzz_AS4, 2.1500F}, {buzz_C4, 2.1500F}, {buzz_G4, 2.1500F}, {buzz_DS4, 2.1500F}, {buzz_DS4, 1.0750F}, {buzz_F4, 1.0750F}, {buzz_G4, 4.3000F}, {buzz_C5, 2.1500F}, {buzz_AS4, 2.1500F}, {buzz_C4, 2.1500F}, {buzz_G4, 2.1500F}, {buzz_DS4, 2.1500F}, {buzz_DS4, 1.0750F}, {buzz_D4, 1.0750F}, {buzz_C5, 0.3583F}, {buzz_G4, 0.3583F}, {buzz_GS4, 0.1792F}, {buzz_AS4, 0.1792F}, {buzz_C5, 0.3583F}, {buzz_G4, 0.3583F}, {buzz_GS4, 0.1792F}, {buzz_AS4, 0.1792F}, {buzz_C5, 0.3583F}, {buzz_G4, 0.3583F}, {buzz_GS4, 0.1792F}, {buzz_AS4, 0.1792F}, {buzz_C5, 0.3583F}, {buzz_G4, 0.3583F}, {buzz_GS4, 0.1792F}, {buzz_AS4, 0.1792F}, {buzz_Si, 0.7167F}, {buzz_GS5, 0.1792F}, {buzz_AS5, 0.1792F}, {buzz_C6, 0.3583F}, {buzz_G5, 0.3583F}, {buzz_GS5, 0.1792F}, {buzz_AS5, 0.1792F}, {buzz_C6, 0.3583F}, {buzz_G5, 0.1792F}, {buzz_GS5, 0.1792F}, {buzz_AS5, 0.1792F}, {buzz_C6, 0.3583F}, {buzz_G5, 0.3583F}, {buzz_GS5, 0.1792F}, {buzz_AS5, 0.1792F}, {0xFFFF, 0}};

song_struct HarryPotter[] = {{buzz_Si, 0.7167F}, {buzz_D4, 0.3583F}, {buzz_G4, 0.5375F}, {buzz_AS4, 0.1792F}, {buzz_A4, 0.3583F}, {buzz_G4, 0.7167F}, {buzz_D5, 0.3583F}, {buzz_C5, 1.0750F}, {buzz_A4, 1.0750F}, {buzz_G4, 0.5375F}, {buzz_AS4, 0.1792F}, {buzz_A4, 0.3583F}, {buzz_F4, 0.7167F}, {buzz_GS4, 0.3583F}, {buzz_D4, 2.1500F}, {buzz_D4, 0.3583F}, {buzz_G4, 0.5375F}, {buzz_AS4, 0.1792F}, {buzz_A4, 0.3583F}, {buzz_G4, 0.7167F}, {buzz_D5, 0.3583F}, {buzz_F5, 0.7167F}, {buzz_E5, 0.3583F}, {buzz_DS5, 0.7167F}, {buzz_B4, 0.3583F}, {buzz_DS5, 0.5375F}, {buzz_D5, 0.1792F}, {buzz_CS5, 0.3583F}, {buzz_CS4, 0.7167F}, {buzz_B4, 0.3583F}, {buzz_G4, 2.1500F}, {buzz_AS4, 0.3583F}, {buzz_D5, 0.7167F}, {buzz_AS4, 0.3583F}, {buzz_D5, 0.7167F}, {buzz_AS4, 0.3583F}, {buzz_DS5, 0.7167F}, {buzz_D5, 0.3583F}, {buzz_CS5, 0.7167F}, {buzz_A4, 0.3583F}, {buzz_AS4, 0.5375F}, {buzz_D5, 0.1792F}, {buzz_CS5, 0.3583F}, {buzz_CS4, 0.7167F}, {buzz_D4, 0.3583F}, {buzz_D5, 2.1500F}, {buzz_Si, 0.3583F}, {buzz_AS4, 0.3583F}, {buzz_D5, 0.7167F}, {buzz_AS4, 0.3583F}, {buzz_D5, 0.7167F}, {buzz_AS4, 0.3583F}, {buzz_F5, 0.7167F}, {buzz_E5, 0.3583F}, {buzz_DS5, 0.7167F}, {buzz_B4, 0.3583F}, {buzz_DS5, 0.5375F}, {buzz_D5, 0.1792F}, {buzz_CS5, 0.3583F}, {buzz_CS4, 0.7167F}, {buzz_AS4, 0.3583F}, {buzz_G4, 2.1500F}, {0xFFFF, 0}};

song_struct RickRoll[] = {{buzz_D5, 1.1902F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_A4, 0.5951F}, {buzz_Si, 1.1902F}, {buzz_B4, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_E5, 0.4463F}, {buzz_E5, 0.4463F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.4463F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.2975F}, {buzz_A4, 0.2975F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_A5, 0.5951F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.5951F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_Si, 0.5951F}, {buzz_Si, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_Si, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.8926F}, {buzz_Si, 0.5951F}, {buzz_Si, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.5951F}, {buzz_Si, 0.2975F}, {buzz_A5, 0.2975F}, {buzz_A5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_A4, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.8926F}, {buzz_Si, 0.5951F}, {buzz_B4, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.5951F}, {buzz_Si, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.5951F}, {buzz_E5, 0.8926F}, {buzz_D5, 1.1902F}, {buzz_D5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_A4, 0.2975F}, {buzz_A4, 0.5951F}, {buzz_Si, 0.8926F}, {buzz_A4, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.2975F}, {buzz_B4, 0.2975F}, {buzz_Si, 0.2975F}, {buzz_E5, 0.2975F}, {buzz_FS5, 0.2975F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_E5, 0.4463F}, {buzz_E5, 0.4463F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.5951F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_A5, 0.5951F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.5951F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_A5, 0.5951F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.5951F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_FS5, 0.4463F}, {buzz_FS5, 0.4463F}, {buzz_E5, 0.8926F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_A5, 0.5951F}, {buzz_CS5, 0.2975F}, {buzz_D5, 0.4463F}, {buzz_CS5, 0.1488F}, {buzz_B4, 0.2975F}, {buzz_A4, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.1488F}, {buzz_B4, 0.1488F}, {buzz_D5, 0.5951F}, {buzz_E5, 0.2975F}, {buzz_CS5, 0.4463F}, {buzz_B4, 0.1488F}, {buzz_A4, 0.5951F}, {buzz_A4, 0.2975F}, {buzz_E5, 0.5951F}, {buzz_D5, 1.1902F}, {buzz_Si, 0.5951F}, {0xFFFF, 0}};

song_struct PinkPanther[] = {{buzz_DS4, 0.2759F}, {buzz_E4, 0.5518F}, {buzz_Si, 0.8277F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.5518F}, {buzz_Si, 0.8277F}, {buzz_DS4, 0.2759F}, {buzz_E4, 0.5518F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.5518F}, {buzz_C5, 0.2759F}, {buzz_B4, 0.5518F}, {buzz_E4, 0.2759F}, {buzz_G4, 0.5518F}, {buzz_B4, 0.2759F}, {buzz_AS4, 1.6555F}, {buzz_A4, 0.2069F}, {buzz_G4, 0.2069F}, {buzz_E4, 0.2069F}, {buzz_D4, 0.2069F}, {buzz_E4, 1.1037F}, {buzz_Si, 1.6555F}, {buzz_DS4, 0.2759F}, {buzz_E4, 0.8277F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.8277F}, {buzz_Si, 0.2759F}, {buzz_DS4, 0.2759F}, {buzz_E4, 0.4139F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.4139F}, {buzz_C5, 0.2759F}, {buzz_B4, 0.4139F}, {buzz_G4, 0.2759F}, {buzz_B4, 0.4139F}, {buzz_E5, 0.2759F}, {buzz_DS5, 3.3110F}, {buzz_D5, 1.1037F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.2759F}, {buzz_DS4, 0.2759F}, {buzz_E4, 0.8277F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.8277F}, {buzz_Si, 0.2759F}, {buzz_DS4, 0.2759F}, {buzz_E4, 0.4139F}, {buzz_FS4, 0.2759F}, {buzz_G4, 0.4139F}, {buzz_C5, 0.2759F}, {buzz_B4, 0.4139F}, {buzz_E4, 0.2759F}, {buzz_G4, 0.4139F}, {buzz_B4, 0.2759F}, {buzz_AS4, 1.6555F}, {buzz_A4, 0.2069F}, {buzz_G4, 0.2069F}, {buzz_E4, 0.2069F}, {buzz_D4, 0.2069F}, {buzz_E4, 0.8277F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.5518F}, {buzz_E5, 0.4139F}, {buzz_D5, 0.2759F}, {buzz_B4, 0.4139F}, {buzz_A4, 0.2759F}, {buzz_G4, 0.4139F}, {buzz_E4, 0.2759F}, {buzz_AS4, 0.4139F}, {buzz_A4, 0.2759F}, {buzz_AS4, 0.4139F}, {buzz_A4, 0.2759F}, {buzz_AS4, 0.4139F}, {buzz_A4, 0.2759F}, {buzz_AS4, 0.4139F}, {buzz_A4, 0.2759F}, {buzz_G4, 0.2069F}, {buzz_E4, 0.2069F}, {buzz_D4, 0.2069F}, {buzz_E4, 0.1380F}, {buzz_E4, 0.1380F}, {buzz_E4, 1.1037F}, {0xFFFF, 0}};

song_struct mii[] = {{buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_F4, 0.2759F}, {buzz_E5, 0.8277F}, {buzz_DS5, 0.2759F}, {buzz_D5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_GS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_GS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_DS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_F4, 0.2759F}, {buzz_E5, 1.1037F}, {buzz_D5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_B4, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_CS4, 0.5518F}, {buzz_B4, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_C4, 0.2759F}, {buzz_B3, 0.5518F}, {buzz_F4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_B3, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.5518F}, {buzz_AS4, 0.5518F}, {buzz_CS5, 0.2759F}, {buzz_D5, 0.2759F}, {buzz_FS5, 0.2759F}, {buzz_A5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 1.1037F}, {buzz_A3, 0.5518F}, {buzz_AS3, 0.5518F}, {buzz_A3, 0.8277F}, {buzz_A3, 0.2759F}, {buzz_A3, 1.1037F}, {buzz_Si, 0.5518F}, {buzz_A3, 0.2759F}, {buzz_AS3, 0.2759F}, {buzz_A3, 0.2759F}, {buzz_F4, 0.5518F}, {buzz_C4, 0.2759F}, {buzz_A3, 0.8277F}, {buzz_A3, 0.2759F}, {buzz_A3, 1.1037F}, {buzz_Si, 1.1037F}, {buzz_B3, 0.5518F}, {buzz_C4, 0.5518F}, {buzz_CS4, 0.8277F}, {buzz_C4, 0.2759F}, {buzz_CS4, 1.1037F}, {buzz_Si, 0.5518F}, {buzz_CS4, 0.2759F}, {buzz_C4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_GS4, 0.5518F}, {buzz_DS4, 0.2759F}, {buzz_CS4, 0.8277F}, {buzz_DS4, 0.2759F}, {buzz_B3, 2.2073F}, {buzz_E4, 0.5518F}, {buzz_E4, 0.5518F}, {buzz_E4, 0.5518F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_F4, 0.2759F}, {buzz_E5, 0.8277F}, {buzz_DS5, 0.2759F}, {buzz_D5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_GS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_GS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_DS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_E5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_F4, 0.2759F}, {buzz_E5, 1.1037F}, {buzz_D5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_B4, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_CS4, 0.5518F}, {buzz_B4, 0.2759F}, {buzz_G4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_A4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_C4, 0.2759F}, {buzz_B3, 0.5518F}, {buzz_F4, 0.2759F}, {buzz_D4, 0.2759F}, {buzz_B3, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_E4, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 0.5518F}, {buzz_AS4, 0.5518F}, {buzz_CS5, 0.2759F}, {buzz_D5, 0.2759F}, {buzz_FS5, 0.2759F}, {buzz_A5, 0.2759F}, {buzz_Si, 0.2759F}, {buzz_Si, 0.5518F}, {buzz_Si, 1.1037F}, {buzz_A3, 0.5518F}, {buzz_AS3, 0.5518F}, {buzz_A3, 0.8277F}, {buzz_A3, 0.2759F}, {buzz_A3, 1.1037F}, {buzz_Si, 0.5518F}, {buzz_A3, 0.2759F}, {buzz_AS3, 0.2759F}, {buzz_A3, 0.2759F}, {buzz_F4, 0.5518F}, {buzz_C4, 0.2759F}, {buzz_A3, 0.8277F}, {buzz_A3, 0.2759F}, {buzz_A3, 1.1037F}, {buzz_Si, 1.1037F}, {buzz_B3, 0.5518F}, {buzz_C4, 0.5518F}, {buzz_CS4, 0.8277F}, {buzz_C4, 0.2759F}, {buzz_CS4, 1.1037F}, {buzz_Si, 0.5518F}, {buzz_CS4, 0.2759F}, {buzz_C4, 0.2759F}, {buzz_CS4, 0.2759F}, {buzz_GS4, 0.5518F}, {buzz_DS4, 0.2759F}, {buzz_CS4, 0.8277F}, {buzz_DS4, 0.2759F}, {buzz_B3, 2.2073F}, {buzz_E4, 0.5518F}, {buzz_E4, 0.5518F}, {buzz_E4, 0.5518F}, {buzz_Si, 0.2759F}, {0xFFFF, 0}};

song_struct zelda[] = {{buzz_AS4, 1.6555F}, {buzz_F4, 0.2759F}, {buzz_F4, 0.2759F}, {buzz_AS4, 0.2759F}, {buzz_GS4, 0.1380F}, {buzz_FS4, 0.1380F}, {buzz_GS4, 1.6555F}, {buzz_AS4, 1.6555F}, {buzz_FS4, 0.2759F}, {buzz_FS4, 0.2759F}, {buzz_AS4, 0.2759F}, {buzz_A4, 0.1380F}, {buzz_G4, 0.1380F}, {buzz_A4, 1.6555F}, {buzz_Si, 2.2073F}, {buzz_AS4, 0.5518F}, {buzz_F4, 0.8277F}, {buzz_AS4, 0.2759F}, {buzz_AS4, 0.1380F}, {buzz_C5, 0.1380F}, {buzz_D5, 0.1380F}, {buzz_DS5, 0.1380F}, {buzz_F5, 1.1037F}, {buzz_F5, 0.2759F}, {buzz_F5, 0.2759F}, {buzz_F5, 0.2759F}, {buzz_FS5, 0.1380F}, {buzz_GS5, 0.1380F}, {buzz_AS5, 1.6555F}, {buzz_AS5, 0.2759F}, {buzz_AS5, 0.2759F}, {buzz_GS5, 0.2759F}, {buzz_FS5, 0.1380F}, {buzz_GS5, 0.4139F}, {buzz_FS5, 0.1380F}, {buzz_F5, 1.1037F}, {buzz_F5, 0.5518F}, {buzz_DS5, 0.4139F}, {buzz_F5, 0.1380F}, {buzz_FS5, 1.1037F}, {buzz_F5, 0.2759F}, {buzz_DS5, 0.2759F}, {buzz_CS5, 0.4139F}, {buzz_DS5, 0.1380F}, {buzz_F5, 1.1037F}, {buzz_DS5, 0.2759F}, {buzz_CS5, 0.2759F}, {buzz_C5, 0.4139F}, {buzz_D5, 0.1380F}, {buzz_E5, 1.1037F}, {buzz_G5, 0.2759F}, {buzz_F5, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.2759F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.2759F}, {buzz_AS4, 0.5518F}, {buzz_F4, 0.8277F}, {buzz_AS4, 0.2759F}, {buzz_AS4, 0.1380F}, {buzz_C5, 0.1380F}, {buzz_D5, 0.1380F}, {buzz_DS5, 0.1380F}, {buzz_F5, 1.1037F}, {buzz_F5, 0.2759F}, {buzz_F5, 0.2759F}, {buzz_F5, 0.2759F}, {buzz_FS5, 0.1380F}, {buzz_GS5, 0.1380F}, {buzz_AS5, 1.6555F}, {buzz_CS6, 0.5518F}, {buzz_C6, 0.5518F}, {buzz_A5, 1.1037F}, {buzz_F5, 0.5518F}, {buzz_FS5, 1.6555F}, {buzz_AS5, 0.5518F}, {buzz_A5, 0.5518F}, {buzz_F5, 1.1037F}, {buzz_F5, 0.5518F}, {buzz_FS5, 1.6555F}, {buzz_AS5, 0.5518F}, {buzz_A5, 0.5518F}, {buzz_F5, 1.1037F}, {buzz_D5, 0.5518F}, {buzz_DS5, 1.6555F}, {buzz_FS5, 0.5518F}, {buzz_F5, 0.5518F}, {buzz_CS5, 1.1037F}, {buzz_AS4, 0.5518F}, {buzz_C5, 0.4139F}, {buzz_D5, 0.1380F}, {buzz_E5, 1.1037F}, {buzz_G5, 0.2759F}, {buzz_F5, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.2759F}, {buzz_F4, 0.1380F}, {buzz_F4, 0.2759F}, {0xFFFF, 0}};

song_struct id0 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id1 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id2 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id3 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id4 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id5 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id6 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id7 [] = { {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id8 [] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id9 [] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id10[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id11[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id12[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id13[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id14[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C4, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
song_struct id15[] = { {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {buzz_C5, 0.15F}, {buzz_Si, 0.05F}, {0xFFFF, 0} };
