@echo off
make_cia --srl="Stage1-DSiWare.nds"
copy "Stage1-DSiWare.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
copy "Stage1-DSiWare.cia" "../TWLoader-update/TWLoader - TWLNAND side.cia"
pause