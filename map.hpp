#pragma once

#include <map>
#include <optional>
#include <vector>

#include <json.hpp>

#include "move.hpp"

struct Coord {
    int x, y;

    constexpr int distance(Coord coord) const {
        auto dx = x - coord.x;
        auto dy = y - coord.y;
        return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
    }

    constexpr Coord operator+ (Move move) const {
        if (move == Move::Up)
            return { x, y - 1 };
        else if (move == Move::Down)
            return { x, y + 1 };
        else if (move == Move::Left)
            return { x - 1, y };
        else if (move == Move::Right)
            return { x + 1, y };
        else
            throw std::logic_error("invalid move");
    }

    constexpr Coord operator- (Move move) const {
        if (move == Move::Up)
            return { x, y + 1 };
        else if (move == Move::Down)
            return { x, y - 1 };
        else if (move == Move::Left)
            return { x + 1, y };
        else if (move == Move::Right)
            return { x - 1, y };
        else
            throw std::logic_error("invalid move");
    }

    static constexpr Coord fromIndex(int index, int width) {
        auto y = index / width;
        auto x = index - y * width;
        return { x, y };
    }
};

static_assert(std::is_literal_type_v<Coord>, "Coord is not a literal type");

struct Cell {
    bool obstructed;
};

static_assert(std::is_literal_type_v<Cell>, "Cell is not a literal type");

class Map {
public:
    Map(int w, int h);
    Map(const nlohmann::json& map_json);

    Map(const Map& map) = default;
    Map& operator= (const Map& map) = default;

    const int width, height;

    constexpr bool isInside(Coord coord) const {
        return coord.x >= 0 && coord.y >= 0 && coord.x < width && coord.y < height;
    }

    inline Cell& at(int idx) { return cells[idx]; }
    inline Cell& at(int x, int y) { return at(x + width * y); }
    inline Cell& at(Coord coord) { return at(coord.x, coord.y); }

    inline const Cell& at(int idx) const { return cells[idx]; }
    inline const Cell& at(int x, int y) const { return at(x + width * y); }
    inline const Cell& at(Coord coord) const { return at(coord.x, coord.y); }

    inline Cell& operator[] (int idx) { return at(idx); }
    inline Cell& operator[] (Coord coord) { return at(coord); }

    inline const Cell& operator[] (int idx) const { return at(idx); }
    inline const Cell& operator[] (Coord coord) const { return at(coord); }

    constexpr const std::map<int, Coord>& getHeads() const { return heads; }
    void setHead(int id, Coord head);

    void print(int highlight_id=-1) const;

    void updateAllDistances();
    void updateDistances(int id);

    int getClosestSnake(int idx) const;
    int getClosestSnake(Coord coord) const;

    int countClosest(int id) const;
    int countTies(int id) const;
    int countAccessible(int id) const;

    bool hasFood(Coord coord) const;

    std::optional<Coord> move(int id, Move move);
    void undoMove(int id, Move move);

private:
    std::vector<Cell> cells;
    std::map<int, Coord> heads;
    std::map<int, std::vector<int>> distancesMap;
    std::vector<int> foodIndices;
};
