#include "GB.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
	GB gameboy(__argv[1]);
	gameboy.execute();
    gameboy.mem->save();
	return 0;
}