/**

	This method is created by Jolty95 (umbjolt in gbatemp) to parse KeyBoard input to TWLoader.
	This code is an application of libctru swkbd using native 3DS software keyboard.
	Please, do not modify it if there is not reason to do it.
	
	Use:
		#std::string keyboardInput(void)  return a C++ string with the input written by the user using touch screen or buttons.

*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>

std::string keyboardInput(void);

#endif // KEYBOARD_H
