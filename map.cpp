#include "map.hpp"

#include <iostream>
#include <queue>

#include "player-registry.hpp"

Map::Map(int w, int h) : width(w), height(h), cells(w * h) {}

Map::Map(const nlohmann::json& map_json)
    : Map(map_json["width"], map_json["height"])
{
    for (auto& cell : cells) {
        cell = Cell { false, -1, -1 };
    }
    for (auto& obstacle : map_json["obstaclePositions"])
        (*this)[(int)obstacle].obstructed = true;
    for (auto& snake : map_json["snakeInfos"]) {
        auto& positions = snake["positions"];
        if (positions.empty())
            continue;
        auto id = g_Registry.getId(snake["id"]);
        heads[id] = Coord::fromIndex(positions[0], width);
        for (auto& pos : positions)
            at(static_cast<int>(pos)).obstructed = true;
        /*if (positions.size() > 1 && snake["tailProtectedForGameTicks"] == 0)
            (*this)[(int)*positions.rbegin()].obstructed = false;*/
    }
    for (auto& index : map_json["foodPositions"])
        foodIndices.push_back(index);
    updateClosest();
}

//Map::Map(const Map& other_map) : Map(other_map.width, other_map.height) {
//    cells = other_map.cells;
//    heads = other_map.heads;
//}

void Map::setHead(int id, Coord head) {
    heads[id] = head;
}

void Map::print(int highlight_id) const {
    std::cout << "\n";
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            auto cell = at(x, y);
            if (cell.obstructed)
                std::cout << "#";
            else if (cell.closestSnake == highlight_id)
                std::cout << ".";
            else
                std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Map::updateClosest() {
    for (auto& cell : cells) {
        cell.closestSnake = -1;
        cell.closestDistance = std::numeric_limits<decltype(cell.closestDistance)>::max();
    }
    //
    using QueueEntry = std::tuple<int, Coord, int>; // snake-id, coordinate, distance
    std::queue<QueueEntry> queue;
    for (auto& kvp : heads)
        queue.emplace(kvp.first, kvp.second, 0);
    //
    int id, distance;
    Coord coord;
    while (!queue.empty()) {
        std::tie(id, coord, distance) = queue.front();
        queue.pop();
        //
        auto& cell = at(coord);
        if (cell.closestDistance < distance)
            continue;
        else if (cell.closestDistance == distance) {
            if (cell.closestSnake == id)
                continue; // Revisit own closest cell
            else if (cell.closestSnake < 0)
                continue; // Tie cell
            else
                cell.closestSnake = -1; // Change to tie cell
        }
        else {
            cell.closestSnake = id;
            cell.closestDistance = distance;
        }
        //
        for (auto move : ALL_MOVES) {
            auto new_coord = coord + move;
            if (!isInside(new_coord))
                continue;
            else if (at(new_coord).obstructed)
                continue;
            queue.emplace(id, new_coord, distance + 1);
        }
    }
}

int Map::countClosest(int id) const {
    auto count = 0;
    for (auto& cell : cells)
        if (cell.closestSnake == id)
            count++;
    return count;
}

bool Map::hasFood(Coord coord) const {
    return std::find(foodIndices.begin(), foodIndices.end(), coord.x + coord.y * width) != foodIndices.end();
}
