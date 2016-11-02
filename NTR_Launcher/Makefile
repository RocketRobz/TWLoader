#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	NTR_Launcher
export TOPDIR		:=	$(CURDIR)

export VERSION_MAJOR	:= 1
export VERSION_MINOR	:= 94
export VERSTRING	:=	$(VERSION_MAJOR).$(VERSION_MINOR)

#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).arm7.elf -9 arm9/$(TARGET).arm9.elf \
			-b $(CURDIR)/icon.bmp "NTR Slot-1 Launcher;Modification of NitroHax by Chishm;Apache Thunder"

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
BootLoader/load.bin	:	BootLoader/source/*
	$(MAKE) -C BootLoader

#---------------------------------------------------------------------------------
arm9/data/load.bin	:	BootLoader/load.bin
	rm -Rf arm9/data
	mkdir arm9/data
	cp $< $@

#---------------------------------------------------------------------------------
arm9/source/version.h : Makefile
	@echo "#ifndef VERSION_H" > $@
	@echo "#define VERSION_H" >> $@
	@echo >> $@
	@echo '#define VERSION_STRING "v'$(VERSION_MAJOR).$(VERSION_MINOR)'"' >> $@
	@echo >> $@
	@echo "#endif // VERSION_H" >> $@

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf	:	arm9/data/load.bin arm9/source/version.h
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
dist-bin	:	$(TARGET).txt $(TARGET).nds License.txt
	zip -X -9 $(TARGET)_v$(VERSTRING).zip $(TARGET).txt $(TARGET).nds License.txt

dist-src	:
	tar --exclude=*~ -cvjf $(TARGET)_src_v$(VERSTRING).tar.bz2 \
	--transform 's,^,/'$(TARGET)'/,' \
	Makefile icon.bmp License.txt Launcher.txt \
	arm7/Makefile arm7/source \
	arm9/Makefile arm9/source arm9/data arm9/graphics \
	BootLoader/Makefile BootLoader/load.ld BootLoader/source

dist-legal	:	BootLoader/load.bin
	tar --exclude=*~ --exclude=read_card.c -cvjf $(TARGET)_src_v$(VERSTRING).tar.bz2 \
	--transform 's,^,/'$(TARGET)'/,' \
	Makefile icon.bmp License.txt Launcher.txt \
	arm7/Makefile arm7/source \
	arm9/Makefile arm9/source arm9/data arm9/graphics \
	BootLoader/Makefile BootLoader/source BootLoader/load.bin

dist	:	dist-bin dist-src

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C BootLoader clean
	rm -f arm9/data/load.bin
	rm -f arm9/source/version.h
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
