@echo off
make clean
make
copy "Stage2.nds" "../7zfile/sdroot/_nds/twloader/TWLD.twldr"
pause