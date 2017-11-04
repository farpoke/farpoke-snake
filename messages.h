#pragma once

#include <json.hpp>

namespace messages
{
    using nlohmann::json;

    // Inbound
    static const auto GAME_ENDED = "se.cygni.snake.api.event.GameEndedEvent";
    static const auto TOURNAMENT_ENDED = "se.cygni.snake.api.event.TournamentEndedEvent";
    static const auto MAP_UPDATE = "se.cygni.snake.api.event.MapUpdateEvent";
    static const auto SNAKE_DEAD = "se.cygni.snake.api.event.SnakeDeadEvent";
    static const auto GAME_STARTING = "se.cygni.snake.api.event.GameStartingEvent";
    static const auto PLAYER_REGISTERED = "se.cygni.snake.api.response.PlayerRegistered";
    static const auto INVALID_PLAYER_NAME = "se.cygni.snake.api.exception.InvalidPlayerName";
    static const auto HEART_BEAT_RESPONSE = "se.cygni.snake.api.response.HeartBeatResponse";
    static const auto GAME_LINK_EVENT = "se.cygni.snake.api.event.GameLinkEvent";
    static const auto GAME_RESULT_EVENT = "se.cygni.snake.api.event.GameResultEvent";

    // Outbound
    static const auto REGISTER_PLAYER_MESSAGE_TYPE = "se.cygni.snake.api.request.RegisterPlayer";
    static const auto START_GAME = "se.cygni.snake.api.request.StartGame";
    static const auto REGISTER_MOVE = "se.cygni.snake.api.request.RegisterMove";
    static const auto HEART_BEAT_REQUEST = "se.cygni.snake.api.request.HeartBeatRequest";
    static const auto CLIENT_INFO = "se.cygni.snake.api.request.ClientInfo";

    json start_game();
    json client_info();
    json default_game_settings();
    json player_registration(std::string snake_name);
    json register_move(std::string next_move, json incoming_json);
    json heart_beat(std::string id);

}
