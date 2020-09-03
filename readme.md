# GGBoy

GGBoy is a multiplatform Game Boy emulator written in C++, using SDL2 to handle input and graphics

<img src="https://i.imgur.com/SirzzN9.png" alt="Link's Awakening Intro" width="300"/> <img src="https://i.imgur.com/EUAjcUA.png" alt="Pokemon Blue" width="300"/>

<img src="https://i.imgur.com/xIffgKx.png" alt="Metroid II: Return of Samus" width="300"/> <img src="https://i.imgur.com/nJhrZeo.png" alt="Link's Awakening" width="300"/>

## Building

* Requires [SDL2 Development Libraries](https://www.libsdl.org/download-2.0.php)
* `g++ main.cpp -o GGBoy -std=c++17 -lSDL2main -lSDL2`

## Running

* Drag rom onto executable
* From terminal: `GGBoy "rom.gb"`
