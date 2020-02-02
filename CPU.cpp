//
// Created by Suliman Alsaid on 11/25/19.
//

#include "CPU.h"

void CPU::init() {


    pc = 0x200;
    opcode = 0;
    I = 0;
    stackPointer = 0;

    unsigned long fileLen;
    char *buffer;
    FILE *gameBinary = fopen(("c8games/" + game).c_str(), "rb");
    if (!gameBinary) {
        std::cerr << "Failed to open file\n";
        return;
    }

    fseek(gameBinary, 0, SEEK_END);
    fileLen = ftell(gameBinary);
    fseek(gameBinary, 0, SEEK_SET);

    buffer = (char *) malloc(fileLen + 1);

    fread(buffer, fileLen, 1, gameBinary);
    fclose(gameBinary);

    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }

    for (int i = 0; i < fileLen; i++) {
        memory[i + 512] = buffer[i];
    }

    free(buffer);

    win.create(sf::VideoMode(640, 320), game);


}

void CPU::cycle() {

    opcode = memory[pc] << 8u | memory[pc + 1];


    switch (opcode & 0xF000) {
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        case 0x2000:
            stackPointer++;
            stack[stackPointer] = pc;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc+=4;
            else
                pc+=2;
            break;
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc+=4;
            else
                pc+=2;
            break;
        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8u] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc+=2;
            break;
        case 0x6000:
            V[(opcode & 0x0F00) >> 8u] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004: {
                    doubleByte sum = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
                    if (sum > 255) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;

                    break;
                }

                case 0x0005:
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006:
                    if(V[(opcode & 0x0F00) >> 8u] & 1u) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8u] /= 2;
                    pc+=2;
                    break;
                case 0x0007:
                    if(V[(opcode & 0x0F00) >> 8u] < V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x00F0) >> 4u] -= V[(opcode & 0x0F00) >> 8];
                    pc+=2;
                    break;
                case 0x000E:
                    if((V[(opcode & 0x0F00) >> 8u] >> 4) & 1u) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8u] *= 2;
                    pc+=2;
                    break;
            }

            break;


        case 0x9000:
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc +=2;
            }
            break;


        // Some opcodes //

        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            pc = (opcode & 0x0FFF) + V[0];
            break;
        case 0xC000:
            //generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx
            break;






        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // 0x00E0: Clears the screen
                    drawFlag = true;
                    pc += 2;
                    break;

                case 0x000E: // 0x00EE: Returns from subroutine
                    pc = stack[stackPointer];
                    stackPointer--;

                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;


        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    //timer shit

}

CPU::CPU(std::string game) {

    this->game = game;

    init();


    while (true) {
        cycle();
        if (drawFlag)
            draw();
        input();

    }


}

void CPU::input() {

    sf::Event e;
    while (win.pollEvent(e)) {
        if (e.type == sf::Event::Closed)
            exit(0);
    }

}

void CPU::draw() {

}
