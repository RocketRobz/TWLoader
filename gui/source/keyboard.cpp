#include <stdio.h>
#include <string>
#include <3ds.h>
#include <stdlib.h>

/**
 * This method show the software original keyboard from the 3ds OS. 
 * @param: const char* text to show a hint in keyboard
 * @return: std::string with user input
 */

std::string keyboardInput(const char* hint) {
    SwkbdState keyboardState;
	char input[64];
	 
    swkbdInit(&keyboardState, SWKBD_TYPE_QWERTY, 2, sizeof(input));
    swkbdSetHintText(&keyboardState, hint);
	swkbdSetFeatures(&keyboardState, SWKBD_DEFAULT_QWERTY);

    swkbdInputText(&keyboardState, input, sizeof(input));

    return std::string(input);
}

/**
 * This method show the software original keyboard from the 3ds OS. 
 * @param: const char* text to show a hint in keyboard
 * @return: std::string with user input
 */

int keyboardInputInt(const char* hint) {
	
	SwkbdState keyState;
    char input[4];

    swkbdInit(&keyState, SWKBD_TYPE_NUMPAD, 2, sizeof(input));
    swkbdSetHintText(&keyState, hint);

    SwkbdButton pressed = swkbdInputText(&keyState, input, 4);
    int res;
    if(pressed == SWKBD_BUTTON_LEFT)
        res = 0;
    else
    {
        res = strtol(input, NULL, 10);
        if(res > 255)
            res = 255;
    }

    return res;
}
