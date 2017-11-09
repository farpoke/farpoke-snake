
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>

#include "client.hpp"
#include "config.hpp"
#include "farpoke-snake.hpp"

void start_ws() {
    auto version = MAKEWORD(2, 2);
    WSADATA data;
    auto err = WSAStartup(version, &data);
    if (err != ERROR_SUCCESS)
        throw std::runtime_error("WSAStartup failed");
}

int main(int, char*[]) {
    start_ws();

    SnakeClient client;

    client.gameLinkCallback = [](std::string url) {
        if (OPEN_GAME_LINK)
            ShellExecute(NULL, NULL, url.c_str(), NULL, NULL, SW_SHOW);
    };

    if constexpr (REPLAY_FILE == nullptr)
        client.run<FarpokeSnake>();
    else
        client.replay<FarpokeSnake>(REPLAY_FILE);

    if constexpr (PAUSE_BEFORE_EXIT)
        std::getchar();

    return 0;
}
