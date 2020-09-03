#define SDL_MAIN_HANDLED
#include "GB.h"
//#include <Windows.h>

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
// 	GB gameboy(__argv[1]);
// 	gameboy.execute();
//     gameboy.mem->save();
// 	return 0;
// }

int main(int argc, char* argv[])
{
  	GB gameboy(__argv[1]);
	gameboy.execute();
    gameboy.mem->save();
	return 0;
}