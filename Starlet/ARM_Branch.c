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

bool THUMB_BR_Is_BranchType_Instr(uint16_t instr) {
    return ((instr >> 12) & 0xF) == 0xD ||
           ((instr >> 12) & 0xF) == 0xE ||
           ((instr >> 12) & 0xF) == 0xF;
}

void THUMB_BR_Execute_BranchType_Instr(struct arm_state* as, uint16_t instr) {
    switch(((instr >> 12) & 0xF)) {
        case 0xD:
            
        break;

        case 0xE:
        break;

        case 0xF:
        break;
    }
}
