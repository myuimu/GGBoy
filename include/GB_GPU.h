#pragma once
#include "SDL.h"
#include "GB_CONST.h"
#include "GB_MEM.h"
#include <stdio.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>

struct sprite
{
    uint8_t xPos;
    uint8_t yPos;
    uint8_t tileLocation;
    uint8_t attributes;
};

class GB_GPU {
    public:
        //std::shared_ptr<std::array<uint8_t,0xFFFF>> memory;
        std::shared_ptr<GB_MEM> memory;

        unsigned int currentCycle = 0;
        uint8_t scale = 3;
        SDL_Window* window = nullptr;
        SDL_Surface* screenSurface = nullptr;
        SDL_Surface* gameSurface = nullptr;
        uint8_t* pixels = nullptr;
        uint8_t screen[144][160][4];

        GB_GPU();

        bool hblank = false;
        bool oam = false;
        bool vblank = false;

        void update(short cycles);

        void drawTiles(uint16_t line);

        void drawSprites(uint16_t line);

        uint8_t getColor(int colorNum, uint16_t address);

        void drawArrayToSurface();

    private:
        static bool spriteSort(const sprite& lhs, const sprite& rhs);
};