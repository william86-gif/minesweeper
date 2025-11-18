// main.cpp
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
#include <memory>
#include <locale>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "Game.h"

int main() {
    // ===== UTF-8 / svenska tecken =====
#ifdef _WIN32
    // Windows-konsol till UTF-8 (så å/ä/ö visas rätt)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Locale för Linux/macOS + standardbiblioteket
    std::setlocale(LC_ALL, "sv_SE.UTF-8");
    // ================================

    std::cout << "Textbaserat Minröj i C++\n";
    std::cout << "------------------------\n";
    std::cout << "(N)ytt spel eller (L)adda sparat spel? ";

    char choice;
    std::cin >> choice;
    choice = static_cast<char>(std::tolower(static_cast<unsigned char>(choice)));

    // rensa rest av raden
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::unique_ptr<Game> game = std::make_unique<Game>();

    if (choice == 'l') {
        std::cout << "Ange filnamn att ladda: ";
        std::string filename;
        std::getline(std::cin, filename);
        if (!game->loadFromFile(filename) || !game->hasBoard()) {
            std::cout << "Kunde inte ladda spelet. Startar nytt istället.\n";
            int rows = 6, cols = 6, mines = 6;
            game = std::make_unique<Game>(rows, cols, mines);
        }
    } else {
        int rows = 6;
        int cols = 6;
        int mines = 6;
        std::string line;

        std::cout << "Ange antal rader (standard 6): ";
        std::getline(std::cin, line);
        if (!line.empty()) {
            try {
                int v = std::stoi(line);
                if (v > 0 && v <= 26) rows = v;
            } catch (...) {
                std::cout << "Ogiltig inmatning, använder standard.\n";
            }
        }

        std::cout << "Ange antal kolumner (standard 6): ";
        std::getline(std::cin, line);
        if (!line.empty()) {
            try {
                int v = std::stoi(line);
                if (v > 0) cols = v;
            } catch (...) {
                std::cout << "Ogiltig inmatning, använder standard.\n";
            }
        }

        std::cout << "Ange antal minor (standard 6): ";
        std::getline(std::cin, line);
        if (!line.empty()) {
            try {
                int v = std::stoi(line);
                if (v > 0 && v < rows * cols) mines = v;
            } catch (...) {
                std::cout << "Ogiltig inmatning, använder standard.\n";
            }
        }

        game = std::make_unique<Game>(rows, cols, mines);
    }

    game->run();

    std::cout << "Tack för att du spelade!\n";
    return 0;
}
