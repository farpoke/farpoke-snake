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

static constexpr auto LOG_FILE_PATH_FORMAT = "play-log-%Y-%m-%d-%H-%M-%S.txt";

static constexpr auto REPLAY_FILE = nullptr;
static constexpr auto FIRST_REPLAY_TICK = 731;
static constexpr auto DEBUG_MODE = REPLAY_FILE != nullptr;

static constexpr auto LOG_INCOMING_JSON = true;
static constexpr auto DUMP_INCOMING_JSON = false;
static constexpr auto OPEN_GAME_LINK = !DEBUG_MODE;
static constexpr auto PRINT_MAP = false;
static constexpr auto PRINT_ALL_FIRST_LEVEL_SCORES = DEBUG_MODE;
static constexpr auto PRINT_FULL_SCORES = DEBUG_MODE;
static constexpr auto MEASURE_THINKING_TIME = true;
static constexpr auto PAUSE_BEFORE_EXIT = DEBUG_MODE;

static constexpr auto CLOSEST_CELL_SCORE = 2;
static constexpr auto CELL_TIE_SCORE = 1;
static constexpr auto ACCESSIBILITY_SCORE = 2;
static constexpr auto OPPONENT_ACCESSIBILITY_PENALTY = 2;

static constexpr auto COLLISION_WARNING_PENALTY = 1000;
static constexpr auto SINGLE_OPTION_RISK_FACTOR = 100;
static constexpr auto TWO_OPTIONS_RISK_FACTOR = 10;

static constexpr auto FOOD_BONUS = 0;
static constexpr auto OUTER_WALL_HUG_PENALTY = 50;
static constexpr auto GRAPH_CUT_PENALTY = 200;

static constexpr auto MAX_DISTANCE_CALCULATED = 1000;
static constexpr auto TARGET_SEARCH_DEPTH = 3;

#undef min
static constexpr auto MIN_SCORE = -100'000'000;
static constexpr auto DEATH_SCORE = -1'000'000;
static constexpr auto DEATH_SCORE_DEPTH_FACTOR = 100;
