#include "GB.h"

GB::GB(std::string fileName) {
    cpu.memory = mem;
    gpu.memory = mem;
    mem->loadRom(fileName);
    cpu.reg.pc = 0x0100;
    cpu.reg.sp = 0xFFFE;
}

void GB::execute() {
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
