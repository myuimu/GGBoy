#pragma once

//Bits in registers
#define Z_FLAG 7
#define N_FLAG 6
#define H_FLAG 5
#define C_FLAG 4
#define LCD_ENABLE 7
#define TIMER_ENABLE 2
#define DIRECTION_ENABLE 4
#define BUTTON_ENABLE 5

//Register Starting States
#define AF_START 0x01B0
#define BC_START 0x0013
#define DE_START 0x00D8
#define HL_START 0x014D

//Special Memory Locations
#define MEMORY_BEGIN 0x000
#define JOYPAD 0xFF00
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define WY 0xFF4A
#define WX 0xFF4B
#define IF 0xFF0F
#define IE 0xFFFF

// Timings
#define BASE_FREQUENCY 4194304
#define DIV_FREQUENCY 16384
#define TIM_00_FREQUENCY 4096
#define TIM_01_FREQUENCY 262144
#define TIM_10_FREQUENCY 65536
#define TIM_11_FREQUENCY 16384
#define MODE_2_CYCLES 80
#define MODE_3_CYCLES 252
#define CYCLES_PER_LINE 456
#define CYCLES_PER_FRAME 70224

#define SERVICE_VECTOR_BEGIN 0x0040
#define SERVICE_VECTOR_LENGTH 8