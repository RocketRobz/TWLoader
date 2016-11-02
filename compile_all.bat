@echo off
cd twlapp
make
patch_ndsheader_dsiware.py TWLapp-hb.nds --mode dsi --maker 01 --code TWLD --title "TWLOADER-TWL" --out TWLapp.nds
make_cia --srl="TWLapp.nds"
copy "TWLapp.cia" "../sdroot/_nds/twloader/cia/TWLapp.cia"
cd ..
cd NTR_Launcher
make
copy "NTR_Launcher.nds" "../sdroot/_nds/twloader/NTR_Launcher.nds"
cd ..
make
pause
