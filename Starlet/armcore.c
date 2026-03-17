/*
    Starmulator - Low Level Wii IOP Emulator

    armemu.c - CPU Core
    
    Copyright :
        - (C) 2026 Abdelali221
        - (C) 2018 Arseniy Novitskiy
    

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

#include "armcore.h"
#include "memory.h"

// Data-processing :

#define DP_AND 0b0000
#define DP_EOR 0b0001
#define DP_SUB 0b0010
#define DP_RSB 0b0011
#define DP_ADD 0b0100
#define DP_ADC 0b0101
#define DP_SBC 0b0110
#define DP_RSC 0b0111
#define DP_TST 0b1000
#define DP_TEQ 0b1001
#define DP_CMP 0b1010
#define DP_CMN 0b1011
#define DP_ORR 0b1100
#define DP_MOV 0b1101
#define DP_BIC 0b1110
#define DP_MVN 0b1111

extern void* Mem_Resolve(uint32_t addr, void* as);

struct arm_state *arm_state_new(size_t program_loc, size_t entrypoint,
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

void arm_state_free(struct arm_state *as)
{
    Mem_free(&as->memory);
    free(as);
}

void arm_state_print(struct arm_state *as)
{
    int i;
    
    printf("\nRegister values after execution:\n");
    for (i = 0; i < NREGS; i++) {
        printf("r%d = (%X) %d\n", i, as->regs[i], (int) as->regs[i]);
    }
    uint32_t pcdata = 0;
    memcpy(&pcdata, Mem_Resolve(as->regs[PC], as), 4);
    
    printf("Data at PC : 0x%X\n", __builtin_bswap32(pcdata));
    printf("cpsr: 0x%x\n", as->cpsr);
    printf("\nGPIO: 0x%08X", __builtin_bswap32(*(uint32_t*)Mem_Resolve(0xd8000e0, as)));
}

void set_cpsr_flags(struct arm_state *as, int result, long long result_long) 
{
    if (result > 2147483647 || result < -2147483648) {
        as->cpsr = as->cpsr | 0x10000000;
    }

    else {
        as->cpsr = as->cpsr & 0xEFFFFFFF;
    }

    //setting c flag
    if (result_long < 0 && result_long > 4294967295) {
        as->cpsr = as->cpsr | 0x20000000; //setting C to 1 
    }
    else {
        as->cpsr = as->cpsr & 0xDFFFFFFF; //setting C to 0
    }

    //setting z flag and n flag
    if (result < 0) {
        as->cpsr = as->cpsr | 0x80000000; //setting N to 1
        as->cpsr = as->cpsr & 0xBFFFFFFF; //setting Z to 0
        // printf("less\n");
    }
    else if (result == 0) {
        as->cpsr = as->cpsr | 0x40000000; //setting Z to 1
        as->cpsr = as->cpsr & 0x7FFFFFFF; //setting N to 0
    }
    else {
        as->cpsr = as->cpsr & 0xBFFFFFFF; //setting Z to 0
        as->cpsr = as->cpsr & 0x7FFFFFFF; //setting N to 0
    }
}

int check_cpsr_flags(struct arm_state *as, uint32_t iw) 
{
    uint32_t condition = (iw >> 28) & 0b1111;

    //check if the operation can be executed
    switch (condition) {
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

bool iw_is_data_processing_instruction(uint32_t iw)
{
    return ((iw >> 26) & 0b11) == 0;
}

uint32_t _rot(uint32_t value, int shift) {
    if(shift == 0) return value;
    return (value << (32 - shift));
}

uint32_t _rotl(uint32_t value, int shift) {
    if(shift == 0) return value;
    return (value << shift);
}

void execute_data_processing_instruction(struct arm_state *as, uint32_t iw)
{    
    uint32_t rm_value;
    uint32_t i_bit = (iw>>25) & 0b1;
    uint32_t opcode = (iw >> 21) & 0xF;
    uint32_t s_bit = (iw >> 20) & 0b1;
    uint32_t rd = (iw >> 12) & 0xF;
    uint32_t rn = (iw >> 16) & 0xF;
    uint8_t rotate = (iw >> 8) & 0xF;
    int result;
    long long result_long;

    if (i_bit == 1) {
        rm_value = _rot(iw & 0xFF, rotate * 2);
    } else {
        rm_value = _rotl(as->regs[(iw & 0xF)], (iw >> 7) & 0x1F);
    }
    

    switch(opcode) {
        case DP_SUB: //sub
            as->regs[rd] = as->regs[rn] - rm_value;
            result_long = (long long) as->regs[rn] - (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_ADD: //add
            as->regs[rd] = as->regs[rn] + rm_value;
            result_long = (long long) as->regs[rn] + (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_ORR:
            as->regs[rd] = as->regs[rn] ^ rm_value;
            result_long = (long long) as->regs[rn] ^ (long long) rm_value;
            result = as->regs[rd];
        break;
        
        case DP_CMP:
            result = as->regs[rn] - rm_value;
            // printf("cmp %d to %d and the result is: %d\n", as->regs[rn], rm_value, result);
            result_long = (long long) as->regs[rn] - (long long) rm_value;
        break;
        
        case DP_CMN: //cmn
            result = as->regs[rn] + rm_value;
            result_long = (long long) as->regs[rn] + (long long) rm_value;  
        break;

        case DP_MOV: //mov
            as->regs[rd] = rm_value;
            result = as->regs[rd];
            result_long = (long long) as->regs[rd];
        break;

        case DP_MVN: //mvn
            as->regs[rd] = ~(rm_value);
            result = ~(rm_value);
            result_long = (long long) as->regs[rd];
        break;
    }

    if (s_bit == 1) {
        set_cpsr_flags(as, result, result_long);
    }
}

bool iw_is_bx_instruction(uint32_t iw) //
{
    return ((iw >> 4) & 0xFFFFFF) == 0x12FFF1;
}

void execute_bx_instruction(struct arm_state *as, uint32_t iw)
{
    uint32_t rn = iw & 0b1111;

    as->regs[PC] = as->regs[rn];
    
    if (iw & 0b1) {
        as->regs[LR] = as->regs[PC] + 4;
    }
}

bool iw_is_branch_instruction(uint32_t iw) 
{
    return ((iw >> 25) & 0b111) == 0b101;
}

void execute_branch_instruction(struct arm_state *as, uint32_t iw) {
    uint32_t signed_bit = (iw >> 23) & 0b1;
    uint32_t l_bit = (iw >> 24) & 0b1;
    uint32_t offset = iw & 0xFFFFFF;
    uint32_t destination;

    //Check signed bit
    if (signed_bit == 0) {
        destination = offset | 0x00000000;
    }
    else {
        destination = offset | 0xFF000000;
    }

    destination = destination << 2;

    if (l_bit == 1) {  
        as->regs[LR] = as->regs[PC] + 4;
    }
    

    as->regs[PC] += (destination + 8);
}

bool iw_is_single_data_transfer_instruction(uint32_t iw)
{
    return ((iw >> 26) & 0b11) == 0b01;
}

void execute_single_data_transfer_instruction(struct arm_state *as, uint32_t iw)
{
    uint8_t rd = (iw>>12) & 0xF;
    uint32_t rn = (iw>>16) & 0xF;
    uint32_t l_bit = (iw>>20) & 0b1;
    uint32_t w_bit = (iw>>21) & 0b1;
    uint32_t b_bit = (iw>>22) & 0b1;
    uint32_t u_bit = (iw>>23) & 0b1;
    uint32_t p_bit = (iw>>24) & 0b1;
    uint32_t i_bit = (iw>>25) & 0b1;
    uint32_t modified_base_value = as->regs[rn];
    uint16_t offset_value;

    //Check i bit
    if (i_bit == 1) {
        offset_value = as->regs[(iw & 0xF)] << ((iw>>7) & 0b11111); 
    }
    else {
        offset_value = iw & 0xFFF;
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

bool iw_is_push(uint32_t iw)
{
    return (((iw >> 25) & 0b111) == 0b100) && (((iw>>20) & 0b1) == 0b0);
}

bool iw_is_pop(uint32_t iw) 
{
    return (((iw >> 25) & 0b111) == 0b100) && (((iw>>20) & 0b1) == 0b1);
}

void execute_push(struct arm_state *as, uint32_t iw) 
{
    uint32_t register_list = iw & 0xFFFF;
    uint32_t rn = (iw>>16) & 0xF;
    uint32_t w_bit = (iw>>21) & 0b1;
    uint32_t s_bit = (iw>>22) & 0b1;
    uint32_t u_bit = (iw>>23) & 0b1;
    uint32_t p_bit = (iw>>24) & 0b1;
    uint32_t modified_base_value = as->regs[rn];
    uint32_t offset_value = 4;
    int i;

    for (i = (NREGS - 1); i >= 0; i--) {
        if ( ((register_list >> i) & 0b1) == 0b1) {
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
            memcpy(Mem_Resolve(modified_base_value, as), &as->regs[i], 4);
            //Check p bit
            if (p_bit == 0) {
                //Check u bit
                if (u_bit == 1) {
                    modified_base_value += offset_value;
                }
                else {
                    modified_base_value -= offset_value;
                }
            }
            //Check w bit
            if (w_bit == 1) {
                as->regs[rn] = modified_base_value;
            }
        }
    }

    as->regs[PC] += 4;
}

void execute_pop(struct arm_state *as, uint32_t iw) 
{
    uint32_t register_list = iw & 0xFFFF;
    uint32_t rn = (iw>>16) & 0xF;
    uint32_t w_bit = (iw>>21) & 0b1;
    uint32_t s_bit = (iw>>22) & 0b1;
    uint32_t u_bit = (iw>>23) & 0b1;
    uint32_t p_bit = (iw>>24) & 0b1;
    uint32_t modified_base_value = as->regs[rn];
    uint32_t offset_value = 4;
    int i;

    for (i = 0; i < NREGS; i++) {
        if ( ((register_list >> i) & 0b1) == 0b1) {
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
            memcpy(&as->regs[i], Mem_Resolve(modified_base_value, as), 4);
            //Check p bit
            if (p_bit == 0) {
                //Check u bit
                if (u_bit == 1) {
                    modified_base_value += offset_value;
                }
                else {
                    modified_base_value -= offset_value;
                }
            }
            //Check w bit
            if (w_bit == 1) {
                as->regs[rn] = modified_base_value;
            }
        }
    }

    as->regs[PC] += 4;
}

int arm_state_execute_one(struct arm_state *as)
{
    uint32_t iw;
    memcpy(&iw, Mem_Resolve(as->regs[PC], as), 4);
    iw = __builtin_bswap32(iw);
    int ret = 0;

    arm_state_print(as);

    if (iw_is_bx_instruction(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_bx_instruction(as, iw);
        } else {
            as->regs[PC] += 4;
        }
    } else if (iw_is_branch_instruction(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_branch_instruction(as, iw);
        }
        else {
            as->regs[PC] += 4;
        }
    } else if (iw_is_data_processing_instruction(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_data_processing_instruction(as, iw);
        }
        as->regs[PC] += 4;
    } else if (iw_is_single_data_transfer_instruction(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_single_data_transfer_instruction(as, iw);
        } else {
            as->regs[PC] += 4;
        }
    } else if (iw_is_push(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_push(as, iw);
        } else {
            as->regs[PC] += 4;
        }
    } else if (iw_is_pop(iw)) {
        if (check_cpsr_flags(as, iw)) {
            execute_pop(as, iw);
        } else {
            as->regs[PC] += 4;
        }
    } else {
        return -1;
    }
    return ret;
}

int thumb_state_execute_one(struct arm_state *as)
{
    uint16_t iw;
    memcpy(&iw, Mem_Resolve(as->regs[PC], as), 2);
    iw = __builtin_bswap16(iw);
    int ret = 0;

    //printf("\nSP: 0x%X | PC: 0x%X | OP: 0x%X        ", as->regs[SP], as->regs[PC], iw);

    /*
    if (thumb_iw_is_bx_instruction(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            thumb_execute_bx_instruction(as, iw);
        }
    } else if (thumb_iw_is_branch_instruction(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            execute_branch_instruction(as, iw);
        }
        else {
            as->regs[PC] += 4;
        }
    } else if (thumb_iw_is_data_processing_instruction(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            thumb_execute_data_processing_instruction(as, iw);
        }
        as->regs[PC] += 4;
    } else if (thumb_iw_is_single_data_transfer_instruction(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            thumb_execute_single_data_transfer_instruction(as, iw);
        }
    } else if (thumb_iw_is_push(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            thumb_execute_push(as, iw);
        }
    } else if (iw_is_pop(iw)) {
        if (thumb_check_cpsr_flags(as, iw)) {
            thumb_execute_pop(as, iw);
        }
    } else {
        return -1;
    }
    */
    return ret;
}

uint32_t instructions_executed = 0;

uint32_t arm_state_execute(struct arm_state *as)
{
    while (as->regs[PC] != 0xFFFF0588) {
        if(!(as->cpsr & T_FLAG)) {
            int ret = arm_state_execute_one(as);
            if(ret != 0) {
                printf("BAD/UNIMPLEMENTED Instuction!");
                fflush(stdout);
                sleep(1);
                break;
            }
        } else {
            thumb_state_execute_one(as);
        }
        fflush(stdout);
        usleep(200000);
        instructions_executed++;
    }

    return as->regs[0];
}

void execute_program(uint32_t *program, uint32_t program_size) 
{
    struct arm_state *as;

    as = arm_state_new(0xFFFF0000, 0xFFFF0000, program, program_size, 0, 0, 0, 0);
    arm_state_execute(as);

    printf("\n-----------------executing program --------------------\n");
    arm_state_print(as);
    arm_state_free(as);    
}