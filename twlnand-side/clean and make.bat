@echo off
make clean
make
make_cia --srl="TWLapp.nds"
copy "TWLapp.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
pause