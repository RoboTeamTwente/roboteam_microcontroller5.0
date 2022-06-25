// AUTOGENERATED. Run generator/main.py to regenerate
// Generated on June 05 2021, 23:47:19

#ifndef __ROBOT_STATE_INFO_H
#define __ROBOT_STATE_INFO_H

#include <stdbool.h>
#include <stdint.h>
#include "BaseTypes.h"

typedef struct _RobotStateInfoPayload {
	uint8_t payload[PACKET_SIZE_ROBOT_STATE_INFO];
} RobotStateInfoPayload;



/** ================================ PACKET ================================ 
[---0--] [---1--] [---2--] [---3--] [---4--] [---5--] [---6--] [---7--] [---8--] [---9--] [--10--] [--11--] [--12--] [--13--] [--14--] [--15--] [--16--] [--17--] [--18--] [--19--] [--20--] [--21--] [--22--] [--23--] [--24--] [--25--] [--26--] [--27--] [--28--] [--29--] [--30--] [--31--] [--32--] [--33--]
11111111 -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- header
-------- 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- id
-------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- xsensAcc1
-------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- xsensAcc2
-------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- xsensYaw
-------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- rateOfTurn
-------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- wheelSpeed1
-------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- -------- -------- -------- -------- wheelSpeed2
-------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- -------- -------- -------- -------- wheelSpeed3
-------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- -------- ----1111 11111111 11111111 11111111 1111---- wheelSpeed4
*/



/** ================================ STRUCT ================================ */
typedef struct _RobotStateInfo {
    uint8_t    header;                  // Header byte indicating the type of packet
    uint8_t    id;                      // Id of the robot 
    float      xsensAcc1;               // xsensAcc1
    float      xsensAcc2;               // xsensAcc2
    float      xsensYaw;                // xsensYaw
    float      rateOfTurn;              // rateOfTurn
    float      wheelSpeed1;             // wheelSpeed1
    float      wheelSpeed2;             // wheelSpeed2
    float      wheelSpeed3;             // wheelSpeed3
    float      wheelSpeed4;             // wheelSpeed4
} RobotStateInfo;



/** ================================ GETTERS ================================ */
static inline uint8_t RobotStateInfo_get_header(RobotStateInfoPayload *rsip){
    return ((rsip->payload[0]));
}
static inline uint8_t RobotStateInfo_get_id(RobotStateInfoPayload *rsip){
    return ((rsip->payload[1] & 0b11110000) >> 4);
}
static inline float RobotStateInfo_get_xsensAcc1(RobotStateInfoPayload *rsip){
    uint32_t xsensAcc1 = ((rsip->payload[1] & 0b00001111) << 28) | ((rsip->payload[2]) << 20) | ((rsip->payload[3]) << 12) | ((rsip->payload[4]) << 4) | ((rsip->payload[5] & 0b11110000) >> 4);
    return (xsensAcc1 * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_xsensAcc2(RobotStateInfoPayload *rsip){
    uint32_t xsensAcc2 = ((rsip->payload[5] & 0b00001111) << 28) | ((rsip->payload[6]) << 20) | ((rsip->payload[7]) << 12) | ((rsip->payload[8]) << 4) | ((rsip->payload[9] & 0b11110000) >> 4);
    return (xsensAcc2 * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_xsensYaw(RobotStateInfoPayload *rsip){
    uint32_t xsensYaw = ((rsip->payload[9] & 0b00001111) << 28) | ((rsip->payload[10]) << 20) | ((rsip->payload[11]) << 12) | ((rsip->payload[12]) << 4) | ((rsip->payload[13] & 0b11110000) >> 4);
    return (xsensYaw * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_rateOfTurn(RobotStateInfoPayload *rsip){
    uint32_t rateOfTurn = ((rsip->payload[13] & 0b00001111) << 28) | ((rsip->payload[14]) << 20) | ((rsip->payload[15]) << 12) | ((rsip->payload[16]) << 4) | ((rsip->payload[17] & 0b11110000) >> 4);
    return (rateOfTurn * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_wheelSpeed1(RobotStateInfoPayload *rsip){
    uint32_t wheelSpeed1 = ((rsip->payload[17] & 0b00001111) << 28) | ((rsip->payload[18]) << 20) | ((rsip->payload[19]) << 12) | ((rsip->payload[20]) << 4) | ((rsip->payload[21] & 0b11110000) >> 4);
    return (wheelSpeed1 * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_wheelSpeed2(RobotStateInfoPayload *rsip){
    uint32_t wheelSpeed2 = ((rsip->payload[21] & 0b00001111) << 28) | ((rsip->payload[22]) << 20) | ((rsip->payload[23]) << 12) | ((rsip->payload[24]) << 4) | ((rsip->payload[25] & 0b11110000) >> 4);
    return (wheelSpeed2 * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_wheelSpeed3(RobotStateInfoPayload *rsip){
    uint32_t wheelSpeed3 = ((rsip->payload[25] & 0b00001111) << 28) | ((rsip->payload[26]) << 20) | ((rsip->payload[27]) << 12) | ((rsip->payload[28]) << 4) | ((rsip->payload[29] & 0b11110000) >> 4);
    return (wheelSpeed3 * 0.0000232830643654) + -50000.0000000000000000;
}
static inline float RobotStateInfo_get_wheelSpeed4(RobotStateInfoPayload *rsip){
    uint32_t wheelSpeed4 = ((rsip->payload[29] & 0b00001111) << 28) | ((rsip->payload[30]) << 20) | ((rsip->payload[31]) << 12) | ((rsip->payload[32]) << 4) | ((rsip->payload[33] & 0b11110000) >> 4);
    return (wheelSpeed4 * 0.0000232830643654) + -50000.0000000000000000;
}



/** ================================ SETTERS ================================ */
static inline void RobotStateInfo_set_header(RobotStateInfoPayload *rsip, uint8_t header){
    rsip->payload[0] = header;
}
static inline void RobotStateInfo_set_id(RobotStateInfoPayload *rsip, uint8_t id){
    rsip->payload[1] = ((id << 4) & 0b11110000) | (rsip->payload[1] & 0b00001111);
}
static inline void RobotStateInfo_set_xsensAcc1(RobotStateInfoPayload *rsip, float _xsensAcc1){
    uint32_t xsensAcc1 = (_xsensAcc1 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[1] = ((xsensAcc1 >> 28) & 0b00001111) | (rsip->payload[1] & 0b11110000);
    rsip->payload[2] = (xsensAcc1 >> 20);
    rsip->payload[3] = (xsensAcc1 >> 12);
    rsip->payload[4] = (xsensAcc1 >> 4);
    rsip->payload[5] = ((xsensAcc1 << 4) & 0b11110000) | (rsip->payload[5] & 0b00001111);
}
static inline void RobotStateInfo_set_xsensAcc2(RobotStateInfoPayload *rsip, float _xsensAcc2){
    uint32_t xsensAcc2 = (_xsensAcc2 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[5] = ((xsensAcc2 >> 28) & 0b00001111) | (rsip->payload[5] & 0b11110000);
    rsip->payload[6] = (xsensAcc2 >> 20);
    rsip->payload[7] = (xsensAcc2 >> 12);
    rsip->payload[8] = (xsensAcc2 >> 4);
    rsip->payload[9] = ((xsensAcc2 << 4) & 0b11110000) | (rsip->payload[9] & 0b00001111);
}
static inline void RobotStateInfo_set_xsensYaw(RobotStateInfoPayload *rsip, float _xsensYaw){
    uint32_t xsensYaw = (_xsensYaw - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[9] = ((xsensYaw >> 28) & 0b00001111) | (rsip->payload[9] & 0b11110000);
    rsip->payload[10] = (xsensYaw >> 20);
    rsip->payload[11] = (xsensYaw >> 12);
    rsip->payload[12] = (xsensYaw >> 4);
    rsip->payload[13] = ((xsensYaw << 4) & 0b11110000) | (rsip->payload[13] & 0b00001111);
}
static inline void RobotStateInfo_set_rateOfTurn(RobotStateInfoPayload *rsip, float _rateOfTurn){
    uint32_t rateOfTurn = (_rateOfTurn - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[13] = ((rateOfTurn >> 28) & 0b00001111) | (rsip->payload[13] & 0b11110000);
    rsip->payload[14] = (rateOfTurn >> 20);
    rsip->payload[15] = (rateOfTurn >> 12);
    rsip->payload[16] = (rateOfTurn >> 4);
    rsip->payload[17] = ((rateOfTurn << 4) & 0b11110000) | (rsip->payload[17] & 0b00001111);
}
static inline void RobotStateInfo_set_wheelSpeed1(RobotStateInfoPayload *rsip, float _wheelSpeed1){
    uint32_t wheelSpeed1 = (_wheelSpeed1 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[17] = ((wheelSpeed1 >> 28) & 0b00001111) | (rsip->payload[17] & 0b11110000);
    rsip->payload[18] = (wheelSpeed1 >> 20);
    rsip->payload[19] = (wheelSpeed1 >> 12);
    rsip->payload[20] = (wheelSpeed1 >> 4);
    rsip->payload[21] = ((wheelSpeed1 << 4) & 0b11110000) | (rsip->payload[21] & 0b00001111);
}
static inline void RobotStateInfo_set_wheelSpeed2(RobotStateInfoPayload *rsip, float _wheelSpeed2){
    uint32_t wheelSpeed2 = (_wheelSpeed2 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[21] = ((wheelSpeed2 >> 28) & 0b00001111) | (rsip->payload[21] & 0b11110000);
    rsip->payload[22] = (wheelSpeed2 >> 20);
    rsip->payload[23] = (wheelSpeed2 >> 12);
    rsip->payload[24] = (wheelSpeed2 >> 4);
    rsip->payload[25] = ((wheelSpeed2 << 4) & 0b11110000) | (rsip->payload[25] & 0b00001111);
}
static inline void RobotStateInfo_set_wheelSpeed3(RobotStateInfoPayload *rsip, float _wheelSpeed3){
    uint32_t wheelSpeed3 = (_wheelSpeed3 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[25] = ((wheelSpeed3 >> 28) & 0b00001111) | (rsip->payload[25] & 0b11110000);
    rsip->payload[26] = (wheelSpeed3 >> 20);
    rsip->payload[27] = (wheelSpeed3 >> 12);
    rsip->payload[28] = (wheelSpeed3 >> 4);
    rsip->payload[29] = ((wheelSpeed3 << 4) & 0b11110000) | (rsip->payload[29] & 0b00001111);
}
static inline void RobotStateInfo_set_wheelSpeed4(RobotStateInfoPayload *rsip, float _wheelSpeed4){
    uint32_t wheelSpeed4 = (_wheelSpeed4 - -50000.0000000000000000) * 42949.6729600000035134;
    rsip->payload[29] = ((wheelSpeed4 >> 28) & 0b00001111) | (rsip->payload[29] & 0b11110000);
    rsip->payload[30] = (wheelSpeed4 >> 20);
    rsip->payload[31] = (wheelSpeed4 >> 12);
    rsip->payload[32] = (wheelSpeed4 >> 4);
    rsip->payload[33] = ((wheelSpeed4 << 4) & 0b11110000) | (rsip->payload[33] & 0b00001111);
}



/** ================================ DECODE ================================ */
static inline void decodeRobotStateInfo(RobotStateInfo *rsi, RobotStateInfoPayload *rsip){
	rsi->header             = RobotStateInfo_get_header(rsip);
	rsi->id                 = RobotStateInfo_get_id(rsip);
	rsi->xsensAcc1          = RobotStateInfo_get_xsensAcc1(rsip);
	rsi->xsensAcc2          = RobotStateInfo_get_xsensAcc2(rsip);
	rsi->xsensYaw           = RobotStateInfo_get_xsensYaw(rsip);
	rsi->rateOfTurn         = RobotStateInfo_get_rateOfTurn(rsip);
	rsi->wheelSpeed1        = RobotStateInfo_get_wheelSpeed1(rsip);
	rsi->wheelSpeed2        = RobotStateInfo_get_wheelSpeed2(rsip);
	rsi->wheelSpeed3        = RobotStateInfo_get_wheelSpeed3(rsip);
	rsi->wheelSpeed4        = RobotStateInfo_get_wheelSpeed4(rsip);
}



/** ================================ ENCODE ================================ */
static inline void encodeRobotStateInfo(RobotStateInfoPayload *rsip, RobotStateInfo *rsi){
	RobotStateInfo_set_header              (rsip, rsi->header);
	RobotStateInfo_set_id                  (rsip, rsi->id);
	RobotStateInfo_set_xsensAcc1           (rsip, rsi->xsensAcc1);
	RobotStateInfo_set_xsensAcc2           (rsip, rsi->xsensAcc2);
	RobotStateInfo_set_xsensYaw            (rsip, rsi->xsensYaw);
	RobotStateInfo_set_rateOfTurn          (rsip, rsi->rateOfTurn);
	RobotStateInfo_set_wheelSpeed1         (rsip, rsi->wheelSpeed1);
	RobotStateInfo_set_wheelSpeed2         (rsip, rsi->wheelSpeed2);
	RobotStateInfo_set_wheelSpeed3         (rsip, rsi->wheelSpeed3);
	RobotStateInfo_set_wheelSpeed4         (rsip, rsi->wheelSpeed4);
}

#endif /*__ROBOT_STATE_INFO_H*/