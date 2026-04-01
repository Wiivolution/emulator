/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_Coproc.c - Coprocessor
    
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

bool ARM_CP_Is_RT_Instr(uint32_t instr) {
    return (((instr >> 24) & 0xF) == 0xE) && (instr & 0x10);
}

void ARM_CP_Execute_RT(struct arm_state *as, uint32_t instr) {

}

bool ARM_CP_Is_DP_Instr(uint32_t instr) {
    return (((instr >> 24) & 0xF) == 0xE) && !(instr & 0x10);
}

void ARM_CP_Execute_DP(struct arm_state *as, uint32_t instr) {
    
}

bool ARM_CP_Is_DT_Instr(uint32_t instr) {
    return (((instr >> 24) & 0xE) == 0xC);
}

void ARM_CP_Execute_DT(struct arm_state *as, uint32_t instr) {
    
}