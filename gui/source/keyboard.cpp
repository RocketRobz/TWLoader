#include <stdio.h>
#include <string>
#include <3ds.h>

std::string keyboardInput(void) {
    SwkbdState keyboardState;
	char input[64];
	 
    swkbdInit(&keyboardState, SWKBD_TYPE_QWERTY, 2, sizeof(input));
    swkbdSetHintText(&keyboardState, "Press a key to search");
	swkbdSetFeatures(&keyboardState, SWKBD_DEFAULT_QWERTY);

    swkbdInputText(&keyboardState, input, sizeof(input));

    return std::string(input);
}
