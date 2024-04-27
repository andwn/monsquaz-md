ifdef MARSDEV
	MDROOT ?= $(MARSDEV)/m68k-elf
else ifeq ($(shell which m68k-elf-gcc),)
	MDROOT ?= /opt/toolchains/mars/m68k-elf
else
	MDROOT ?= $(subst /bin/m68k-elf-gcc,,$(shell which m68k-elf-gcc))
endif

MDBIN  := $(MDROOT)/bin
MDLIB  := $(MDROOT)/lib
MDINC  := $(MDROOT)/include

TARGET  ?= monsquaz

# m68k GCC and Binutils
CC   = $(MDBIN)/m68k-elf-gcc
AS   = $(MDBIN)/m68k-elf-as
LD   = $(MDBIN)/m68k-elf-ld
NM   = $(MDBIN)/m68k-elf-nm
OBJC = $(MDBIN)/m68k-elf-objcopy

ASMZ80   := bin/sjasm
MDTILER  := bin/mdtiler

# Some files needed are in a versioned directory
CC_VER := $(shell $(CC) -dumpversion)
PLUGIN = $(MDROOT)/m68k-elf/libexec/gcc/m68k-elf/$(CC_VER)/liblto_plugin.so

INCS   = -Isrc
INCS  += -I$(MDROOT)/m68k-elf/lib/gcc/m68k-elf/$(CC_VER)/include
LIBS   = -L$(MDROOT)/m68k-elf/lib/gcc/m68k-elf/$(CC_VER) -lgcc

CCFLAGS  = -m68000 -mshort -Wall -Wextra -pedantic -std=c99 -ffreestanding
ASFLAGS  = -x assembler-with-cpp -m68000 -Wa,--register-prefix-optional,--bitwise-or
LDFLAGS  = -T md.ld -nostdlib

Z80FLAGS = -isrc
EXTRA =

# Generate m68k object target list
SS    = $(wildcard src/*.s)
CS    = $(wildcard src/*.c)
OBJS  = $(SS:.s=.o)
OBJS += $(CS:.c=.o)

SPRS  = $(wildcard res/spr/*.png)
SPRO  = $(SPRS:.png=.spr)
SPROP = $(SPRS:.png=.pat)

BGS   = $(wildcard res/bg/*.png)
BGS  += $(wildcard res/bg2/*.png)
BGO   = $(BGS:.png=.map)
BGOP  = $(BGS:.png=.pat)

ZSRCS = $(wildcard src/*.s80)
ZOBJS = $(ZSRCS:.s80=.bin)

.PHONY: all release debug tools

all: release

release: EXTRA  = -Ofast -fomit-frame-pointer -flto -fuse-linker-plugin
release: tools $(SPRO) $(BGO) $(ZOBJS) $(TARGET).gen $(TARGET).lst

# Gens-KMod, BlastEm and UMDK support GDB tracing, enabled by this target
debug: EXTRA = -g -Og -DDEBUG -DKDEBUG
debug: tools $(SPRO) $(BGO) $(ZOBJS) $(TARGET).gen $(TARGET).lst

tools: $(ASMZ80) $(MDTILER)

# Symbol listings for both CPUs
$(TARGET).lst: $(TARGET).elf
	$(NM) --plugin=$(PLUGIN) -n $< > $@

# m68k stuff

$(TARGET).gen: $(TARGET).elf
	@echo "Stripping ELF header"
	@$(OBJC) -O binary $< temp.bin
	@dd if=temp.bin of=$@ bs=8K conv=sync
	@rm -f temp.bin

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.s
	@echo "AS $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CCFLAGS) $(EXTRA) $(INCS) -c $< -o $@

# z80

%.bin: %.s80
	$(ASMZ80) $(Z80FLAGS) $< $@ $*.lst

# Resources

res/bg/%.map: res/bg/%.png
	$(MDTILER) -o 0x0001 -m "$<" "res/bg/$*.pat" "res/bg/$*.map"

res/bg2/%.map: res/bg2/%.png
	$(MDTILER) -o 0x60EB -m "$<" "res/bg2/$*.pat" "res/bg2/$*.map"

%.spr: %.png
	$(MDTILER) -s "$<" "$@"

# Tools

bin:
	mkdir -p bin

$(ASMZ80): bin
	c++ -w -DMAX_PATH=MAXPATHLEN tools/sjasm/*.cpp -o $(ASMZ80)

$(MDTILER): bin
	cc tools/mdtiler-mod/*.c -o $(MDTILER) -lpng


.PHONY: clean

clean:
	rm -f $(SPRO) $(SPROP) $(BGO) $(BGOP)
	rm -f $(OBJS) $(ZOBJS) src/*.lst
	rm -f $(TARGET).gen $(TARGET).elf $(TARGET).lst
