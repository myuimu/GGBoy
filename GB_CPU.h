#pragma once
#include <string>
#include "GB_MEM.h"
#include "GB_CONST.h"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <memory>

class GB_CPU {
	public:

		struct registers {

			union { //AF
				struct {
					uint8_t f;
					uint8_t a;
				};
				uint16_t af = AF_START;
			};

			union { //BC
				struct {
					uint8_t c;
					uint8_t b;
				};
				uint16_t bc = BC_START;
			};

			union { //DE
				struct {
					uint8_t e;
					uint8_t d;
				};
				uint16_t de = DE_START;
			};

			union { //HL
				struct {
					uint8_t l;
					uint8_t h;
				};
				uint16_t hl = HL_START;
			};

			//Program counter and stack pointer
			uint16_t pc, sp;

            //Interrupt master enable
            bool ime;
		} reg;

        std::shared_ptr<GB_MEM> memory;

		struct instruction {
			std::string disassembly;
			uint8_t cycles;
			void (GB_CPU::*execute)();
		};

		const instruction instructions[256] = {
			{ "NOP", 4, &GB_CPU::nop },				// 0x00
			{ "LD BC,nn", 12, &GB_CPU::LDNnn },				// 0x01
			{ "LD (BC),A", 8, &GB_CPU::LDr1r2 },				// 0x02
			{ "INC BC", 8, &GB_CPU::INC16 },				// 0x03
			{ "INC B", 4, &GB_CPU::INC },				// 0x04
			{ "DEC B", 4, &GB_CPU::DEC },				// 0x05
			{ "LD B,n", 8, &GB_CPU::LDnnN },				// 0x06
			{ "RLCA", 4, &GB_CPU::CBA },				// 0x07
			{ "LD (nn),SP", 20, &GB_CPU::LDnnSP },				// 0x08
			{ "ADD HL,BC", 8, &GB_CPU::ADD16 },				// 0x09
			{ "LD A,(BC)", 8, &GB_CPU::LDr1r2 },				// 0x0A
			{ "DEC BC", 8, &GB_CPU::DEC16 },				// 0x0B
			{ "INC C", 4, &GB_CPU::INC },				// 0x0C
			{ "DEC C", 4, &GB_CPU::DEC },				// 0x0D
			{ "LD C,n", 8, &GB_CPU::LDnnN },				// 0x0E
			{ "RRCA", 4, &GB_CPU::CBA },				// 0x0F
			{ "STOP", 4, &GB_CPU::STOP },				// 0x10
			{ "LD DE,nn", 12, &GB_CPU::LDNnn },				// 0x11
			{ "LD (DE),A", 8, &GB_CPU::LDr1r2 },				// 0x12
			{ "INC DE", 8, &GB_CPU::INC16 },				// 0x13
			{ "INC D", 4, &GB_CPU::INC },				// 0x14
			{ "DEC D", 4, &GB_CPU::DEC },				// 0x15
			{ "LD D,n", 8, &GB_CPU::LDnnN },				// 0x16
			{ "RLA", 4, &GB_CPU::CBA },				// 0x17
			{ "JR n", 8, &GB_CPU::JRccn },				// 0x18
			{ "ADD HL,DE", 8, &GB_CPU::ADD16 },				// 0x19
			{ "LD A,(DE)", 8, &GB_CPU::LDr1r2 },				// 0x1A
			{ "DEC DE", 8, &GB_CPU::DEC16 },				// 0x1B
			{ "INC E", 4, &GB_CPU::INC },				// 0x1C
			{ "DEC E", 4, &GB_CPU::DEC },				// 0x1D
			{ "LD E,n", 8, &GB_CPU::LDnnN },				// 0x1E
			{ "RRA", 4, &GB_CPU::CBA },				// 0x1F
			{ "JR NZ,n", 8, &GB_CPU::JRccn },				// 0x20
			{ "LD HL,nn", 12, &GB_CPU::LDNnn },				// 0x21
			{ "LDI (HL),A", 8, &GB_CPU::LDI },				// 0x22
			{ "INC HL", 8, &GB_CPU::INC16 },				// 0x23
			{ "INC H", 4, &GB_CPU::INC },				// 0x24
			{ "DEC H", 4, &GB_CPU::DEC },				// 0x25
			{ "LD H,n", 8, &GB_CPU::LDnnN },				// 0x26
			{ "DAA", 4, &GB_CPU::DAA },				// 0x27
			{ "JR Z,n", 8, &GB_CPU::JRccn },				// 0x28
			{ "ADD HL,HL", 8, &GB_CPU::ADD16 },				// 0x29
			{ "LDI A, (HL)", 8, &GB_CPU::LDI },				// 0x2A
			{ "DEC HL", 8, &GB_CPU::DEC16 },				// 0x2B
			{ "INC L", 4, &GB_CPU::INC },				// 0x2C
			{ "DEC L", 4, &GB_CPU::DEC },				// 0x2D
			{ "LD L,n", 8, &GB_CPU::LDnnN },				// 0x2E
			{ "CPL", 4, &GB_CPU::cpl },				// 0x2F
			{ "JR NC,n", 8, &GB_CPU::JRccn },				// 0x30
			{ "LD SP,nn", 12, &GB_CPU::LDNnn },				// 0x31
			{ "LDD (HL),A", 8, &GB_CPU::LDD },				// 0x32
			{ "INC SP", 8, &GB_CPU::INC16 },				// 0x33
			{ "INC (HL)", 12, &GB_CPU::INC },				// 0x34
			{ "DEC (HL)", 12, &GB_CPU::DEC },				// 0x35
			{ "LD (HL),n", 12, &GB_CPU::LDr1r2 },				// 0x36
			{ "SCF", 4, &GB_CPU::SCF },				// 0x37
			{ "JR C,n", 8, &GB_CPU::JRccn },				// 0x38
			{ "ADD HL,SP", 8, &GB_CPU::ADD16 },				// 0x39
			{ "LDD A,(HL)", 8, &GB_CPU::LDD },				// 0x3A
			{ "DEC SP", 8, &GB_CPU::DEC16 },				// 0x3B
			{ "INC A", 4, &GB_CPU::INC },				// 0x3C
			{ "DEC A", 4, &GB_CPU::DEC },				// 0x3D
			{ "LD A,#", 8, &GB_CPU::LDr1r2 },				// 0x3E
			{ "CCF", 4, &GB_CPU::CCF },				// 0x3F
			{ "LD B,B", 4, &GB_CPU::LDr1r2 },				// 0x40
			{ "LD B,C", 4, &GB_CPU::LDr1r2 },				// 0x41
			{ "LD B,D", 4, &GB_CPU::LDr1r2 },				// 0x42
			{ "LD B,E", 4, &GB_CPU::LDr1r2 },				// 0x43
			{ "LD B,H", 4, &GB_CPU::LDr1r2 },				// 0x44
			{ "LD B,L", 4, &GB_CPU::LDr1r2 },				// 0x45
			{ "LD B,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x46
			{ "LD B,A", 4, &GB_CPU::LDr1r2 },				// 0x47
			{ "LD C,B", 4, &GB_CPU::LDr1r2 },				// 0x48
			{ "LD C,C", 4, &GB_CPU::LDr1r2 },				// 0x49
			{ "LD C,D", 4, &GB_CPU::LDr1r2 },				// 0x4A
			{ "LD C,E", 4, &GB_CPU::LDr1r2 },				// 0x4B
			{ "LD C,H", 4, &GB_CPU::LDr1r2 },				// 0x4C
			{ "LD C,L", 4, &GB_CPU::LDr1r2 },				// 0x4D
			{ "LD C,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x4E
			{ "LD C,A", 4, &GB_CPU::LDr1r2 },				// 0x4F
			{ "LD D,B", 4, &GB_CPU::LDr1r2 },				// 0x50
			{ "LD D,C", 4, &GB_CPU::LDr1r2 },				// 0x51
			{ "LD D,D", 4, &GB_CPU::LDr1r2 },				// 0x52
			{ "LD D,E", 4, &GB_CPU::LDr1r2 },				// 0x53
			{ "LD D,H", 4, &GB_CPU::LDr1r2 },				// 0x54
			{ "LD D,L", 4, &GB_CPU::LDr1r2 },				// 0x55
			{ "LD D,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x56
			{ "LD D,A", 4, &GB_CPU::LDr1r2 },				// 0x57
			{ "LD E,B", 4, &GB_CPU::LDr1r2 },				// 0x58
			{ "LD E,C", 4, &GB_CPU::LDr1r2 },				// 0x59
			{ "LD E,D", 4, &GB_CPU::LDr1r2 },				// 0x5A
			{ "LD E,E", 4, &GB_CPU::LDr1r2 },				// 0x5B
			{ "LD E,H", 4, &GB_CPU::LDr1r2 },				// 0x5C
			{ "LD E,L", 4, &GB_CPU::LDr1r2 },				// 0x5D
			{ "LD E,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x5E
			{ "LD E,A", 4, &GB_CPU::LDr1r2 },				// 0x5F
			{ "LD H,B", 4, &GB_CPU::LDr1r2 },				// 0x60
			{ "LD H,C", 4, &GB_CPU::LDr1r2 },				// 0X61
			{ "LD H,D", 4, &GB_CPU::LDr1r2 },				// 0x62
			{ "LD H,E", 4, &GB_CPU::LDr1r2 },				// 0x63
			{ "LD H,H", 4, &GB_CPU::LDr1r2 },				// 0x64
			{ "LD H,L", 4, &GB_CPU::LDr1r2 },				// 0x65
			{ "LD H,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x66
			{ "LD H,A", 4, &GB_CPU::LDr1r2 },				// 0x67
			{ "LD L,B", 4, &GB_CPU::LDr1r2 },				// 0x68
			{ "LD L,C", 4, &GB_CPU::LDr1r2 },				// 0x69
			{ "LD L,D", 4, &GB_CPU::LDr1r2 },				// 0x6A
			{ "LD L,E", 4, &GB_CPU::LDr1r2 },				// 0x6B
			{ "LD L,H", 4, &GB_CPU::LDr1r2 },				// 0x6C
			{ "LD L,L", 4, &GB_CPU::LDr1r2 },				// 0x6D
			{ "LD L,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x6E
			{ "LD L,A", 4, &GB_CPU::LDr1r2 },				// 0x6F
			{ "LD (HL),B", 8, &GB_CPU::LDr1r2 },				// 0x70
			{ "LD (HL),C", 8, &GB_CPU::LDr1r2 },				// 0x71
			{ "LD (HL),D", 8, &GB_CPU::LDr1r2 },				// 0x72
			{ "LD (HL),E", 8, &GB_CPU::LDr1r2 },				// 0x73
			{ "LD (HL),H", 8, &GB_CPU::LDr1r2 },				// 0x74
			{ "LD (HL),L", 8, &GB_CPU::LDr1r2 },				// 0x75
			{ "HALT", 4, &GB_CPU::HALT },				// 0x76
			{ "LD (HL),A", 8, &GB_CPU::LDr1r2 },				// 0x77
			{ "LD A,B", 4, &GB_CPU::LDr1r2 },				// 0x78
			{ "LD A,C", 4, &GB_CPU::LDr1r2 },				// 0x79
			{ "LD A,D", 4, &GB_CPU::LDr1r2 },				// 0x7A
			{ "LD A,E", 4, &GB_CPU::LDr1r2 },				// 0x7B
			{ "LD A,H", 4, &GB_CPU::LDr1r2 },				// 0x7C
			{ "LD A,L", 4, &GB_CPU::LDr1r2 },				// 0x7D
			{ "LD A,(HL)", 8, &GB_CPU::LDr1r2 },				// 0x7E
			{ "LD A,A", 4, &GB_CPU::LDr1r2 },				// 0x7F
			{ "ADD A,B", 4, &GB_CPU::ADD },				// 0x80
			{ "ADD A,C", 4, &GB_CPU::ADD },				// 0x81
			{ "ADD A,D", 4, &GB_CPU::ADD },				// 0x82
			{ "ADD A,E", 4, &GB_CPU::ADD },				// 0x83
			{ "ADD A,H", 4, &GB_CPU::ADD },				// 0x84
			{ "ADD A,L", 4, &GB_CPU::ADD },				// 0x85
			{ "ADD A,(HL)", 8, &GB_CPU::ADD },				// 0x86
			{ "ADD A,A", 4, &GB_CPU::ADD },				// 0x87
			{ "ADC A,B", 4, &GB_CPU::ADD },				// 0x88
			{ "ADC A,C", 4, &GB_CPU::ADD },				// 0x89
			{ "ADC A,D", 4, &GB_CPU::ADD },				// 0x8A
			{ "ADC A,E", 4, &GB_CPU::ADD },				// 0x8B
			{ "ADC A,H", 4, &GB_CPU::ADD },				// 0x8C
			{ "ADC A,L", 4, &GB_CPU::ADD },				// 0x8D
			{ "ADC A,(HL)", 8, &GB_CPU::ADD },				// 0x8E
			{ "ADC A,A", 4, &GB_CPU::ADD },				// 0x8F
			{ "SUB B", 4, &GB_CPU::SUB },				// 0x90
			{ "SUB C", 4, &GB_CPU::SUB },				// 0x91
			{ "SUB D", 4, &GB_CPU::SUB },				// 0x92
			{ "SUB E", 4, &GB_CPU::SUB },				// 0x93
			{ "SUB H", 4, &GB_CPU::SUB },				// 0x94
			{ "SUB L", 4, &GB_CPU::SUB },				// 0x95
			{ "SUB (HL)", 8, &GB_CPU::SUB },				// 0x96
			{ "SUB A", 4, &GB_CPU::SUB },				// 0x97
			{ "SBC A,B", 4, &GB_CPU::SUB },				// 0x98
			{ "SBC A,C", 4, &GB_CPU::SUB },				// 0x99
			{ "SBC A,D", 4, &GB_CPU::SUB },				// 0x9A
			{ "SBC A,E", 4, &GB_CPU::SUB },				// 0x9B
			{ "SBC A,H", 4, &GB_CPU::SUB },				// 0x9C
			{ "SBC A,L", 4, &GB_CPU::SUB },				// 0x9D
			{ "SBC A,(HL)", 8, &GB_CPU::SUB },				// 0x9E
			{ "SBC A,A", 4, &GB_CPU::SUB },				// 0x9F
			{ "AND B", 4, &GB_CPU::AND },				// 0xA0
			{ "AND C", 4, &GB_CPU::AND },				// 0xA1
			{ "AND D", 4, &GB_CPU::AND },				// 0xA2
			{ "AND E", 4, &GB_CPU::AND },				// 0xA3
			{ "AND H", 4, &GB_CPU::AND },				// 0xA4
			{ "AND L", 4, &GB_CPU::AND },				// 0xA5
			{ "AND (HL)", 8, &GB_CPU::AND },				// 0xA6
			{ "AND A", 4, &GB_CPU::AND },				// 0xA7
			{ "XOR B", 4, &GB_CPU::XOR },				// 0xA8
			{ "XOR C", 4, &GB_CPU::XOR },				// 0xA9
			{ "XOR D", 4, &GB_CPU::XOR },				// 0xAA
			{ "XOR E", 4, &GB_CPU::XOR },				// 0xAB
			{ "XOR H", 4, &GB_CPU::XOR },				// 0xAC
			{ "XOR L", 4, &GB_CPU::XOR },				// 0xAD
			{ "XOR (HL)", 8, &GB_CPU::XOR },				// 0xAE
			{ "XOR A", 4, &GB_CPU::XOR },				// 0xAF
			{ "OR B", 4, &GB_CPU::OR },				// 0xB0
			{ "OR C", 4, &GB_CPU::OR },				// 0xB1
			{ "OR D", 4, &GB_CPU::OR },				// 0xB2
			{ "OR E", 4, &GB_CPU::OR },				// 0xB3
			{ "OR H", 4, &GB_CPU::OR },				// 0xB4
			{ "OR L", 4, &GB_CPU::OR },				// 0xB5
			{ "OR (HL)", 8, &GB_CPU::OR },				// 0xB6
			{ "OR A", 4, &GB_CPU::OR },				// 0xB7
			{ "CP B", 4, &GB_CPU::CP },				// 0xB8
			{ "CP C", 4, &GB_CPU::CP },				// 0xB9
			{ "CP D", 4, &GB_CPU::CP },				// 0xBA
			{ "CP E", 4, &GB_CPU::CP },				// 0xBB
			{ "CP H", 4, &GB_CPU::CP },				// 0xBC
			{ "CP L", 4, &GB_CPU::CP },				// 0xBD
			{ "CP (HL)", 8, &GB_CPU::CP },				// 0xBE
			{ "CP A", 4, &GB_CPU::CP },				// 0xBF
			{ "RET NZ", 8, &GB_CPU::retcc },				// 0xC0
			{ "POP BC", 12, &GB_CPU::popNN },				// 0xC1
			{ "JP NZ,nn", 12, &GB_CPU::JPccnn },				// 0xC2
			{ "JP nn", 16, &GB_CPU::JPnn },				// 0xC3
			{ "CALL NZ,nn", 12, &GB_CPU::CAL },				// 0xC4
			{ "PUSH BC", 16, &GB_CPU::pushNN },				// 0xC5
			{ "ADD A,#", 8, &GB_CPU::ADD },				// 0xC6
			{ "RST $00", 32, &GB_CPU::RST },				// 0xC7
			{ "RET Z", 8, &GB_CPU::retcc },				// 0xC8
			{ "RET", 8, &GB_CPU::ret },				// 0xC9
			{ "JP Z,nn", 12, &GB_CPU::JPccnn },				// 0xCA
			{ "CB xx", 8, &GB_CPU::CB },				// 0xCB
			{ "CALL Z,nn", 12, &GB_CPU::CAL },				// 0xCC
			{ "CALL nn", 12, &GB_CPU::CAL },				// 0xCD
			{ "ADD A,#", 8, &GB_CPU::ADD },				// 0xCE
			{ "RST $08", 32, &GB_CPU::RST },				// 0xCF
			{ "RET NC", 8, &GB_CPU::retcc },				// 0xD0
			{ "POP DE", 12, &GB_CPU::popNN },				// 0xD1
			{ "JP NC,nn", 12, &GB_CPU::JPccnn },				// 0xD2
			{ "UNUSED", 0, nullptr },				// 0xD3
			{ "CALL NC,nn", 12, &GB_CPU::CAL },				// 0xD4
			{ "PUSH DE", 16, &GB_CPU::pushNN },				// 0xD5
			{ "SUB n", 8, &GB_CPU::SUB },				// 0xD6
			{ "RST $10", 32, &GB_CPU::RST },				// 0xD7
			{ "RET C", 8, &GB_CPU::retcc },				// 0xD8
			{ "RETI", 8, &GB_CPU::reti },				// 0xD9
			{ "JP C,nn", 12, &GB_CPU::JPccnn },				// 0xDA
			{ "UNUSED", 0, nullptr },				// 0xDB
			{ "CALL C,nn", 12, &GB_CPU::CAL },				// 0xDC
			{ "UNUSED", 0, nullptr },				// 0xDD
			{ "SBC A,#", 8, &GB_CPU::SUB },				// 0xDE
			{ "RST $18", 32, &GB_CPU::RST },				// 0xDF
			{ "LD ($FF00+n),A", 12, &GB_CPU::LDr1r2 },				// 0xE0
			{ "POP HL", 12, &GB_CPU::popNN },				// 0xE1
			{ "LD ($FF00+C),A", 8, &GB_CPU::LDr1r2 },				// 0xE2
			{ "UNUSED", 0, nullptr },				// 0xE3
			{ "UNUSED", 4, nullptr },				// 0xE4
			{ "PUSH HL", 16, &GB_CPU::pushNN },				// 0xE5
			{ "AND #", 8, &GB_CPU::AND },				// 0xE6
			{ "RST $20", 32, &GB_CPU::RST },				// 0xE7
			{ "ADD SP,r8", 16, &GB_CPU::ADDSPr8 },				// 0xE8
			{ "JP (HL)", 4, &GB_CPU::JPHL },				// 0xE9
			{ "LD (nn),A", 16, &GB_CPU::LDr1r2 },				// 0xEA
			{ "UNUSED", 0, nullptr },				// 0xEB
			{ "UNUSED", 0, nullptr },				// 0xEC
			{ "UNUSED", 0, nullptr },				// 0xED
			{ "XOR #", 8, &GB_CPU::XOR },				// 0xEE
			{ "RST $28", 32, &GB_CPU::RST },				// 0xEF
			{ "LD A,($FF00+n)", 12, &GB_CPU::LDr1r2 },				// 0xF0
			{ "POP AF", 12, &GB_CPU::popNN },				// 0xF1
			{ "LD A,($FF00+C)", 8, &GB_CPU::LDr1r2 },				// 0xF2
			{ "di", 4, &GB_CPU::di },				// 0xF3
			{ "UNUSED", 0, nullptr },				// 0xF4
			{ "PUSH AF", 16, &GB_CPU::pushNN },				// 0xF5
			{ "OR #", 8, &GB_CPU::OR },				// 0xF6
			{ "RST $30", 32, &GB_CPU::RST },				// 0xF7
			{ "LDHL SP,n", 12, &GB_CPU::LDHLSPn },				// 0xF8
			{ "LD SP,HL", 8, &GB_CPU::LDSPHL },				// 0xF9
			{ "LD A,(nn)", 16, &GB_CPU::LDr1r2 },				// 0xFA
			{ "EI", 4, &GB_CPU::ei },				// 0xFB
			{ "UNUSED", 0, nullptr },				// 0xFC
			{ "UNUSED", 0, nullptr },				// 0xFD
			{ "CP #", 8, &GB_CPU::CP },				// 0xFE
			{ "RST $38", 32, &GB_CPU::RST },				// 0xFF
		};

        //Status keeping variables
        uint16_t cycles = 0;
        uint8_t lastJoypadState = 0;
        uint8_t lastLCDState = 0;
        uint8_t lastTimerState = 0;
        bool halted = false;
        bool stopped = false;

        //Steps the cpu one instruction and checks for interrupts
        //Returns number of cycles executed
        int16_t execute()
		{
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


        //Returns true of an interrupt occured
        bool checkInterrupts()
        {
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

        void printRegs()
        {
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

        void HALT()
        {
            halted = true;
        }

        void STOP()
        {
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

        void reti()
        {
            ret();
            reg.ime = 1;
        }

        void ret()
        {
            pop(reg.pc);
        }

        void UNUSED()
        {
            reg.pc++;
        }

        void retcc()
        {
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

        void RST()
        {
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

        void push(uint16_t value)
        {
            reg.sp -= 2;
            writeWordToMemory(reg.sp, value);
        }

        uint16_t getWordFromMemory(uint16_t address)
        {
            return (memory->read(address + 1) << 8) + memory->read(address);
        }

        void writeWordToMemory(uint16_t address, uint16_t value)
        {
            memory->write(address + 1, value >> 8); //Write high byte
            memory->write(address, value & 0xFF); //Write low byte
        }

        void pop(uint16_t &destination)
        {
            destination = getWordFromMemory(reg.sp);
            reg.sp += 2;
        }

		void nop() {
			reg.pc += 1;
		}

        void pushNN()
        {
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

        void popNN()
        {
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

        void JPHL()
        {
            reg.pc = reg.hl;
        }

		void JPnn() {
			reg.pc = getWordFromMemory(reg.pc + 1);
		}

        void CCF()
        {
            RES(N_FLAG,reg.f);
            RES(H_FLAG,reg.f);

            if(testBit(C_FLAG,reg.f))
                RES(C_FLAG,reg.f);
            else
                SET(C_FLAG,reg.f);
            reg.pc++;
        }

        void CB()
        {
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

        void CBA()
        {
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

        void RLC(uint8_t & operand) 
        {
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

        void RRC(uint8_t & operand)
        {
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

        void RL(uint8_t & operand)
        {
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

        void RR(uint8_t & operand)
        {
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

        void SLA(uint8_t & operand)
        {
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

        void SRA(uint8_t & operand)
        {
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

        void SWAP(uint8_t & operand)
        {
            operand = (operand << 4) + ((operand & 0xF0) >> 4);
            if(operand == 0)
                SET(Z_FLAG,reg.f);
            else
                RES(Z_FLAG,reg.f);
            
            RES(N_FLAG,reg.f);
            RES(H_FLAG,reg.f);
            RES(C_FLAG,reg.f);
        }

        void SRL(uint8_t & operand)
        {
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

        void BIT(uint8_t bit, uint8_t operand)
        {
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

        bool testBit(uint8_t bit, uint16_t operand)
        {
            return (operand & (1 << bit)) >> bit;
        }

        void RES(uint8_t bit, uint8_t & operand)
        {
            operand &= ~(1 << bit);
        }

        void SET(uint8_t bit, uint8_t & operand)
        {
            operand |= (1 << bit);
        }

        void setMem(uint8_t bit, uint16_t address)
        {
            memory->write(address, memory->read(address) | (1 << bit));
        }

        void resMem(uint8_t bit, uint16_t address)
        {
            memory->write(address, memory->read(address) & ~(1 << bit));
        }

        void SCF()
        {
            RES(N_FLAG,reg.f);
            RES(H_FLAG,reg.f);
            SET(C_FLAG,reg.f);
            reg.pc++;
        }

        void DAA()
        {
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

        void JPccnn()
        {
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

        void JRccn()
        {
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

		void INC()
		{
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

        void INC16()
        {
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

		void DEC()
		{
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

        void DEC16()
        {
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

        void LDnnSP()
        {
            uint16_t address = getWordFromMemory(reg.pc + 1);
            writeWordToMemory(address,reg.sp);
            reg.pc += 3;
        }

        void ADDSPr8()
        {
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

        void LDHLSPn()
        {
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

		void LDr1r2()
		{
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

        void LDSPHL()
        {
            reg.sp = reg.hl;
            reg.pc += 1;
        }

        void LDI()
        {
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

        void LDnnN()
        {
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

        void LDD()
        {
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

		void CAL()
		{
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

        void OR()
		{
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

		void XOR()
		{
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

        void AND()
        {
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

        void CP()
        {
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

        void ADD()
        {
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

        void ADD16()
        {
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

		void SUB()
		{
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

        void LDNnn()
        {
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

        void cpl()
        {
            reg.a = ~(reg.a);
            SET(N_FLAG,reg.f);
            SET(H_FLAG,reg.f);
            reg.pc++;
        }

        void di()
        {
            reg.pc++;
            cycles = 4 + execute();
            reg.ime = 0;
        }
        void ei()
        {
            reg.pc++;
            cycles = 4 + execute();
            reg.ime = 1;
        }
};