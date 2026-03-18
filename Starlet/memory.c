/*
    Starmulator - Low Level Wii IOP Emulator

    memory.c - Memory Management
    
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

#include "hollywood.h"
#include "memory.h"
#include "dev.h"

MMU_Table Mem_Table[] = {
    0x80000000, 0x817FFFFF, 0x00000000, 0x017FFFFF, MEM_1,
    0xC0000000, 0xC17FFFFF, 0x00000000, 0x017FFFFF, MEM_1,
    0xD0000000, 0xD3FFFFFF, 0x10000000, 0x13FFFFFF, MEM_2,
    0xD0000000, 0xD3FFFFFF, 0x10000000, 0x13FFFFFF, MEM_2,
    0xFFFE0000, 0xFFFEFFFF, 0x00000000, 0x0000FFFF, ARM_SRAM_A,
    0xFFFF0000, 0xFFFFFFFF, 0x00010000, 0x0001FFFF, ARM_SRAM_B,
    0x0D400000,	0x0D40FFFF, 0x00000000, 0x0000FFFF, ARM_SRAM_A,
    0x0D410000,	0x0D41FFFF, 0x00010000, 0x0001FFFF, ARM_SRAM_B,
    0x0D000000,	0x0D00FFFF, 0x00000000, 0x0000FFFF, REGS,
    0x0D800000,	0x0D80FFFF, 0x00000000, 0x0000FFFF, REGS
};

int Mem_Init(Memory* mem) {
    printf("\nAllocating MEM1...");
    fflush(stdout);
    mem->MEM1 = malloc(24*1024);
    if(mem->MEM1 == NULL) {
        return -1;
    }
    printf("0x%X", mem->MEM1);
    printf("\nAllocating MEM2...");
    fflush(stdout);
    mem->MEM2 = malloc(MEM_64MB);
    if(mem->MEM2 == NULL) {
        return -2;
    }
    printf("0x%X", mem->MEM2);
    printf("\nAllocating SRAM...");
    fflush(stdout);
    mem->SRAM = malloc(96 * 1024);
    if(mem->SRAM == NULL) {
        return -3;
    }
    printf("0x%X\n", mem->SRAM);
    return 0;
}

int Mem_free(Memory* mem) {
    free(mem->MEM1);
    free(mem->MEM2);
    free(mem->SRAM);
    return 0;    
}

void* Mem_ResolveSRAM(uint32_t addr, arm_state *as, uint8_t i) {
    if(!(as->HW_regs[HW_SRNPROT] & 0x20)) {
        return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
    } else {
        return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
    }
}

void* Mem_Resolve(uint32_t addr, void *_as) {
    arm_state *as = (arm_state*)_as;
    void* retaddr = NULL;
    for(int i = 0; i < 10; i++) {
        if(addr >= Mem_Table[i].MMU_Addr_Start &&
           addr <= Mem_Table[i].MMU_Addr_End)
        {
            if(Mem_Table[i].Mem == MEM_1) {
                retaddr = (as->memory.MEM1 + (addr - Mem_Table[i].MMU_Addr_Start));
            } else if(Mem_Table[i].Mem == MEM_2) {
                retaddr = (as->memory.MEM2 + (addr - Mem_Table[i].MMU_Addr_Start));
            } else if(Mem_Table[i].Mem == ARM_SRAM_A || Mem_Table[i].Mem == ARM_SRAM_B) {
                retaddr = Mem_ResolveSRAM(addr, as, i);
            } else if(Mem_Table[i].Mem == REGS) {
                retaddr = &(as->HW_regs[(addr - Mem_Table[i].MMU_Addr_Start) / 4]);
            }
        }
    }
    if(retaddr != NULL) {
        return retaddr;
    }
    retaddr = Dev_ResolveRegs(addr);
    if(retaddr != NULL) {
        return retaddr;
    }
    printf("\nAddress invalid! | addr: 0x%X \n", addr);
    //ARM_Print_State(as);
    fflush(stdout);
    Mem_free(&as->memory);
    exit(-1);
}