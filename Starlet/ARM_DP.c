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
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] & rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] & (long long) rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_EOR:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] ^ rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] ^ (long long) rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_SUB:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] - rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] - (long long) rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_RSB:
            as->regs[(instr >> 12) & 0xF] = rm_value - as->regs[(instr >> 16) & 0xF];
            result_long = (long long)rm_value - (long long) as->regs[(instr >> 16) & 0xF];
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_ADD:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] + rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] + (long long) rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_ADC:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] + rm_value + ((as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[(instr >> 16) & 0xF] + (long long) (rm_value + ((as->cpsr & C_FLAG) * C_FLAG));
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_SBC:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] - rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long) as->regs[(instr >> 16) & 0xF] - (long long) rm_value - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_RSC:
            as->regs[(instr >> 12) & 0xF] = rm_value - as->regs[(instr >> 16) & 0xF] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result_long = (long long)rm_value - (long long) as->regs[(instr >> 16) & 0xF] - (!(as->cpsr & C_FLAG) * C_FLAG);
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_TST:
            result_long = (long long) as->regs[(instr >> 16) & 0xF] & (long long) rm_value;
            result = as->regs[(instr >> 16) & 0xF] & rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_TEQ:
            result_long = (long long) as->regs[(instr >> 16) & 0xF] ^ (long long) rm_value;
            result = as->regs[(instr >> 16) & 0xF] ^ rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMP:
            result_long = (long long) as->regs[(instr >> 16) & 0xF] - (long long) rm_value;
            result = as->regs[(instr >> 16) & 0xF] - rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_CMN:
            result_long = (long long) as->regs[(instr >> 16) & 0xF] + (long long) rm_value;
            result = as->regs[(instr >> 16) & 0xF] + rm_value;
            ARM_SetCPSR(as, result, result_long);
        break;
        
        case DP_ORR:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] | rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] | (long long) rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_MOV:
            as->regs[(instr >> 12) & 0xF] = rm_value;
            result_long = (long long) as->regs[(instr >> 12) & 0xF];
            result = as->regs[(instr >> 12) & 0xF];
        break;
        
        case DP_BIC:
            as->regs[(instr >> 12) & 0xF] = as->regs[(instr >> 16) & 0xF] & ~rm_value;
            result_long = (long long) as->regs[(instr >> 16) & 0xF] & (long long) ~rm_value;
            result = as->regs[(instr >> 12) & 0xF];
        break;

        case DP_MVN:
            as->regs[(instr >> 12) & 0xF] = ~(rm_value);
            result = ~(rm_value);
            result_long = (long long) as->regs[(instr >> 12) & 0xF];
        break;
    }

    if (instr & S_BIT) {
        ARM_SetCPSR(as, result, result_long);
    }
}

