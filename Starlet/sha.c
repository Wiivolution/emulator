/*
    Starmulator - Low Level Wii IOP Emulator

    sha.c - SHA Engine
    
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

uint8_t* SHA_Regs;
pthread_t SHA_thread;

device SHA = {
    0, NULL, 0xD030000, 0x1c, 2
};

void* SHA_EventHandler(void* args) {
    while(1) {
        if(SHA_Regs[0] != 0) {
            printf("\nCMD: 0x%X", SHA_Regs[0]);
            fflush(stdout);
            SHA_Regs[0] = 0;
        }
    }
}

int SHA_Init() {
    SHA_Regs = malloc(0x1c);
    SHA.ptr = SHA_Regs;
    printf("\n SHA.ptr: 0x%X", SHA.ptr);
    Dev_AddDevice(&SHA);
    pthread_create(&SHA_thread, NULL, SHA_EventHandler, NULL);
    return 0;
}

int SHA_Deinit() {
    pthread_join(SHA_thread, NULL);
    Dev_RemoveDevice(SHA.ID);
    free(SHA_Regs);
    return 0;
}