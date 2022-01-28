/** @brief The AssuredPacketManager handles transmitting, retransmittion, and receiving ACKs
 * 
 * There should always be only one AssuredPacketManager (APM) struct! 
 * An APM keeps track of the sequence number by itself. Two APM's would track two different 
 * sequence number. If an AssuredAck packet then comes in, it will be unclear to which APM 
 * this AssuredAck belongs to.. 
 * 
 * Currently, the APM is limited to having only a single AssuredPacket in transport. Having more
 * packets in transport would be possible if multiple sequence numbers are tracked. However, this
 * requires arrays, more buffers, and what not, and I don't want to build that right now. The 
 * benefits would be minimal. The simplest way to extend the APM with this functionality would be
 * to create multiple instances, and give each instance their own range of sequence number. This
 * would assure that any incoming AssuredAck will always be absorbed by the correct APM instance.
 * 
 * To send an AssuredPacket, take the following steps:
 * 1. Check if the APM is in the READY state. If not, there is already another AssuredPacket waiting to be sent
 * 2. Give the message to the APM using the APM_sendAssuredMessage function. The APM will prepend it with an
 *    AssuredPacket header and sent it out as soon as possible.
 * 3. Give any received AssuredAck to the APM. The APM wil retransmit the AssuredPacket every N times until an
 *    AssuredAck with the correct sequence number has been received.
 * 4. Wait for the APM to go back to the READY state. You can now be assured that the packet has been received
 *    on the other side
 * 
 **/


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "BaseTypes.h"
#include "AssuredPacket.h"
#include "AssuredAck.h"

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

// The time needed in milliseconds before a packet is being retransmitted
static const RETRANSMISSION_DELAY_MS = 500;

/**
 * @brief Struct to track the state, sequence number, and current message
 * 
 * @param state The state tracks if the APM is either READY, AWAITING_TRANSMISSION, or AWAITING_ACK.
 * @param transmission_timestamp The timestamp in milliseconds of the last transmission attempts. If the time 
 *  between this timestamp and the current timestamp exceeds RETRANSMISSION_DELAY_MS, the packet will be marked
 *  for retransmission by switching the APM to the state AWAITING_TRANSMISSION
 * @param sequence_number The sequence number tracks the "unique" (wraps around after 31) id of the AssuredPacket.
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

bool APM_isReady(AssuredPacketManager* apm){
    return apm->state == READY;
}

bool APM_isAwaitingTransmission(AssuredPacketManager* apm){
    return apm->state == AWAITING_TRANSMISSION;
}

bool APM_isAwaitingAck(AssuredPacketManager* apm){
    return apm->state == AWAITING_ACK;
}

bool APM_sendAssuredPacket(AssuredPacketManager* apm, uint8_t* message, uint8_t length){
    // An AssuredPacket can only be sent if another one is not already in transit
    if(!APM_isReady(apm))
        return false;

    // TODO replace the magic number 127 with a constant, reflecting the max SX1280 buffer size
    // Ensure that the packet can actually be transmitted by the SX1280. Yes, it might be possible
    // to send larger packets over UART, but please just split it up into a few smaller packets.
    if(127 < length + PACKET_SIZE_ROBOT_ASSURED_PACKET)
        return false;

    AssuredPacketPayload app;
    AssuredPacket_set_header(&app, PACKET_TYPE_ROBOT_ASSURED_PACKET);
    AssuredPacket_set_remVersion(&app, LOCAL_REM_VERSION);
    AssuredPacket_set_sequenceNumber(&app, apm->sequence_number);


    memcpy(apm->message_buffer + PACKET_SIZE_ROBOT_ASSURED_PACKET, app.payload, length);
}
