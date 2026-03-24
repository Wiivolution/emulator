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
#include <pthread.h>

#include "dev.h"
#include "nand.h"

NAND_REGS* NAND_Regs;
pthread_t NAND_thread;

device NAND = {
    0, NULL, 0x0d010000, 0x1C, 1
};

void* NAND_EventHandler(void* args) {
    while(1) {
        if(NAND_Regs->CTRL) {
            printf("\nNAND -> CMD: 0x%X | CONFIG: 0x%X | ADDR1: 0x%X | ADDR2: 0x%X | DATABUF: 0x%X", __builtin_bswap32(NAND_Regs->CTRL), __builtin_bswap32(NAND_Regs->CONFIG), __builtin_bswap32(NAND_Regs->ADDR1), __builtin_bswap32(NAND_Regs->ADDR2), __builtin_bswap32(NAND_Regs->DATABUF));
            NAND_Regs->CTRL = 0;
        }
    }
}

int NAND_Init() {
    NAND_Regs = calloc(0x1c, 1);
    NAND.ptr = NAND_Regs;
    Dev_AddDevice(&NAND);
    pthread_create(&NAND_thread, NULL, NAND_EventHandler, NULL);
    return 0;
}

int NAND_Deinit() {
    pthread_join(NAND_thread, NULL);
    Dev_RemoveDevice(NAND.ID);
    free(NAND_Regs);
    return 0;
}