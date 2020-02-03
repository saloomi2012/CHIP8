//
// Created by Suliman Alsaid on 11/25/19.
//

#include "CPU.h"

void CPU::init() {
    srand(time(NULL));
    pc = 0x200;
    opcode = 0;
    I = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = false;

    for(int i = 0; i < 16; i++) {
        V[i] = 0;
        key[i] = 0;
    }



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
        case 0x0000: {
            switch (opcode & 0x00FF) {
                case 0x00E0: // 0x00E0: Clears the screen
                    drawFlag = true;
                    pc += 2;
                    break;

                case 0x00EE: // 0x00EE: Returns from subroutine
                    pc = stack[stackPointer];
                    stackPointer--;


                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
                    pc += 2;
                    break;
            }
            break;
        }


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
        case 0xC000: {
            byte num = rand() % 256;
            V[(opcode & 0x0F00) >> 8] = num & (opcode & 0x00FF);
            pc+=2;
            break;
        }
        case 0xD000:
            printf("Unimplemented opcode: 0x%X\n", opcode);
            break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E:
                    if(key[V[(opcode & 0x0F00) >> 8]] != 0) {
                        pc+=4;
                    } else {
                        pc+=2;
                    }
                    break;
                case 0x00A1:
                    if(key[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc+=4;
                    } else {
                        pc+=2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch(opcode&0x00FF) {
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delayTimer;
                    pc+=2;
                    break;
                case 0x000A:
                    printf("Unimplemented opcode: 0x%X\n", opcode);
                    break;
                case 0x0015:
                     delayTimer = V[(opcode & 0x0F00) >> 8];
                     pc +=2;
                     break;
                case 0x0018:
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    pc +=2;
                    break;
                case 0x001E:
                    I += V[(opcode & 0x0F00) >> 8];
                    pc+=2;
                    break;
                case 0x0029:
                    printf("Unimplemented opcode: 0x%X\n", opcode);
                    break;
                case 0x0033:
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055:
                    for(int i = 0; i < 16; i++) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;
                case 0x0065:
                    for(int i = 0; i < 16; i++) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;



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
        switch(e.type) {
            case sf::Event::KeyPressed:
                switch(e.key.code) {
                    case sf::Keyboard::Num1:
                        key[0x1] = 1;
                        break;
                    case sf::Keyboard::Num2:
                        key[0x2] = 1;
                        break;
                    case sf::Keyboard::Num3:
                        key[0x3] = 1;
                        break;
                    case sf::Keyboard::Num4:
                        key[0xC] = 1;
                        break;
                    case sf::Keyboard::Q:
                        key[0x4] = 1;
                        break;
                    case sf::Keyboard::W:
                        key[0x5] = 1;
                        break;
                    case sf::Keyboard::E:
                        key[0x6] = 1;
                        break;
                    case sf::Keyboard::R:
                        key[0xD] = 1;
                        break;
                    case sf::Keyboard::A:
                        key[0x7] = 1;
                        break;
                    case sf::Keyboard::S:
                        key[0x8] = 1;
                        break;
                    case sf::Keyboard::D:
                        key[0x9] = 1;
                        break;
                    case sf::Keyboard::F:
                        key[0xE] = 1;
                        break;
                    case sf::Keyboard::Z:
                        key[0xA] = 1;
                        break;
                    case sf::Keyboard::X:
                        key[0x0] = 1;
                        break;
                    case sf::Keyboard::C:
                        key[0xB] = 1;
                        break;
                    case sf::Keyboard::V:
                        key[0xF] = 1;
                        break;
                    default:
                        break;
                }
                break;
            case sf::Event::KeyReleased:
                switch(e.key.code) {
                    case sf::Keyboard::Num1:
                        key[0x1] = 0;
                        break;
                    case sf::Keyboard::Num2:
                        key[0x2] = 0;
                        break;
                    case sf::Keyboard::Num3:
                        key[0x3] = 0;
                        break;
                    case sf::Keyboard::Num4:
                        key[0xC] = 0;
                        break;
                    case sf::Keyboard::Q:
                        key[0x4] = 0;
                        break;
                    case sf::Keyboard::W:
                        key[0x5] = 0;
                        break;
                    case sf::Keyboard::E:
                        key[0x6] = 0;
                        break;
                    case sf::Keyboard::R:
                        key[0xD] = 0;
                        break;
                    case sf::Keyboard::A:
                        key[0x7] = 0;
                        break;
                    case sf::Keyboard::S:
                        key[0x8] = 0;
                        break;
                    case sf::Keyboard::D:
                        key[0x9] = 0;
                        break;
                    case sf::Keyboard::F:
                        key[0xE] = 0;
                        break;
                    case sf::Keyboard::Z:
                        key[0xA] = 0;
                        break;
                    case sf::Keyboard::X:
                        key[0x0] = 0;
                        break;
                    case sf::Keyboard::C:
                        key[0xB] = 0;
                        break;
                    case sf::Keyboard::V:
                        key[0xF] = 0;
                        break;
                    default:
                        break;
                }
                break;
            case sf::Event::Closed:
                exit(0);
            default:
                break;

        }
    }

}

void CPU::draw() {
    printf("Draw opcode: 0x%X\n", opcode);
    if(opcode == 0x00E0) {
        win.clear(sf::Color::Black);

    }

    drawFlag = false;

}
