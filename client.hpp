#pragma once

#include <chrono>
#include <fstream>
#include <functional>
#include <memory>

#include <easywsclient.hpp>
#include <json.hpp>

#include "snake-base.hpp"

class SnakeClient {
public:
    SnakeClient() = default;
    SnakeClient(const SnakeClient&) = delete;
    SnakeClient& operator= (const SnakeClient&) = delete;
    ~SnakeClient() = default;

    void run(SnakePtr snake);

    template<typename T, typename ... Args>
    void run(Args && ... args) {
        run(std::make_shared<T>(std::forward<Args>(args)...));
    }

    void replay(std::string filename, SnakePtr snake);

    template<typename T, typename ... Args>
    void replay(std::string filename, Args && ... args) {
        replay(filename, std::make_shared<T>(std::forward<Args>(args)...));
    }

    void stop();

    std::function<void(std::string)> gameLinkCallback;

private:
    void openLogFile();
    void connect();
    void sendClientInfo();
    void registerSnake();
    void maybeSendHeartbeat();
    void routeMessage(const std::string& message);

    using clock = std::chrono::steady_clock;

    std::unique_ptr<easywsclient::WebSocket> socket;
    clock::time_point lastHeartbeat;
    SnakePtr snake;
    bool alive;
    std::ofstream logFile;
};
