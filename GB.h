#pragma once
#include "GB_CPU.h"
#include "GB_MEM.h"
#include "GB_GPU.h"
#include "GB_CONST.h"
#include <SDL2/SDL.h>
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

		GB(std::string fileName)
		{
            cpu.memory = mem;
            gpu.memory = mem;
			mem->loadRom(fileName);
			cpu.reg.pc = 0x0100;
			cpu.reg.sp = 0xFFFE;
		}

        void execute()
        {
            mem->data()[0xFF00] = 0xFF;
            mem->write(0xFF40, mem->read(0xFF40) | 0b10000000);
            short cycles = 0;
            int totalCycles = 0;
            auto ticks = SDL_GetTicks();
            int totalFrames = 0;
            while(cycles != -1 && !quit)
            {
                if(totalCycles >= 70224)//Only handle input and events once per frame
                {
                    //Handle SDL Events
                    while(SDL_PollEvent(&e) != 0)
                    {
                        //User requests quit
                        if(e.type == SDL_QUIT)
                            quit = true;
                    }

                    const unsigned char* keystate = SDL_GetKeyboardState(NULL);
                    mem->handleButton(keystate);

                    //If frame finished early
                    int frameTicks = SDL_GetTicks() - ticks;
                    if( frameTicks < SCREEN_TICKS_PER_FRAME )
                    {
                        //Wait remaining time
                        SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
                    }

                    ticks = SDL_GetTicks();
                    totalCycles = 0;  
                    totalFrames++;
                }

                cycles = cpu.execute();
                gpu.update(cycles);
                mem->updateTimers(cycles);

                totalCycles += cycles;
            }
        }
};