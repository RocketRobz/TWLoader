@echo off
mkdir "7zfile/flashcardsdroot (all)/_nds"
mkdir "7zfile/sdroot/3ds/TWLoader"
mkdir "7zfile/sdroot/3ds/TWLoader_demo"
mkdir "7zfile/sdroot/_nds/twloader/loadflashcard"
mkdir "7zfile/sdroot/_nds/twloader/cia"
cd "TWLoader-update/prebuilts"
copy "ace_rpg.nds" "../../7zfile/sdroot/_nds/twloader/loadflashcard/ace_rpg.nds"
copy "r4.nds" "../../7zfile/sdroot/_nds/twloader/loadflashcard/r4.nds"
copy "loadcard_dstt.nds" "../../7zfile/sdroot/_nds/loadcard_dstt.nds"
copy "GBARunner2.nds" "../../7zfile/sdroot/_nds/GBARunner2.nds"
copy "GBARunner2_flashcard.nds" "../../7zfile/flashcardsdroot (all)/GBARunner2.nds"
cd ..
copy "unofficial-bootstrap.nds" "../7zfile/sdroot/_nds/unofficial-bootstrap.nds"
copy "release-bootstrap.nds" "../7zfile/sdroot/_nds/release-bootstrap.nds"
cd ..
cd "twlnand-stage1"
make clean
make
python patch_ndsheader_dsiware.py Stage1.nds --mode dsi --maker 01 --code TWLD --title "TWLOADER-TWL" --out Stage1-DSiWare.nds
make_cia --srl="Stage1-DSiWare.nds"
copy "Stage1-DSiWare.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader - TWLNAND side.cia"
copy "Stage1-DSiWare.cia" "../TWLoader-update/TWLoader - TWLNAND side.cia"
cd ..
cd "twlnand-stage2"
make clean
make
copy "Stage2.nds" "../7zfile/sdroot/_nds/twloader/TWLD.twldr"
copy "Stage2.nds" "../TWLoader-update/TWLD.twldr"
cd ..
cd NTR_Launcher
make clean
make
copy "NTR_Launcher.nds" "../7zfile/sdroot/_nds/twloader/NTR_Launcher.nds"
copy "NTR_Launcher.nds" "../TWLoader-update/NTR_Launcher.nds"
cd ..
cd gui
make clean
make -j8 COMPILE_3DSX=0
copy "TWLoader.cia" "../TWLoader-update/TWLoader.cia"
copy "TWLoader.cia" "../7zfile/sdroot/_nds/twloader/cia/TWLoader.cia"
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader/TWLoader.3dsx"
copy "TWLoader.3dsx" "../TWLoader-update/TWLoader.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader/TWLoader.smdh"
copy "TWLoader.smdh" "../TWLoader-update/TWLoader.smdh"
make clean
make -j8 COMPILE_3DSX=1 3dsx
copy "TWLoader.3dsx" "../7zfile/sdroot/3ds/TWLoader_demo/TWLoader_demo.3dsx"
copy "TWLoader.3dsx" "../TWLoader-update/TWLoader_demo.3dsx"
copy "TWLoader.smdh" "../7zfile/sdroot/3ds/TWLoader/TWLoader_demo.smdh"
copy "TWLoader.smdh" "../TWLoader-update/TWLoader_demo.smdh"
cd ..
cd flashcard-side
make clean
make
copy "flashcard-side.nds" "../7zfile/flashcardsdroot (all)/_nds/twloader.nds"
copy "flashcard-side.nds" "../TWLoader-update/twloader.nds"
cd ..
cd flashcard-softreset
make clean
make
copy "flashcard-softreset.nds" "../7zfile/flashcardsdroot (all)/SR.TWLDR"
pause



