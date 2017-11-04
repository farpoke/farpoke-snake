#pragma once

#include <chrono>
#include <string>

static constexpr auto TRAINING = true;

static constexpr auto SNAKE_NAME = "sneaky-snek";

static constexpr auto SERVER_URL = 
    TRAINING
    ? "ws://snake.cygni.se:80/training"
    : "ws://snake.cygni.se:80/tournament";

static constexpr auto POLL_TIMEOUT_MS = 100;

static constexpr auto CLIENT_OS_NAME = "Windows";
static constexpr auto CLIENT_VERSION = "farpoke-1.0";

static constexpr auto HEARTBEAT_PERIOD = std::chrono::seconds(2);

static constexpr auto DUMP_INCOMING_JSON = false;
static constexpr auto PRINT_MAP = false;
static constexpr auto MEASURE_THINKING_TIME = true;
static constexpr auto PAUSE_BEFORE_EXIT = true;

static constexpr auto COLLISION_WARNING_PENALTY = 100;
static constexpr auto FOOD_BONUS = 10;
