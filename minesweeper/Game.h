// Game.h
#pragma once
#include <memory>
#include <string>
#include "Board.h"

class Game {
public:
    Game(int rows, int cols, int mines);
    Game(); // tom konstruktor för att kunna välja senare

    void run();

    bool loadFromFile(const std::string &filename);
    bool hasBoard() const;

private:
    std::unique_ptr<Board> board_;
    bool gameOver_;
    bool won_;

    static bool parseCoordinate(const std::string &line, int &row, int &col);
    static std::string parseFilename(const std::string &line);
};
