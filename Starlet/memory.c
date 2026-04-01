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

void* boot0_rom = NULL;

MMU_Table Mem_Table[] = {
    0x80000000, 0x817FFFFF, 0x00000000, 0x017FFFFF, MEM_1,
    0xC0000000, 0xC17FFFFF, 0x00000000, 0x017FFFFF, MEM_1,
    0xD0000000, 0xD3FFFFFF, 0x10000000, 0x13FFFFFF, MEM_2,
    0xD0000000, 0xD3FFFFFF, 0x10000000, 0x13FFFFFF, MEM_2,
    0xFFFE0000, 0xFFFEFFFF, 0x00000000, 0x0000FFFF, ARM_SRAM_A,
    0xFFFF0000, 0xFFFFFFFF, 0x00010000, 0x0001FFFF, ARM_SRAM_B,
    0xFFF00000, 0xFFF0FFFF, 0x00000000, 0x0000FFFF, ARM_SRAM_A,
    0xFFF10000, 0xFFF1FFFF, 0x00010000, 0x0001FFFF, ARM_SRAM_B,
    0x0D400000,	0x0D40FFFF, 0x00000000, 0x0000FFFF, ARM_SRAM_A,
    0x0D410000,	0x0D41FFFF, 0x00010000, 0x0001FFFF, ARM_SRAM_B,
    0x0D000000,	0x0D00FFFF, 0x00000000, 0x0000FFFF, REGS,
    0x0D800000,	0x0D80FFFF, 0x00000000, 0x0000FFFF, REGS
};

int Mem_Init(Memory* mem, void* boot0) {
    printf("\nAllocating MEM1...");
    fflush(stdout);
    mem->MEM1 = calloc(MEM_24MB, 1);
    if(mem->MEM1 == NULL) {
        return -1;
    }
    printf("0x%X", mem->MEM1);
    printf("\nAllocating MEM2...");
    fflush(stdout);
    mem->MEM2 = calloc(MEM_64MB, 1);
    if(mem->MEM2 == NULL) {
        return -2;
    }
    printf("0x%X", mem->MEM2);
    printf("\nAllocating SRAM...");
    fflush(stdout);
    mem->SRAM = calloc(96 * 1024, 1);
    if(mem->SRAM == NULL) {
        return -3;
    }
    if(boot0 == NULL) {
        printf("\nInvalid boot0 buffer!");
        return -4;
    }
    boot0_rom = boot0;
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
    if(!(as->HW_regs[HW_BOOT0 / 4] & 0x800)) {
        if(!(as->HW_regs[HW_SRNPROT / 4] & 0x20)) {
            switch(Mem_Table[i].Mem) {
                case ARM_SRAM_A:
                    return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case ARM_SRAM_B:
                    return (as->memory.SRAM + 0x10000 + (addr - Mem_Table[i].MMU_Addr_Start));            
                break;
            }        
        } else {
            switch(Mem_Table[i].Mem) {
                case ARM_SRAM_B:
                    return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case ARM_SRAM_A:
                    return (as->memory.SRAM + 0x10000 + (addr - Mem_Table[i].MMU_Addr_Start));            
                break;
            }
        }
    } else {
        if(!(as->HW_regs[HW_SRNPROT / 4] & 0x20)) {
            switch(Mem_Table[i].Mem) {
                case ARM_SRAM_A:
                    return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case ARM_SRAM_B:
                    if(addr >= 0xFFFF0000) {
                        return (boot0_rom + (addr & 0xFFF));
                    } else {
                        return (as->memory.SRAM + 0x10000 + (addr - Mem_Table[i].MMU_Addr_Start));
                    }                            
                break;
            }        
        } else {
            switch(Mem_Table[i].Mem) {
                case ARM_SRAM_B:
                    return (as->memory.SRAM + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case ARM_SRAM_A:
                    return (as->memory.SRAM + 0x10000 + (addr - Mem_Table[i].MMU_Addr_Start));            
                break;
            }
        }
    }
}

void* Mem_Resolve(uint32_t addr, void *_as) {
    arm_state *as = (arm_state*)_as;
    void* retaddr = NULL;
    for(int i = 0; i < 12; i++) {
        if(addr >= Mem_Table[i].MMU_Addr_Start &&
           addr <= Mem_Table[i].MMU_Addr_End)
        {
            switch(Mem_Table[i].Mem) {
                case MEM_1:
                    retaddr = (as->memory.MEM1 + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case MEM_2:
                    retaddr = (as->memory.MEM2 + (addr - Mem_Table[i].MMU_Addr_Start));
                break;

                case ARM_SRAM_A:
                case ARM_SRAM_B:
                    retaddr = Mem_ResolveSRAM(addr, as, i);
                break;

                case REGS:
                    retaddr = &(as->HW_regs[(addr - Mem_Table[i].MMU_Addr_Start) / 4]);
                break;
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