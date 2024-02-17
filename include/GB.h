#pragma once
#include "GB_CPU.h"
#include "GB_MEM.h"
#include "GB_GPU.h"
#include "GB_CONST.h"
#include "SDL.h"
#include <string>
#include <iostream>
#include <exception>
#include <iomanip>
#include <bitset>
#include <array>
#include <memory>
#include <chrono>
#include <cmath>

class GB {
	public:
        //Memory shared between cpu and gpu
        std::shared_ptr<GB_MEM> mem = std::make_shared<GB_MEM>();
        GB_CPU cpu;
        GB_GPU gpu;

        //SDL variables
        bool quit = false;
        SDL_Event e;
        const int SCREEN_FPS = 60;
        const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

		GB(std::string fileName);

        void execute();
};