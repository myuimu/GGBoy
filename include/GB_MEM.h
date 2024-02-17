#pragma once
#include "GB_CONST.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "SDL.h"
#include <cmath>

class GB_MEM {
	public:
        std::vector<unsigned char> fullRom;
		unsigned char memory[0x10000];
        unsigned char RAMBanks[0x8000];
        bool saving = false;
        bool MBC1 = false;
        bool MBC2 = false;
        bool MBC3 = false;
        unsigned char currentROMBank = 1;
        unsigned short ROMBanks = 0;
        unsigned char currentRAMBank = 0;
        bool enableRam = false;
        bool ROMBanking = false;

        const int DIV_CYCLES = 256;
        const int TIM_00_CYCLES = 64;
        const int TIM_01_CYCLES = 4096;
        const int TIM_10_CYCLES = 1024;
        const int TIM_11_CYCLES = 256;
        int elapsedTimerCycles = 0;
        int elapsedDividerCycles = 0;
        
        enum buttons
        {
            A = 0,
            B = 1,
            START = 3,
            SELECT = 2,
            UP = 2,
            DOWN = 3,
            LEFT = 1,
            RIGHT = 0
        };
        unsigned char pressedButtons = 0xFF;

		void loadRom(std::string &fileName);

		unsigned char read(unsigned short index);

		void write(unsigned short index, unsigned char value);

        void handleBanking(unsigned short index, unsigned char value);

        void enableRAMBanks(unsigned short index, unsigned char data);

        void changeLoROMBank(unsigned char data);

        void changeHiROMBank(unsigned char data);

        void changeRAMBank(unsigned char data);

        void changeROMRAMMode(unsigned char data);

        void handleButton(const unsigned char * keys);

        void updateTimers(int cycles);

        void save();

        unsigned char copy = 0;

        unsigned char& operator[](int index);

        unsigned char* data();
};