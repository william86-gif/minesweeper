// Board.cpp
#include "Board.h"
#include <random>
#include <fstream>
#include <iostream>

Board::Board(int rows, int cols, int mines)
    : rows_(rows), cols_(cols), mines_(mines),
      grid_(rows, std::vector<Tile>(cols)) {
    placeMinesRandomly();
    calculateAdjacents();
}

Tile &Board::tile(int r, int c) {
    return grid_[r][c];
}

const Tile &Board::tile(int r, int c) const {
    return grid_[r][c];
}

bool Board::inBounds(int r, int c) const {
    return r >= 0 && r < rows_ && c >= 0 && c < cols_;
}

bool Board::reveal(int r, int c) {
    if (!inBounds(r, c)) return false;
    Tile &t = grid_[r][c];

    if (t.revealed || t.flagged) {
        return true; // ingenting händer
    }

    t.revealed = true;

    if (t.hasMine) {
        return false; // boom
    }

    // Flood-fill om inga angränsande minor
    if (t.adjacentMines == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr;
                int nc = c + dc;
                if (inBounds(nr, nc) && !grid_[nr][nc].revealed) {
                    reveal(nr, nc);
                }
            }
        }
    }
    return true;
}

void Board::toggleFlag(int r, int c) {
    if (!inBounds(r, c)) return;
    Tile &t = grid_[r][c];
    if (t.revealed) return;
    t.flagged = !t.flagged;
}

bool Board::allSafeRevealed() const {
    int safeCount = 0;
    int revealedSafe = 0;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            const Tile &t = grid_[r][c];
            if (!t.hasMine) {
                ++safeCount;
                if (t.revealed) ++revealedSafe;
            }
        }
    }
    return safeCount > 0 && safeCount == revealedSafe;
}

void Board::print(bool revealAll) const {
    // Kolumnhuvud
    std::cout << "      ";
    for (int c = 0; c < cols_; ++c) {
        std::cout << (c + 1) << "  ";
    }
    std::cout << "\n";

    // Övre ram
    std::cout << "    +";
    for (int c = 0; c < cols_; ++c) {
        std::cout << "---";
    }
    std::cout << "+\n";

    for (int r = 0; r < rows_; ++r) {
        char rowLabel = static_cast<char>('a' + r);
        // Rad med innehåll
        std::cout << "  " << rowLabel << " |";
        for (int c = 0; c < cols_; ++c) {
            const Tile &t = grid_[r][c];
            char ch = ' ';

            if (revealAll) {
                if (t.hasMine) {
                    ch = 'O'; // bomb
                } else if (t.adjacentMines > 0) {
                    ch = static_cast<char>('0' + t.adjacentMines);
                } else {
                    ch = 'x'; // säkra tomma rutor
                }
            } else {
                if (t.flagged) {
                    ch = 'F';
                } else if (t.revealed) {
                    if (t.hasMine) {
                        ch = 'O';
                    } else if (t.adjacentMines > 0) {
                        ch = static_cast<char>('0' + t.adjacentMines);
                    } else {
                        ch = 'x';
                    }
                } else {
                    ch = ' ';
                }
            }

            std::cout << " " << ch << " ";
        }
        std::cout << "|\n";

        // Horisontella linjer mellan raderna
        std::cout << "    +";
        for (int c = 0; c < cols_; ++c) {
            std::cout << "---";
        }
        std::cout << "+\n";
    }
}

bool Board::saveToFile(const std::string &filename, bool gameOver, bool won) const {
    std::ofstream out(filename);
    if (!out) {
        return false;
    }

    // Format:
    // rows cols mines
    // gameOver won
    // sedan en rad per ruta: hasMine revealed flagged adjacentMines
    out << rows_ << " " << cols_ << " " << mines_ << "\n";
    out << (gameOver ? 1 : 0) << " " << (won ? 1 : 0) << "\n";

    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            const Tile &t = grid_[r][c];
            out << t.hasMine << " "
                << t.revealed << " "
                << t.flagged << " "
                << t.adjacentMines << "\n";
        }
    }
    return true;
}

std::unique_ptr<Board> Board::loadFromFile(const std::string &filename,
                                           bool &gameOver,
                                           bool &won) {
    std::ifstream in(filename);
    if (!in) {
        return nullptr;
    }

    int rows, cols, mines;
    if (!(in >> rows >> cols >> mines)) {
        return nullptr;
    }
    int go, w;
    if (!(in >> go >> w)) {
        return nullptr;
    }
    gameOver = (go != 0);
    won = (w != 0);

    auto board = std::make_unique<Board>(rows, cols, mines);

    // Skriv över slumpdata med sparade värden
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Tile t;
            if (!(in >> t.hasMine >> t.revealed >> t.flagged >> t.adjacentMines)) {
                return nullptr;
            }
            board->grid_[r][c] = t;
        }
    }

    return board;
}

void Board::placeMinesRandomly() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distRow(0, rows_ - 1);
    std::uniform_int_distribution<> distCol(0, cols_ - 1);

    int placed = 0;
    while (placed < mines_) {
        int r = distRow(gen);
        int c = distCol(gen);
        if (!grid_[r][c].hasMine) {
            grid_[r][c].hasMine = true;
            ++placed;
        }
    }
}

void Board::calculateAdjacents() {
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c].hasMine) {
                grid_[r][c].adjacentMines = -1;
                continue;
            }

            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (inBounds(nr, nc) && grid_[nr][nc].hasMine) {
                        ++count;
                    }
                }
            }
            grid_[r][c].adjacentMines = count;
        }
    }
}
