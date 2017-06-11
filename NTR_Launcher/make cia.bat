@echo off
make_cia --srl="NTR_Launcher.nds"
copy "NTR_Launcher.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - NTR Launcher.cia"
pause