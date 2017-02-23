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
TARGET		:=	TWLapp-hb
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
# English
CartPrompt01.s			:	../$(IMAGES)/CartPrompt01.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt02.s			:	../$(IMAGES)/CartPrompt02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt03.s			:	../$(IMAGES)/CartPrompt03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt04.s			:	../$(IMAGES)/CartPrompt04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
# French
CartPrompt01FR.s			:	../$(IMAGES)/CartPrompt01FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt02FR.s			:	../$(IMAGES)/CartPrompt02FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt03FR.s			:	../$(IMAGES)/CartPrompt03FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt04FR.s			:	../$(IMAGES)/CartPrompt04FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
# Italian
CartPrompt01IT.s			:	../$(IMAGES)/CartPrompt01IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt02IT.s			:	../$(IMAGES)/CartPrompt02IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt03IT.s			:	../$(IMAGES)/CartPrompt03IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

CartPrompt04IT.s			:	../$(IMAGES)/CartPrompt04IT.bmp
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
# Japanese
Bot02JA.s			:	../$(IMAGES)/Bot02JA.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03JA.s			:	../$(IMAGES)/Bot03JA.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04JA.s			:	../$(IMAGES)/Bot04JA.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05JA.s			:	../$(IMAGES)/Bot05JA.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

# English
Bot02.s			:	../$(IMAGES)/Bot02.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03.s			:	../$(IMAGES)/Bot03.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04.s			:	../$(IMAGES)/Bot04.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05.s			:	../$(IMAGES)/Bot05.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	
# French
Bot02FR.s			:	../$(IMAGES)/Bot02FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03FR.s			:	../$(IMAGES)/Bot03FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04FR.s			:	../$(IMAGES)/Bot04FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05FR.s			:	../$(IMAGES)/Bot05FR.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	
# German
Bot02DE.s			:	../$(IMAGES)/Bot02DE.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03DE.s			:	../$(IMAGES)/Bot03DE.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04DE.s			:	../$(IMAGES)/Bot04DE.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05DE.s			:	../$(IMAGES)/Bot05DE.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

# Italian
Bot02IT.s			:	../$(IMAGES)/Bot02IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03IT.s			:	../$(IMAGES)/Bot03IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04IT.s			:	../$(IMAGES)/Bot04IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05IT.s			:	../$(IMAGES)/Bot05IT.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	
# Spanish
Bot02ES.s			:	../$(IMAGES)/Bot02ES.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot03ES.s			:	../$(IMAGES)/Bot03ES.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot04ES.s			:	../$(IMAGES)/Bot04ES.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot05ES.s			:	../$(IMAGES)/Bot05ES.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q


Bot09.s			:	../$(IMAGES)/Bot09.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Bot10.s			:	../$(IMAGES)/Bot10.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

white.s			:	../$(IMAGES)/white.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q
	
Top000.s			:	../$(IMAGES)/Top000.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top001.s			:	../$(IMAGES)/Top001.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top002.s			:	../$(IMAGES)/Top002.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top003.s			:	../$(IMAGES)/Top003.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top004.s			:	../$(IMAGES)/Top004.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top005.s			:	../$(IMAGES)/Top005.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top006.s			:	../$(IMAGES)/Top006.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top007.s			:	../$(IMAGES)/Top007.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top008.s			:	../$(IMAGES)/Top008.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top009.s			:	../$(IMAGES)/Top009.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top010.s			:	../$(IMAGES)/Top010.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top011.s			:	../$(IMAGES)/Top011.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top012.s			:	../$(IMAGES)/Top012.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top013.s			:	../$(IMAGES)/Top013.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top014.s			:	../$(IMAGES)/Top014.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top015.s			:	../$(IMAGES)/Top015.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top016.s			:	../$(IMAGES)/Top016.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top017.s			:	../$(IMAGES)/Top017.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top018.s			:	../$(IMAGES)/Top018.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top019.s			:	../$(IMAGES)/Top019.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top020.s			:	../$(IMAGES)/Top020.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top021.s			:	../$(IMAGES)/Top021.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top022.s			:	../$(IMAGES)/Top022.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top023.s			:	../$(IMAGES)/Top023.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top024.s			:	../$(IMAGES)/Top024.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top025.s			:	../$(IMAGES)/Top025.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top026.s			:	../$(IMAGES)/Top026.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top027.s			:	../$(IMAGES)/Top027.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top028.s			:	../$(IMAGES)/Top028.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top029.s			:	../$(IMAGES)/Top029.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top030.s			:	../$(IMAGES)/Top030.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top031.s			:	../$(IMAGES)/Top031.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top032.s			:	../$(IMAGES)/Top032.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top033.s			:	../$(IMAGES)/Top033.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top034.s			:	../$(IMAGES)/Top034.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top035.s			:	../$(IMAGES)/Top035.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top036.s			:	../$(IMAGES)/Top036.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top037.s			:	../$(IMAGES)/Top037.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top038.s			:	../$(IMAGES)/Top038.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top039.s			:	../$(IMAGES)/Top039.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top040.s			:	../$(IMAGES)/Top040.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top041.s			:	../$(IMAGES)/Top041.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top042.s			:	../$(IMAGES)/Top042.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top043.s			:	../$(IMAGES)/Top043.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top044.s			:	../$(IMAGES)/Top044.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top045.s			:	../$(IMAGES)/Top045.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top046.s			:	../$(IMAGES)/Top046.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top047.s			:	../$(IMAGES)/Top047.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top048.s			:	../$(IMAGES)/Top048.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top049.s			:	../$(IMAGES)/Top049.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top050.s			:	../$(IMAGES)/Top050.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top051.s			:	../$(IMAGES)/Top051.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top052.s			:	../$(IMAGES)/Top052.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top053.s			:	../$(IMAGES)/Top053.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top054.s			:	../$(IMAGES)/Top054.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top055.s			:	../$(IMAGES)/Top055.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top056.s			:	../$(IMAGES)/Top056.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top057.s			:	../$(IMAGES)/Top057.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top058.s			:	../$(IMAGES)/Top058.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top059.s			:	../$(IMAGES)/Top059.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top060.s			:	../$(IMAGES)/Top060.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top061.s			:	../$(IMAGES)/Top061.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top062.s			:	../$(IMAGES)/Top062.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top063.s			:	../$(IMAGES)/Top063.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top064.s			:	../$(IMAGES)/Top064.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top065.s			:	../$(IMAGES)/Top065.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top066.s			:	../$(IMAGES)/Top066.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top067.s			:	../$(IMAGES)/Top067.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top068.s			:	../$(IMAGES)/Top068.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top069.s			:	../$(IMAGES)/Top069.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top070.s			:	../$(IMAGES)/Top070.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top071.s			:	../$(IMAGES)/Top071.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top072.s			:	../$(IMAGES)/Top072.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top073.s			:	../$(IMAGES)/Top073.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top074.s			:	../$(IMAGES)/Top074.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top075.s			:	../$(IMAGES)/Top075.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top076.s			:	../$(IMAGES)/Top076.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top077.s			:	../$(IMAGES)/Top077.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top078.s			:	../$(IMAGES)/Top078.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top079.s			:	../$(IMAGES)/Top079.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top080.s			:	../$(IMAGES)/Top080.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top081.s			:	../$(IMAGES)/Top081.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top082.s			:	../$(IMAGES)/Top082.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top083.s			:	../$(IMAGES)/Top083.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top084.s			:	../$(IMAGES)/Top084.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top085.s			:	../$(IMAGES)/Top085.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top086.s			:	../$(IMAGES)/Top086.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top087.s			:	../$(IMAGES)/Top087.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top088.s			:	../$(IMAGES)/Top088.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top089.s			:	../$(IMAGES)/Top089.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top090.s			:	../$(IMAGES)/Top090.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top091.s			:	../$(IMAGES)/Top091.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top092.s			:	../$(IMAGES)/Top092.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top093.s			:	../$(IMAGES)/Top093.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top094.s			:	../$(IMAGES)/Top094.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top095.s			:	../$(IMAGES)/Top095.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top096.s			:	../$(IMAGES)/Top096.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top097.s			:	../$(IMAGES)/Top097.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top098.s			:	../$(IMAGES)/Top098.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top099.s			:	../$(IMAGES)/Top099.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top100.s			:	../$(IMAGES)/Top100.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top101.s			:	../$(IMAGES)/Top101.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top102.s			:	../$(IMAGES)/Top102.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top103.s			:	../$(IMAGES)/Top103.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top104.s			:	../$(IMAGES)/Top104.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top105.s			:	../$(IMAGES)/Top105.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top106.s			:	../$(IMAGES)/Top106.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top107.s			:	../$(IMAGES)/Top107.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top108.s			:	../$(IMAGES)/Top108.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top109.s			:	../$(IMAGES)/Top109.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top110.s			:	../$(IMAGES)/Top110.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top111.s			:	../$(IMAGES)/Top111.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top112.s			:	../$(IMAGES)/Top112.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top113.s			:	../$(IMAGES)/Top113.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top114.s			:	../$(IMAGES)/Top114.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top115.s			:	../$(IMAGES)/Top115.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top116.s			:	../$(IMAGES)/Top116.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top117.s			:	../$(IMAGES)/Top117.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top118.s			:	../$(IMAGES)/Top118.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top119.s			:	../$(IMAGES)/Top119.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top120.s			:	../$(IMAGES)/Top120.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top121.s			:	../$(IMAGES)/Top121.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top122.s			:	../$(IMAGES)/Top122.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top123.s			:	../$(IMAGES)/Top123.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q

Top124.s			:	../$(IMAGES)/Top124.bmp
	grit	$< -gB8 -gzl -fts -gTff00ff -o $@ -q


# DSi BootSplash.
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
