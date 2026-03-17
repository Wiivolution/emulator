/*
    Starmulator - Low Level Wii IOP Emulator

    main.c - Main program
    
    Copyright (C) 2026 Abdelali221

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "armcore.h"
#include "dev.h"
#include "aes.h"
#include "sha.h"
#include "nand.h"

int main(int argc, char *argv[])
{
    char* file_name = argv[1];
    if (file_name == NULL) {
        printf("Usage: ./armemu <file_name>\n");
        return -1;
    }
    FILE* fd = fopen(file_name, "rb");
    if(!fd) {
        printf("\nError opening file %s!\n", file_name);
        return -1;
    }
    fseek(fd, 0, SEEK_END);
    uint32_t size = ftell(fd);
    if(!size) {
        printf("\nFile size is 0! %d\n", size);
        return -1;
    }
    fseek(fd, 0, SEEK_SET);
    uint32_t program[size / 4];
    fread(program, 1, size, fd);
    printf("\n Size = %d\n", size);
    AES_Init();
    SHA_Init();
    NAND_Init();
    execute_program(program, size);
    
    return 0;
}