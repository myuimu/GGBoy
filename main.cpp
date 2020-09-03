#define SDL_MAIN_HANDLED
#include "GB.h"

int main(int argc, char* argv[])
{
  	GB gameboy(__argv[1]);
	gameboy.execute();
    gameboy.mem->save();
	return 0;
}