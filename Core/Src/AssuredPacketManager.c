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

#include "AssuredPacketManager.h"

#include "robot.h"

// The time needed in milliseconds before a packet is being retransmitted
static const RETRANSMISSION_DELAY_MS = 500;

bool APM_isReady(AssuredPacketManager* apm){
    return apm->state == READY;
}

bool APM_isAwaitingTransmission(AssuredPacketManager* apm){
    // Check if an AssuredAck is expected and if the RETRANSMISSION_DELAY_MS is exceeded
    if(APM_isAwaitingAck(apm)){
        uint32_t current_time = HAL_GetTick();
        if(RETRANSMISSION_DELAY_MS < current_time - apm->transmission_timestamp)
            apm->state = AWAITING_TRANSMISSION;
    }

    return apm->state == AWAITING_TRANSMISSION;
}

bool APM_isAwaitingAck(AssuredPacketManager* apm){
    return apm->state == AWAITING_ACK;
}

bool APM_sendAssuredPacket(AssuredPacketManager* apm, uint8_t* message, uint8_t length){
    // An AssuredPacket can only be sent if another one is not already in transit
    if(!APM_isReady(apm))
        return false;

    // Length of the complete message, including the AssuredAck header
    uint8_t total_length = length + PACKET_SIZE_ROBOT_ASSURED_PACKET;

    // TODO replace the magic number 127 with a constant, reflecting the max SX1280 buffer size
    // Ensure that the packet can actually be transmitted by the SX1280. Yes, it might be possible
    // to send larger packets over UART, but please just split it up into a few smaller packets.
    if(127 < total_length)
        return false;

    // Increase the sequence number
    apm->sequence_number++;

    // Create the AssuredPacket header
    RobotAssuredPacketPayload rapp;
    RobotAssuredPacket_set_header(&rapp, PACKET_TYPE_ROBOT_ASSURED_PACKET);
    RobotAssuredPacket_set_remVersion(&rapp, LOCAL_REM_VERSION);
    RobotAssuredPacket_set_id(&rapp, robot_getID());
    RobotAssuredPacket_set_sequenceNumber(&rapp, apm->sequence_number);
    RobotAssuredPacket_set_messageLength(&rapp, total_length);

    // Copy the AssuredPacket header into the message buffer
    memcpy(apm->message_buffer, rapp.payload, PACKET_SIZE_ROBOT_ASSURED_PACKET);
    // Copy the rest of the message into the message buffer
    memcpy(apm->message_buffer + PACKET_SIZE_ROBOT_ASSURED_PACKET, message, length);

    apm->message_length = total_length;
    apm->state = AWAITING_TRANSMISSION;
}

void APM_absorbAssuredAck(AssuredPacketManager* apm, RobotAssuredAckPayload* raap){
    // Get the sequence number from the AssuredAck
    uint8_t ack_sequence_number = RobotAssuredAck_get_sequenceNumber(raap);
    // Ensure the sequence number is correct
    if(ack_sequence_number != apm->sequence_number)
        return;
    
    // Reset message_length to 0, to prevent accidental retransmission
    apm->message_length = 0;
    // Ready to send a new AssuredPacket
    apm->state = READY;

}