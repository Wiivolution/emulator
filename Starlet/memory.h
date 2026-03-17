/*
    Starmulator - Low Level Wii IOP Emulator

    memory.h - Memory Management
    
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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#define MEM_1       0
#define MEM_2       1
#define REGS        2
#define ARM_SRAM_A  3
#define ARM_SRAM_B  4

#define MEM_24MB 24 * 1024 * 1024 // bytes
#define MEM_64MB 64 * 1024 * 1024 // bytes

typedef struct Memory {
    uint8_t *MEM1;
    uint8_t *MEM2;
    uint8_t *SRAM;
} Memory;

typedef struct MMU_Table {
    uint32_t MMU_Addr_Start;
    uint32_t MMU_Addr_End;
    uint32_t Real_Addr_Start;
    uint32_t Real_Addr_End;
    uint8_t  Mem;
} MMU_Table;

#include "ARM_Core.h"

int Mem_Init(Memory* mem);
int Mem_free(Memory* mem);
void* Mem_Resolve(uint32_t addr, void* as);

#endif