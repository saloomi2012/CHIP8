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

    for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 32; j++) {
            screen[i][j].setPosition(i*10, j*10);
            screen[i][j].setSize({10, 10});
            screen[i][j].setFillColor(sf::Color::Black);
        }
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
    byte x   = (opcode >> 8) & 0x000F; // the lower 4 bits of the high byte
    byte y   = (opcode >> 4) & 0x000F; // the upper 4 bits of the low byte
    byte n   = opcode & 0x000F; // the lowest 4 bits
    byte kk  = opcode & 0x00FF; // the lowest 8 bits
    doubleByte nnn = opcode & 0x0FFF; // the lowest 12 bits

    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (kk) {
                case 0x00E0: // 0x00E0: Clears the screen
                    for(int i = 0; i < 64; i++) {
                        for(int j = 0; j < 32; j++) {
                            screen[i][j].setFillColor(sf::Color::Black);
                        }
                    }
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
            pc = nnn;
            break;
        case 0x2000:
            stack[stackPointer++] = pc+2;
            pc = nnn;
            break;
        case 0x3000:
            pc += (V[x] == kk) ? 4 : 2;
            break;
        case 0x4000:
            pc += (V[x] != kk) ? 4 : 2;
            break;
        case 0x5000:
            pc += (V[x] == V[y]) ? 4 : 2;
            break;
        case 0x6000:
            V[x] = kk;
            pc += 2;
            break;
        case 0x7000:
            V[x] += kk;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000:
                    V[x] = V[y];
                    break;
                case 0x0001:
                    V[x] = V[x] | V[y];
                    break;
                case 0x0002:
                    V[x] = V[x] & V[y];
                    break;
                case 0x0003:
                    V[x] = V[x] ^ V[y];
                    break;
                case 0x0004: {
                    V[0xF] = ((int) V[x] + (int) V[y]) > 255 ? 1 : 0;
                    V[x] = V[x] + V[y];
                    break;
                }

                case 0x0005:
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] = V[x] - V[y];
                    break;
                case 0x0006:
                    V[0xF] = V[x] & 0x1;
                    V[x] = (V[x] >> 1);
                    break;
                case 0x0007:
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0x000E:
                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] = (V[x] << 1);
                    break;
            }
            pc+=2;
            break;


        case 0x9000:
            switch (n) {
                case 0x0:
                    pc += (V[x] != V[y]) ? 4 : 2;
                    break;
                default:

                    break;
            }
            break;


        // Some opcodes //

        case 0xA000: // ANNN: Sets I to the address NNN
            I = nnn;
            pc += 2;
            break;
        case 0xB000:
            pc = nnn + V[0];
            break;
        case 0xC000: {
            byte num = rand() % 256;
            V[x] = num & kk;
            pc+=2;
            break;
        }
        case 0xD000: {
            V[0xF] = 0;
            int pixelBytes = n;
            for(int i = 0; i < pixelBytes; i++) {
                int xcor = V[x];
                int ycor = V[y];

                byte b = memory[I+i];
                int bits[8];
                bits[0] = (b & 0b10000000) >> 7;
                bits[1] = (b & 0b01000000) >> 6;
                bits[2] = (b & 0b00100000) >> 5;
                bits[3] = (b & 0b00010000) >> 4;
                bits[4] = (b & 0b00001000) >> 3;
                bits[5] = (b & 0b00000100) >> 2;
                bits[6] = (b & 0b00000010) >> 1;
                bits[7] = b & 0b00000001;
                for(int j = 0; j < 8; j++) {
                    while(xcor+j > 63) {
                        xcor -= 64;
                    }
                    while(ycor+i > 31) {
                        ycor -= 32;
                    }
                    if(screen[xcor+j][ycor+i].getFillColor() == sf::Color::White && bits[j] == 1) {
                        V[0xF] = 1;
                        screen[xcor+j][ycor+i].setFillColor(sf::Color::Black);
                    } else if (screen[xcor+j][ycor+i].getFillColor() == sf::Color::Black && bits[j] == 1){
                        screen[xcor+j][ycor+i].setFillColor(sf::Color::White);
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
        case 0xE000:
            switch(kk) {
                case 0x009E:
                    pc += (key[V[x]]) ? 4 : 2;
                    break;
                case 0x00A1:
                    pc += (!key[V[x]]) ? 4 : 2;
                    break;
            }
            break;
        case 0xF000: {
            switch (kk) {
                case 0x0007:
                    V[x] = delayTimer;
                    pc += 2;
                    break;
                case 0x000A: {
                    bool change = false;
                    while (!change) {
                        input();
                        for (int i = 0; i < 16; i++) {
                            if (key[i]) {
                                V[x] = key[i];
                                change = true;
                            }
                        }
                    }
                    pc += 2;
                    break;
                }
                case 0x0015:
                    delayTimer = V[x];
                    pc += 2;
                    break;
                case 0x0018:
                    soundTimer = V[x];
                    pc += 2;
                    break;
                case 0x001E:
                    V[0xF] = (I + V[x] > 0xfff) ? 1 : 0;
                    I = I + V[x];
                    pc += 2;
                    break;
                case 0x0029:
                    I = 5 * V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0033:
                    memory[I]   = (V[x] % 1000) / 100; // hundred's digit
                    memory[I+1] = (V[x] % 100) / 10;   // ten's digit
                    memory[I+2] = (V[x] % 10);
                    pc += 2;
                    break;
                case 0x0055:
                    for (int i = 0; i < x; i++) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;
                case 0x0065:
                    for (int i = 0; i < x; i++) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;


            }
            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);










    }

    //timer shit

}

CPU::CPU(std::string game) {

    this->game = game;

    init();

    sf::Clock clock;
    win.setFramerateLimit(100);

    while (true) {
        if(clock.getElapsedTime().asMilliseconds() > 16) {
            if(delayTimer > 0) delayTimer--;
            if(soundTimer > 0) soundTimer--;
            clock.restart();
        }
        cycle();
        if (drawFlag)
            draw();
        if(soundTimer > 0) {
            printf("beep!\n");
        }
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
    for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 32; j++) {
             win.draw(screen[i][j]);
        }
    }


    win.display();
    drawFlag = false;

}
