// Tile.h
#pragma once

struct Tile {
    bool hasMine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacentMines = 0;
};
