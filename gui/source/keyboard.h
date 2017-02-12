/**

	This method is created by Jolty95 (umbjolt in gbatemp) to parse KeyBoard input to TWLoader.
	This code is an application of libctru swkbd using native 3DS software keyboard.
	Please, do not modify it if there is not reason to do it.
	
	Use:
		#std::string keyboardInput(const wchar_t*)  return a C++ string with the input written by the user using touch screen or buttons.
		#int keyboardInputInt(void)  return a int with the input written by the user using touch screen or buttons. If input is >255 return = 255 and if cancel, return 0

*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>

/**
 * This method show the software original keyboard from the 3ds OS. 
 * @param: const char* text to show a hint in keyboard
 * @return: std::string with user input
 */

std::string keyboardInput(const wchar_t* hint);

/**
 * This method show the software original keyboard from the 3ds OS. 
 * @param: const char* text to show a hint in keyboard
 * @return: int with user input
 */

int keyboardInputInt(const char* hint);

#endif // KEYBOARD_H
