@echo off
cd twlnand-side
make clean
cd ..
cd NTR_Launcher
make clean
cd ..
cd gui
make clean
cd ..
rmdir /s /q "7zfile"
