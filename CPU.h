//
// Created by Suliman Alsaid on 11/25/19.
//

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


typedef unsigned char byte;
typedef unsigned short doubleByte;

class CPU {
public:

    explicit CPU(std::string game);

private:
    void cycle();
    bool draw();
    void setKeys();

    s


    byte memory[4096];
    byte V[16];
    byte screen[64*32];
    byte delayTimer;
    byte soundTimer;
    byte key[16];

    doubleByte stack[16];
    doubleByte stackPointer;
    doubleByte opcode;
    doubleByte I;
    doubleByte pc;

    bool drawFlag;

    std::string game;

    unsigned char chip8_fontset[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };


};


#endif //CHIP8_CPU_H
