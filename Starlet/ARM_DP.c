/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_DP.c - Data-processing Unit
    
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
#include <unistd.h>

#include "ARM_Core.h"

#define I_BIT 0x02000000
#define S_BIT 0x00100000

// Operands :

enum DP_Operands {
    DP_AND = 0b0000,
    DP_EOR = 0b0001,
    DP_SUB = 0b0010,
    DP_RSB = 0b0011,
    DP_ADD = 0b0100,
    DP_ADC = 0b0101,
    DP_SBC = 0b0110,
    DP_RSC = 0b0111,
    DP_TST = 0b1000,
    DP_TEQ = 0b1001,
    DP_CMP = 0b1010,
    DP_CMN = 0b1011,
    DP_ORR = 0b1100,
    DP_MOV = 0b1101,
    DP_BIC = 0b1110,
    DP_MVN = 0b1111
};

bool ARM_DP_Is_DataProcessing(uint32_t instr) {
    return ((instr >> 26) & 0b11) == 0;
}

bool ARM_DP_Carry(uint32_t a, uint32_t b) {
    return a >= b;
}

void ARM_DP_Execute(struct arm_state* as, uint32_t instr) {
    uint8_t rd = (instr >> 12) & 0xF;
    uint8_t rm = (instr >> 16) & 0xF;
    uint32_t rm_value;
    uint8_t rotate = (instr >> 8) & 0xF;
    int result;
    long long result_long;

    if (instr & I_BIT) { // I bit
        rm_value = _rot(instr & 0xFF, rotate * 2);
    } else {
        rm_value = _rotl(as->regs[(instr & 0xF)], (instr >> 7) & 0x1F);
    }

    switch((instr >> 21) & 0xF) { // opcode
        case DP_AND:
            as->regs[rd] = as->regs[rm] & rm_value;
            result_long = (long long) as->regs[rm] & (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_EOR:
            as->regs[rd] = as->regs[rm] ^ rm_value;
            result_long = (long long) as->regs[rm] ^ (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_SUB:
            as->regs[rd] = as->regs[rm] - rm_value;
            result_long = (long long) as->regs[rm] - (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_RSB:
            as->regs[rd] = rm_value - as->regs[rm];
            result_long = (long long)rm_value - (long long) as->regs[rm];
            result = as->regs[rd];
        break;
        
        case DP_ADD:
            as->regs[rd] = as->regs[rm] + rm_value;
            result_long = (long long) as->regs[rm] + (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_ADC:
            as->regs[rd] = as->regs[rm] + rm_value + ((as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[rm] + (long long) (rm_value + ((as->cpsr & C_FLAG) * C_FLAG));
            result = as->regs[rd];
        break;
        
        case DP_SBC:
            as->regs[rd] = as->regs[rm] - rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[rm] - (long long) rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[rd];
        break;
        
        case DP_RSC:
            as->regs[rd] = rm_value - as->regs[rm] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long)rm_value - (long long) as->regs[rm] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[rd];
        break;
        
        case DP_TST:
            result_long = (long long) as->regs[rm] & (long long) rm_value;
            result = as->regs[rm] & rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_TEQ:
            result_long = (long long) as->regs[rm] ^ (long long) rm_value;
            result = as->regs[rm] ^ rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMP:
            result_long = (long long) as->regs[rm] - (long long) rm_value;
            result = as->regs[rm] - rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMN:
            result_long = (long long) as->regs[rm] + (long long) rm_value;
            result = as->regs[rm] + rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_ORR:
            as->regs[rd] = as->regs[rm] | rm_value;
            result_long = (long long) as->regs[rm] | (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_MOV:
            as->regs[rd] = rm_value;
            result_long = (long long) as->regs[rd];
            result = as->regs[rd];
        break;
        
        case DP_BIC:
            as->regs[rd] = as->regs[rm] & ~rm_value;
            result_long = (long long) as->regs[rm] & (long long) ~rm_value;
            result = as->regs[rd];
        break;

        case DP_MVN:
            as->regs[rd] = ~(rm_value);
            result = ~(rm_value);
            result_long = (long long) as->regs[rd];
        break;
    }

    if (instr & S_BIT) {
        ARM_SetCPSR(as, result, result_long);
    }
    if((instr & 0xF) == PC && !(instr & I_BIT)) {
        as->regs[rd] += 8;
    }
}

int32_t asr(int32_t val, unsigned int shift) {
    return (int32_t)((int64_t)val >> shift);
}

void THUMB_DP_Execute_Shift(struct arm_state* as, uint16_t instr) {
    uint8_t shift = ((instr >> 6) & 0x1F);
    uint8_t rm = (instr >> 3) & 0x7;
    uint8_t rt = (instr & 0x7); // Target
    switch(instr & 0xF800) {
        case 0x0000: // LSL
            if(shift < 32) {
                printf("\nLSL r%d, 0x%X", rt, shift);
                as->regs[rt] = as->regs[rm] << shift;
                ARM_SetCPSR(as, as->regs[rt], (long long) as->regs[rt]);
            } else if((shift > 32)) {
                as->cpsr |= (as->regs[rt]) * C_FLAG;
            } else {
                as->cpsr |= ~C_FLAG;
            }
        break;

        case 0x0800: // LSR
            if(shift < 32) {
                as->regs[rt] = as->regs[rm] >> shift;
                ARM_SetCPSR(as, as->regs[rt], (long long) as->regs[rt]);
            } else if((shift > 32)) {
                as->cpsr |= (as->regs[rt]) * C_FLAG;
            } else {
                as->cpsr |= ~C_FLAG;
            } 
        break;
        
        case 0x1000: // ASR
            if(shift < 32) {
                as->regs[rt] = asr(as->regs[rm], shift);
                ARM_SetCPSR(as, as->regs[rt], (long long) as->regs[rt]);
            } else if((shift >= 32)) {
                as->cpsr |= ((as->regs[rt] & 0x80000000) >> 2) & C_FLAG;
                if(((as->regs[rt] & 0x80000000) >> 31)) {
                    as->regs[rt] = 0xFFFFFFF;
                } else {
                    as->regs[rt] = 0;
                }
            }
        break;
    }

    as->regs[PC] += 2;
}

void THUMB_DP_Execute_immop(struct arm_state* as, uint16_t instr) {
    uint8_t rd = (instr & 0x700) >> 8;
    printf("\nimmop: 0x%X", (instr & 0x1800) >> 11);
    switch((instr & 0x1800) >> 11) {
        case 0: // MOV
            as->regs[rd] = instr & 0xFF;
            ARM_SetCPSR(as,
                        (uint32_t)as->regs[rd],
                        (long long)as->regs[rd]);
        break;

        case 1: // CMP
            ARM_SetCPSR(as,
                        (uint32_t)as->regs[rd] - instr & 0xFF,
                        (long long)as->regs[rd] - instr & 0xFF);
        break;

        case 2: // ADD
            as->regs[rd] += instr & 0xFF;
            ARM_SetCPSR(as,
                        (uint32_t)as->regs[rd],
                        (long long)as->regs[rd]);
        break;

        case 3: // SUB
            as->regs[rd] -= instr & 0xFF;
            ARM_SetCPSR(as,
                        (uint32_t)as->regs[rd],
                        (long long)as->regs[rd]);
        break;
    }
    as->regs[PC] += 2;
}

void THUMB_DP_Execute_Special(struct arm_state* as, uint16_t instr) {
    uint8_t rm = ((instr >> 3) & 0x7) + (((instr >> 6) & 1) * 8),
            rd = (instr & 0x7) + (((instr >> 7) & 1) * 8);
    switch((instr & 0x300) >> 8) {
        case 0: // ADD
            as->regs[rd] += as->regs[rm];
        break;

        case 1:
        
        break;

        case 2: // MOV
            as->regs[rd] = as->regs[rm];
        break;
    }
    as->regs[PC] += 2;
}

void THUMB_DP_Execute_Regs(struct arm_state* as, uint16_t instr) {
    uint8_t rd = instr & 0x7;
    uint8_t rm = (instr >> 3) & 0x7;
    int result;
    long long result_long;
    switch((instr >> 6) & 0xF) { // opcode
        case DP_AND:
            as->regs[rd] = as->regs[rd] & as->regs[rm];
            result_long = (long long) as->regs[rd] & (long long) as->regs[rm];
            result = as->regs[rd];
        break;

        case DP_ORR:
            as->regs[rd] = as->regs[rd] | as->regs[rm];
            result_long = (long long) as->regs[rd] | (long long) as->regs[rm];
            result = as->regs[rd];
        break;

        /*
        
        case DP_EOR:
            as->regs[rd] = as->regs[rd] ^ rm_value;
            result_long = (long long) as->regs[rd] ^ (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_SUB:
            as->regs[rd] = as->regs[rd] - rm_value;
            result_long = (long long) as->regs[rd] - (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_RSB:
            as->regs[rd] = rm_value - as->regs[rd];
            result_long = (long long)rm_value - (long long) as->regs[rd];
            result = as->regs[rd];
        break;
        
        case DP_ADC:
            as->regs[rd] = as->regs[rd] + rm_value + ((as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[rd] + (long long) (rm_value + ((as->cpsr & C_FLAG) * C_FLAG));
            result = as->regs[rd];
        break;
        
        case DP_SBC:
            as->regs[rd] = as->regs[rd] - rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[rd] - (long long) rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[rd];
        break;
        
        case DP_RSC:
            as->regs[rd] = rm_value - as->regs[rd] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long)rm_value - (long long) as->regs[rd] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[rd];
        break;
        
        case DP_TST:
            result_long = (long long) as->regs[rd] & (long long) rm_value;
            result = as->regs[rd] & rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_TEQ:
            result_long = (long long) as->regs[rd] ^ (long long) rm_value;
            result = as->regs[rd] ^ rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMP:
            result_long = (long long) as->regs[rd] - (long long) rm_value;
            result = as->regs[rd] - rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMN:
            result_long = (long long) as->regs[rd] + (long long) rm_value;
            result = as->regs[rd] + rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_MOV:
            as->regs[rd] = rm_value;
            result_long = (long long) as->regs[rd];
            result = as->regs[rd];
        break;
        
        case DP_BIC:
            as->regs[rd] = as->regs[rd] & ~rm_value;
            result_long = (long long) as->regs[rd] & (long long) ~rm_value;
            result = as->regs[rd];
        break;

        case DP_MVN:
            as->regs[rd] = ~(rm_value);
            result = ~(rm_value);
            result_long = (long long) as->regs[rd];
        break;
        */
    }
    ARM_SetCPSR(as,
                (uint32_t)as->regs[rd],
                (long long)as->regs[rd]);
    as->regs[PC] += 2;
}

void THUMB_add_sp_pc_imm(struct arm_state* as, uint16_t instr) {
    uint8_t rd = (instr >> 8) & 0x7;
    if(instr & 0x800) {
        as->regs[rd] = as->regs[SP] + (instr & 0xFF);
    } else {
        as->regs[rd] = as->regs[PC] + (instr & 0xFF);
    }
    as->regs[PC] += 2;
}