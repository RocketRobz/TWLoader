@echo off
cd twlnand-side
make
patch_ndsheader_dsiware.py TWLapp-hb.nds --mode dsi --maker 01 --code TWLD --title "TWLOADER-TWL" --out TWLapp.nds
make_cia --srl="TWLapp.nds"
copy "TWLapp.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
cd ..
cd NTR_Launcher
make
copy "NTR_Launcher.nds" "../7zfile/sdroot/_nds/twloader/NTR_Launcher.nds"
cd ..
cd gui
make
copy "TWLoader.cia" "../7zfile/sdroot/TWLoader.cia"
pause
