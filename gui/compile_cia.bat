@echo off
make clean
make COMPILE_3DSX=0 cia
copy "TWLoader.cia" "../TWLoader-update/TWLoader.cia"
copy "TWLoader.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader.cia"
pause
