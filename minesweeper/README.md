# Textbaserat Minröj (C++)

Ett enkelt textbaserat Minröj-spel gjort i C++. Spelet körs i terminalen och stödjer:

- Avslöja rutor (`r b2`)
- Flagga potentiella minor (`f c3`)
- Spara spelet till fil (`s save.txt`)
- Ladda sparat spel vid start
- Fullt UTF-8-stöd för svenska tecken

## Kompilering (VS Code)

Tryck **Ctrl + Shift + B** för att köra build-tasken som kompilerar:

main.cpp Board.cpp Game.cpp -o minesweeper

## Kör spelet

Windows:

.\minesweeper.exe

## Start

Vid start väljer du:

- **N** för nytt spel
- **L** för att ladda sparat spel
