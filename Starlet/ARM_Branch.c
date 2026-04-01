/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_Branch.c - Branch Handler
    
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ARM_Core.h"
#include "memory.h"

int SignExtend(uint32_t x, uint8_t k) {
    return (x >> (k-1) & 1) ? (x | ~((1 << k) - 1)) : x;
}

bool ARM_BR_Is_BX_Instr(uint32_t instr) {
    return ((instr >> 4) & 0xFFFFFF) == 0x12FFF1;
}

void ARM_BR_Execute_BX(struct arm_state *as, uint32_t instr) {
    uint8_t rn = instr & 0b1111;

    as->regs[PC] = (as->regs[rn] & 0xFFFFFFFE);
    
    if (instr & 0x20) { // BLX
        as->regs[LR] = as->regs[PC] + 4;
    }

    as->cpsr |= (as->regs[rn] << 4) & 0x10;
}

bool ARM_BR_Is_Branch_Instr(uint32_t instr) {
    return ((instr >> 25) & 0b111) == 0b101;
}

void ARM_BR_Execute_Branch(struct arm_state *as, uint32_t instr) {
    uint32_t l_bit = (instr >> 24) & 0b1;
    uint32_t offset = instr & 0xFFFFFF;
    uint32_t destination;

    if ((instr >> 23) & 0b1) {
        destination = offset | 0xFF000000;
    } else {
        destination = offset | 0x00000000;
    }

    destination = destination << 2;

    if (l_bit == 1) {  
        as->regs[LR] = as->regs[PC] + 4;
    }

    as->regs[PC] += (destination + 8);
}


void THUMB_BR_Execute_cond(struct arm_state *as, uint16_t instr) {
    printf("\nbranch-> cond: 0x%X | is fulfilled? %s", 
          (instr >> 8) & 0xF, 
          THUMB_Is_cond_fulfilled(as, (instr >> 8) & 0xF) ? "yes" : "no");
    printf("\noffset: %d", (int8_t)(instr & 0xFF));
    if(THUMB_Is_cond_fulfilled(as, (instr >> 8) & 0xF)) {
        if(instr & 0x80) {
            as->regs[PC] -= (instr & 0xFF) << 1;
        } else {
            as->regs[PC] += ((instr & 0xFF) << 1) + 4;
        }
    } else {
        as->regs[PC] += 2;
    }
}

void THUMB_BR_Execute_uncond(struct arm_state *as, uint16_t instr) {
    if(instr & 0x200) {
        as->regs[PC] -= (instr & 0x1FF) << 1;
    } else {
        as->regs[PC] += (instr & 0x1FF) << 1;
    }
}

void THUMB_BR_Execute_BL_BLX(struct arm_state *as, uint16_t instr) {
    uint8_t H = (instr >> 11) & 0x3;
    int16_t offset = instr & 0x7FF;
    if (H == 2) {
        as->regs[LR] = as->regs[PC] + (SignExtend(offset, 11) << 12);
        as->regs[PC] += 2;
    } else if (H == 3) {
        uint32_t temp_PC = as->regs[PC];
        as->regs[PC] = as->regs[LR] + (offset << 1);
        as->regs[PC] += 4;
        as->regs[LR] = (temp_PC + 2) | 1;
    } else if (H == 1) {
        as->regs[PC] = (as->regs[LR] + (offset << 1)) & 0xFFFFFFFC;
        as->regs[LR] = (as->regs[PC] + 2) | 1;
        as->cpsr |=  ~T_FLAG;
    }
}

void THUMB_BR_Execute_BX(struct arm_state *as, uint16_t instr) {
    uint8_t rn = (instr >> 3) & 0xF;
    as->regs[PC] = (as->regs[rn] & 0xFFFFFFFE);
    as->cpsr |= (as->regs[rn] << 4) & 0x10;
}
