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
#include <openssl/sha.h>

#include "ARM_Core.h"
#include "dev.h"
#include "sha.h"

struct SHA_REGS* SHA_Regs;
struct arm_state *as;
pthread_t SHA_thread;

device SHA = {
    0, NULL, 0xD030000, 0x1c, 2
};

void* SHA_EventHandler(void* args) {
    while(1) {
        if(SHA_Regs->CTRL != 0) {
            printf("\nSHA -> CMD: 0x%X | blocks: 0x%X | hash: 0x%X, 0x%X, 0x%X, 0x%X", __builtin_bswap32(SHA_Regs->CTRL), ((__builtin_bswap32(SHA_Regs->CTRL) & 0x1FF) * 6) + 1, __builtin_bswap32(SHA_Regs->H0),
                    __builtin_bswap32(SHA_Regs->H1), __builtin_bswap32(SHA_Regs->H2),
                    __builtin_bswap32(SHA_Regs->H3), __builtin_bswap32(SHA_Regs->H4));
            fflush(stdout);
            
            if(__builtin_bswap32(SHA_Regs->CTRL) & 0x80000000) {
                //SHA1((uint8_t*)Mem_Resolve(__builtin_bswap32(SHA_Regs->SRC) , as), ((__builtin_bswap32(SHA_Regs->CTRL) & 0x1FF) * 6), (uint8_t*)&SHA_Regs->H0);
            }
            
            SHA_Regs->CTRL = 0;
        }
    }
}

int SHA_Init(struct arm_state *_as) {
    as = _as;
    SHA_Regs = calloc(0x1c, 1);
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