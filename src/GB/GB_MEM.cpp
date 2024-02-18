#include "GB_MEM.h"

void GB_MEM::loadRom(std::string &fileName) {
    std::ifstream rom(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    std::streamoff size = rom.tellg();
    fullRom.resize(size);
    //std::cout << size;

    rom.seekg(std::ios::beg);
    rom.read((char*)&fullRom[0], size * sizeof(unsigned char));
    rom.close();

    //fullRom.push_back(0);
    //while (rom.read(reinterpret_cast<char *>(fullRom.front()) + memoryLocation, 1)) { memoryLocation++; fullRom.push_back(0);}

    for (int i = 0; i < 0x4000; i++)
    {
        //std::cout << std::hex << static_cast<int>(fullRom[i]) << " ";
        memory[i] = fullRom[i];
    }

    switch(memory[0x147]) //Get MBC Type
    {
        case 0x3:
            saving = true;
        case 0x1:
        case 0x2:
            MBC1 = true;
            break;
        case 0x6:
            saving = true;
        case 0x5:
            MBC2 = true;
            break;
        case 0x10:
        case 0x13:
            saving = true;
        case 0x11:
        case 0x12:
            MBC3 = true;
            break;
        default:
            break;
    }

    if(saving)
    {
        std::string saveFile = "";
        for(int i = 0; i < 16; i++)
        {
            if(memory[0x134 + i] != 0)
                saveFile += (char)memory[0x134+i];
        }
        saveFile += ".sav";
        std::ifstream saveInput(saveFile, std::ios::in | std::ios::binary | std::ios::ate);

        saveInput.seekg(std::ios::beg);
        saveInput.read((char*)&RAMBanks[0], 0x8000 * sizeof(unsigned char));
        saveInput.close();
    }

    switch(memory[0x0148]) //Get number of rom banks
    {
        case 0 ... 8:
            ROMBanks = pow(2,memory[0x0148]+1) - 1;
            break;
        case 0x52:
            ROMBanks = 72;
            break;
        case 0x53:
            ROMBanks = 80;
            break;
        case 0x54:
            ROMBanks = 96;
            break;
    }
}

unsigned char GB_MEM::read(unsigned short index) {
    // Uncomment if using gameboy-doctor
    // if (index == LY) {
    //     return 0x90;
    // }

    switch(index)
    {
        case 0x0000 ... 0x3FFF: //ROM Bank 0
            return memory[index];
        case 0x4000 ... 0x7FFF: //ROM Bank 1 to n
        {
            unsigned short newAddress = index - 0x4000;
            return fullRom[newAddress + (currentROMBank*0x4000)];
        }
        case 0x8000 ... 0x9FFF: //VRAM
            return memory[index];
        case 0xA000 ... 0xBFFF: //External RAM if any
        {
            unsigned short newAddress = index - 0xA000;
            return RAMBanks[newAddress + (currentRAMBank*0x2000)];
        }
        case 0xC000 ... 0xCFFF: //Work RAM Bank 0
            return memory[index];
        case 0xD000 ... 0xDFFF: //Work RAM Bank 1
            return memory[index];
        case 0xE000 ... 0xFDFF: //Same as 0xC000 - 0xDDFF
            return memory[index - 0x2000];
        case 0xFE00 ... 0xFE9F: //Sprite Attribute Table (OAM)
            return memory[index];
        case 0xFEA0 ... 0xFEFF: //Not Usable
            return 0xFF;
        case 0xFF00:
            memory[0xFF00] |= 0x0F;
            switch((memory[0xFF00] & 0x30) >> 4)
            {
                case 0: //Buttons and directions
                    memory[0xFF00] &= (pressedButtons >> 4) + 0xF0;
                    memory[0xFF00] &= (pressedButtons & 0xf) + 0xF0;
                    break;
                case 1: //Directions
                    memory[0xFF00] &= (pressedButtons >> 4) + 0xF0;
                    break;
                case 2: //Buttons
                    memory[0xFF00] &= (pressedButtons & 0xf) + 0xF0;
                    break;
                case 3: //Nothing
                    memory[0xFF00] |= 0x0F;
                    break;
            }
            return memory[index];
        case 0xFF01 ... 0xFF4C: //IO Ports
            return memory[index];
        case 0xFF4D: // CGB KEY1 register - should always read 0xFF for DMG
            return 0xFF;
        case 0xFF4E ... 0xFF73: //IO Ports
            return memory[index];
        case 0xFF74: // CGB mode register - should always read 0xFF for DMG
            return 0xFF;
        case 0xFF75 ... 0xFF7F: // Undocumented IO registers
            return memory[index];
        case 0xFF80 ... 0xFFFE: //High RAM
            return memory[index];
        case 0xFFFF: //Interrupt Enable Register
            return memory[index];
        default:
            return 0xFF;
    }
}

void GB_MEM::write(unsigned short index, unsigned char value) {
    switch(index)
    {
        case 0x0000 ... 0x3FFF: //ROM Bank 0
            handleBanking(index,value);
            break;
        case 0x4000 ... 0x7FFF: //ROM Bank 1 to n
            handleBanking(index,value);
            break;
        case 0x8000 ... 0x9FFF: //VRAM
            memory[index] = value;
            break;
        case 0xA000 ... 0xBFFF: //External RAM if any
            if(enableRam)
            {
                unsigned short newAddress = index - 0xA000;
                RAMBanks[newAddress + (currentRAMBank*0x2000)] = value;
            }
            break;
        case 0xC000 ... 0xCFFF: //Work RAM Bank 0
            memory[index] = value;
            break;
        case 0xD000 ... 0xDFFF: //Work RAM Bank 1
            memory[index] = value;
            break;
        case 0xE000 ... 0xFDFF: //Same as 0xC000 - 0xDDFF
            memory[index - 0x2000] = value;
            break;
        case 0xFE00 ... 0xFE9F: //Sprite Attribute Table (OAM)
            memory[index] = value;
            break;
        case 0xFEA0 ... 0xFEFF: //Not Usable
            return;
            break;
        case 0xFF00: //Joypad
            memory[index] &= 0x0F; //Clear high nibble
            memory[index] |= value & 0xF0; //Set high nibble only
            break;
        case 0xFF01: //Serial Port
            memory[index] = value;
            //std::cout << (char)value;
            break;
        case 0xFF02 ... 0xFF03: //Serial Port
            memory[index] = value;
            break;
        case 0xFF04: //Divider register - Writing always makes timer 0
            memory[index] = 0;
            break;
        case 0xFF05 ... 0xFF44: //IO Ports
            memory[index] = value;
            break;
        case 0xFF45: //LYC Compare Register
            memory[index] = value;
            if(this->read(LY) == this->read(LYC))
            {
                this->write(STAT, this->read(STAT) | ( 1 << 2)); //Set Coincidence flag
                if(this->read(STAT) & (1 << 6)) //If LYC interrupt is enabled
                    this->write(0xFF0F, this->read(0xFF0F) | (1 << 1));
            }
            else
                this->write(STAT, this->read(STAT) & 0b11111011); //Reset Coincidence flag
            break;
        case 0xFF46: //Initiate DMA Transfer
            memory[index] = value;
            for(int i = 0; i < 0xA0; i++)
            {
                memory[0xFE00 + i] = this->read((value << 8) + i);
            }
            break;
        case 0xFF47 ... 0xFF4C: // IO Ports
            memory[index] = value;
            break;
        case 0xFF4D: // CGB KEY1 register - not writable in DMG mode
            break;
        case 0xFF4E ... 0xFF73: // IO Ports
            memory[index] = value;
            break;
        case 0xFF74: // CGB mode register - not writable in DMG mode
            break;
        case 0xFF75 ... 0xFF7F: // Undocumented
            memory[index] = value;
            break;
        case 0xFF80 ... 0xFFFE: //High RAM
            memory[index] = value;
            break;
        case 0xFFFF: //Interrupt Enable Register
            memory[index] = value;
            break;
        default:
            return;
    }
}

void GB_MEM::handleBanking(unsigned short index, unsigned char value) {
    switch(index)
    {
        case 0x0000 ... 0x1FFF: //Enable RAM
            if(MBC1 || MBC2 || MBC3)
            {
                enableRAMBanks(index,value);
            }
            break;
        case 0x2000 ... 0x3FFF: //Change ROM Bank
            if(MBC1 || MBC2 || MBC3)
            {
                changeLoROMBank(value);
            }
            break;
        case 0x4000 ... 0x5FFF: //ROM or RAM Bank change
            if(MBC1)
            {
                if(ROMBanking)
                {
                    changeHiROMBank(value);
                }
                else
                {
                    changeRAMBank(value);
                }
            }
            if(MBC3)
                changeRAMBank(value);
            break;
        case 0x6000 ... 0x7FFF:
            if(MBC1)
            {
                changeROMRAMMode(value);
            }
            break;
    }
}

void GB_MEM::enableRAMBanks(unsigned short index, unsigned char data) {
    if(MBC2 && ((read(index) & (1 << 4)) == (1 << 4)))
        return;

    unsigned char testData = data & 0xF;
    if(testData == 0xA)
        enableRam = true;
    else if (testData == 0)
        enableRam = false;
}

void GB_MEM::changeLoROMBank(unsigned char data) {
    if(MBC2)
    {
        currentROMBank = data & 0xF;
        if(currentROMBank == 0)
            currentROMBank++;
        return;
    }

    unsigned char lower5 = data & 0x1F;
    //lower5 &= ROMBanks;
    currentROMBank &= 0xE0; //Remove lower 5 bits
    currentROMBank |= lower5;

    if(MBC3)
        currentROMBank = data;

    if(currentROMBank == 0)
        currentROMBank++;
}

void GB_MEM::changeHiROMBank(unsigned char data) {
    //Remove upper 3 bits
    currentROMBank &= 0x1F;

    //Remove lower 5 bits of data
    data &= 0xE0;
    //data &= ROMBanks;
    currentROMBank |= data;
    if(currentROMBank == 0)
        currentROMBank++;
}

void GB_MEM::changeRAMBank(unsigned char data) {
    currentRAMBank = data & 0x3; //Set ram bank to lower 2 bits of data
}

void GB_MEM::changeROMRAMMode(unsigned char data) {
    data &= 0x1;
    if(data == 0)
        ROMBanking = true;
    else
        ROMBanking = false;

    if(ROMBanking)
        currentRAMBank = 0;
}

void GB_MEM::handleButton(const unsigned char *keys) {
    unsigned char joypadState = memory[0xFF00];

    if(keys[SDL_SCANCODE_UP])
        pressedButtons &= ~(1 << UP);
    if(keys[SDL_SCANCODE_DOWN])
        pressedButtons &= ~(1 << DOWN);
    if(keys[SDL_SCANCODE_LEFT])
        pressedButtons &= ~(1 << LEFT);
    if(keys[SDL_SCANCODE_RIGHT])
        pressedButtons &= ~(1 << RIGHT);
    if(keys[SDL_SCANCODE_X])
        pressedButtons &= ~(1 << (A + 4));
    if(keys[SDL_SCANCODE_Z])
        pressedButtons &= ~(1 << (B + 4));
    if(keys[SDL_SCANCODE_RETURN])
        pressedButtons &= ~(1 << (START + 4));
    if(keys[SDL_SCANCODE_RSHIFT])
        pressedButtons &= ~(1 << (SELECT + 4));

    if(!keys[SDL_SCANCODE_UP])
        pressedButtons |= (1 << UP);
    if(!keys[SDL_SCANCODE_DOWN])
        pressedButtons |= (1 << DOWN);
    if(!keys[SDL_SCANCODE_LEFT])
        pressedButtons |= (1 << LEFT);
    if(!keys[SDL_SCANCODE_RIGHT])
        pressedButtons |= (1 << RIGHT);
    if(!keys[SDL_SCANCODE_X])
        pressedButtons |= (1 << (A + 4));
    if(!keys[SDL_SCANCODE_Z])
        pressedButtons |= (1 << (B + 4));
    if(!keys[SDL_SCANCODE_RETURN])
        pressedButtons |= (1 << (START + 4));
    if(!keys[SDL_SCANCODE_RSHIFT])
        pressedButtons |= (1 << (SELECT + 4));

    switch((memory[0xFF00] & 0x30) >> 4)
    {
        case 0: //Buttons and directions
            memory[0xFF00] &= (pressedButtons >> 4) + 0xF0;
            memory[0xFF00] &= (pressedButtons & 0xf) + 0xF0;
            break;
        case 1: //Directions
            memory[0xFF00] &= (pressedButtons >> 4) + 0xF0;
            break;
        case 2: //Buttons
            memory[0xFF00] &= (pressedButtons & 0xf) + 0xF0;
            break;
        case 3: //Nothing
            memory[0xFF00] |= 0x0F;
            break;
    }

    if((memory[0xFF00] & 0x0f) < (joypadState & 0x0f))
    {
        memory[0xFF0F] |= (1 << 4); //Request joypad interrupt
    }
}

void GB_MEM::updateTimers(int cycles) {
    //Increment Divider
    elapsedDividerCycles += cycles;
    if(elapsedDividerCycles >= DIV_CYCLES)
    {
        elapsedDividerCycles -= DIV_CYCLES;
        memory[0xFF04]++;
    }

    if(memory[0xFF07] & 0x04) //If timer enabled
    {
        elapsedTimerCycles += cycles;
        unsigned char timerClock = memory[0xFF07] & 0x03;
        int cyclesNeeded = 0;
        switch(timerClock)
        {
            case 0:
                cyclesNeeded = TIM_00_CYCLES;
                break;
            case 1:
                cyclesNeeded = TIM_01_CYCLES;
                break;
            case 2:
                cyclesNeeded = TIM_10_CYCLES;
                break;
            case 3:
                cyclesNeeded = TIM_11_CYCLES;
                break;
        }
        if(elapsedTimerCycles >= cyclesNeeded)
        {
            elapsedTimerCycles -= cyclesNeeded;
            memory[0xFF05]++;
            if(memory[0xFF05] == 0) //Timer overflowed
            {
                memory[0xFF05] = memory[0xFF06]; //Fill with Timer Modulo value
                memory[0xFF0F] |= (1 << 2); //Request timer interrupt
            }
        }
    }
}

void GB_MEM::save() {
    if(saving)
    {
        std::ofstream outFile;
        std::string fileName = "";
        for(int i = 0; i < 16; i++)
        {
            if(memory[0x134 + i] != 0)
                fileName += (char)memory[0x134+i];
        }
        outFile.open(fileName + ".sav", std::ios::binary);
        outFile.write((char*)&RAMBanks[0], 0x8000 * sizeof(unsigned char));
        outFile.close();
    }
}

unsigned char &GB_MEM::operator[](int index) {
    return memory[index];
}

unsigned char *GB_MEM::data() {
    return memory;
}
