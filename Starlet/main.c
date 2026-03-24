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

#include "ARM_Core.h"

int main(int argc, char *argv[])
{
    if (argv[1] == NULL || argv[2] == NULL) {
        printf("Usage: ./armemu <boot0_binary> <boot1_binary>\n");
        return -1;
    }
    FILE* fd = fopen(argv[1], "rb");
    if(!fd) {
        printf("\nError opening file %s!\n", argv[1]);
        return -1;
    }
    fseek(fd, 0, SEEK_END);
    uint32_t b0_size = ftell(fd);
    if(!b0_size) {
        printf("\nFile size is 0! %d\n", b0_size);
        return -1;
    }
    fseek(fd, 0, SEEK_SET);
    uint32_t boot0[b0_size / 4];
    fread(boot0, 1, b0_size, fd);
    fclose(fd);
    fd = fopen(argv[2], "rb");
    if(!fd) {
        printf("\nError opening file %s!\n", argv[2]);
        return -1;
    }
    fseek(fd, 0, SEEK_END);
    uint32_t b1_size = ftell(fd);
    if(!b1_size) {
        printf("\nFile size is 0! %d\n", b1_size);
        return -1;
    }
    fseek(fd, 0, SEEK_SET);
    uint32_t boot1[b1_size / 4];
    fread(boot1, 1, b1_size, fd);

    ARM_LoadAndExecute(boot0, b0_size, boot1, b1_size);
    
    return 0;
}