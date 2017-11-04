#include "client.hpp"

#include <iostream>

#include "config.hpp"
#include "map.hpp"
#include "messages.h"

using easywsclient::WebSocket;
using nlohmann::json;

void SnakeClient::run(SnakePtr snake) {
    this->snake = snake;
    connect();
    registerSnake();
    while (socket->getReadyState() != WebSocket::CLOSED) {
        socket->poll(POLL_TIMEOUT_MS);
        socket->dispatch([this](const std::string& message) {
            routeMessage(message);
        });
        maybeSendHeartbeat();
    }
}

void SnakeClient::stop() {
    if (socket->getReadyState() != WebSocket::CLOSED)
        socket->close();
}

void SnakeClient::connect() {
    socket.reset(WebSocket::from_url(SERVER_URL));
    sendClientInfo();
}

void SnakeClient::sendClientInfo() {
    auto msg = messages::client_info();
    socket->send(msg.dump());
}

void SnakeClient::registerSnake() {
    auto msg = messages::player_registration(snake->name);
    socket->send(msg.dump());
}

void SnakeClient::maybeSendHeartbeat() {
    auto now = clock::now();
    if ((now - lastHeartbeat) > HEARTBEAT_PERIOD) {
        lastHeartbeat = now;
        if (!snake->hasGuid())
            return;
        auto message = messages::heart_beat(snake->getGuid());
        socket->send(message.dump());
    }
}

void SnakeClient::routeMessage(const std::string& message) {
    auto json_in = json::parse(message);
    //
    if constexpr (DUMP_INCOMING_JSON)
        std::cout << json_in.dump(2) << std::endl;
    //
    auto type = json_in["type"];
    //
    // Player registration
    if (type == messages::PLAYER_REGISTERED) {
        std::cout << "Successful registration.\n";
        lastHeartbeat = clock::now();
        snake->snakeRegistered(json_in["receivingPlayerId"]);
    }
    //
    // Invalid player name
    else if (type == messages::INVALID_PLAYER_NAME) {
        throw std::runtime_error("Invalid player name");
    }
    //
    // Game link
    else if (type == messages::GAME_LINK_EVENT) {
        auto url = json_in["url"];
        if (gameLinkCallback) gameLinkCallback(url);
    }
    //
    // Game starting
    else if (type == messages::GAME_STARTING) {
        std::cout << "Starting game...\n";
        alive = true;
        snake->gameStarting();
    }
    //
    // Map update
    else if (type == messages::MAP_UPDATE) {
        if (alive) {
            auto json_map = json_in["map"];
            Map map(json_map);
            auto move = snake->getNextMove(map);
            auto response = messages::register_move(toString(move), json_in);
            socket->send(response.dump());
        }
    }
    //
    // Snake death
    else if (type == messages::SNAKE_DEAD) {
        auto id = json_in["playerId"];
        std::cout << "Snake died! " << id << ", " << json_in["deathReason"] << "\n";
        if (id == snake->getId())
            alive = false;
    }
    //
    // Game results
    else if (type == messages::GAME_RESULT_EVENT) {}
    //
    // Game end
    else if (type == messages::GAME_ENDED) {
        std::cout << "Game ended.\n";
        if constexpr (TRAINING) {
            std::cout << "Stopping training client...\n";
            stop();
        }
    }
    //
    // Tournament end
    else if (type == messages::TOURNAMENT_ENDED) {
        std::cout << "Tournament ended. Stopping client...\n";
        stop();
    }
    //
    // Heartbeat
    else if (type == messages::HEART_BEAT_RESPONSE) {}
    //
    // Wat?
    else {
        std::cout << "wat? " << json_in.dump(2) << std::endl;
    }
}
