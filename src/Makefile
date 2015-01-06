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
sources := loop32.S loop64.S hammertest.c
targets := hammertest32 hammertest64 benchmark32 benchmark64

# gmake magic
.PHONY: default all clean generate

#targets
default: all

pagemap_read32.o: pagemap_read.c pagemap_read.h
	${CC} -m32 ${CFLAGS} -o pagemap_read32.o -c pagemap_read.c

pagemap_read64.o: pagemap_read.c pagemap_read.h
	${CC} -m64 ${CFLAGS} -o pagemap_read64.o -c pagemap_read.c

loop32.o: loop32.c
	${CC} -m32 ${CFLAGS} -c loop32.c

loop64.o: loop64.c
	${CC} -m64 ${CFLAGS} -c loop64.c

hammertest32.o: hammertest.c
	${CC} -m32 ${CFLAGS} -o hammertest32.o -c hammertest.c

hammertest64.o: hammertest.c
	${CC} -m64 ${CFLAGS} -o hammertest64.o -c hammertest.c

hammertest32: hammertest32.o loop32.o pagemap_read32.o
	${CC} -m32 ${LDFLAGS} -o hammertest32 loop32.o pagemap_read32.o hammertest32.o

hammertest64: hammertest64.o loop64.o pagemap_read64.o
	${CC} -m64 ${LDFLAGS} -o hammertest64 loop64.o pagemap_read64.o hammertest64.o

bench32.o: bench32.S
	${CC} -m32 ${ASFLAGS} -c bench32.S

bench64.o: bench64.S
	${CC} -m64 ${ASFLAGS} -c bench64.S

benchmark32.o: benchmark.c
	${CC} -m32 ${CFLAGS} -o benchmark32.o -c benchmark.c

benchmark32: benchmark32.o bench32.o
	${CC} -m32 ${LDFLAGS} -o benchmark32 bench32.o benchmark32.o

benchmark64.o: benchmark.c
	${CC} -m64 ${CFLAGS} -o benchmark64.o -c benchmark.c

benchmark64: benchmark64.o bench64.o
	${CC} -m64 ${LDFLAGS} -o benchmark64 bench64.o benchmark64.o

all: $(targets)

clean:
	$(RM) $(targets) *.o
