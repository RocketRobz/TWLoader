@echo off
tools\bannertool makebanner -i "app\banner.png" -ca "app\banneraudio.bcwav" -o "app\banner.bin"
tools\makerom -f cia -target t -exefslogo -o TWLoader.cia -elf twlNANDfiler.elf -rsf app\build-cia.rsf -banner app\banner.bin -icon app\icon.bin -major 1 -minor 1
pause