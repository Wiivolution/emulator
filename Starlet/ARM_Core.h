/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_Core.h - CPU Core
    
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

#define MODE_USER 0b10000
#define MODE_FIQ  0b10001
#define MODE_IRQ  0b10010
#define MODE_SPVS 0b10011
#define MODE_ABRT 0b10111
#define MODE_UNDF 0b11011
#define MODE_SYST 0b11111

#define T_FLAG 0x10
#define F_FLAG 0x20
#define I_FLAG 0x40
#define A_FLAG 0x80
#define E_FLAG 0x100
#define V_FLAG 0x10000000
#define C_FLAG 0x20000000
#define Z_FLAG 0x40000000
#define N_FLAG 0x80000000

struct vectortable {
    uint32_t Reset;
    uint32_t Undef_instr;
    uint32_t SWI;
    uint32_t Abort_prefetch;
    uint32_t Abort_data;
    uint32_t reserved;
    uint32_t IRQ;
    uint32_t FIQ;
};

#define NREGS 16
#define SP 13
#define LR 14 
#define PC 15 

typedef struct arm_state {
    uint32_t regs[NREGS];
    uint32_t cpsr;
    Memory   memory;
    uint32_t HW_regs[0x100];
} arm_state;

void ARM_LoadAndExecute(uint32_t *boot0, uint32_t b0_size, uint32_t *boot1, uint32_t b1_size);
void ARM_Print_State(struct arm_state *as);
void ARM_SetCPSR(struct arm_state *as, int result, long long result_long);
uint32_t _rot(uint32_t value, int shift);
uint32_t _rotl(uint32_t value, int shift);
bool THUMB_Is_cond_fulfilled(struct arm_state *as, uint8_t cond);

#endif