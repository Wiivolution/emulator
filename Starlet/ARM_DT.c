/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_DT.c - Data-transfer Unit
    
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
#include "memory.h"

bool ARM_is_push(uint32_t iw)
{
    return (((iw >> 25) & 0b111) == 0b100) && (((iw>>20) & 0b1) == 0b0);
}

bool iw_is_pop(uint32_t iw) 
{
    return (((iw >> 25) & 0b111) == 0b100) && (((iw>>20) & 0b1) == 0b1);
}

bool instr_is_single_data_transfer_instruction(uint32_t instr)
{
    return ((instr >> 26) & 0b11) == 0b01;
}

void execute_single_data_transfer_instruction(struct arm_state *as, uint32_t instr)
{
    uint8_t rd = (instr>>12) & 0xF;
    uint32_t rn = (instr>>16) & 0xF;
    uint32_t l_bit = (instr>>20) & 0b1;
    uint32_t w_bit = (instr>>21) & 0b1;
    uint32_t b_bit = (instr>>22) & 0b1;
    uint32_t u_bit = (instr>>23) & 0b1;
    uint32_t p_bit = (instr>>24) & 0b1;
    uint32_t i_bit = (instr>>25) & 0b1;
    uint32_t modified_base_value = as->regs[rn];
    uint16_t offset_value;

    //Check i bit
    if (i_bit == 1) {
        offset_value = as->regs[(instr & 0xF)] << ((instr>>7) & 0b11111); 
    }
    else {
        offset_value = instr & 0xFFF;
    }

    //Check p bit
    if (p_bit == 1) {
        //Check u bit
        if (u_bit == 1) {
            modified_base_value += offset_value; 
        }
        else {
            modified_base_value -= offset_value;
        }
    }

    if(rn == PC) {
        modified_base_value += 8;
    }

    //Check b bit
    if (b_bit == 1) {
        // Check l bit
        if (l_bit == 1) {
            memcpy(&as->regs[rd], Mem_Resolve(modified_base_value, as), 1); //ldrb
            as->regs[rd] = __builtin_bswap32(as->regs[rd]);
        }
    } else {
        //Check l bit
        if (l_bit == 1) {
            memcpy(&as->regs[rd], Mem_Resolve(modified_base_value, as), 4); //ldr
            as->regs[rd] = __builtin_bswap32(as->regs[rd]);
        }
        else {
            as->regs[rd] = __builtin_bswap32(as->regs[rd]);
            memcpy(Mem_Resolve(modified_base_value, as), &as->regs[rd], 4); // str
            as->regs[rd] = __builtin_bswap32(as->regs[rd]);
        }
    }

    //Check p bit
    if (p_bit == 0) {
        //Check u bit
        if (u_bit == 1) {
            modified_base_value += offset_value;
        }
        else {
            modified_base_value += offset_value;
        }
    }

    //Check w bit
    if (w_bit == 1) {
        as->regs[rn] = modified_base_value;
    }

    as->regs[PC] += 4;
}