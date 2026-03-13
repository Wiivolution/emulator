CC = $(PREFIX)as
PREFIX = /opt/devkitpro/devkitARM/bin/arm-none-eabi-
READELF = $(PREFIX)objdump

OBJS = program.elf
PROGS = program

all: ${OBJS}

%.elf: %.s
	@echo "  ASSEMBLE  $<"
	@$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

readelf:
	@$(READELF) -drS program.elf
