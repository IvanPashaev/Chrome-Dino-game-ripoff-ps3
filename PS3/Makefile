#---------------------------------------------------------------------------------
# Chrome Dino Ripoff (PS3) - PSL1GHT build
#
#   make        -> ChromeDinoPS3.self (+ .fake.self for ps3load)
#   make pkg    -> ChromeDinoPS3.pkg (installable package)
#   make run    -> ps3load ChromeDinoPS3.self
#   make clean  -> remove build artifacts
#
# Requires the PS3 toolchain from /usr/local/ps3dev (PS3DEV / PSL1GHT).
# raylib is taken from $(PORTLIBS) and is the PSL1GHT/RSX build of raylib.
#---------------------------------------------------------------------------------
.SUFFIXES:

ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

#---------------------------------------------------------------------------------
# Application metadata (used by ppu_rules for SELF/PKG generation)
#---------------------------------------------------------------------------------
TITLE		:=	Chrome Dino Ripoff
APPID		:=	CDGR00001
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000

include $(PSL1GHT)/ppu_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories with assets embedded into the executable
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	ChromeDinoPS3
BUILD		:=	build
SOURCES		:=	.
DATA		:=	sprites music data
INCLUDES	:=	.
SHADERS		:=

#---------------------------------------------------------------------------------
# Extra files copied into the package root (ICON0.PNG, PIC1.PNG, ...)
# Uncomment after creating the files in pkgfiles/
#---------------------------------------------------------------------------------
#PKGFILES	:=	$(CURDIR)/pkgfiles
#ICON0		:=	$(CURDIR)/pkgfiles/ICON0.PNG

#---------------------------------------------------------------------------------
# Libraries required to link the raylib RSX/OpenGL ES backend
# NOTE: the PSL1GHT raylib port has no raudio module, so audio is handled
#       by the PSL1GHT sound libraries (MP3/OGG decoding + SPU mixer)
#---------------------------------------------------------------------------------
LIBS		:=	-lraylib -lEGL -lGL -lrsx -lgcm_sys -lsysutil -lio \
				-lstdc++ -lsimdmath -lrt -llv2 \
				-lsysmodule -laudioplayer -lmpg123 -logg \
				-lspu_sound -laudio -lm

#---------------------------------------------------------------------------------
# Code generation options
#---------------------------------------------------------------------------------
CFLAGS		=	-O2 -Wall -std=gnu99 -mcpu=cell -mtune=cell \
				$(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)
LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

LIBDIRS		:=	$(PORTLIBS)

#---------------------------------------------------------------------------------
# Nothing below this line should need editing
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(foreach dir,$(SHADERS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)
export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# Object file lists
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.png $(dir)/*.mp3 $(dir)/*.bin)))

#---------------------------------------------------------------------------------
# Use CXX for linking C++ projects, CC for pure C projects
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# Include and library search paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD) -I$(PORTLIBS)/include
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib

.PHONY: all clean pkg run

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).fake.self \
		$(OUTPUT).pkg $(OUTPUT).gnpdrm.pkg EBOOT.BIN

#---------------------------------------------------------------------------------
pkg: $(BUILD) $(OUTPUT).pkg

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------------
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

# main.c includes generated asset headers, so build embedded files first
main.o: jump.mp3.o Home_Screen.png.o cave_Background.png.o \
	dino_sprite.png.o object.png.o

#---------------------------------------------------------------------------------
# Embedded assets (PNG textures, MP3 music)
# bin2o generates <name>_<ext>.o plus <name>_<ext>.h with the byte arrays
#---------------------------------------------------------------------------------
%.png.o: %.png
	@echo $(notdir $<)
	@$(bin2o)

%.mp3.o: %.mp3
	@echo $(notdir $<)
	@$(bin2o)

%.bin.o: %.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
