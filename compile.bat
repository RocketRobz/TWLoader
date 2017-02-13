@echo off
mkdir "7zfile/flashcardroot/_nds"
mkdir "7zfile/sdroot/3ds/TWLoader"
mkdir "7zfile/sdroot/_nds/twloader/loadflashcard"
mkdir "7zfile/sdroot/_nds/twloader/cia"
cd ..
cd "TWLoader-update/prebuilts"
copy "ace_rpg.nds" "../../TWLoader/7zfile/sdroot/_nds/twloader/loadflashcard/ace_rpg.nds"
copy "GBARunner2.nds" "../../TWLoader/7zfile/sdroot/_nds/GBARunner2.nds"
copy "GBARunner2_flashcard.nds" "../../TWLoader/7zfile/flashcardroot/GBARunner2.nds"
copy "loadcard_dstt.nds" "../../TWLoader/7zfile/sdroot/_nds/loadcard_dstt.nds"
copy "r4.nds" "../../TWLoader/7zfile/sdroot/_nds/twloader/loadflashcard/r4.nds"
copy "r4idsn.nds" "../../TWLoader/7zfile/sdroot/_nds/twloader/loadflashcard/r4idsn.nds"
cd ..
copy "unofficial-bootstrap.nds" "../TWLoader/7zfile/sdroot/_nds/unofficial-bootstrap.nds"
copy "release-bootstrap.nds" "../TWLoader/7zfile/sdroot/_nds/release-bootstrap.nds"
copy "release-bootstrap" "../TWLoader/7zfile/sdroot/_nds/release-bootstrap"
copy "unofficial-bootstrap" "../TWLoader/7zfile/sdroot/_nds/unofficial-bootstrap"
cd ..
cd "TWLoader/twlnand-side"
make
patch_ndsheader_dsiware.py TWLapp-hb.nds --mode dsi --maker 01 --code TWLD --title "TWLOADER-TWL" --out TWLapp.nds
make_cia --srl="TWLapp.nds"
copy "TWLapp.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
copy "TWLapp.cia" "../../TWLoader-update/TWLoader - TWLNAND side.cia"
cd ..
cd NTR_Launcher
make
copy "NTR_Launcher.nds" "../7zfile/sdroot/_nds/twloader/NTR_Launcher.nds"
copy "NTR_Launcher.nds" "../../TWLoader-update/NTR_Launcher.nds"
cd ..
cd gui
make clean
make COMPILE_3DSX=0 cia
copy "TWLoader.cia" "../../TWLoader-update/TWLoader.cia"
copy "TWLoader.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader.cia"
make clean
make COMPILE_3DSX=1 3dsx
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader/TWLoader.3dsx"
copy "TWLoader.3dsx" "../../TWLoader-update/TWLoader.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader/TWLoader.smdh"
copy "TWLoader.smdh" "../../TWLoader-update/TWLoader.smdh"
cd ..
cd flashcard-side
make
copy "flashcard-side.nds" "../7zfile/flashcardroot/_nds/twloader.nds"
copy "flashcard-side.nds" "../../TWLoader-update/twloader.nds"
pause



