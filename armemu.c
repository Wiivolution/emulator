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
#include <time.h>

#define NANO_PER_SEC 1000000000.0

#include "armemu.h"

uint8_t* read_mem(uint32_t location, struct arm_state *as);

struct arm_state *arm_state_new(size_t mem_size, size_t program_loc, size_t entrypoint,
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

    as->stack = (uint8_t *) malloc(STACK_SIZE);

    if (as->stack == NULL) {
        printf("malloc() failed, exiting.\n");
        exit(-1);
    }

    as->memory = (uint8_t *) malloc(mem_size);
    if(as->memory == NULL) {
        printf("Failed memory allocation! Requested size : 0x%8X", mem_size);
        exit(-1);
    }
    as->mem_size = mem_size;
    if(mem_size < program_loc + program_size) {
        printf("Can't write program to mem!");
        exit(-1);
    }
    
    memcpy(&as->memory[program_loc], program, program_size);
    
    // Initialize all registers to zero.
    as->cpsr = 0;
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->regs[PC] = (unsigned int) entrypoint;
    as->regs[SP] = (unsigned int) as->stack + STACK_SIZE;
    
    as->regs[0] = arg0;
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;

    // Inititalize all the counts to 0
    as->computational_count = 0;
    as->memory_count = 0;
    as->branch_count = 0;

    return as;
}

void arm_state_free(struct arm_state *as)
{
    free(as->stack);
    free(as->memory);
    free(as);
}

void arm_state_print(struct arm_state *as, uint32_t emu_result)
{
    int i;
    
    printf("stack size = %d\n", STACK_SIZE);
    printf("Register values after execution:\n");
    for (i = 0; i < NREGS; i++) {
        printf("r%d = (%X) %d\n", i, as->regs[i], (int) as->regs[i]);
    }
    uint32_t pcdata = 0;
    memcpy(&pcdata, read_mem(as->regs[PC], as), 4);
    
    printf("Data at PC : 0x%X\n", __builtin_bswap32(pcdata));
    printf("cpsr: 0x%x\n", as->cpsr);
    printf("Total Instructions Executed: %d\n", (as->computational_count+as->memory_count+as->branch_count));
    printf("Total Computational Instructions Executed: %d\n", as->computational_count);
    printf("Total Memory Instructions Executed: %d\n", as->memory_count);
    printf("Total Branch Instructions Executed: %d\n", as->branch_count);
    printf("ARM Emulator Result: %d\n", emu_result);

}

uint8_t* read_mem(uint32_t location, struct arm_state *as) {
    return &as->memory[location - as->mem_offset];
}

void set_cpsr_flags(struct arm_state *as, int result, long long result_long) 
{
    //setting v flag
    if (result > 2147483647 || result < -2147483648) {
        as->cpsr = as->cpsr | 0x10000000; //setting v to 1
        // printf("v here\n");

    }
    else {
        as->cpsr = as->cpsr & 0xEFFFFFFF; //setting v to 0
        // printf("c here\n");
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
        case 0: //EQ
            return ((as->cpsr >> 30) & 0b1) == 1;
        case 1: //NE
            return ((as->cpsr >> 30) & 0b1) == 0;
        case 11: //LT
            return ((as->cpsr >> 28) & 0b1) != ((as->cpsr >> 31) & 0b1);
        case 12: //GT
            return (((as->cpsr >> 30) & 0b1) == 0) && (((as->cpsr >> 31) & 0b1) == ((as->cpsr >> 28) & 0b1));
        case 14: //AL
            return true;
    }
}

bool iw_is_data_processing_instruction(uint32_t iw)
{
    return ((iw >> 26) & 0b11) == 0;
}

void execute_data_processing_instruction(struct arm_state *as, uint32_t iw)
{    
    uint32_t rm_value;
    uint32_t i_bit = (iw>>25) & 0b1;
    uint32_t opcode = (iw >> 21) & 0xF;
    uint32_t s_bit = (iw >> 20) & 0b1;
    uint32_t rd = (iw >> 12) & 0xF;
    uint32_t rn = (iw >> 16) & 0xF;
    int result;
    long long result_long;

    if (i_bit == 1) {
        rm_value = iw & 0xFF;
    }
    else {
        rm_value = as->regs[(iw & 0xF)];
    }

    switch(opcode) {
        case 2: //sub
            as->regs[rd] = as->regs[rn] - rm_value;
            result_long = (long long) as->regs[rn] - (long long) rm_value;
            result = as->regs[rd];
            break;
        case 4: //add
            as->regs[rd] = as->regs[rn] + rm_value;
            result_long = (long long) as->regs[rn] + (long long) rm_value;
            result = as->regs[rd];
            break;
        case 10: //cmp
            result = as->regs[rn] - rm_value;
            // printf("cmp %d to %d and the result is: %d\n", as->regs[rn], rm_value, result);
            result_long = (long long) as->regs[rn] - (long long) rm_value;
            break;
        case 11: //cmn
            result = as->regs[rn] + rm_value;
            result_long = (long long) as->regs[rn] + (long long) rm_value;  
            break;
        case 13: //mov
            as->regs[rd] = rm_value;
            result = as->regs[rd];
            result_long = (long long) as->regs[rd];
            break;
        case 15: //mvn
            as->regs[rd] = ~(rm_value);
            result = ~(rm_value);
            result_long = (long long) as->regs[rd];
            break;
    }

    if (s_bit == 1) {
        set_cpsr_flags(as, result, result_long);
    }
}

bool iw_is_bx_instruction(uint32_t iw)
{
    return ((iw >> 4) & 0xFFFFFF) == 0b000100101111111111110001;
}

void execute_bx_instruction(struct arm_state *as, uint32_t iw)
{
    uint32_t rn = iw & 0b1111;

    as->regs[PC] = as->regs[rn];
}

bool iw_is_branch_instruction(uint32_t iw) 
{
    return ((iw >> 25) & 0b111) == 0b101;
}

int execute_branch_instruction(struct arm_state *as, uint32_t iw) {
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

    if(as->regs[PC] + destination + 8 < (as->mem_size + as->mem_offset)) {
        as->regs[PC] += (destination + 8);
    } else {
        printf("Invalid branch!\n Abort...");
        return -1;
    }
    return 0;
}

bool iw_is_single_data_transfer_instruction(uint32_t iw)
{
    return ((iw >> 26) & 0b11) == 0b01;
}

void execute_single_data_transfer_instruction(struct arm_state *as, uint32_t iw)
{
    uint32_t rd = (iw>>12) & 0xF;
    uint32_t rn = (iw>>16) & 0xF;
    uint32_t l_bit = (iw>>20) & 0b1;
    uint32_t w_bit = (iw>>21) & 0b1;
    uint32_t b_bit = (iw>>22) & 0b1;
    uint32_t u_bit = (iw>>23) & 0b1;
    uint32_t p_bit = (iw>>24) & 0b1;
    uint32_t i_bit = (iw>>25) & 0b1;
    uint32_t modified_base_value = as->regs[rn];
    uint32_t offset_value;

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


    printf("\n Modified base value: %d", modified_base_value);
    //Check b bit
    if (b_bit == 1) {
        // Check l bit
        if (l_bit == 1) {
            memcpy(&as->regs[rd], read_mem(modified_base_value, as), 4); //ldrb
        }
    }
    else {
        //Check l bit
        if (l_bit == 1) {
            memcpy(&as->regs[rd], read_mem(modified_base_value, as), 4); //ldr
        }
        else {
            memcpy(read_mem(modified_base_value, as), &as->regs[rd], 4);
            *read_mem(modified_base_value, as) = as->regs[rd]; //str
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
            *read_mem(modified_base_value, as) = as->regs[i];
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
            memcpy(&as->regs[i], read_mem(modified_base_value, as), 4);
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
    memcpy(&iw, read_mem(as->regs[PC], as), 4);
    iw = __builtin_bswap32(iw);
    int ret = 0;

    printf("\nr3: %d | PC: %d | OP: 0x%X        ", as->regs[3], as->regs[PC], iw);

    if (iw_is_bx_instruction(iw)) {
        as->branch_count++;
        if (check_cpsr_flags(as, iw)) {
            execute_bx_instruction(as, iw);
        }
    } else if (iw_is_branch_instruction(iw)) {
        as->branch_count++;
        if (check_cpsr_flags(as, iw)) {
            ret = execute_branch_instruction(as, iw);
        }
        else {
            as->regs[PC] += 4;
        }
    } else if (iw_is_data_processing_instruction(iw)) {
        as->computational_count++;
        if (check_cpsr_flags(as, iw)) {
            execute_data_processing_instruction(as, iw);
        }
        as->regs[PC] += 4;
    } else if (iw_is_single_data_transfer_instruction(iw)) {
        as->memory_count++;
        if (check_cpsr_flags(as, iw)) {
            execute_single_data_transfer_instruction(as, iw);
        }
    } else if (iw_is_push(iw)) {
        as->memory_count++;
        if (check_cpsr_flags(as, iw)) {
            execute_push(as, iw);
        }
    } else if (iw_is_pop(iw)) {
        as->memory_count++;
        if (check_cpsr_flags(as, iw)) {
            execute_pop(as, iw);
        }
    } else {
        return -1;
    }
    return ret;
}

uint32_t arm_state_execute(struct arm_state *as)
{
    while (as->regs[PC] < (as->mem_size + as->mem_offset)) {
        int ret = arm_state_execute_one(as);
        if(ret != 0) {
            printf("BAD/UNIMPLEMENTED Instuction!");
            fflush(stdout);
            break;
        }
        fflush(stdout);
        usleep(500000);
    }

    return as->regs[0];
}

void execute_program(uint32_t *program, uint32_t program_size) 
{
    struct arm_state *as;
    uint32_t assembler_result, emu_result;

    struct timespec start, end;

    as = arm_state_new(8192, 0, 0xFFFF0040, program, program_size, 0, 0, 0, 0);
    as->mem_offset = 0xFFFF0000;
    arm_state_print(as, emu_result);
    clock_gettime(CLOCK_MONOTONIC, &start);
    emu_result = arm_state_execute(as);
    clock_gettime(CLOCK_MONOTONIC, &end);

    uint32_t time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);

    printf("\n-----------------executing program --------------------\n");
    arm_state_print(as, emu_result);
    printf("\nExecution time : %dns", time_spent);
    //printf("\nMIPS : %lld\n\n", time_spent / (as->computational_count+as->memory_count+as->branch_count) / 1000);
    arm_state_free(as);    
}

int main(int argc, char **argv)
{

    FILE* fd = fopen("program.bin", "rb");
    if(!fd) {
        printf("\nError opening file!\n");
        return -1;
    }
    fseek(fd, 0, SEEK_END);
    uint32_t size = ftell(fd);
    if(!size) {
        printf("\nFile size is 0! %d\n", size);
        return -1;
    }
    fseek(fd, 0, SEEK_SET);
    uint32_t program[size / 4];
    fread(program, 1, size, fd);
    printf("\n Size = %d\n", size);

    execute_program(program, size);
    
    return 0;
}
