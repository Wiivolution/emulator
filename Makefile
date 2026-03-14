PROGS = armemu 

CFILES = armcore.c memory.c dev.c aes.c main.c

all : ${PROGS}

armemu : ${CFILES}
	gcc -o armemu ${CFILES}

test : armemu
	./armemu

clean:
	rm -rf armemu ${OBJS}