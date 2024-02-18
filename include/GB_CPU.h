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
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xD3
			{ "CALL NC,nn", 12, &GB_CPU::CAL },				// 0xD4
			{ "PUSH DE", 16, &GB_CPU::pushNN },				// 0xD5
			{ "SUB n", 8, &GB_CPU::SUB },				// 0xD6
			{ "RST $10", 32, &GB_CPU::RST },				// 0xD7
			{ "RET C", 8, &GB_CPU::retcc },				// 0xD8
			{ "RETI", 8, &GB_CPU::reti },				// 0xD9
			{ "JP C,nn", 12, &GB_CPU::JPccnn },				// 0xDA
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xDB
			{ "CALL C,nn", 12, &GB_CPU::CAL },				// 0xDC
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xDD
			{ "SBC A,#", 8, &GB_CPU::SUB },				// 0xDE
			{ "RST $18", 32, &GB_CPU::RST },				// 0xDF
			{ "LD ($FF00+n),A", 12, &GB_CPU::LDr1r2 },				// 0xE0
			{ "POP HL", 12, &GB_CPU::popNN },				// 0xE1
			{ "LD ($FF00+C),A", 8, &GB_CPU::LDr1r2 },				// 0xE2
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xE3
			{ "UNUSED", 4, &GB_CPU::UNUSED },				// 0xE4
			{ "PUSH HL", 16, &GB_CPU::pushNN },				// 0xE5
			{ "AND #", 8, &GB_CPU::AND },				// 0xE6
			{ "RST $20", 32, &GB_CPU::RST },				// 0xE7
			{ "ADD SP,r8", 16, &GB_CPU::ADDSPr8 },				// 0xE8
			{ "JP (HL)", 4, &GB_CPU::JPHL },				// 0xE9
			{ "LD (nn),A", 16, &GB_CPU::LDr1r2 },				// 0xEA
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xEB
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xEC
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xED
			{ "XOR #", 8, &GB_CPU::XOR },				// 0xEE
			{ "RST $28", 32, &GB_CPU::RST },				// 0xEF
			{ "LD A,($FF00+n)", 12, &GB_CPU::LDr1r2 },				// 0xF0
			{ "POP AF", 12, &GB_CPU::popNN },				// 0xF1
			{ "LD A,($FF00+C)", 8, &GB_CPU::LDr1r2 },				// 0xF2
			{ "di", 4, &GB_CPU::di },				// 0xF3
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xF4
			{ "PUSH AF", 16, &GB_CPU::pushNN },				// 0xF5
			{ "OR #", 8, &GB_CPU::OR },				// 0xF6
			{ "RST $30", 32, &GB_CPU::RST },				// 0xF7
			{ "LDHL SP,n", 12, &GB_CPU::LDHLSPn },				// 0xF8
			{ "LD SP,HL", 8, &GB_CPU::LDSPHL },				// 0xF9
			{ "LD A,(nn)", 16, &GB_CPU::LDr1r2 },				// 0xFA
			{ "EI", 4, &GB_CPU::ei },				// 0xFB
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xFC
			{ "UNUSED", 0, &GB_CPU::UNUSED },				// 0xFD
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
        int16_t execute();

        //Returns true if an interrupt occurred
        bool checkInterrupts();

        void printRegs();
        void printRegsForLog();

        void HALT();
        void STOP();
        void reti();
        void ret();
        void UNUSED();
        void retcc();
        void RST();
        void push(uint16_t value);
        uint16_t getWordFromMemory(uint16_t address);
        void writeWordToMemory(uint16_t address, uint16_t value);
        void pop(uint16_t &destination);
		void nop();
        void pushNN();
        void popNN();
        void JPHL();
		void JPnn();
        void CCF();
        void CB();
        void CBA();
        void RLC(uint8_t & operand);
        void RRC(uint8_t & operand);
        void RL(uint8_t & operand);
        void RR(uint8_t & operand);
        void SLA(uint8_t & operand);
        void SRA(uint8_t & operand);
        void SWAP(uint8_t & operand);
        void SRL(uint8_t & operand);
        void BIT(uint8_t bit, uint8_t operand);
        bool testBit(uint8_t bit, uint16_t operand);
        void RES(uint8_t bit, uint8_t & operand);
        void SET(uint8_t bit, uint8_t & operand);
        void setMem(uint8_t bit, uint16_t address);
        void resMem(uint8_t bit, uint16_t address);
        void SCF();
        void DAA();
        void JPccnn();
        void JRccn();
		void INC();
        void INC16();
		void DEC();
        void DEC16();
        void LDnnSP();
        void ADDSPr8();
        void LDHLSPn();
		void LDr1r2();
        void LDSPHL();
        void LDI();
        void LDnnN();
        void LDD();
		void CAL();
        void OR();
		void XOR();
        void AND();
        void CP();
        void ADD();
        void ADD16();
		void SUB();
        void LDNnn();
        void cpl();
        void di();
        void ei();
};