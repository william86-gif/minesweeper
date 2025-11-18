// Game.cpp
#include "Game.h"
#include <iostream>
#include <cctype>
#include <limits>

Game::Game(int rows, int cols, int mines)
    : board_(std::make_unique<Board>(rows, cols, mines)),
      gameOver_(false), won_(false) {}

Game::Game()
    : board_(nullptr), gameOver_(false), won_(false) {}

void Game::run() {
    while (!gameOver_ && !won_) {
        board_->print();
        std::cout << "Kommandon:\n";
        std::cout << "  r <ruta>     = undersök ruta (t.ex. r b2)\n";
        std::cout << "  f <ruta>     = markera/avmarkera mina (t.ex. f c3)\n";
        std::cout << "  s <filnamn>  = spara spelet (t.ex. s sparfil.txt)\n";
        std::cout << "  q            = avsluta utan att spara\n";
        std::cout << "Vad vill du göra? ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nInläsningsfel. Avslutar.\n";
            return;
        }

        if (line.empty()) {
            std::cout << "Tom inmatning. Försök igen.\n";
            continue;
        }

        char cmd = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        if (cmd == 'q') {
            std::cout << "Avslutar spelet.\n";
            return;
        } else if (cmd == 's') {
            std::string filename = parseFilename(line);
            if (filename.empty()) {
                std::cout << "Inget filnamn angivet. Kommandot: s <filnamn>\n";
                continue;
            }
            if (board_->saveToFile(filename, gameOver_, won_)) {
                std::cout << "Spelet sparades till \"" << filename << "\".\n";
            } else {
                std::cout << "Kunde inte spara till \"" << filename << "\".\n";
            }
            continue;
        } else if (cmd == 'r' || cmd == 'f') {
            int row, col;
            if (!parseCoordinate(line, row, col)) {
                std::cout << "Ogiltig ruta. Exempel: r b2 eller f c3\n";
                continue;
            }
            if (!board_->inBounds(row, col)) {
                std::cout << "Ruta utanför spelplanen.\n";
                continue;
            }

            if (cmd == 'f') {
                board_->toggleFlag(row, col);
            } else { // reveal
                bool ok = board_->reveal(row, col);
                if (!ok) {
                    gameOver_ = true;
                    board_->print(true); // visa allt
                    char rowLabel = static_cast<char>('a' + row);
                    std::cout << "Pang!! Game Over. Ruta "
                              << rowLabel << (col + 1)
                              << " innehöll en bomb.\n";
                    break;
                }
            }

            if (board_->allSafeRevealed()) {
                won_ = true;
                board_->print(true);
                std::cout << "Grattis! Du har vunnit – alla säkra rutor är avslöjade!\n";
                break;
            }
        } else {
            std::cout << "Okänt kommando. Använd r/f/s/q.\n";
        }
    }
}

bool Game::loadFromFile(const std::string &filename) {
    bool go = false, w = false;
    auto loadedBoard = Board::loadFromFile(filename, go, w);
    if (!loadedBoard) {
        std::cout << "Kunde inte läsa spelet från \"" << filename << "\".\n";
        return false;
    }

    board_ = std::move(loadedBoard);
    gameOver_ = go;
    won_ = w;

    if (gameOver_) {
        std::cout << "Det sparade spelet var redan slut.\n";
    } else if (won_) {
        std::cout << "Det sparade spelet var redan vunnet.\n";
    } else {
        std::cout << "Spelet laddades från \"" << filename << "\".\n";
    }
    return true;
}

bool Game::hasBoard() const {
    return static_cast<bool>(board_);
}

bool Game::parseCoordinate(const std::string &line, int &row, int &col) {
    // Förväntar sig t.ex. "r b2" eller "f a10"
    std::size_t pos = 1;
    while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
        ++pos;
    }
    if (pos >= line.size()) return false;

    char rowChar = static_cast<char>(std::tolower(static_cast<unsigned char>(line[pos])));
    if (rowChar < 'a' || rowChar > 'z') return false;
    row = rowChar - 'a';
    ++pos;

    while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
        ++pos;
    }
    if (pos >= line.size()) return false;

    int colNum = 0;
    bool hasDigit = false;
    while (pos < line.size() && std::isdigit(static_cast<unsigned char>(line[pos]))) {
        hasDigit = true;
        colNum = colNum * 10 + (line[pos] - '0');
        ++pos;
    }
    if (!hasDigit) return false;
    col = colNum - 1; // 1-baserat till 0-baserat

    return true;
}

std::string Game::parseFilename(const std::string &line) {
    std::size_t pos = 1;
    while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
        ++pos;
    }
    if (pos >= line.size()) return "";
    std::string filename = line.substr(pos);
    while (!filename.empty() &&
           std::isspace(static_cast<unsigned char>(filename.back()))) {
        filename.pop_back();
    }
    return filename;
}
