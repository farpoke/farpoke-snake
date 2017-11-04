#pragma once

#include <memory>
#include <optional>
#include <string>

class Map;
enum class Move;

class SnakeBase {
public:
    SnakeBase() = delete;
    SnakeBase(std::string name);
    virtual ~SnakeBase() = default;

    constexpr bool hasGuid() const { return guid.has_value(); }
    constexpr const std::string& getGuid() const { return guid.value(); }

    constexpr int getId() const { return id; }

    void snakeRegistered(std::string guid);

    virtual void gameStarting();

    virtual Move getNextMove(const Map& map) = 0;

    const std::string name;

private:
    std::optional<std::string> guid;
    int id;
    
};

typedef std::shared_ptr<SnakeBase> SnakePtr;
