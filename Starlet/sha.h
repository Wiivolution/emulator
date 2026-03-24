/*
    Starmulator - Low Level Wii IOP Emulator

    sha.h - SHA Engine
    
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

#ifndef _SHA_H_
#define _SHA_H_

struct SHA_REGS {
	uint32_t CTRL;
	uint32_t SRC;
	uint32_t H0;
	uint32_t H1;
	uint32_t H2;
	uint32_t H3;
	uint32_t H4;
};

int SHA_Init(struct arm_state *_as);
int SHA_Deinit();

#endif