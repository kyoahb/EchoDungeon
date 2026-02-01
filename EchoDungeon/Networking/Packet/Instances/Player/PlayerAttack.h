#pragma once
#include "Networking/Packet/Packet.h"
#include "Imports/common.h"

// Packet type: 22
// Packet name: PlayerAttack
/**
 * @brief Packet sent by client to server to indicate a player attack action.
 */
class PlayerAttackPacket : public Packet {
public:
    uint32_t player_id;

    PlayerAttackPacket() : Packet(22, true), player_id(0) {}

    PlayerAttackPacket(uint32_t _player_id)
        : Packet(22, true),
        player_id(_player_id){
    }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(header, is_reliable, player_id);
    }

    PACKET_TO_ENET(PlayerAttackPacket)
    PACKET_DESERIALIZE(PlayerAttackPacket)
};