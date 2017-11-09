#include "map.hpp"

#include <iostream>
#include <queue>

#include "config.hpp"
#include "player-registry.hpp"

Map::Map(int w, int h) : width(w), height(h), cells(w * h) {}

Map::Map(const nlohmann::json& map_json)
    : Map(map_json["width"], map_json["height"])
{
    for (auto& cell : cells) {
        cell = Cell { false };
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
    }
    for (auto& index : map_json["foodPositions"])
        foodIndices.push_back(index);
}

void Map::setHead(int id, Coord head) {
    heads[id] = head;
    updateDistances(id);
}

void Map::print(int highlight_id) const {
    std::cout << "\n";
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            auto cell = at(x, y);
            if (cell.obstructed)
                std::cout << "#";
            else if (getClosestSnake({ x, y }) == highlight_id)
                std::cout << ".";
            else
                std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Map::updateAllDistances() {
    for (auto& kvp : heads)
        updateDistances(kvp.first);
}

void Map::updateDistances(int id) {
    if (heads.find(id) == heads.end())
        throw std::runtime_error("invalid or missing head");
    //
    std::vector<int> distances(cells.size(), std::numeric_limits<int>::max());
    //
    using QueueEntry = std::pair<Coord, int>; // coordinate, distance
    std::queue<QueueEntry> queue;
    queue.emplace(heads[id], 0);
    //
    Coord coord;
    int distance;
    while (!queue.empty()) {
        std::tie(coord, distance) = queue.front();
        queue.pop();
        //
        auto idx = coord.x + coord.y * width;
        if (distances[idx] <= distance)
            continue;
        else
            distances[idx] = distance;
        //
        if (distance >= MAX_DISTANCE_CALCULATED)
            continue;
        //
        for (auto move : ALL_MOVES) {
            auto new_coord = coord + move;
            if (!isInside(new_coord))
                continue;
            else if (at(new_coord).obstructed)
                continue;
            queue.emplace(new_coord, distance + 1);
        }
    }
    //
    distancesMap[id] = distances;
}

int Map::getClosestSnake(int idx) const {
    auto winner = -1;
    auto winner_distance = std::numeric_limits<int>::max();
    for (auto& kvp : distancesMap) {
        const auto dist = kvp.second[idx];
        if (dist > winner_distance)
            continue;
        else if (dist < winner_distance) {
            winner_distance = dist;
            winner = kvp.first;
        }
        else
            winner = -1; // tie
    }
    return winner;
}

int Map::getClosestSnake(Coord coord) const {
    return getClosestSnake(coord.x + coord.y * width);
}

int Map::countClosest(int id) const {
    auto count = 0;
    for (size_t idx = 0; idx < cells.size(); idx++)
        if (getClosestSnake(idx) == id)
            count++;
    return count;
}

int Map::countTies(int id) const {
    auto count = 0;
    auto& distances = distancesMap.at(id);
    for (size_t idx = 0; idx < cells.size(); idx++) {
        if (distances[idx] == std::numeric_limits<int>::max())
            continue;
        else if (getClosestSnake(idx) == -1)
            count++;
    }
    return count;
}

int Map::countAccessible(int id) const {
    auto count = 0;
    auto& distances = distancesMap.at(id);
    for (size_t idx = 0; idx < cells.size(); idx++) {
        if (distances[idx] < std::numeric_limits<int>::max())
            count++;
    }
    return count;
}

int Map::countOpponentAccessible(int id) const {
    int total = 0;
    for (auto& kvp : heads) {
        if (kvp.first != id)
            total += countAccessible(kvp.first);
    }
    return total;
}

bool Map::hasFood(Coord coord) const {
    return std::find(foodIndices.begin(), foodIndices.end(), coord.x + coord.y * width) != foodIndices.end();
}

std::optional<Coord> Map::move(int id, Move move) {
    auto head = heads[id];
    auto new_head = head + move;
    if (!isInside(new_head))
        return std::nullopt;
    else if (at(new_head).obstructed)
        return std::nullopt;
    at(new_head).obstructed = true;
    heads[id] = new_head;
    return new_head;
}

void Map::undoMove(int id, Move move) {
    auto head = heads[id];
    at(head).obstructed = false;
    heads[id] = head - move;
}

int Map::assignRegions() {
    for (auto& cell : cells)
        cell.regionNumber = -1;
    regionCount = 0;
    const std::function<void(Coord)> fill_region = [&](Coord start_coord) {
        std::vector<Coord> coords;
        coords.push_back(start_coord);
        while (!coords.empty()) {
            const auto coord = coords.back();
            coords.pop_back();
            if (!isInside(coord))
                return;
            auto& cell = at(coord);
            if (cell.obstructed || cell.regionNumber != -1)
                return;
            cell.regionNumber = regionCount;
            for (auto move : ALL_MOVES)
                coords.push_back(coord + move);
        }
    };
    for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
        const Coord coord { x, y };
        auto& cell = at(coord);
        if (cell.obstructed || cell.regionNumber != -1)
            continue;
        regionCount++;
        fill_region(coord);
    }
    return regionCount;
}

int Map::getRegionSize(int number) const {
    int count = 0;
    for (auto& cell : cells)
        if (cell.regionNumber == number)
            count++;
    return count;
}
