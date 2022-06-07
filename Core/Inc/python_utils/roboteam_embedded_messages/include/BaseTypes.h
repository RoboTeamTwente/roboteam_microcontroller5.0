// AUTOGENERATED. Run generator/main.py to regenerate
// Generated on June 05 2021, 23:47:19

#ifndef __BASETYPES_H
#define __BASETYPES_H

// Warning : Check the unicode table before assigning a byte, to make sure that the byte isn't used for anything special : https://unicode-table.com/
// For example, don't use the following bytes
// 0b00000000 : The null-terminator, used to signal the end of strings / arrays / etc.
// 0b00001010 : The byte for newline, used for line termination.

typedef enum _PACKET_TYPE {
	PACKET_TYPE_ROBOT_COMMAND     = 0b00001111,
	PACKET_TYPE_ROBOT_FEEDBACK    = 0b00110011,
	PACKET_TYPE_ROBOT_STATE_INFO  = 0b00111100,
	PACKET_TYPE_ROBOT_BUZZER      = 0b01010101,
	PACKET_TYPE_BASESTATION_STATISTICS = 0b01011010,
	PACKET_TYPE_BASESTATION_GET_STATISTICS = 0b01100110,
	PACKET_TYPE_BASESTATION_LOG   = 0b01101001
} PACKET_TYPE;

typedef enum _PACKET_SIZE {
	PACKET_SIZE_ROBOT_COMMAND     = 11,
	PACKET_SIZE_ROBOT_FEEDBACK    = 11,
	PACKET_SIZE_ROBOT_STATE_INFO  = 34,
	PACKET_SIZE_ROBOT_BUZZER      = 8,
	PACKET_SIZE_BASESTATION_STATISTICS = 33,
	PACKET_SIZE_BASESTATION_GET_STATISTICS = 1,
	PACKET_SIZE_BASESTATION_LOG   = 1
} PACKET_SIZE;

# endif /*__BASETYPES_H*/
