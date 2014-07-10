# tools
CC := gcc
RM := rm -f
CP := cp

# flags
CFLAGS := -O1 -m32
ASFLAGS := -m32
LDFLAGS := -m32
LDLIBS :=

# sources
sources := loop.S sanity.c
targets := sanity

# gmake magic
.PHONY: default all clean generate

#targets
default: all
all: $(targets)

clean: 
	$(RM) $(targets) *.o

#dependencies
sanity: loop.o sanity.o
loop.o: loop.S
sanity.o: sanity.c
