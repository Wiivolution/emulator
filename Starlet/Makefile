PROGS = armemu 

CFILES = ARM_Coproc.c ARM_Core.c ARM_DP.c ARM_DT.c ARM_Branch.c memory.c dev.c aes.c sha.c nand.c main.c

all : ${PROGS}

armemu : ${CFILES}
	gcc -o armemu -lcrypto ${CFILES}

test : armemu
	./armemu

clean:
	rm -rf armemu ${OBJS}