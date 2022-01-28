#ifndef __ASSURED_PACKET_MANAGER_H
#define __ASSURED_PACKET_MANAGER_H

/**
 * @file AssuredPacketManager.h
 * @author Emiel Steerneman
 * @brief There should always only be one instance of this class!
 * @date 2022-01-28
 */

#include "BaseTypes.h"
#include "RobotAssuredPacket.h"
#include "RobotAssuredAck.h"

/**
 * @brief Three states to keep track of the state of an AssuredPacket message transmitted by the robot
 * READY : There is no AssuredPacket in transport, and such a packet can be sent
 * AWAITING_TRANSMISSION : There is an AssuredPacket in the buffer, which is ready to be sent
 * AWAITING_ACK : An AssuredPacket has been sent, and the APM is now waiting for an AssuredAck
 */
typedef enum {
    READY,
    AWAITING_TRANSMISSION,
    AWAITING_ACK
} ASSURED_STATE ;

/**
 * @brief Struct to track the state, sequence number, and current message
 * 
 * @param state The state tracks if the APM is either READY, AWAITING_TRANSMISSION, or AWAITING_ACK.
 * @param transmission_timestamp The timestamp in milliseconds of the last transmission attempts. If the time 
 *  between this timestamp and the current timestamp exceeds RETRANSMISSION_DELAY_MS, the packet will be marked
 *  for retransmission by switching the APM to the state AWAITING_TRANSMISSION
 * @param sequence_number The sequence number tracks the "unique" (wraps around after 255) id of the AssuredPacket.
 *  Technically, this number is not needed right now since the APM currently only supports
 *  one AssuredPacket in transit at the time, but it good to have nonetheless I think.
 * @param message_length The size of the message in the buffer, INCLUDING the size of the AssuredPacket header
 * @param message_buffer The buffer holds the AssuredPacket header, and the data to be sent
 */
typedef struct _AssuredPacketManager {
    ASSURED_STATE state;
    uint32_t transmission_timestamp;
    uint32_t sequence_number;
    uint8_t message_length;
    uint8_t message_buffer[127];
} AssuredPacketManager;

bool APM_isReady(AssuredPacketManager* apm);

bool APM_isAwaitingTransmission(AssuredPacketManager* apm);

bool APM_isAwaitingAck(AssuredPacketManager* apm);

bool APM_sendAssuredPacket(AssuredPacketManager* apm, uint8_t* message, uint8_t length);

void APM_absorbAssuredAck(AssuredPacketManager* apm, RobotAssuredAckPayload* raap);

#endif /*__ASSURED_PACKET_MANAGER*/