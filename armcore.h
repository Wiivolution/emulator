/*
    Starmulator - Low Level Wii IOP Emulator

    armcore.h - CPU Core
    
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

#ifndef _ARMCORE_H_
#define _ARMCORE_H_

#include "memory.h"

#define NREGS 16
#define SP 13
#define LR 14 
#define PC 15 

#define V_FLAG 0x10000000
#define C_FLAG 0x20000000
#define Z_FLAG 0x40000000
#define N_FLAG 0x80000000

typedef struct vectortable {
    uint32_t Reset;
    uint32_t Undef_instr;
    uint32_t SWI;
    uint32_t Abort_prefetch;
    uint32_t Abort_data;
    uint32_t reserved;
    uint32_t IRQ;
    uint32_t FIQ;
} vectortable;

typedef struct arm_state {
    uint32_t regs[NREGS];
    uint32_t cpsr;
    Memory   memory;
    uint32_t HW_regs[0x400];
} arm_state;

void execute_program(uint32_t *program, uint32_t program_size);

#endif