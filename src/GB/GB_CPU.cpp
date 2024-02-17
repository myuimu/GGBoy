#include "GB_CPU.h"

int16_t GB_CPU::execute() {
    if(stopped)
    {
        if((memory->read(JOYPAD) & 0x0F) == 0x0F)
            return 4;
        else
        {
            memory->write(JOYPAD, lastJoypadState);
            memory->write(LCDC, lastLCDState); //Re enable lcd if necessary
            memory->write(TAC, lastTimerState);
            stopped = false;
        }
    }

    uint8_t op = memory->read(reg.pc);
    auto inst = instructions[op].execute;
    if (inst != nullptr && !stopped)
    {

        cycles = instructions[op].cycles;
        (this->*inst)();

        checkInterrupts();

        uint16_t tempCycles = cycles;
        cycles = 0;
        //printRegs();
        return tempCycles;
    }

    printRegs();
    return -1;
}

bool GB_CPU::checkInterrupts() {
    //Interrupts:
    //0 - V-Blank
    //1 - LCD Stat
    //2 - Timer
    //3 - Serial
    //4 - Joypad
    if(reg.ime || halted)
    {
        for(uint8_t i = 0; i < 5; i++)
        {
            if((testBit(i, memory->read(IE)) || halted) && testBit(i, memory->read(IF))) //Check interrupt enable register and Interrupt flags
            {                                                                            //All interrupts are enabled if halted
                if(halted)
                {
                    halted = false;
                    reg.pc++;
                    if(!reg.ime || !(testBit(i, memory->read(IE))))
                    {
                        return true;
                    }
                }
                push(reg.pc);
                reg.ime = 0; //Disable interrupts
                resMem(i,IF); //Reset interrupt flag

                reg.pc = SERVICE_VECTOR_BEGIN + (SERVICE_VECTOR_LENGTH * i); //Set PC to service vector location

                return true;
            }
        }
    }
    return false;
}

void GB_CPU::printRegs() {
    std::cout << std::hex << "Instruction 0x" << static_cast<int>(memory->read(reg.pc)) << " , " << instructions[memory->read(reg.pc)].disassembly << "!\n";
    std::cout << "AF: " << std::hex << std::setfill('0') << std::setw(4) << static_cast<int>(reg.af) << "\n";
    std::cout << std::hex << "BC: " << std::setfill('0') << std::setw(4) << static_cast<int>(reg.bc) << "\n";
    std::cout << std::hex << "DE: " << std::setfill('0') << std::setw(4) << static_cast<int>(reg.de) << "\n";
    std::cout << std::hex << "HL: " << std::setfill('0') << std::setw(4) << static_cast<int>(reg.hl) << "\n";
    std::cout << std::hex << "PC: " << std::setfill('0') << std::setw(4) << static_cast<int>(reg.pc) << "\n";
    std::cout << std::hex << "SP: " << std::setfill('0') << std::setw(4) << static_cast<int>(reg.sp) << "\n";
    std::cout << std::hex << "LCDC: " << std::setfill('0') << std::setw(4) << static_cast<int>(memory->read(0xFF40)) << "\n";
    std::cout << std::hex << "STAT: " << std::setfill('0') << std::setw(4) << static_cast<int>(memory->read(0xFF41)) << "\n";
    std::cout << std::hex << "LY: " << std::setfill('0') << std::setw(4) << static_cast<int>(memory->read(0xFF44)) << "\n";
    std::cout << "Flags: " << std::bitset<8>(reg.f) << "\n";
}

void GB_CPU::HALT() {
    halted = true;
}

void GB_CPU::STOP() {
    stopped = true;
    lastJoypadState = memory->read(JOYPAD);
    lastLCDState = memory->read(LCDC);
    lastTimerState = memory->read(TAC);
    resMem(BUTTON_ENABLE, JOYPAD); //Enable all buttons
    resMem(DIRECTION_ENABLE, JOYPAD);
    resMem(LCD_ENABLE, LCDC); //Disable LCD
    resMem(TIMER_ENABLE, TAC); //Stop Timer
    reg.pc+=2;
}

void GB_CPU::reti() {
    ret();
    reg.ime = 1;
}

void GB_CPU::ret() {
    pop(reg.pc);
}

void GB_CPU::UNUSED() {
    reg.pc++;
}

void GB_CPU::retcc() {
    bool jump = false;
    switch(memory->read(reg.pc))
    {
        case 0xC0: //Z flag reset
            jump = !testBit(Z_FLAG, reg.f);
            break;
        case 0xC8: //Z flag set
            jump = testBit(Z_FLAG, reg.f);
            break;
        case 0xD0: //C flag reset
            jump = !testBit(C_FLAG, reg.f);
            break;
        case 0xD8: //C flag set
            jump = testBit(C_FLAG, reg.f);
            break;
    }

    if(jump)
    {
        cycles += 12;
        ret();
        return;
    }

    reg.pc++;
}

void GB_CPU::RST() {
    push(reg.pc+1);
    uint8_t n = 0;
    switch(memory->read(reg.pc))
    {
        case 0xC7:
            n = 0x00;
            break;
        case 0xCF:
            n = 0x08;
            break;
        case 0xD7:
            n = 0x10;
            break;
        case 0xDF:
            n = 0x18;
            break;
        case 0xE7:
            n = 0x20;
            break;
        case 0xEF:
            n = 0x28;
            break;
        case 0xF7:
            n = 0x30;
            break;
        case 0xFF:
            n = 0x38;
            break;
    }

    reg.pc = MEMORY_BEGIN + n;
}

void GB_CPU::push(uint16_t value) {
    reg.sp -= 2;
    writeWordToMemory(reg.sp, value);
}

uint16_t GB_CPU::getWordFromMemory(uint16_t address) {
    return (memory->read(address + 1) << 8) + memory->read(address);
}

void GB_CPU::writeWordToMemory(uint16_t address, uint16_t value) {
    memory->write(address + 1, value >> 8); //Write high byte
    memory->write(address, value & 0xFF); //Write low byte
}

void GB_CPU::pop(uint16_t &destination) {
    destination = getWordFromMemory(reg.sp);
    reg.sp += 2;
}

void GB_CPU::nop() {
    reg.pc += 1;
}

void GB_CPU::pushNN() {
    uint16_t value;
    switch(memory->read(reg.pc))
    {
        case 0xF5:
            value = reg.af;
            break;
        case 0xC5:
            value = reg.bc;
            break;
        case 0xD5:
            value = reg.de;
            break;
        case 0xE5:
            value = reg.hl;
            break;
    }
    push(value);
    reg.pc++;
}

void GB_CPU::popNN() {
    uint16_t * destination = nullptr;
    switch(memory->read(reg.pc))
    {
        case 0xF1:
            destination = &reg.af;
            break;
        case 0xC1:
            destination = &reg.bc;
            break;
        case 0xD1:
            destination = &reg.de;
            break;
        case 0xE1:
            destination = &reg.hl;
            break;
    }

    pop(*destination);
    reg.f &= 0xf0;
    reg.pc++;
}

void GB_CPU::JPHL() {
    reg.pc = reg.hl;
}

void GB_CPU::JPnn() {
    reg.pc = getWordFromMemory(reg.pc + 1);
}

void GB_CPU::CCF() {
    RES(N_FLAG,reg.f);
    RES(H_FLAG,reg.f);

    if(testBit(C_FLAG,reg.f))
        RES(C_FLAG,reg.f);
    else
        SET(C_FLAG,reg.f);
    reg.pc++;
}

void GB_CPU::CB() {
    reg.pc++;
    uint8_t * operand = nullptr;
    uint8_t tempHL = memory->read(reg.hl);
    bool HL = false;
    switch(memory->read(reg.pc) & 0x0F) //Check lower nibble for operand
    {
        case 0x0:
        case 0x8:
            operand = &reg.b;
            break;
        case 0x1:
        case 0x9:
            operand = &reg.c;
            break;
        case 0x2:
        case 0xA:
            operand = &reg.d;
            break;
        case 0x3:
        case 0xB:
            operand = &reg.e;
            break;
        case 0x4:
        case 0xC:
            operand = &reg.h;
            break;
        case 0x5:
        case 0xD:
            operand = &reg.l;
            break;
        case 0x6:
        case 0xE:
            operand = &tempHL;
            HL = true;
            break;
        case 0x7:
        case 0xF:
            operand = &reg.a;
            break;
    }

    switch(memory->read(reg.pc))
    {
        case 0x00 ... 0x07:
            RLC(*operand);
            break;
        case 0x08 ... 0x0F:
            RRC(*operand);
            break;
        case 0x10 ... 0x17:
            RL(*operand);
            break;
        case 0x18 ... 0x1F:
            RR(*operand);
            break;
        case 0x20 ... 0x27:
            SLA(*operand);
            break;
        case 0x28 ... 0x2F:
            SRA(*operand);
            break;
        case 0x30 ... 0x37:
            SWAP(*operand);
            break;
        case 0x38 ... 0x3F:
            SRL(*operand);
            break;
        case 0x40 ... 0x47:
            BIT(0,*operand);
            break;
        case 0x48 ... 0x4F:
            BIT(1,*operand);
            break;
        case 0x50 ... 0x57:
            BIT(2,*operand);
            break;
        case 0x58 ... 0x5F:
            BIT(3,*operand);
            break;
        case 0x60 ... 0x67:
            BIT(4,*operand);
            break;
        case 0x68 ... 0x6F:
            BIT(5,*operand);
            break;
        case 0x70 ... 0x77:
            BIT(6,*operand);
            break;
        case 0x78 ... 0x7F:
            BIT(7,*operand);
            break;
        case 0x80 ... 0x87:
            RES(0,*operand);
            break;
        case 0x88 ... 0x8F:
            RES(1,*operand);
            break;
        case 0x90 ... 0x97:
            RES(2,*operand);
            break;
        case 0x98 ... 0x9F:
            RES(3,*operand);
            break;
        case 0xA0 ... 0xA7:
            RES(4,*operand);
            break;
        case 0xA8 ... 0xAF:
            RES(5,*operand);
            break;
        case 0xB0 ... 0xB7:
            RES(6,*operand);
            break;
        case 0xB8 ... 0xBF:
            RES(7,*operand);
            break;
        case 0xC0 ... 0xC7:
            SET(0,*operand);
            break;
        case 0xC8 ... 0xCF:
            SET(1,*operand);
            break;
        case 0xD0 ... 0xD7:
            SET(2,*operand);
            break;
        case 0xD8 ... 0xDF:
            SET(3,*operand);
            break;
        case 0xE0 ... 0xE7:
            SET(4,*operand);
            break;
        case 0xE8 ... 0xEF:
            SET(5,*operand);
            break;
        case 0xF0 ... 0xF7:
            SET(6,*operand);
            break;
        case 0xF8 ... 0xFF:
            SET(7,*operand);
            break;
    }

    if(HL)
        memory->write(reg.hl, tempHL);

    reg.pc++;
}

void GB_CPU::CBA() {
    switch(memory->read(reg.pc))
    {
        case 0x07:
            RLC(reg.a);
            break;
        case 0x17:
            RL(reg.a);
            break;
        case 0x0F:
            RRC(reg.a);
            break;
        case 0x1F:
            RR(reg.a);
    }
    RES(Z_FLAG,reg.f);
    reg.pc++;
}

void GB_CPU::RLC(uint8_t &operand) {
    bool newBit0 = testBit(7,operand);
    bool newCarry = testBit(7,operand);

    operand = operand << 1;
    operand += newBit0;
    RES(C_FLAG,reg.f);
    if(newCarry)
        SET(C_FLAG,reg.f);

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::RRC(uint8_t &operand) {
    uint8_t newBit7 = testBit(0,operand) << 7;
    bool newCarry = testBit(0,operand) << C_FLAG;

    operand = operand >> 1;
    operand += newBit7;
    RES(C_FLAG,reg.f);
    if(newCarry)
        SET(C_FLAG,reg.f);

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::RL(uint8_t &operand) {
    uint8_t newBit0 = testBit(C_FLAG,reg.f);
    bool newCarry = testBit(7,operand);

    operand = operand << 1;
    operand += newBit0;
    RES(C_FLAG,reg.f);
    if(newCarry)
        SET(C_FLAG,reg.f);

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::RR(uint8_t &operand) {
    uint8_t newBit7 = testBit(C_FLAG,reg.f) << 7;
    bool newCarry = testBit(0,operand);

    operand = operand >> 1;
    operand += newBit7;
    RES(C_FLAG,reg.f);
    if(newCarry)
        SET(C_FLAG,reg.f);

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::SLA(uint8_t &operand) {
    if(testBit(7,operand))
        SET(C_FLAG,reg.f);
    else
        RES(C_FLAG,reg.f);//Set C flag to bit 7 of operand

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    operand = operand << 1;

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::SRA(uint8_t &operand) {
    uint8_t msb = operand & (1 << 7);
    if(testBit(0,operand))
        SET(C_FLAG,reg.f);
    else
        RES(C_FLAG,reg.f);//Set c flag to bit 0 of operand

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    operand = operand >> 1;
    operand += msb;

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::SWAP(uint8_t &operand) {
    operand = (operand << 4) + ((operand & 0xF0) >> 4);
    if(operand == 0)
        SET(Z_FLAG,reg.f);
    else
        RES(Z_FLAG,reg.f);

    RES(N_FLAG,reg.f);
    RES(H_FLAG,reg.f);
    RES(C_FLAG,reg.f);
}

void GB_CPU::SRL(uint8_t &operand) {
    if(testBit(0,operand))
        SET(C_FLAG,reg.f);
    else
        RES(C_FLAG,reg.f); //Set C flag to bit 0 of operand

    RES(N_FLAG,reg.f); //Reset N and H flags
    RES(H_FLAG,reg.f);

    operand = operand >> 1;

    if(operand == 0) //Set Z flag
    {
        SET(Z_FLAG,reg.f);
    }
    else
    {
        RES(Z_FLAG,reg.f);
    }
}

void GB_CPU::BIT(uint8_t bit, uint8_t operand) {
    RES(N_FLAG,reg.f);
    SET(H_FLAG,reg.f);

    if(testBit(bit, operand)) //If bit of operand is not 0
    {
        RES(Z_FLAG,reg.f);
    }
    else //Bit is 0
    {
        SET(Z_FLAG,reg.f);
    }
}

bool GB_CPU::testBit(uint8_t bit, uint16_t operand) {
    return (operand & (1 << bit)) >> bit;
}

void GB_CPU::RES(uint8_t bit, uint8_t &operand) {
    operand &= ~(1 << bit);
}

void GB_CPU::SET(uint8_t bit, uint8_t &operand) {
    operand |= (1 << bit);
}

void GB_CPU::setMem(uint8_t bit, uint16_t address) {
    memory->write(address, memory->read(address) | (1 << bit));
}

void GB_CPU::resMem(uint8_t bit, uint16_t address) {
    memory->write(address, memory->read(address) & ~(1 << bit));
}

void GB_CPU::SCF() {
    RES(N_FLAG,reg.f);
    RES(H_FLAG,reg.f);
    SET(C_FLAG,reg.f);
    reg.pc++;
}

void GB_CPU::DAA() {
    int temp = reg.a;
    if(!testBit(N_FLAG,reg.f)) //If n flag is not set - last op was addition
    {
        if(testBit(H_FLAG,reg.f) || ((temp & 0x0f) > 0x09)) //H flag
            temp += 0x6;

        if(testBit(C_FLAG,reg.f) || temp > 0x9F) //C flag
            temp += 0x60;
    }
    else //Subtraction occured
    {
        if(testBit(H_FLAG,reg.f))
        {
            temp -= 0x6;
            if(!testBit(C_FLAG,reg.f))
                temp &= 0xFF;
        }
        if(testBit(C_FLAG,reg.f))
            temp -= 0x60;
    }

    if(temp & 0x100)
        SET(C_FLAG,reg.f);

    RES(H_FLAG,reg.f);
    if(temp == 0) //Set Z flag
        SET(Z_FLAG,reg.f);
    else
        RES(Z_FLAG,reg.f);

    reg.a = temp & 0xff;
    reg.pc++;
}

void GB_CPU::JPccnn() {
    bool jump = false;
    switch(memory->read(reg.pc))
    {
        case 0xC2: //Z flag reset
            jump = !testBit(Z_FLAG,reg.f);
            break;
        case 0xCA: //Z flag set
            jump = testBit(Z_FLAG,reg.f);
            break;
        case 0xD2: //C flag reset
            jump = !testBit(C_FLAG,reg.f);
            break;
        case 0xDA: //C flag set
            jump = testBit(C_FLAG,reg.f);
            break;
    }

    if(jump)
    {
        reg.pc = getWordFromMemory(reg.pc + 1);
        cycles += 4;
    }
    else
    {
        reg.pc += 3;
    }

}

void GB_CPU::JRccn() {
    bool jump = false;
    switch(memory->read(reg.pc))
    {
        case 0x20: //Z flag reset
            jump = !testBit(Z_FLAG,reg.f);
            break;
        case 0x28: //Z flag set
            jump = testBit(Z_FLAG,reg.f);
            break;
        case 0x30: //C flag reset
            jump = !testBit(C_FLAG,reg.f);
            break;
        case 0x38: //C flag set
            jump = testBit(C_FLAG,reg.f);
            break;
        case 0x18:
            jump = true;
            break;
    }

    reg.pc += 2;

    if(jump)
    {
        int8_t finalValue = memory->read(reg.pc - 1);
        reg.pc += finalValue;
        cycles += 4;
    }
}

void GB_CPU::INC() {
    uint8_t* r;
    uint8_t tempHL = memory->read(reg.hl);
    bool HL = false;
    switch (memory->read(reg.pc))
    {
        case 0x3C:
            r = &reg.a;
            break;
        case 0x04:
            r = &reg.b;
            break;
        case 0x0C:
            r = &reg.c;
            break;
        case 0x14:
            r = &reg.d;
            break;
        case 0x1c:
            r = &reg.e;
            break;
        case 0x24:
            r = &reg.h;
            break;
        case 0x2c:
            r = &reg.l;
            break;
        case 0x34:
            r = &tempHL;
            HL = true;
            break;
    }

    if ((*r & 0xf) + 1 > 0xf) //Check for half carry
    {
        SET(H_FLAG,reg.f); //Set H Flag
    }
    else
    {
        RES(H_FLAG,reg.f); //Clear H Flag
    }

    (*r)++;

    RES(N_FLAG,reg.f); //Clear N Flag
    if (*r == 0)
        SET(Z_FLAG,reg.f); //Set Z Flag
    else
        RES(Z_FLAG,reg.f); //Clear Z Flag

    if(HL)
        memory->write(reg.hl, tempHL);
    reg.pc += 1;
}

void GB_CPU::INC16() {
    uint16_t* r;
    switch(memory->read(reg.pc))
    {
        case 0x03:
            r = &reg.bc;
            break;
        case 0x13:
            r = &reg.de;
            break;
        case 0x23:
            r = &reg.hl;
            break;
        case 0x33:
            r = &reg.sp;
            break;
    }

    (*r)++;

    reg.pc++;
}

void GB_CPU::DEC() {
    uint8_t* r;
    uint8_t tempHL = memory->read(reg.hl);
    bool HL = false;
    switch (memory->read(reg.pc))
    {
        case 0x3D:
            r = &reg.a;
            break;
        case 0x05:
            r = &reg.b;
            break;
        case 0x0D:
            r = &reg.c;
            break;
        case 0x15:
            r = &reg.d;
            break;
        case 0x1D:
            r = &reg.e;
            break;
        case 0x25:
            r = &reg.h;
            break;
        case 0x2D:
            r = &reg.l;
            break;
        case 0x35:
            r = &tempHL;
            HL = true;
            break;
    }

    if ((*r & 0xf) - 1 < 0) //Check for half carry
    {
        SET(H_FLAG,reg.f); //Set H Flag
    }
    else
    {
        RES(H_FLAG,reg.f); //Clear H Flag
    }

    (*r)--;

    SET(N_FLAG,reg.f); //Set N Flag
    if (*r == 0)
    {
        SET(Z_FLAG,reg.f); //Set Z Flag
    }
    else
    {
        RES(Z_FLAG,reg.f); //Clear Z Flag
    }

    if(HL)
        memory->write(reg.hl, tempHL);
    reg.pc += 1;
}

void GB_CPU::DEC16() {
    uint16_t* r;
    switch (memory->read(reg.pc))
    {
        case 0x0b:
            r = &reg.bc;
            break;
        case 0x1b:
            r = &reg.de;
            break;
        case 0x2b:
            r = &reg.hl;
            break;
        case 0x3b:
            r = &reg.sp;
            break;
    }

    (*r)--;

    reg.pc += 1;
}

void GB_CPU::LDnnSP() {
    uint16_t address = getWordFromMemory(reg.pc + 1);
    writeWordToMemory(address,reg.sp);
    reg.pc += 3;
}

void GB_CPU::ADDSPr8() {
    int8_t value = memory->read(reg.pc + 1);
    uint8_t unsignedValue = memory->read(reg.pc + 1);
    uint16_t finalValue = reg.sp + value;

    if((reg.sp & 0xff) + unsignedValue > 0xff)
    {
        SET(C_FLAG,reg.f); //Set C flag
    }
    else
    {
        RES(C_FLAG,reg.f);
    }

    if((((reg.sp & 0xf) + (unsignedValue & 0xf)) & 0x10) == 0x10)
    {
        SET(H_FLAG,reg.f); //Set H flag
    }
    else
    {
        RES(H_FLAG,reg.f);
    }

    RES(Z_FLAG,reg.f);
    RES(N_FLAG,reg.f);

    reg.sp = finalValue;

    reg.pc += 2;
}

void GB_CPU::LDHLSPn() {
    int8_t value = memory->read(reg.pc + 1);
    uint8_t unsignedValue = memory->read(reg.pc + 1);
    uint16_t finalValue = reg.sp + value;

    if((reg.sp & 0xff) + unsignedValue > 0xff)
    {
        SET(C_FLAG,reg.f); //Set C flag
    }
    else
    {
        RES(C_FLAG,reg.f);
    }

    if(((reg.sp & 0xf) + (unsignedValue & 0xf)) & 0x10)
    {
        SET(H_FLAG,reg.f); //Set H flag
    }
    else
    {
        RES(H_FLAG,reg.f);
    }

    RES(Z_FLAG,reg.f);
    RES(N_FLAG,reg.f);

    reg.hl = finalValue;

    reg.pc += 2;
}

void GB_CPU::LDr1r2() {
    uint8_t* source = nullptr;
    uint8_t* destination = nullptr;
    uint16_t location = getWordFromMemory(reg.pc + 1);
    uint16_t writeLocation = reg.hl;
    uint8_t imm = memory->read(reg.pc + 1);
    uint8_t tempMem = memory->read(reg.hl);
    bool writingToMemory = false;
    bool writingToLocation = false;
    switch (memory->read(reg.pc))
    {
        case 0x7F:
            source = &reg.a;
            destination = &reg.a;
            break;
        case 0x78:
            source = &reg.b;
            destination = &reg.a;
            break;
        case 0x79:
            source = &reg.c;
            destination = &reg.a;
            break;
        case 0x7A:
            source = &reg.d;
            destination = &reg.a;
            break;
        case 0x7B:
            source = &reg.e;
            destination = &reg.a;
            break;
        case 0x7C:
            source = &reg.h;
            destination = &reg.a;
            break;
        case 0x7D:
            source = &reg.l;
            destination = &reg.a;
            break;
        case 0x7E:
            source = &tempMem;
            destination = &reg.a;
            break;

        case 0x40:
            source = &reg.b;
            destination = &reg.b;
            break;
        case 0x41:
            source = &reg.c;
            destination = &reg.b;
            break;
        case 0x42:
            source = &reg.d;
            destination = &reg.b;
            break;
        case 0x43:
            source = &reg.e;
            destination = &reg.b;
            break;
        case 0x44:
            source = &reg.h;
            destination = &reg.b;
            break;
        case 0x45:
            source = &reg.l;
            destination = &reg.b;
            break;
        case 0x46:
            source = &tempMem;
            destination = &reg.b;
            break;

        case 0x48:
            source = &reg.b;
            destination = &reg.c;
            break;
        case 0x49:
            source = &reg.c;
            destination = &reg.c;
            break;
        case 0x4A:
            source = &reg.d;
            destination = &reg.c;
            break;
        case 0x4B:
            source = &reg.e;
            destination = &reg.c;
            break;
        case 0x4C:
            source = &reg.h;
            destination = &reg.c;
            break;
        case 0x4D:
            source = &reg.l;
            destination = &reg.c;
            break;
        case 0x4E:
            source = &tempMem;
            destination = &reg.c;
            break;

        case 0x50:
            source = &reg.b;
            destination = &reg.d;
            break;
        case 0x51:
            source = &reg.c;
            destination = &reg.d;
            break;
        case 0x52:
            source = &reg.d;
            destination = &reg.d;
            break;
        case 0x53:
            source = &reg.e;
            destination = &reg.d;
            break;
        case 0x54:
            source = &reg.h;
            destination = &reg.d;
            break;
        case 0x55:
            source = &reg.l;
            destination = &reg.d;
            break;
        case 0x56:
            source = &tempMem;
            destination = &reg.d;
            break;

        case 0x58:
            source = &reg.b;
            destination = &reg.e;
            break;
        case 0x59:
            source = &reg.c;
            destination = &reg.e;
            break;
        case 0x5A:
            source = &reg.d;
            destination = &reg.e;
            break;
        case 0x5B:
            source = &reg.e;
            destination = &reg.e;
            break;
        case 0x5C:
            source = &reg.h;
            destination = &reg.e;
            break;
        case 0x5D:
            source = &reg.l;
            destination = &reg.e;
            break;
        case 0x5E:
            source = &tempMem;
            destination = &reg.e;
            break;

        case 0x60:
            source = &reg.b;
            destination = &reg.h;
            break;
        case 0x61:
            source = &reg.c;
            destination = &reg.h;
            break;
        case 0x62:
            source = &reg.d;
            destination = &reg.h;
            break;
        case 0x63:
            source = &reg.e;
            destination = &reg.h;
            break;
        case 0x64:
            source = &reg.h;
            destination = &reg.h;
            break;
        case 0x65:
            source = &reg.l;
            destination = &reg.h;
            break;
        case 0x66:
            source = &tempMem;
            destination = &reg.h;
            break;

        case 0x68:
            source = &reg.b;
            destination = &reg.l;
            break;
        case 0x69:
            source = &reg.c;
            destination = &reg.l;
            break;
        case 0x6A:
            source = &reg.d;
            destination = &reg.l;
            break;
        case 0x6B:
            source = &reg.e;
            destination = &reg.l;
            break;
        case 0x6C:
            source = &reg.h;
            destination = &reg.l;
            break;
        case 0x6D:
            source = &reg.l;
            destination = &reg.l;
            break;
        case 0x6E:
            source = &tempMem;
            destination = &reg.l;
            break;

        case 0x70:
            source = &reg.b;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x71:
            source = &reg.c;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x72:
            source = &reg.d;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x73:
            source = &reg.e;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x74:
            source = &reg.h;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x75:
            source = &reg.l;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0x36:
            source = &imm;
            destination = &tempMem;
            writingToMemory = true;
            reg.pc += 1;
            break;

            //LD n,A cases
        case 0x47:
            source = &reg.a;
            destination = &reg.b;
            break;
        case 0x4F:
            source = &reg.a;
            destination = &reg.c;
            break;
        case 0x57:
            source = &reg.a;
            destination = &reg.d;
            break;
        case 0x5F:
            source = &reg.a;
            destination = &reg.e;
            break;
        case 0x67:
            source = &reg.a;
            destination = &reg.h;
            break;
        case 0x6F:
            source = &reg.a;
            destination = &reg.l;
            break;
        case 0x02:
            source = &reg.a;
            tempMem = memory->read(reg.bc);
            destination = &tempMem;
            writingToMemory = true;
            writeLocation = reg.bc;
            break;
        case 0x12:
            source = &reg.a;
            tempMem = memory->read(reg.de);
            destination = &tempMem;
            writingToMemory = true;
            writeLocation = reg.de;
            break;
        case 0x77:
            source = &reg.a;
            destination = &tempMem;
            writingToMemory = true;
            break;
        case 0xEA:
        {

            source = &reg.a;
            tempMem = memory->read(location);
            destination = &tempMem;
            writingToMemory = true;
            writeLocation = location;
            reg.pc += 2;
        }
            break;

            //LD A,n cases
        case 0x0A:
            tempMem = memory->read(reg.bc);
            source = &tempMem;
            destination = &reg.a;
            break;
        case 0x1A:
            tempMem = memory->read(reg.de);
            source = &tempMem;
            destination = &reg.a;
            break;
        case 0xFA:
        {
            tempMem = memory->read(location);
            source = &tempMem;
            destination = &reg.a;
            reg.pc += 2;
        }
            break;
        case 0x3E:
            source = &imm;
            destination = &reg.a;
            reg.pc++;
            break;

            //LDH Cases
        case 0xE0:
            source = &reg.a;
            tempMem = memory->read(0xFF00 + imm);
            destination = &tempMem;
            writingToMemory = true;
            writeLocation = 0xFF00 + imm;
            reg.pc++;
            break;
        case 0xF0:
            tempMem = memory->read(0xFF00 + imm);
            source = &tempMem;
            destination = &reg.a;
            reg.pc++;
            break;
        case 0xE2:
            source = &reg.a;
            tempMem = memory->read(0xFF00 + reg.c);
            destination = &tempMem;
            writingToMemory = true;
            writeLocation = 0xFF00 + reg.c;
            break;
        case 0xF2:
            tempMem = memory->read(0xFF00 + reg.c);
            source = &tempMem;
            destination = &reg.a;
            break;
    }

    *destination = *source;

    if(writingToMemory)
    {
        memory->write(writeLocation, tempMem);
    }
    reg.pc += 1;
}

void GB_CPU::LDSPHL() {
    reg.sp = reg.hl;
    reg.pc += 1;
}

void GB_CPU::LDI() {
    uint8_t* source;
    uint8_t* destination;

    uint8_t tempHL = memory->read(reg.hl);
    bool writing = false;

    switch(memory->read(reg.pc))
    {
        case 0x2A:
            source = &tempHL;
            destination = &reg.a;
            break;
        case 0x22:
            source = &reg.a;
            destination = &tempHL;
            writing = true;
            break;
    }

    *destination = *source;

    if(writing)
        memory->write(reg.hl,tempHL);

    reg.hl++;

    reg.pc += 1;
}

void GB_CPU::LDnnN() {
    uint8_t* destination = nullptr;
    switch(memory->read(reg.pc))
    {
        case 0x06:
            destination = &reg.b;
            break;
        case 0x0E:
            destination = &reg.c;
            break;
        case 0x16:
            destination = &reg.d;
            break;
        case 0x1E:
            destination = &reg.e;
            break;
        case 0x26:
            destination = &reg.h;
            break;
        case 0x2E:
            destination = &reg.l;
            break;
    }

    *destination = memory->read(reg.pc + 1);
    reg.pc += 2;
}

void GB_CPU::LDD() {
    uint8_t* source;
    uint8_t* destination;

    uint8_t tempHL = memory->read(reg.hl);
    bool writing = false;

    switch(memory->read(reg.pc))
    {
        case 0x32:
            destination = &tempHL;
            writing = true;
            source = &reg.a;
            break;
        case 0x3A:
            source = &tempHL;
            destination = &reg.a;
            break;
    }

    *destination = *source;

    if(writing)
        memory->write(reg.hl,tempHL);

    reg.hl--;

    reg.pc += 1;
}

void GB_CPU::CAL() {
    bool branch = 0;
    switch (memory->read(reg.pc))
    {
        case 0xC4: //Z flag reset
            branch = !testBit(Z_FLAG,reg.f);
            break;
        case 0xCC: //Z flag set
            branch = testBit(Z_FLAG,reg.f);
            break;
        case 0xD4: //C flag reset
            branch = !testBit(C_FLAG,reg.f);
            break;
        case 0xDC: //C flag set
            branch = testBit(C_FLAG,reg.f);
            break;
        case 0xCD:
            branch = true;
            break;
    }

    if (branch)
    {
        push(reg.pc+3);
        reg.pc = getWordFromMemory(reg.pc + 1);
        cycles += 12;
    }
    else
    {
        reg.pc += 3;
    }
}

void GB_CPU::OR() {
    uint8_t value;
    switch (memory->read(reg.pc))
    {
        case 0xB7:
            value = reg.a;
            break;
        case 0xB0:
            value = reg.b;
            break;
        case 0xB1:
            value = reg.c;
            break;
        case 0xB2:
            value = reg.d;
            break;
        case 0xB3:
            value = reg.e;
            break;
        case 0xB4:
            value = reg.h;
            break;
        case 0xB5:
            value = reg.l;
            break;
        case 0xB6:
            value = memory->read(reg.hl);
            break;
        case 0xF6:
            value = memory->read(reg.pc + 1);
            reg.pc += 1;
            break;
    }

    reg.a |= value;

    if (reg.a == 0)
        SET(Z_FLAG,reg.f);
    else
        RES(Z_FLAG,reg.f);

    RES(N_FLAG,reg.f);
    RES(H_FLAG,reg.f);
    RES(C_FLAG,reg.f);

    reg.pc += 1;
}

void GB_CPU::XOR() {
    uint8_t value;
    switch (memory->read(reg.pc))
    {
        case 0xAF:
            value = reg.a;
            break;
        case 0xA8:
            value = reg.b;
            break;
        case 0xA9:
            value = reg.c;
            break;
        case 0xAA:
            value = reg.d;
            break;
        case 0xAB:
            value = reg.e;
            break;
        case 0xAC:
            value = reg.h;
            break;
        case 0xAD:
            value = reg.l;
            break;
        case 0xAE:
            value = memory->read(reg.hl);
            break;
        case 0xEE:
            value = memory->read(reg.pc + 1);
            reg.pc += 1;
            break;
    }

    reg.a ^= value;

    if (reg.a == 0)
        SET(Z_FLAG,reg.f);
    else
        RES(Z_FLAG,reg.f);

    RES(N_FLAG,reg.f);
    RES(H_FLAG,reg.f);
    RES(C_FLAG,reg.f);

    reg.pc += 1;
}

void GB_CPU::AND() {
    uint8_t value;
    switch(memory->read(reg.pc))
    {
        case 0xA7:
            value = reg.a;
            break;
        case 0xA0:
            value = reg.b;
            break;
        case 0xA1:
            value = reg.c;
            break;
        case 0xA2:
            value = reg.d;
            break;
        case 0xA3:
            value = reg.e;
            break;
        case 0xA4:
            value = reg.h;
            break;
        case 0xA5:
            value = reg.l;
            break;
        case 0xA6:
            value = memory->read(reg.hl);
            break;
        case 0xE6:
            value = memory->read(reg.pc+1);
            reg.pc++;
            break;
    }

    reg.a &= value;

    if (reg.a == 0)
        SET(Z_FLAG,reg.f);
    else
        RES(Z_FLAG,reg.f);

    RES(N_FLAG,reg.f);
    SET(H_FLAG,reg.f);
    RES(C_FLAG,reg.f);

    reg.pc += 1;
}

void GB_CPU::CP() {
    uint8_t value;
    uint8_t newA = reg.a;
    bool msb = testBit(7,reg.a);
    switch(memory->read(reg.pc))
    {
        case 0xBF:
            value = reg.a;
            break;
        case 0xB8:
            value = reg.b;
            break;
        case 0xB9:
            value = reg.c;
            break;
        case 0xBA:
            value = reg.d;
            break;
        case 0xBB:
            value = reg.e;
            break;
        case 0xBC:
            value = reg.h;
            break;
        case 0xBD:
            value = reg.l;
            break;
        case 0xBE:
            value = memory->read(reg.hl);
            break;
        case 0xFE:
            value = memory->read(reg.pc + 1);
            reg.pc += 1;
            break;
    }

    if (((newA & 0xf) - (value & 0xf)) < 0)
        SET(H_FLAG,reg.f); //Set H flag
    else
        RES(H_FLAG,reg.f); //Clear H flag

    newA -= value;

    if (value > reg.a)
        SET(C_FLAG,reg.f); //Set C flag
    else
        RES(C_FLAG,reg.f); //Clear C flag


    SET(N_FLAG,reg.f); //Set N flag

    if (newA == 0)
        SET(Z_FLAG,reg.f); //Set Z flag
    else
        RES(Z_FLAG,reg.f); //Clear Z Flag

    reg.pc += 1;
}

void GB_CPU::ADD() {
    uint8_t value;
    bool carry = testBit(C_FLAG,reg.f);
    bool adc = false;
    bool msb = testBit(7,reg.a);

    switch(memory->read(reg.pc))
    {
        case 0x87:
            value = reg.a;
            break;
        case 0x80:
            value = reg.b;
            break;
        case 0x81:
            value = reg.c;
            break;
        case 0x82:
            value = reg.d;
            break;
        case 0x83:
            value = reg.e;
            break;
        case 0x84:
            value = reg.h;
            break;
        case 0x85:
            value = reg.l;
            break;
        case 0x86:
            value = memory->read(reg.hl);
            break;
        case 0xC6:
            value = memory->read(reg.pc + 1);
            reg.pc++;
            break;

            //ADC Cases
        case 0x8F:
            value = reg.a;
            adc = true;
            break;
        case 0x88:
            value = reg.b;
            adc = true;
            break;
        case 0x89:
            value = reg.c;
            adc = true;
            break;
        case 0x8A:
            value = reg.d;
            adc = true;
            break;
        case 0x8B:
            value = reg.e;
            adc = true;
            break;
        case 0x8C:
            value = reg.h;
            adc = true;
            break;
        case 0x8D:
            value = reg.l;
            adc = true;
            break;
        case 0x8E:
            value = memory->read(reg.hl);
            adc = true;
            break;
        case 0xCE:
            value = memory->read(reg.pc + 1);
            adc = true;
            reg.pc++;
            break;


    }

    if(value + reg.a > 0xff)
        SET(C_FLAG,reg.f); //Set C flag
    else
        RES(C_FLAG,reg.f);

    if(((reg.a & 0xf) + (value & 0xf)) & 0x10)
        SET(H_FLAG,reg.f); //Set H flag
    else
        RES(H_FLAG,reg.f);

    reg.a += value;

    if(adc)
    {
        if (((reg.a & 0xf) + (carry & 0xf)) & 0x10)
            SET(H_FLAG,reg.f); //Set H flag
        if (carry + reg.a > 0xff)
            SET(C_FLAG,reg.f); //Set C flag
        reg.a += carry;
    }

    if(reg.a == 0)
        SET(Z_FLAG,reg.f); //Set Z flag
    else
        RES(Z_FLAG,reg.f);

    RES(N_FLAG,reg.f); //Reset N flag

    reg.pc += 1;

}

void GB_CPU::ADD16() {
    uint16_t * source = nullptr;

    switch(memory->read(reg.pc))
    {
        case 0x09:
            source = &reg.bc;
            break;
        case 0x19:
            source = &reg.de;
            break;
        case 0x29:
            source = &reg.hl;
            break;
        case 0x39:
            source = &reg.sp;
            break;
    }

    RES(N_FLAG,reg.f); //Reset N flag

    if(reg.hl + *source > 0xffff)
        SET(C_FLAG,reg.f); //Set C flag
    else
        RES(C_FLAG,reg.f);

    if(((reg.hl & 0x0fff) + (*source & 0x0fff)) & 0x1000)
        SET(H_FLAG,reg.f);
    else
        RES(H_FLAG,reg.f);

    reg.hl += *source;

    reg.pc++;
}

void GB_CPU::SUB() {
    uint8_t value;
    bool carry = testBit(C_FLAG,reg.f);
    bool sbc = false;
    switch (memory->read(reg.pc))
    {
        case 0x97:
            value = reg.a;
            break;
        case 0x90:
            value = reg.b;
            break;
        case 0x91:
            value = reg.c;
            break;
        case 0x92:
            value = reg.d;
            break;
        case 0x93:
            value = reg.e;
            break;
        case 0x94:
            value = reg.h;
            break;
        case 0x95:
            value = reg.l;
            break;
        case 0x96:
            value = memory->read(reg.hl);
            break;
        case 0xd6:
            value = memory->read(reg.pc + 1);
            reg.pc += 1;
            break;

            //SBC Cases
        case 0x9f:
            value = reg.a;
            sbc = true;
            break;
        case 0x98:
            value = reg.b;
            sbc = true;
            break;
        case 0x99:
            value = reg.c;
            sbc = true;
            break;
        case 0x9A:
            value = reg.d;
            sbc = true;
            break;
        case 0x9B:
            value = reg.e;
            sbc = true;
            break;
        case 0x9C:
            value = reg.h;
            sbc = true;
            break;
        case 0x9D:
            value = reg.l;
            sbc = true;
            break;
        case 0x9E:
            value = memory->read(reg.hl);
            sbc = true;
            break;
        case 0xDE:
            value = memory->read(reg.pc + 1);
            sbc = true;
            reg.pc++;
            break;
    }

    if (((reg.a & 0xf) - (value & 0xf)) < 0)
        SET(H_FLAG,reg.f); //Set H flag
    else
        RES(H_FLAG,reg.f); //Clear H flag

    if (value > reg.a)
        SET(C_FLAG,reg.f); //Set C flag
    else
        RES(C_FLAG,reg.f); //Clear C flag

    reg.a -= value;

    if(sbc)
    {
        if (((reg.a & 0xf) - (carry & 0xf)) < 0)
            SET(H_FLAG,reg.f); //Set H flag
        if (carry > reg.a)
            SET(C_FLAG,reg.f); //Set C flag
        reg.a -= carry;
    }


    SET(N_FLAG,reg.f); //Set N flag

    if (reg.a == 0)
        SET(Z_FLAG,reg.f); //Set Z flag
    else
        RES(Z_FLAG,reg.f); //Clear Z Flag

    reg.pc += 1;
}

void GB_CPU::LDNnn() {
    uint16_t* destination = nullptr;
    switch(memory->read(reg.pc))
    {
        case 0x01:
            destination = &reg.bc;
            break;
        case 0x11:
            destination = &reg.de;
            break;
        case 0x21:
            destination = &reg.hl;
            break;
        case 0x31:
            destination = &reg.sp;
            break;
    }

    *destination = getWordFromMemory(reg.pc + 1);
    reg.pc += 3;
}

void GB_CPU::cpl() {
    reg.a = ~(reg.a);
    SET(N_FLAG,reg.f);
    SET(H_FLAG,reg.f);
    reg.pc++;
}

void GB_CPU::di() {
    reg.pc++;
    cycles = 4 + execute();
    reg.ime = 0;
}

void GB_CPU::ei() {
    reg.pc++;
    cycles = 4 + execute();
    reg.ime = 1;
}
