/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_Core.c - CPU Core
    
    Copyright (C) 2026 Abdelali221

    Based on : https://github.com/Goomble/Arm-Emulator    

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
#include "ARM_Branch.h"
#include "ARM_DP.h"
#include "ARM_DT.h"
#include "memory.h"

struct arm_state *ARM_State_New(size_t program_loc, size_t entrypoint,
                                uint32_t *program, size_t program_size,
                                uint32_t arg0, uint32_t arg1,
                                uint32_t arg2, uint32_t arg3)
{
    struct arm_state *as;
    int i;

    as = (struct arm_state *) malloc(sizeof(struct arm_state));
    if (as == NULL) {
        printf("malloc() failed, exiting.\n");
        exit(-1);
    }

    printf("Initializing Memory...");

    if(Mem_Init(&as->memory) < 0) {
        printf("\nmemory init failed!");
        exit(-1);
    } 

    printf("Done.\n");
    printf("Putting program file into address 0x%X| Program size: 0x%X\n", Mem_Resolve(program_loc, as), program_size);
    fflush(stdout);
    memset(as->HW_regs, 0, 0x100);
    memcpy(Mem_Resolve(program_loc, as), program, program_size);

    // Initialize all registers to zero.
    as->cpsr = 0;
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->regs[PC] = entrypoint;
    
    as->regs[0] = arg0;
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;

    return as;
}

void ARM_State_Free(struct arm_state *as)
{
    Mem_free(&as->memory);
    free(as);
}

void ARM_Print_State(struct arm_state *as)
{
    int i;
    
    printf("\nRegister values after execution:\n");
    for (i = 0; i < NREGS; i++) {
        printf("r%d = (%X) %d\n", i, as->regs[i], (int) as->regs[i]);
    }
    uint32_t pcdata = 0;
    memcpy(&pcdata, Mem_Resolve(as->regs[PC], as), 4);
    
    printf("Data at PC : 0x%X\n", __builtin_bswap32(pcdata));
    printf("cpsr: 0x%x", as->cpsr);
    if(as->cpsr & C_FLAG) {
        printf(" C ");
    }
    if(as->cpsr & N_FLAG) {
        printf(" N ");
    }
    if(as->cpsr & Z_FLAG) {
        printf(" Z ");
    }
    if(as->cpsr & V_FLAG) {
        printf(" V ");
    }
    printf("\nGPIO: 0x%08X\n", __builtin_bswap32(*(uint32_t*)Mem_Resolve(0xd8000e0, as)));
}

void ARM_SetCPSR(struct arm_state *as, int result, long long result_long) 
{
    if (result > 2147483647 || result < -2147483648) {
        as->cpsr = as->cpsr | V_FLAG;
    } else {
        as->cpsr = as->cpsr & (0xFFFFFFFF - V_FLAG);
    }

    if(result >= 0) {
        as->cpsr |= C_FLAG;
    } else {
        as->cpsr &= (0xFFFFFFFF - C_FLAG);
    }

    if (result < 0) {
        as->cpsr = as->cpsr | N_FLAG; //setting N to 1
        as->cpsr = as->cpsr & (0xFFFFFFFF - Z_FLAG); //setting Z to 0
    } else if (result == 0) {
        as->cpsr = as->cpsr | Z_FLAG; //setting Z to 1
        as->cpsr = as->cpsr & (0xFFFFFFFF - N_FLAG); //setting N to 0
    } else {
        as->cpsr = as->cpsr & (0xFFFFFFFF - Z_FLAG); //setting Z to 0
        as->cpsr = as->cpsr & (0xFFFFFFFF - N_FLAG); //setting N to 0
    }
}

bool ARM_Is_cond_fulfilled(struct arm_state *as, uint32_t instr) {
    switch ((instr >> 28) & 0xFF) {
        case 0b0000: //EQ
            return ((as->cpsr >> 30) & 0b1) == 1;
        case 0b0001: //NE
            return ((as->cpsr >> 30) & 0b1) == 0;
        case 0b0010:
            return (as->cpsr & C_FLAG);
        case 0b0011:
            return !(as->cpsr & C_FLAG);
        case 0b0100:
            return (as->cpsr & N_FLAG);
        case 0b0101:
            return !(as->cpsr & N_FLAG);
        case 0b0110:
            return (as->cpsr & V_FLAG);
        case 0b0111:
            return !(as->cpsr & V_FLAG);
        case 0b1000:
            return (as->cpsr & C_FLAG) && !(as->cpsr & Z_FLAG);
        case 0b1001:
            return !(as->cpsr & C_FLAG) || (as->cpsr & Z_FLAG);
        case 0b1010:
            return (as->cpsr & C_FLAG) && !(as->cpsr & Z_FLAG);
        case 0b1011: //LT
            return ((as->cpsr >> 28) & 0b1) != ((as->cpsr >> 31) & 0b1);
        case 0b1100: //GT
            return (((as->cpsr >> 30) & 0b1) == 0) && (((as->cpsr >> 31) & 0b1) == ((as->cpsr >> 28) & 0b1));
        case 0b1101: //GT
            return (((as->cpsr >> 30) & 0b1) == 1) && (((as->cpsr >> 31) & 0b1) != ((as->cpsr >> 28) & 0b1));
        case 0b1110: //AL
            return true;
    }
}

uint32_t _rot(uint32_t value, int shift) {
    if(shift == 0) return value;
    return (value << (32 - shift));
}

uint32_t _rotl(uint32_t value, int shift) {
    if(shift == 0) return value;
    return (value << shift);
}

int ARM_Execute_Single(struct arm_state *as)
{
    uint32_t instr;
    memcpy(&instr, Mem_Resolve(as->regs[PC], as), 4);
    instr = __builtin_bswap32(instr);
    int ret = 0;

    //ARM_Print_State(as);

    if (ARM_BR_Is_BX_Instr(instr)) {
        if(ARM_Is_cond_fulfilled(as, instr)) {
            ARM_BR_Execute_BX(as, instr);
        } else {
            as->regs[PC] += 4;
        }
    } else if (ARM_BR_Is_Branch_Instr(instr)) {
        if (ARM_Is_cond_fulfilled(as, instr)) {
            ARM_BR_Execute_Branch(as, instr);
        } else {
            as->regs[PC] += 4;
        }
    } else if (ARM_DP_Is_DataProcessing(instr)) {
        if (ARM_Is_cond_fulfilled(as, instr)) {
            ARM_DP_Execute(as, instr);
        }
        as->regs[PC] += 4;
    } else if (ARM_DT_Is_SDT_Instr(instr)) {
        if (ARM_Is_cond_fulfilled(as, instr)) {
            ARM_DT_Execute_SDT_Instr(as, instr);
        } else {
            as->regs[PC] += 4;
        }
    } else if (ARM_DT_Is_Push_Instr(instr)) {
        if (ARM_Is_cond_fulfilled(as, instr)) {
            ARM_DT_Execute_Push(as, instr);
        } else {
            as->regs[PC] += 4;
        }
    } else if (ARM_DT_Is_Pop_Instr(instr)) {
        ARM_DT_Execute_Pop(as, instr);
    } else {
        return -1;
    }
    return ret;
}

uint32_t instructions_executed = 0;

uint32_t ARM_Execute(struct arm_state *as)
{
    while (as->regs[PC] != 0xFFFF0588) {
        if(!(as->cpsr & T_FLAG)) {
            int ret = ARM_Execute_Single(as);
            if(ret != 0) {
                printf("\nBAD/UNIMPLEMENTED Instuction!");
                fflush(stdout);
                sleep(1);
                break;
            }
        } else {
            //THUMB_Execute_Single(as);
        }
        fflush(stdout);
        //usleep(200000);
        instructions_executed++;
    }

    return as->regs[0];
}

void ARM_LoadAndExecute(uint32_t *program, uint32_t program_size) 
{
    struct arm_state *as;

    as = ARM_State_New(0xFFFF0000, 0xFFFF0000, program, program_size, 0, 0, 0, 0);
    ARM_Execute(as);

    printf("\n-----------------executing program --------------------\n");
    ARM_Print_State(as);
    ARM_State_Free(as);    
}