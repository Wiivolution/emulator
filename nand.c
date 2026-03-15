/*
    Starmulator - Low Level Wii IOP Emulator

    nand.c - NAND Management
    
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dev.h"

uint8_t* NAND_Regs;

device NAND = {
    0, NULL, 0x0d010000, 0x1C, 1
};

int NAND_Init() {
    NAND_Regs = malloc(0x1c);
    NAND.ptr = NAND_Regs;
    Dev_AddDevice(&NAND);
    return 0;
}

int NAND_Deinit() {
    Dev_RemoveDevice(NAND.ID);
    free(NAND_Regs);
    return 0;
}