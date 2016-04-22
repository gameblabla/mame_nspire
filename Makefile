TARGET = mame

# set this the operating system you're building for
# (actually you'll probably need your own main makefile anyways)
# MAMEOS = msdos
MAMEOS = odx

# extension for executables
# EXE = .exe
EXE =

# CPU core include paths
VPATH=src $(wildcard src/cpu/*)

CC          = gcc
CPP         = gcc
LD          = gcc

MD = @mkdir
RM = @rm -f
CP = @cp
MV = @mv
DEVLIBS =

EMULATOR = $(TARGET)$(EXE)

DEFS = -D__ODX__ -DLSB_FIRST -DALIGN_INTS -DALIGN_SHORTS -DINLINE="static inline" -Dasm="__asm__ __volatile__" -DMAME_UNDERCLOCK -DMAME_FASTSOUND -DENABLE_AUTOFIRE -DBIGCASE

W_OPTS = -Wall -Wno-write-strings -Wno-sign-compare -fpermissive

F_OPTS = -fsigned-char -fdata-sections -ffunction-sections 
CFLAGS = -Os -Isrc -Isrc/$(MAMEOS) -Isrc/zlib $(W_OPTS) $(F_OPTS)

LIB_PATH  = $(TOOLCHAIN)/lib
LIBS      = -lSDL -lm -Wl,--as-needed -Wl,--gc-sections -flto -lz
LDFLAGS = $(CFLAGS) -L$(LIB_PATH) $(LIBS) 

OBJ = obj_$(TARGET)_$(MAMEOS)
OBJDIRS = $(OBJ) $(OBJ)/cpu $(OBJ)/sound $(OBJ)/$(MAMEOS) \
	$(OBJ)/drivers $(OBJ)/machine $(OBJ)/vidhrdw $(OBJ)/sndhrdw \
	$(OBJ)/zlib

all:	maketree $(EMULATOR)

# include the various .mak files
include src/core.mak
include src/sauce.mak
include src/rules.mak
include src/sound.mak
include src/$(MAMEOS)/$(MAMEOS).mak

# combine the various definitions to one
CDEFS = $(DEFS) $(COREDEFS) $(CPUDEFS) $(SOUNDDEFS)

$(EMULATOR): $(COREOBJS) $(OSOBJS) $(DRVOBJS)
	$(LD) $(LDFLAGS) $(COREOBJS) $(OSOBJS) $(LIBS) $(DRVOBJS) -o $@

$(OBJ)/%.o: src/%.c
	@echo Compiling $<...
	$(CC) $(CDEFS) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: src/%.cpp
	@echo Compiling $<...
	$(CPP) $(CDEFS) $(CFLAGS) -fno-rtti -c $< -o $@

$(OBJ)/%.o: src/%.s
	@echo Compiling $<...
	$(CPP) $(CDEFS) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: src/%.S
	@echo Compiling $<...
	$(CPP) $(CDEFS) $(CFLAGS) -c $< -o $@

$(sort $(OBJDIRS)):
	$(MD) $@

maketree: $(sort $(OBJDIRS))

clean:
	$(RM) -r $(OBJ)
	$(RM) $(EMULATOR)
