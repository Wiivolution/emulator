PROGS = armemu 

all : ${PROGS}

%.o : %.bin
	$(bin2o) -i $< -o $@

armemu : armemu.c
	gcc -o armemu armemu.c

test : armemu
	./armemu

clean:
	rm -rf armemu ${OBJS}