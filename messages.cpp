#include "messages.h"
#include "config.hpp"

namespace messages
{

    json start_game() {
        json start_game_msg;
        start_game_msg["type"] = START_GAME;

        return start_game_msg;
    }

    json client_info() {
        json client_info_msg;
        client_info_msg["type"] = CLIENT_INFO;
        client_info_msg["language"] = "C++";
        client_info_msg["languageVersion"] = "17";
        client_info_msg["operatingSystem"] = CLIENT_OS_NAME;
        client_info_msg["clientVersion"] = CLIENT_VERSION;

        return client_info_msg;
    }

    json player_registration(std::string snake_name) {
        json player_registration_msg;
        player_registration_msg["type"] = REGISTER_PLAYER_MESSAGE_TYPE;
        player_registration_msg["playerName"] = snake_name;

        return player_registration_msg;
    }

    json register_move(std::string next_move, json incoming_json) {
        json register_move_msg;
        register_move_msg["type"] = REGISTER_MOVE;
        register_move_msg["direction"] = next_move;
        register_move_msg["gameTick"] = incoming_json["gameTick"];
        register_move_msg["receivingPlayerId"] = incoming_json["receivingPlayerId"];
        register_move_msg["gameId"] = incoming_json["gameId"];

        return register_move_msg;
    }

    json heart_beat(std::string id) {
        json heart_beat;
        heart_beat["type"] = HEART_BEAT_REQUEST;
        heart_beat["receivingPlayerId"] = id;

        return heart_beat;
    }

}
