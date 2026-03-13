/*
    Starmulator - Low Level Wii IOP Emulator

    armemu.c - CPU Core
    
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

#ifndef _ARMEMU_H_
#define _ARMEMU_H_

#define NREGS 16
#define SP 13
#define LR 14 
#define PC 15 
#define STACK_SIZE 1024

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
    uint8_t  *stack;
    uint8_t  *memory;
    uint32_t mem_size;
    uint64_t mem_offset;
    uint32_t computational_count;
    uint32_t memory_count;
    uint32_t branch_count;
} arm_state;

#endif