/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_DT.h - Data-transfer Unit
    
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

#ifndef _ARM_DT_H_
#define _ARM_DT_H_

// ARM :

bool ARM_DT_Is_SDT_Instr(uint32_t instr);
void ARM_DT_Execute_SDT_Instr(struct arm_state *as, uint32_t instr);
bool ARM_DT_Is_Push_Instr(uint32_t instr);
void ARM_DT_Execute_Push(struct arm_state *as, uint32_t instr);
bool ARM_DT_Is_Pop_Instr(uint32_t instr);
void ARM_DT_Execute_Pop(struct arm_state *as, uint32_t instr);

// THUMB :

void THUMB_DT_Execute_Push(struct arm_state *as, uint16_t instr);
void THUMB_DT_Execute_Pop(struct arm_state *as, uint16_t instr);
void THUMB_DT_Execute_LD_Lit(struct arm_state *as, uint16_t instr);
void THUMB_DT_Execute_Im(struct arm_state *as, uint16_t instr);
void THUMB_DT_Execute_SP(struct arm_state *as, uint16_t instr);

#endif