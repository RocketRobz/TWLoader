#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules


#---------------------------------------------------------------------------------
# External tools
#---------------------------------------------------------------------------------
MAKEROM 	?= makerom
BANNERTOOL 	?= bannertool

#---------------------------------------------------------------------------------
# 3DSX
#---------------------------------------------------------------------------------

COMPILE_3DSX  = 0

#---------------------------------------------------------------------------------
# Generate version number
#---------------------------------------------------------------------------------

VERSION_DIRTY := $(subst v,,$(shell git describe --tags --abbrev=0))
VERSION_PARTS := $(subst ., ,$(VERSION_DIRTY))

VERSION_MAJOR := 4
VERSION_MINOR := 2
VERSION_MICRO := 0

#VERSION_MAJOR := $(word 1, $(VERSION_PARTS))
#VERSION_MINOR := $(word 2, $(VERSION_PARTS))
#VERSION_MICRO := $(word 3, $(VERSION_PARTS))
#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#
# NO_SMDH: if set to anything, no SMDH file is generated.
# ROMFS is the directory which contains the RomFS, relative to the Makefile (Optional)
# APP_TITLE is the name of the app stored in the SMDH file (Optional)
# APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.png
#     - icon.png
#     - <libctru folder>/default_icon.png
#---------------------------------------------------------------------------------
TARGET		:=	TWLoader
BUILD		:=	build
SOURCES		:=	source source/img
DATA		:=	data
INCLUDES	:=	include
#NO_SMDH		:=	1
ROMFS		:=	romfs
APP_TITLE	:= 	TWLoader
APP_DESCRIPTION :=  CTR mode .nds frontend
APP_AUTHOR	:=	Robz8
ICON		:=	app/icon.png
BNR_AUDIO	:=	app/BannerAudio.wav
BNR_IMAGE	:=  app/banner.png
LOGO		:=	app/twlv2-padded.lz11
RSF_FILE	:=	app/build-cia.rsf
#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
DEBUG ?= 0
 ifeq ($(DEBUG), 1)
	CFLAGS	:=	-g
	ASFLAGS	:=	-g
	LDFLAGS	:= 	-g
endif

ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	+=	-Wall -O2 -mword-relocations \
			-fomit-frame-pointer -ffunction-sections \
			-DCOMPILE_3DSX=$(COMPILE_3DSX) -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_MICRO=$(VERSION_MICRO) \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	+=	$(ARCH)
LDFLAGS	+=	-specs=3dsx.specs $(ARCH) -Wl,-Map,$(notdir $(sort $*.map))

LIBS	:= -lsfil -lpng -ljpeg -lz -lsf2d -lcitro3d -lctru -lm -lstdc++



#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB) $(PORTLIBS)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(sort $(wildcard $(dir)/*.c))))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(sort $(wildcard $(dir)/*.cpp))))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(sort $(wildcard $(dir)/*.s))))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(sort $(wildcard $(dir)/*.v.pica))))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(sort $(wildcard $(dir)/*.shlist))))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(sort $(wildcard $(dir)/*.*))))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o)

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES	:=	$(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(ICON)),)
	icons := $(sort $(wildcard *.png))
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all cia

#---------------------------------------------------------------------------------
all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(TARGET).smdh $(TARGET).cia.smdh $(TARGET).cia $(TARGET).elf


#---------------------------------------------------------------------------------
cia: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile cia

#---------------------------------------------------------------------------------
3dsx: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile 3dsx

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(OUTPUT).cia $(OUTPUT).3dsx

3dsx: $(OUTPUT).3dsx

cia: $(OUTPUT).cia

ifeq ($(strip $(NO_SMDH)),)
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(OUTPUT).smdh
$(OUTPUT).smdh	:	$(TOPDIR)/Makefile
else
$(OUTPUT).3dsx	:	$(OUTPUT).elf
endif

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT).elf	:	$(OFILES)

$(OUTPUT).smdh	:	$(APP_ICON)
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)"  -p "$(APP_AUTHOR)" -i $(APP_ICON) -o $@
	@echo "built ... $(notdir $@)"

$(OUTPUT).cia.smdh	:	$(APP_ICON)
	@bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_TITLE)"  -p "$(APP_AUTHOR)" -i $(APP_ICON) -o $@
	@echo "built ... $(notdir $@)"
	
$(OUTPUT).cia	:	$(OUTPUT).elf $(OUTPUT).cia.smdh $(TARGET).bnr
	@makerom	-f cia -target t -exefslogo -o $@ \
				-elf $(OUTPUT).elf -rsf $(TOPDIR)/$(RSF_FILE) \
				-logo "$(TOPDIR)/$(LOGO)" -DAPP_ROMFS="$(TOPDIR)/$(ROMFS)" -major $(VERSION_MAJOR) \
				-minor $(VERSION_MINOR) -micro $(VERSION_MICRO) -DAPP_VERSION_MAJOR="$(VERSION_MAJOR)" \
				-banner $(TARGET).bnr \
				-icon $(OUTPUT).cia.smdh
	@echo "built ... $(notdir $@)"

$(TARGET).bnr	:	$(TOPDIR)/$(BNR_IMAGE) $(TOPDIR)/$(BNR_AUDIO)
	@bannertool	makebanner -o $@ -ci $(TOPDIR)/$(BNR_IMAGE) -ca $(TOPDIR)/$(BNR_AUDIO)
	@echo "built ... $@"

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.jpeg.o %_jpeg.h :	%.jpeg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.png.o	%_png.h :	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $(sort $*.shbin))
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $(sort $*.shbin.o)
endef

%.shbin.o %_shbin.h : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o %_shbin.h : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o %_shbin.h : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)/$(file)))

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------