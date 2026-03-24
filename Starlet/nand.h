/*
    Starmulator - Low Level Wii IOP Emulator

    nand.h - NAND Management
    
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

#ifndef _NAND_H_
#define _NAND_H_

typedef struct NAND_REGS {
	uint32_t CTRL;
    uint32_t CONFIG;
	uint32_t ADDR1;
	uint32_t ADDR2;
	uint32_t DATABUF;
	uint32_t ECCBUF;
    uint32_t UNK;
} NAND_REGS;

int NAND_Init();
int NAND_Deinit();

#endif