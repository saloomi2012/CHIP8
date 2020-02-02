//
// Created by Suliman Alsaid on 11/25/19.
//

#include "CPU.h"

void CPU::setKeys() {

}

void CPU::cycle() {

}

CPU::CPU(std::string game) {

    pc = 0x200;
    opcode = 0;
    I = 0;
    stackPointer = 0;

    unsigned long fileLen;
    char *buffer;
    FILE* gameBinary = fopen(("c8games/"+game).c_str(), "rb");
    if(!gameBinary) {
        std::cerr << "Failed to open file\n";
        return;
    }

    fseek(gameBinary, 0, SEEK_END);
    fileLen=ftell(gameBinary);
    fseek(gameBinary, 0, SEEK_SET);

    buffer=(char *)malloc(fileLen+1);

    fread(buffer, fileLen, 1, gameBinary);
    fclose(gameBinary);

    for(int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }

    for(int i = 0; i < fileLen; i++) {
        memory[i+512] = buffer[i];
    }

    free(buffer);







}
