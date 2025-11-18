// Board.h
#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Tile.h"

class Board {
public:
    Board(int rows, int cols, int mines);

    int rows() const { return rows_; }
    int cols() const { return cols_; }
    int mines() const { return mines_; }

    Tile &tile(int r, int c);
    const Tile &tile(int r, int c) const;

    bool inBounds(int r, int c) const;

    // Avslöja en ruta, returnerar false om en mina träffas
    bool reveal(int r, int c);

    // Sätt/ta bort flagga
    void toggleFlag(int r, int c);

    // Har spelaren vunnit?
    bool allSafeRevealed() const;

    // Skriv ut spelplanen. Om revealAll=true visas allt (används vid Game Over / vinst).
    void print(bool revealAll = false) const;

    // Spara hela spelbrädet + status
    bool saveToFile(const std::string &filename, bool gameOver, bool won) const;

    // Ladda ett färdigt Board från fil
    static std::unique_ptr<Board> loadFromFile(const std::string &filename,
                                               bool &gameOver,
                                               bool &won);

private:
    int rows_;
    int cols_;
    int mines_;
    std::vector<std::vector<Tile>> grid_;

    void placeMinesRandomly();
    void calculateAdjacents();
};
