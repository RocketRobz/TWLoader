# Homebrew Menu
The Homebrew Menu is a simple launcher menu which can be used to replace the menu on several DS flashcards. This menu supports the argv protocol needed for devkitARM compiled applications to make use of nitrofs as well as providing a soft reset feature where applications can exit back to the menu. Exiting to the menu is as simple as returning from main() or calling exit(0).

Currently the menu is a simple text based interface but we intend to improve this in the near future. Wintermute and Normmatt have discussed using the UI code from akaio for hbmenu and we hope to do this fairly soon.

Installation on your card is a simple matter of copying BOOT.NDS and the appropriate bootstrap file from the hbmenu folder in this archive. At present we have bootstrap launchers for the original R4, ezflash 5, dstt and Acekard 2(i). Copying all the files will give you a single SD card which will boot the Homebrew Menu on all 4 cards.

The Homebrew Menu also supports passing arguments to launched .nds files via .argv files. The testfiles folder has an nds file which lists arguments and some sample .argv files. These are simple text files which start with the name of the nds file to run and a list of arguments to pass to the application. Here's a quick sample .argv file.
```shell
# This is a comment
# commands can be placed on multiple lines
# first the name of the nds file to run
# this can include a path relative to the .argv file
# absolute paths are not currently supported
argvTest.nds

# a single argument
argument1

# multiple arguments separated by spaces
argument2 argument3

# multiple arguments separated by tabs
argument4	argument5

# or even separated by both tabs and spaces
argument6 argument7	argument8
```

# License
Note: While the GPL license allows you to distribute modified versions of this program it would be appreciated if any improvements are contributed to devkitPro. Ultimately the community as a whole is better served by having a single official source for tools, applications and libraries.

The latest sources may be obtained from devkitPro git using the command: `git clone git@github.com:devkitPro/nds-hb-menu.git`

```
 Copyright (C) 2005 - 2011
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ```
