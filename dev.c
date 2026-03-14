/*
    Starmulator - Low Level Wii IOP Emulator

    dev.c - Devices MMIO & Management
    
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "dev.h"

device* Dev_Table = NULL;
uint32_t Devices_count = 0;

int Dev_AddDevice(device* dev) {
    Devices_count++;
    device* temp = malloc(sizeof(device) * Devices_count);
    if(temp == NULL) {
        printf("\nDev mem allocation failed!");
        Devices_count--;
        return -1;
    }
    if(Dev_Table != NULL) {
        memcpy(temp, Dev_Table, sizeof(device) * (Devices_count - 1));
        free(Dev_Table);
    }
    memcpy(temp + sizeof(device) * (Devices_count - 1), dev, sizeof(device));
    Dev_Table = temp;
    return 0;
}

uint8_t* Dev_ResolveRegs(uint32_t addr) {
    for(int i = 0; i < Devices_count; i++) {
        if(addr >= Dev_Table[i].regs_addr &&
           addr <= Dev_Table[i].regs_addr + Dev_Table[i].regs_length)
        {
            return (uint8_t*)(Dev_Table[i].ptr + (addr - Dev_Table[i].regs_addr));
        }
    }
    return NULL;
}