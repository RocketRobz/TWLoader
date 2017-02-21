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
#include <maxmod7.h>

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------

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

void PowerOnSlot() {
	// Power On Slot
	while(REG_SCFG_MC&0x0C !=  0x0C); // wait until state<>3
	if(REG_SCFG_MC&0x0C != 0x00) return; //  exit if state<>0
	
	REG_SCFG_MC = 0x04;    // wait 1ms, then set state=1
	while(REG_SCFG_MC&0x0C != 0x04);
	
	REG_SCFG_MC = 0x08;    // wait 10ms, then set state=2      
	while(REG_SCFG_MC&0x0C != 0x08);
	
	REG_ROMCTRL = 0x20000000; // wait 27ms, then set ROMCTRL=20000000h
	
	while(REG_ROMCTRL&0x8000000 != 0x8000000);
	
}

void PowerOffSlot() {
	while(REG_SCFG_MC&0x0C !=  0x0C); // wait until state<>3
	if(REG_SCFG_MC&0x0C != 0x08) return 1; // exit if state<>2      
	
	REG_SCFG_MC = 0x0C; // set state=3 
	while(REG_SCFG_MC&0x0C != 0x00); // wait until state=0
}

void TWL_ResetSlot1() {
	PowerOffSlot();
	for (int i = 0; i < 30; i++) { swiWaitForVBlank(); }
	PowerOnSlot(); 
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
	
	mmInstall(FIFO_MAXMOD);
	
	SetYtrigger(80);
	
	installSoundFIFO();
	installSystemFIFO();

	fifoWaitValue32(FIFO_USER_01);
	if(fifoCheckValue32(FIFO_USER_02)) { TWL_ResetSlot1(); }
	fifoSendValue32(FIFO_USER_03, 1);
	
	fifoWaitValue32(FIFO_USER_07);
	if(fifoCheckValue32(FIFO_USER_04)) { REG_SCFG_CLK = 0x0181; }
	if(fifoCheckValue32(FIFO_USER_05)) {
		// Switch to NTR Mode
		REG_SCFG_ROM = 0x703;
		REG_SCFG_EXT = 0x93A40000;
	}


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

