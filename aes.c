/*
    Starmulator - Low Level Wii IOP Emulator

    aes.c - AES Engine
    
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

uint8_t* AES_Regs;
pthread_t AES_thread;

device AES = {
    0, NULL, 0x0d020000, 0x14, 2
};

void* AES_EventHandler(void* args) {
    while(1) {
        if(AES_Regs[0] != 0) {
            printf("\nCMD: 0x%X", AES_Regs[0]);
            AES_Regs[0] = 0;
        }
    }
}

int AES_Init() {
    AES_Regs = malloc(0x1c);
    AES.ptr = AES_Regs;
    Dev_AddDevice(&AES);
    pthread_create(&AES_thread, NULL, AES_EventHandler, NULL);
    return 0;
}

int AES_Deinit() {
    pthread_join(AES_thread, NULL);
    Dev_RemoveDevice(AES.ID);
    free(AES_Regs);
    return 0;
}