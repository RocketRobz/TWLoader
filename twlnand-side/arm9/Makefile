#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# MAXMOD_SOUNDBANK contains a directory of music and sound effect files
#---------------------------------------------------------------------------------
TARGET		:=	twlapp-hb
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include
DATA		:=	../data
IMAGES		:=	../BootSplash
MUSIC       :=  ../music

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2\
 		-march=armv5te -mtune=arm946e-s -fomit-frame-pointer\
		-ffast-math \
		$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS	:=	-lfat -lmm9 -lnds9
 
 
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBNDS)
 
#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
PNGFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*))) soundbank.bin
BMPFILES	:=	$(foreach dir,$(IMAGES),$(notdir $(wildcard $(dir)/*.bmp)))
 
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

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(PNGFILES:.png=.o) $(BMPFILES:.bmp=.o) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)


export OUTPUT	:=	$(CURDIR)/$(TARGET)
 
.PHONY: $(BUILD) clean

all : $(BUILD)
 
#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds

#---------------------------------------------------------------------------------
else
 
#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# graphics
#---------------------------------------------------------------------------------
#Cartridge Wait Prompt (bottom screen)
CartPrompt01.s			:	../$(IMAGES)/CartPrompt01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt02.s			:	../$(IMAGES)/CartPrompt02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt03.s			:	../$(IMAGES)/CartPrompt03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt04.s			:	../$(IMAGES)/CartPrompt04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

#DSi Cartridge Wait Prompt (bottom screen)
DSiCartPrompt01.s			:	../$(IMAGES)/DSiCartPrompt01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSiCartPrompt02.s			:	../$(IMAGES)/DSiCartPrompt02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSiCartPrompt03.s			:	../$(IMAGES)/DSiCartPrompt03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSiCartPrompt04.s			:	../$(IMAGES)/DSiCartPrompt04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	
#Cartridge Wait Prompt (bottom screen, no H&S message)
CartPromptNoHS01.s			:	../$(IMAGES)/CartPromptNoHS01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPromptNoHS02.s			:	../$(IMAGES)/CartPromptNoHS02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPromptNoHS03.s			:	../$(IMAGES)/CartPromptNoHS03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPromptNoHS04.s			:	../$(IMAGES)/CartPromptNoHS04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	

# Main Boot Splash sequence
Bot00.s			:	../$(IMAGES)/Bot00.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot01.s			:	../$(IMAGES)/Bot01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot02.s			:	../$(IMAGES)/Bot02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03.s			:	../$(IMAGES)/Bot03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04.s			:	../$(IMAGES)/Bot04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05.s			:	../$(IMAGES)/Bot05.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot06.s			:	../$(IMAGES)/Bot06.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot07.s			:	../$(IMAGES)/Bot07.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot08.s			:	../$(IMAGES)/Bot08.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot09.s			:	../$(IMAGES)/Bot09.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot10.s			:	../$(IMAGES)/Bot10.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top00.s			:	../$(IMAGES)/Top00.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top01.s			:	../$(IMAGES)/Top01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top02.s			:	../$(IMAGES)/Top02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top03.s			:	../$(IMAGES)/Top03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top04.s			:	../$(IMAGES)/Top04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top05.s			:	../$(IMAGES)/Top05.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top06.s			:	../$(IMAGES)/Top06.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top07.s			:	../$(IMAGES)/Top07.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top08.s			:	../$(IMAGES)/Top08.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top09.s			:	../$(IMAGES)/Top09.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top10.s			:	../$(IMAGES)/Top10.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top11.s			:	../$(IMAGES)/Top11.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top12.s			:	../$(IMAGES)/Top12.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top13.s			:	../$(IMAGES)/Top13.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top14.s			:	../$(IMAGES)/Top14.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top15.s			:	../$(IMAGES)/Top15.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top16.s			:	../$(IMAGES)/Top16.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top17.s			:	../$(IMAGES)/Top17.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top18.s			:	../$(IMAGES)/Top18.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top19.s			:	../$(IMAGES)/Top19.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top20.s			:	../$(IMAGES)/Top20.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top21.s			:	../$(IMAGES)/Top21.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top22.s			:	../$(IMAGES)/Top22.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top23.s			:	../$(IMAGES)/Top23.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top24.s			:	../$(IMAGES)/Top24.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top25.s			:	../$(IMAGES)/Top25.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top26.s			:	../$(IMAGES)/Top26.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top27.s			:	../$(IMAGES)/Top27.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top28.s			:	../$(IMAGES)/Top28.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top29.s			:	../$(IMAGES)/Top29.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top30.s			:	../$(IMAGES)/Top30.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top31.s			:	../$(IMAGES)/Top31.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top32.s			:	../$(IMAGES)/Top32.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top33.s			:	../$(IMAGES)/Top33.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top34.s			:	../$(IMAGES)/Top34.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top35.s			:	../$(IMAGES)/Top35.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top36.s			:	../$(IMAGES)/Top36.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top37.s			:	../$(IMAGES)/Top37.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

# DSi BootSplash. Used if TWL mode detected
DSi01.s			:	../$(IMAGES)/DSi01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi02.s			:	../$(IMAGES)/DSi02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi03.s			:	../$(IMAGES)/DSi03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi04.s			:	../$(IMAGES)/DSi04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi05.s			:	../$(IMAGES)/DSi05.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi06.s			:	../$(IMAGES)/DSi06.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi07.s			:	../$(IMAGES)/DSi07.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi08.s			:	../$(IMAGES)/DSi08.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi09.s			:	../$(IMAGES)/DSi09.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi10.s			:	../$(IMAGES)/DSi10.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi11.s			:	../$(IMAGES)/DSi11.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi12.s			:	../$(IMAGES)/DSi12.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi13.s			:	../$(IMAGES)/DSi13.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi14.s			:	../$(IMAGES)/DSi14.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi15.s			:	../$(IMAGES)/DSi15.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi16.s			:	../$(IMAGES)/DSi16.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi17.s			:	../$(IMAGES)/DSi17.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi18.s			:	../$(IMAGES)/DSi18.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi19.s			:	../$(IMAGES)/DSi19.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi20.s			:	../$(IMAGES)/DSi20.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi21.s			:	../$(IMAGES)/DSi21.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi22.s			:	../$(IMAGES)/DSi22.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi23.s			:	../$(IMAGES)/DSi23.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi24.s			:	../$(IMAGES)/DSi24.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi25.s			:	../$(IMAGES)/DSi25.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi26.s			:	../$(IMAGES)/DSi26.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi27.s			:	../$(IMAGES)/DSi27.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi28.s			:	../$(IMAGES)/DSi28.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi29.s			:	../$(IMAGES)/DSi29.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi30.s			:	../$(IMAGES)/DSi30.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi31.s			:	../$(IMAGES)/DSi31.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi32.s			:	../$(IMAGES)/DSi32.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi33.s			:	../$(IMAGES)/DSi33.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi34.s			:	../$(IMAGES)/DSi34.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

DSi35.s			:	../$(IMAGES)/DSi35.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

#DSi version of bottom screen
BotDSi00.s			:	../$(IMAGES)/BotDSi00.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi01.s			:	../$(IMAGES)/BotDSi01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi02.s			:	../$(IMAGES)/BotDSi02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi03.s			:	../$(IMAGES)/BotDSi03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi04.s			:	../$(IMAGES)/BotDSi04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi05.s			:	../$(IMAGES)/BotDSi05.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi06.s			:	../$(IMAGES)/BotDSi06.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi07.s			:	../$(IMAGES)/BotDSi07.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi08.s			:	../$(IMAGES)/BotDSi08.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi09.s			:	../$(IMAGES)/BotDSi09.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi10.s			:	../$(IMAGES)/BotDSi10.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

BotDSi11.s			:	../$(IMAGES)/BotDSi11.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
soundbank.bin : $(AUDIOFILES)
#---------------------------------------------------------------------------------
	@mmutil $^ -d -osoundbank.bin -hsoundbank.h
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)
	
#---------------------------------------------------------------------------------
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#---------------------------------------------------------------------------------
%.s %.h   : %.png %.grit
#---------------------------------------------------------------------------------
	grit $< -fts -o$*
 
-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
