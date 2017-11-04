#include "farpoke-snake.hpp"

#include <chrono>
#include <iostream>

#include "config.hpp"
#include "map.hpp"
#include "move.hpp"

using namespace std::chrono;

FarpokeSnake::FarpokeSnake() : SnakeBase(SNAKE_NAME) {}

Move FarpokeSnake::getNextMove(Map& map) {
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
    auto my_head = map.getHeads().at(id);
    auto current_move = Move::Left;
    auto current_move_score = std::numeric_limits<int>::min();
    //
    for (auto move : ALL_MOVES) {
        auto new_head = my_head + move;
        if (!map.isInside(new_head) || map[new_head].obstructed)
            continue;
        map.setHead(id, new_head);
        auto score 
            = map.countClosest(id) * CLOSEST_CELL_SCORE
            + map.countTies(id) * CELL_TIE_SCORE
            + map.countAccessible(id) * ACCESSIBILITY_SCORE;
        for (auto& head : map.getHeads()) {
            if (new_head.distance(head.second) == 1)
                score -= COLLISION_WARNING_PENALTY;
        }
        if (map.hasFood(new_head))
            score += FOOD_BONUS;
        if (new_head.x == 0 || new_head.y == 0 || new_head.x == map.width - 1 || new_head.y == map.height - 1)
            score -= OUTER_WALL_HUG_PENALTY;
        if (score > current_move_score) {
            current_move = move;
            current_move_score = score;
        }
    }
    //
    if constexpr (MEASURE_THINKING_TIME) {
        auto duration = high_resolution_clock::now() - start_time;
        clock_accumulator += duration;
        ticks_count++;
        if (ticks_count == 50) {
            auto us_total = duration_cast<microseconds>(clock_accumulator).count();
            std::cout << "- thinking duration: " << (us_total / ticks_count) << " us\n";
            clock_accumulator = high_resolution_clock::duration();
            ticks_count = 0;
        }
    }
    //
    return current_move;
}
