/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>

//---------------------------------------------------------------------------------
void ReturntoDSiMenu() {
//---------------------------------------------------------------------------------
	// This will skip the power-off/sleep mode screen when returning to HOME Menu
	i2cWriteRegister(0x4A, 0x70, 0x01);		// Bootflag = Warmboot/SkipHealthSafety
	i2cWriteRegister(0x4A, 0x11, 0x01);		// Reset to DSi/3DS HOME Menu
}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	if(fifoCheckValue32(FIFO_USER_06)) {
		ReturntoDSiMenu();
	}
}

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

int PowerOnSlot() {
    
    REG_SCFG_MC = 0x04;    // set state=1
    while(REG_SCFG_MC&1);
    
    REG_SCFG_MC = 0x08;    // set state=2      
    while(REG_SCFG_MC&1);
    
    REG_ROMCTRL = 0x20000000; // set ROMCTRL=20000000h
    return 0;
}

int PowerOffSlot() {
    if(REG_SCFG_MC&1) return 1; 
    
    REG_SCFG_MC = 0x0C; // set state=3 
    while(REG_SCFG_MC&1);
    return 0;
}

int TWL_ResetSlot1() {
	PowerOffSlot();
	for (int i = 0; i < 30; i++) { swiWaitForVBlank(); }
	PowerOnSlot();
	return 0;
}

void SCFGFifoCheck (void)
{
    // if(fifoCheckValue32(FIFO_USER_07)) {	// This breaks support for DSTT and it's clones
		fifoWaitValue32(FIFO_USER_07);
        if(fifoCheckValue32(FIFO_USER_04)) { REG_SCFG_CLK = 0x0181; }
        if(fifoCheckValue32(FIFO_USER_05)) { REG_SCFG_ROM = 0x703; }
        if(fifoCheckValue32(FIFO_MAXMOD)) { *(u16*)(0x4004700) |= BIT(13); }
		// if(fifoCheckValue32(FIFO_USER_08)) { REG_SCFG_EXT = 0x93A50000; }
        // fifoSendValue32(FIFO_USER_07, 0);
    // }
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
    nocashMessage("ARM7 main.c main");
	
	// SCFG_CLK
	// 0x0180 : NTR
	// 0x0181 : NTR+SD
	// 0x0187 : TWL
	// REG_SCFG_CLK = 0x0181;
	
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	
	fifoInit();
	
	SetYtrigger(80);
	
	installSystemFIFO();

	i2cWriteRegister(0x4A, 0x12, 0x00);		// Press power-button for auto-reset.
											// Fixes issue with DS games staying on (with screens off) for 5 seconds after pressing POWER button.

	// fifoWaitValue32(FIFO_USER_01);
	// if(fifoCheckValue32(FIFO_USER_02)) { TWL_ResetSlot1(); }
	// fifoSendValue32(FIFO_USER_03, 1);
	
    SCFGFifoCheck();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);
	
	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		// fifocheck();
		swiWaitForVBlank();
	}
	return 0;
}

