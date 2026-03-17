CC = $(PREFIX)as
PREFIX = /opt/devkitpro/devkitARM/bin/arm-none-eabi-
OBJDUMP = $(PREFIX)objdump
OBJCOPY = $(PREFIX)objcopy

OBJS = program.elf AES-test.elf
PROGS = program.bin AES-test.bin

all: ${OBJS} ${PROGS}

%.elf: %.s
	@echo "  ASSEMBLE  $<"
	@$(CC) -mbig-endian -c $< -o $@

%.bin: %.elf
	@$(OBJCOPY) -O binary $< $@

readelf:
	@$(OBJDUMP) -drS AES-test.elf
