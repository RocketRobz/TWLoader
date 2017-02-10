# TWLoader
TWLoader is a CTR-mode frontend for nds-bootstrap that looks like the Nintendo DSi Menu.

# Usage / FAQ

**Please, read the _wiki_ before asking a question** [Wiki](https://github.com/Robz8/TWLoader/wiki)

# Setting up Development Environment for windows
- Download [Git for Windows](https://git-for-windows.github.io/)
- Install it and select "run git from windows command prompt"
- Download the [DevKitPro auto-installer](http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/)
- Install it and select "DevKitArm"
- This will install DevKitArm r46 which currently does not work, see https://github.com/Robz8/TWLoader/issues/82. So we must downgrade to r45
- Download [DevKitArm r45](https://sourceforge.net/projects/devkitpro/files/devkitARM/devkitARM_r45/)
- Open explorer and go to "C:\devkitPro"
- Delete the folder "devkitARM"
- Unzip "devkitARM_r45-win32.exe" into "C:\devkitPro"
- DevKitPro's libdns will not work, see https://github.com/ahezard/nds-bootstrap/issues/34 so we have to install ahezard's version
- Download ahezard [libnds](https://github.com/ahezard/libnds/archive/master.zip)
- Open explorer and go to "C:\devkitPro"
- Delete the folder "libnds"
- Unzip "libnds-master.zip" into "C:\devkitPro"
- Rename "libnds-master" to "libnds"
- Download [makerom](https://github.com/Steveice10/buildtools/raw/master/3ds/makerom32.exe) and rename it to "makerom.exe"
- Move "makerom.exe" to "C:\devkitPro\msys\bin"
- Download [bannertool](https://github.com/Steveice10/buildtools/raw/master/3ds/bannertool32.exe) and rename it to "bannertool.exe"
- Move "bannertool.exe" to "C:\devkitPro\msys\bin"
- Download [make_cia](https://github.com/ihaveamac/ctr_toolkit/releases/download/make_cia6.4builds/make_cia6.4builds.zip)
- Extract make_cia.exe from the windows folder to "C:\devkitPro\msys\bin"
- Open msys from the start menu (DevKitPro > msys)
- It will open into "C:\devkitPro\msys\home\$USERNAME" by default.
- Run the following commands to download all the git folders needed to compile
- "git clone https://github.com/Robz8/TWLoader.git"
- "git clone https://github.com/Jolty95/TWLoader-update.git"
- "git clone https://github.com/xerpi/sfillib"
- "cd sfillib/libsfil"
- "make && make install"
- "cd ../.."
- "git clone https://github.com/xerpi/sf2dlib"
- "cd sf2dlib/sf2dlib"
- "make && make install"
- "cd ../.."
- "git clone https://github.com/xerpi/sftdlib"
- "cd sftdlib/sftdlib"
- "make && make install"
- "cd ../.."

# Building

- Open msys from the start menu (DevKitPro > msys)
- "cd TWLoader"
- "compile.bat"

# Credits

- ahezard: [nds-bootstrap](https://github.com/ahezard/nds-bootstrap)
- Apache Thunder: NTR-Launcher code, and the recreation of the DS/DSi boot screens.
- billy-acuna: TWLoader logo.
- MarcusD: Rainbow colors in Notification LED code.
- shutterbug2000: File browsing code.
- spinal_cord: DSi4DS assets.
