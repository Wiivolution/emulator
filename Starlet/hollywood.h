/*
    Starmulator - Low Level Wii IOP Emulator

    hollywood.h - Hollywood registers
    
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

#ifndef __HOLLYWOOD_H__
#define __HOLLYWOOD_H__

// INT : Interface
// STS : Status?
// EN  : Enable 

#define HW_IPC_PPCMSG        0x00000000
#define HW_IPC_PPCCTRL       0x00000004
#define HW_IPC_ARMMSG        0x00000008
#define HW_IPC_ARLCTRL       0x0000000C

#define HW_TIMER             0x00000010
#define HW_ALARM             0x00000014

#define HW_VI1CFG            0x00000018
#define HW_VIDIM             0x0000001C
#define HW_VISOLID           0x00000024

#define HW_PPCIRQFLAG        0x00000030
#define HW_PPCIRQMASK        0x00000034
#define HW_ARMIRQFLAG        0x00000038
#define HW_ARMIRQMASK        0x0000003C
#define HW_ARMFIQMASK        0x00000040
#define HW_IOPINTPPC         0x00000044
#define HW_WDGINTSTS         0x00000048
#define HW_WDGCFG            0x0000004C
#define HW_DMAADRINTSTS      0x00000050
#define HW_CPUADRINTSTS      0x00000054
#define HW_DBGINTSTS         0x00000058
#define HW_DBGINTEN          0x0000005C
#define HW_SRNPROT           0x00000060
#define HW_AHBPROT           0x00000064

#define HW_BOOT0             0x0000018C

#endif
