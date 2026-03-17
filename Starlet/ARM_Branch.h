/*
    Starmulator - Low Level Wii IOP Emulator

    ARM_Branch.h - Branch Handler
    
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

#ifndef _ARM_BRANCH_H_
#define _ARM_BRANCH_H_

bool ARM_BR_Is_BX_Instr(uint32_t instr);
void ARM_BR_Execute_BX(struct arm_state *as, uint32_t instr);
bool ARM_BR_Is_Branch_Instr(uint32_t instr);
void ARM_BR_Execute_Branch(struct arm_state *as, uint32_t instr);

#endif