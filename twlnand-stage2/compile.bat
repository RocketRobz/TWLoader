@echo off
make
copy "Stage2.nds" "../7zfile/sdroot/_nds/twloader/TWLD.twldr"
copy "Stage2.nds" "../TWLoader-update/TWLD.twldr"
pause