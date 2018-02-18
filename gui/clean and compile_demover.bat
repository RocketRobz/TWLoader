@echo off
make clean
make -j8 COMPILE_3DSX=1 3dsx
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader_demo/TWLoader_demo.3dsx"
copy "TWLoader.3dsx" "../TWLoader-update/TWLoader_demo.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader_demo/TWLoader_demo.smdh"
copy "TWLoader.smdh" "../TWLoader-update/TWLoader_demo.smdh"
pause
