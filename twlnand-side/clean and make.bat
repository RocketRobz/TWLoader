@echo off
make clean
make
Py -2 patch_ndsheader_dsiware.py TWLapp-hb.nds --mode dsi --maker 01 --code TWLD --title "TWLOADER-TWL" --out TWLapp.nds
make_cia --srl="TWLapp.nds"
copy "TWLapp.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
pause