@echo off
make clean
make -j8 COMPILE_3DSX=0
copy "TWLoader.cia" "../TWLoader-update/TWLoader.cia"
copy "TWLoader.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader.cia"
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader/TWLoader.3dsx"
copy "TWLoader.3dsx" "../TWLoader-update/TWLoader.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader/TWLoader.smdh"
copy "TWLoader.smdh" "../TWLoader-update/TWLoader.smdh"
pause
