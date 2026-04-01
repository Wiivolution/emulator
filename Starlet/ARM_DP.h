/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_DP.h - Data-processing Unit (ALU)
    
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

#ifndef _ARM_DP_H_
#define _ARM_DP_H_

// ARM :

bool ARM_DP_Is_DataProcessing(uint32_t instr);
void ARM_DP_Execute(struct arm_state* as, uint32_t instr);

// THUMB :

void THUMB_DP_Execute_Shift(struct arm_state* as, uint16_t instr);
void THUMB_DP_Execute_immop(struct arm_state* as, uint16_t instr);
void THUMB_DP_Execute_Special(struct arm_state* as, uint16_t instr);
void THUMB_DP_Execute_Regs(struct arm_state* as, uint16_t instr);
void THUMB_add_sp_pc_imm(struct arm_state* as, uint16_t instr);

#endif