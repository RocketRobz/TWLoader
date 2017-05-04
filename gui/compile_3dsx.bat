@echo off
make clean
make COMPILE_3DSX=1 3dsx
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader/TWLoader.3dsx"
copy "TWLoader.3dsx" "../TWLoader-update/TWLoader.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader/TWLoader.smdh"
copy "TWLoader.smdh" "../TWLoader-update/TWLoader.smdh"
pause
