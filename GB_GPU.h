#pragma once
#include <SDL2/SDL.h>
#include "GB_CONST.h"
#include <stdio.h>
#include <chrono>

class GB_GPU {
    public:
        //std::shared_ptr<std::array<unsigned char,0xFFFF>> memory;
        std::shared_ptr<GB_MEM> memory;

        unsigned int currentCycle = 0;
        unsigned char scale = 3;
        SDL_Window* window = nullptr;
        SDL_Surface* screenSurface = nullptr;
        SDL_Surface* gameSurface = nullptr;
        unsigned char* pixels = nullptr;

        GB_GPU()
        {
            //Initialize SDL
            if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
            {
                printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
            }
            else
            {
                //Create window
                window = SDL_CreateWindow( "Gameboy Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160*scale, 144*scale, SDL_WINDOW_SHOWN );
                if( window == nullptr )
                {
                    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
                }
                else
                {
                    //Get window surface
                    screenSurface = SDL_GetWindowSurface( window );

                    //Fill the surface white
                    SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x0, 0x0, 0x0 ) );
            
                    //Update the surface
                    SDL_UpdateWindowSurface( window );
                }

                gameSurface = SDL_CreateRGBSurface(0, 160, 144, 32, 0, 0, 0, 0);
                if (gameSurface == NULL) 
                {
                    SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
                    exit(1);
                }
                pixels = (unsigned char*)gameSurface -> pixels;
            }
            
        }

        bool hblank = false;
        bool oam = false;
        bool vblank = false;

        void update(short cycles)
        {
            if(memory->read(LCDC) & 0b10000000 != 0b10000000) //Lcd enabled
            {
                currentCycle = 0;
                memory->write(LY,0);
                memory->write(STAT, memory->read(STAT) | 0b00000001);
                memory->write(STAT, memory->read(STAT) & 0b11111101);
                return;
            }

            currentCycle += cycles;
            if(currentCycle < MODE_2_CYCLES && !vblank)  //LCD is accessing OAM - Mode 2
            {
                if((memory->read(STAT) & (1 << 5)) && !oam) //If OAM interrupt is enabled and not requested already
                {
                    memory->write(0xFF0F, memory->read(0xFF0F) | (1 << 1));
                    oam = true;
                }
                memory->write(STAT, memory->read(STAT) | 0b00000010);
                memory->write(STAT, memory->read(STAT) & 0b11111110);
            }
            else if(currentCycle < MODE_3_CYCLES && !vblank) //LCD is accessing VRAM - Mode 3
            {
                memory->write(STAT, memory->read(STAT) | 0b00000011);
            }
            else if(currentCycle <= CYCLES_PER_LINE && !vblank)//HBlank period - Mode 0
            {
                if((memory->read(STAT) & (1 << 3)) && !hblank) //If HBlank interrupt is enabled and not requested already
                {
                    memory->write(0xFF0F, memory->read(0xFF0F) | (1 << 1));
                    hblank = true;
                }
                memory->write(STAT, memory->read(STAT) & 0b11111100);
            }
            else //Advance line
            {
                memory->write(LY, memory->read(LY) + 1);
                unsigned short line = memory->read(LY);
                currentCycle -= CYCLES_PER_LINE;
                if (line <= 144) //Draw line on screen
                {
                    drawTiles(line);
                    drawSprites(line);
                    //SDL_UpdateWindowSurface( window );
                    //std::this_thread::sleep_for(std::chrono::duration<double>(0.01)); //Pause after drawing scanline
                }

                if(line > 153) //Return to top of screen
                {
                    memory->write(LY, 0);
                    drawTiles(0);
                    drawSprites(0);
                    vblank = false;
                }
                else if(line == 144) //Start of vBlank - Trigger interrupt - Draw frame
                {
                    memory->write(0xFF0F, memory->read(0xFF0F) | 1);

                    SDL_BlitScaled(gameSurface, NULL, screenSurface, NULL);
                    SDL_UpdateWindowSurface( window );
                }
                else if(line == 153)
                {
                    currentCycle = CYCLES_PER_LINE - 3;
                }

                if(memory->read(LY) == memory->read(LYC))
                {
                    memory->write(STAT, memory->read(STAT) | ( 1 << 2)); //Set Coincidence flag
                    if(memory->read(STAT) & (1 << 6)) //If LYC interrupt is enabled
                        memory->write(0xFF0F, memory->read(0xFF0F) | (1 << 1));
                }
                else
                    memory->write(STAT, memory->read(STAT) & 0b11111011); //Reset Coincidence flag

                //Reset interrupt flags
                oam = false;
                hblank = false;
            }

            if(memory->read(LY) >= 144)//vBlank period - Mode 1
            {
                if((memory->read(STAT) & (1 << 5)) && !vblank) //If VBlank interrupt is enabled and not requested already
                {
                    memory->write(0xFF0F, memory->read(0xFF0F) | (1 << 1));
                    vblank = true;
                }
                memory->write(STAT, memory->read(STAT) | 0b00000001);
                memory->write(STAT, memory->read(STAT) & 0b11111101);
            }
        }

        void drawTiles(unsigned short line)
        {
            unsigned short displayAddress = 0;
            unsigned short dataStart = 0;
            bool usingWindow = false;
            bool unsign = true;

            //Background area and window location
            unsigned char scrollY = memory->read(SCY);
            unsigned char scrollX = memory->read(SCX);
            unsigned char windowY = memory->read(WY);
            unsigned char windowX = memory->read(WX) - 7;

            //Check if window is enabled
            if(memory->read(LCDC) & (1 << 5))
            {
                //Check if current line is within window range
                if(windowY <= line)
                {
                    usingWindow = true;
                }
            }

            switch(memory->read(LCDC) & (1 << (usingWindow ? 6 : 3))) //Get tile display range
            {
                case 0: //9800-9BFF
                    displayAddress = 0x9800;
                    break;
                case (1 << 3): //9C00-9FFF
                case (1 << 6):
                    displayAddress = 0x9C00;
                    break;
            }
            switch(memory->read(LCDC) & (1 << 4)) //Get tile data range
            {
                case 0: //8800-97FF
                    unsign = false;
                    dataStart = 0x8800;
                    break;
                case (1<< 4): //8000-8FFF
                    dataStart = 0x8000;
                    break;
            }

            //Total tile area: 32x32 tiles (tile = 8x8 pixels)
            //Screen area: 20x18 tiles
            unsigned char yPos = 0;
            if(!usingWindow)
                yPos = scrollY + line;
            else
                yPos = line - windowY;
            
            //Which vertical tile is being used
            unsigned short tileRow = (((unsigned char)(yPos/8))*32);

            for(int i = 0; i < 160; i++)
            {
                unsigned char xPos = i + scrollX;

                //Put x in window space if using window
                if(usingWindow && (i >= windowX))
                    xPos = i - windowX;

                //Which horizontal tile is being used
                unsigned short tileCol = (xPos/8);
                short tileNum;

                //Get tile ID number
                unsigned short tileAddress = displayAddress + tileRow + tileCol;
                if(unsign)
                    tileNum = (unsigned char)memory->read(tileAddress);
                else
                    tileNum = (char)memory->read(tileAddress);

                //Get memory location of tile ID
                unsigned short tileLocation = dataStart;

                if(unsign)
                    tileLocation += (tileNum * 16);
                else
                    tileLocation += ((tileNum + 128) * 16);
                
                //Get the correct line of the tile to draw
                unsigned char column = yPos % 8;
                column *= 2; //Each line is two bytes
                unsigned char data1 = memory->read(tileLocation + column);
                unsigned char data2 = memory->read(tileLocation + column + 1);

                int colorBit = xPos % 8;
                colorBit -= 7;
                colorBit *= -1;

                //Combine the bits to get the color number of the pixel
                int colorNum = (data2 & (1 << colorBit)) >> (colorBit);
                colorNum <<= 1;
                colorNum |= (data1 & (1 << colorBit)) >> colorBit;

                unsigned char col = getColor(colorNum, 0xFF47);
                if (xPos % 8 == 7 && (col == 0)) {}
                    //col += 2;

                unsigned char red = 0;
                unsigned char blue = 0;
                unsigned char green = 0;

                switch(col)
                {
                    case 0x00: //White
                        red = 255;
                        green = 255;
                        blue = 255;
                        break;
                    case 0x01: //Light Gray
                        red = 0xCC;
                        green = 0xCC;
                        blue = 0xCC;
                        break;
                    case 0x02: //Dark Gray
                        red = 0x77;
                        green = 0x77;
                        blue = 0x77;
                        break;
                }

                //Check that the pixel is in bounds
                if((line < 0) || (line > 143) || (i < 0) || (i > 159))
                    continue;
                
                pixels[4 * (line * gameSurface -> w + i) + 0] = blue;
                pixels[4 * (line * gameSurface -> w + i) + 1] = green;
                pixels[4 * (line * gameSurface -> w + i) + 2] = red;
            }
        }

        void drawSprites(unsigned short line)
        {
            bool tallSprites = false;
            if((memory->read(LCDC) & 0x4) == 0x4)
                tallSprites = true;

            for(int sprite = 0; sprite < 40; sprite++)
            {
                //Sprite takes 4 bytes in sprite attribute table
                unsigned char index = sprite*4;
                unsigned char yPos = memory->read(0xFE00+index) - 16;
                unsigned char xPos = memory->read(0xFE00+index+1) - 8;
                unsigned char tileLocation = memory->read(0xFE00+index+2);
                unsigned char attributes = memory->read(0xFE00+index+3);

                bool yFlip = ((attributes & 0x40) == 0x40);
                bool xFlip = ((attributes & 0x20) == 0x20);
            
                unsigned char ySize = 8;
                if(tallSprites)
                {
                    ySize = 16;
                }
                //If current sprite connects with current scanline
                if(line >= yPos && line < (yPos+ySize))
                {
                    int spriteLine = line - yPos;

                    if(yFlip)
                    {
                        spriteLine -= ySize;
                        spriteLine *= -1;
                    }

                    spriteLine *= 2;
                    unsigned short dataAddress = (0x8000 + (tileLocation * 16)) + spriteLine;
                    unsigned char data1 = memory->read(dataAddress);
                    unsigned char data2 = memory->read(dataAddress+1);

                    for(int tilePixel = 7; tilePixel >= 0; tilePixel--)
                    {
                        int colorBit = tilePixel;

                        if(xFlip)
                        {
                            colorBit -= 7;
                            colorBit *= -1;
                        }

                        int colorNum = (data2 & (1 << colorBit)) >> colorBit;
                        colorNum <<= 1;
                        colorNum |= (data1 & (1 <<colorBit)) >> colorBit;

                        unsigned short colorAddress = (attributes & (1 << 4)) >> 4 ? 0xFF49 : 0xFF48;
                        unsigned char color = getColor(colorNum, colorAddress);

                        //White is transparent for sprites
                        if(colorNum == 0)
                            continue;

                        unsigned char red = 0;
                        unsigned char blue = 0;
                        unsigned char green = 0;

                        switch(color)
                        {
                            case 0x00: //White
                                red = 255;
                                green = 255;
                                blue = 255;
                                break;
                            case 0x01: //Light Gray
                                red = 0xCC;
                                green = 0xCC;
                                blue = 0xCC;
                                break;
                            case 0x02: //Dark Gray
                                red = 0x77;
                                green = 0x77;
                                blue = 0x77;
                                break;
                        }

                        int xPix = 0 - tilePixel;
                        xPix += 7;
                        int pixel = xPix + xPos;

                        //Check that the pixel is in bounds
                        if((line < 0) || (line > 143) || (pixel < 0) || (pixel > 159))
                            continue;
                
                        pixels[4 * (line * gameSurface -> w + pixel) + 0] = blue;
                        pixels[4 * (line * gameSurface -> w + pixel) + 1] = green;
                        pixels[4 * (line * gameSurface -> w + pixel) + 2] = red;
                    }
                }
            }
        }

        unsigned char getColor(int colorNum, unsigned short address)
        {
            unsigned char color = 0x00; //Default white
            unsigned char palette = memory->read(address);
            unsigned char hi = 0;
            unsigned char lo = 0;
            
            //Bits of the palette the color ID matches to
            switch(colorNum)
            {
                case 0:
                    hi = 1;
                    lo = 0;
                    break;
                case 1:
                    hi = 3;
                    lo = 2;
                    break;
                case 2:
                    hi = 5;
                    lo = 4;
                    break;
                case 3:
                    hi = 7;
                    lo = 6;
                    break;
            }

            //Use the palette to get the color
            color = (palette & (1 << hi)) >> hi;
            color <<= 1;
            color |= (palette & (1 << lo)) >> lo;

            return color;
        }
};