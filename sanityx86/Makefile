# tools
CC := gcc
RM := rm -f
CP := cp

# flags
CFLAGS := -O1
ASFLAGS :=
LDFLAGS :=
LDLIBS :=

# sources
sources := loop32.S loop64.S sanity.c
targets := sanity32 sanity64

# gmake magic
.PHONY: default all clean generate

#targets
default: all

loop32.o: loop32.S
	${CC} -m32 ${ASFLAGS} -c loop32.S

loop64.o: loop64.S
	${CC} -m64 ${ASFLAGS} -c loop64.S

sanity32.o: sanity.c
	${CC} -m32 ${CFLAGS} -o sanity32.o -c sanity.c

sanity64.o: sanity.c
	${CC} -m64 ${CFLAGS} -o sanity64.o -c sanity.c

sanity32: sanity32.o loop32.o
	${CC} -m32 ${LDFLAGS} -o sanity32 loop32.o sanity32.o

sanity64: sanity64.o loop64.o
	${CC} -m64 ${LDFLAGS} -o sanity64 loop64.o sanity64.o

all: $(targets)

clean: 
	$(RM) $(targets) *.o

