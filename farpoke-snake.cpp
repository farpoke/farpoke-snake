#include "farpoke-snake.hpp"

#include <chrono>
#include <iostream>

#include "config.hpp"
#include "map.hpp"
#include "move.hpp"

using namespace std::chrono;

FarpokeSnake::FarpokeSnake() : SnakeBase(SNAKE_NAME) {}

static Coord coordStack[TARGET_SEARCH_DEPTH];
static Move moveStack[TARGET_SEARCH_DEPTH];

static int directionOptions;
static int directionMaxCollisionPenalty;

void printFullScoreDebugPrefix() {
    printf("? ");
    for (int i = 0; i < TARGET_SEARCH_DEPTH; i++)
        printf("%c",
               moveStack[i] == Move::Up ? '^' :
               moveStack[i] == Move::Down ? 'v' :
               moveStack[i] == Move::Left ? '<' :
               moveStack[i] == Move::Right ? '>' : '?');
    printf(" ");
}

int scoreMove(Map& map, int id, Move move, int depth) {
    const auto maybe_new_head = map.move(id, move);
    if (!maybe_new_head)
        return DEATH_SCORE + depth * DEATH_SCORE_DEPTH_FACTOR;
    const auto new_head = *maybe_new_head;
    //
    coordStack[depth - 1] = new_head;
    if constexpr (PRINT_FULL_SCORES)
        moveStack[depth - 1] = move;
    int total_collision_penalty = 0;
    //
    int score = MIN_SCORE;
    //
    map.assignRegions();
    std::map<int, int> neighbor_region_sizes;
    for (auto move : ALL_MOVES) {
        const auto neighbor_coord = new_head + move;
        if (!map.isInside(neighbor_coord))
            continue;
        const auto& neighbor = map[neighbor_coord];
        if (neighbor.obstructed)
            continue;
        const auto number = neighbor.regionNumber;
        const auto iter = neighbor_region_sizes.find(number);
        if (iter != neighbor_region_sizes.end())
            continue;
        neighbor_region_sizes[number] = map.getRegionSize(number);
    }
    //
    if (depth < TARGET_SEARCH_DEPTH) {
        for (auto next_move : ALL_MOVES) {
            auto next_score = scoreMove(map, id, next_move, depth + 1);
            if (next_score > score)
                score = next_score;
        }
    }
    else {
        directionOptions++;
        map.updateAllDistances();
        score = map.countClosest(id) * CLOSEST_CELL_SCORE;
        score += map.countTies(id) * CELL_TIE_SCORE;
        score += map.countAccessible(id) * ACCESSIBILITY_SCORE;
        score -= map.countOpponentAccessible(id) * OPPONENT_ACCESSIBILITY_PENALTY;
        if (map.hasFood(new_head))
            score += FOOD_BONUS;
        if (new_head.x == 0 || new_head.y == 0 || new_head.x == map.width - 1 || new_head.y == map.height - 1)
            score -= OUTER_WALL_HUG_PENALTY;
        //
        if constexpr (PRINT_FULL_SCORES) {
            printFullScoreDebugPrefix();
            const auto closest = map.countClosest(id);
            const auto ties = map.countTies(id);
            const auto accessible = map.countAccessible(id);
            const auto opponent_accessible = map.countOpponentAccessible(id);
            printf(" %4d %4d %4d %4d", closest, ties, accessible, opponent_accessible);
            if (neighbor_region_sizes.size() > 1)
                printf(" c ");
            else
                printf("   ");
        }
        //
        for (int i = 0; i < TARGET_SEARCH_DEPTH; i++) {
            const auto c = coordStack[i];
            for (auto& head : map.getHeads()) {
                if (head.first == id)
                    continue;
                if (head.second.distance(c) <= (i + 1))
                    total_collision_penalty += COLLISION_WARNING_PENALTY / (i + 1);
            }
        }
        score -= total_collision_penalty;
        //;
        if (directionMaxCollisionPenalty < total_collision_penalty)
            directionMaxCollisionPenalty = total_collision_penalty;
    }
    //
    if (neighbor_region_sizes.size() > 1)
        score -= GRAPH_CUT_PENALTY;
    //
    if constexpr (PRINT_FULL_SCORES) {
        if (depth == TARGET_SEARCH_DEPTH) {
            printf("%6d %10d\n", -total_collision_penalty, score);
        }
    }
    //
    map.undoMove(id, move);
    return score;
}

Move FarpokeSnake::getNextMove(int tick, Map& map) {
    if constexpr (PRINT_MAP)
        map.print(getId());
    //
    static high_resolution_clock::duration clock_accumulator;
    static int ticks_count;
    high_resolution_clock::time_point start_time;
    if constexpr (MEASURE_THINKING_TIME)
        start_time = high_resolution_clock::now();
    //
    const auto id = getId();
    //auto my_head = map.getHeads().at(id);
    auto current_move = Move::Left;
    auto current_move_score = MIN_SCORE;
    //
    int up_score, down_score, left_score, right_score;
    //
    for (auto move : ALL_MOVES) {
        directionOptions = 0;
        directionMaxCollisionPenalty = 0;
        auto score = scoreMove(map, id, move, 1);
        if (directionOptions == 1)
            score -= directionMaxCollisionPenalty * SINGLE_OPTION_RISK_FACTOR;
        else if (directionOptions == 2)
            score -= directionMaxCollisionPenalty * TWO_OPTIONS_RISK_FACTOR;
        if constexpr (PRINT_ALL_FIRST_LEVEL_SCORES) {
            if (move == Move::Up)
                up_score = score;
            else if (move == Move::Down)
                down_score = score;
            else if (move == Move::Left)
                left_score = score;
            else if (move == Move::Right)
                right_score = score;
        }
        if (score > current_move_score) {
            current_move = move;
            current_move_score = score;
        }
    }
    //
    if constexpr (PRINT_ALL_FIRST_LEVEL_SCORES) {
        printf("? %3d   U: %10d%c  D: %10d%c  L: %10d%c  R: %10d%c\n",
               tick,
               up_score, current_move == Move::Up ? '^' : ' ', 
               down_score, current_move == Move::Down ? 'v' : ' ',
               left_score, current_move == Move::Left ? '<' : ' ',
               right_score, current_move == Move::Right ? '>' : ' ');
    }
    //
    if constexpr (MEASURE_THINKING_TIME) {
        const auto duration = high_resolution_clock::now() - start_time;
        clock_accumulator += duration;
        ticks_count++;
        if (ticks_count == 50) {
            const auto us_total = duration_cast<microseconds>(clock_accumulator).count();
            std::cout << "- thinking duration: " << std::setprecision(3) << (us_total / ticks_count / 1000.0) << " ms\n";
            clock_accumulator = high_resolution_clock::duration();
            ticks_count = 0;
        }
    }
    //
    return current_move;
}
