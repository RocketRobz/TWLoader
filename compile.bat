@echo off
mkdir "7zfile/flashcardroot/_nds
mkdir "7zfile/sdroot/_nds/twloader/loadflashcard"
mkdir "7zfile/sdroot/_nds/twloader/cia"
cd prebuilts
copy "ace_rpg.nds" "../7zfile/sdroot/_nds/twloader/loadflashcard/ace_rpg.nds"
copy "GBARunner2_flashcard.nds" "../7zfile/flashcardroot/GBARunner2.nds"
copy "GBARunner2.nds" "../7zfile/sdroot/_nds/GBARunner2.nds"
copy "loadcard_dstt.nds" "../7zfile/sdroot/_nds/loadcard_dstt.nds"
copy "r4.nds" "../7zfile/sdroot/_nds/twloader/loadflashcard/r4.nds"
copy "r4idsn.nds" "../7zfile/sdroot/_nds/twloader/loadflashcard/r4idsn.nds"
cd ..
start /wait wget.bat https://github.com/Jolty95/TWLoader-update/raw/master/unofficial-bootstrap.nds ^& exit
start /wait wget.bat https://github.com/Jolty95/TWLoader-update/raw/master/release-bootstrap.nds ^& exit
start /wait wget.bat https://raw.githubusercontent.com/Jolty95/TWLoader-update/master/release-bootstrap ^& exit
start /wait wget.bat https://raw.githubusercontent.com/Jolty95/TWLoader-update/master/unofficial-bootstrap ^& exit
copy "unofficial-bootstrap.nds" "7zfile/sdroot/_nds/unofficial-bootstrap.nds"
del "unofficial-bootstrap.nds"
copy "release-bootstrap.nds" "7zfile/sdroot/_nds/release-bootstrap.nds"
del release-bootstrap.nds
copy "release-bootstrap" "7zfile/sdroot/_nds/release-bootstrap"
del release-bootstrap
copy "unofficial-bootstrap" "7zfile/sdroot/_nds/unofficial-bootstrap"
del unofficial-bootstrap
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
copy "TWLoader.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader.cia"
cd ..
cd flashcard-side
make
copy "flashcard-side.nds" "../7zfile/flashcardroot/_nds/twloader.nds"
pause



