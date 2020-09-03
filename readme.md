# GGBoy

GGBoy is a Game Boy emulator for Windows written in C++, using SDL2 to handle input and graphics

## Building

* Requires [SDL2 Development Libraries](https://www.libsdl.org/download-2.0.php)
* `g++ main.cpp -o GGBoy -std=c++17 -lSDL2main -lSDL2 -mwindows`

## Running

* Drag rom onto executable
* From terminal: `GGBoy "rom.gb"`
