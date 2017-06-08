@echo off
make clean
make
copy "flashcard-softreset.nds" "../7zfile/flashcardsdroot (all)/SR.TWLDR"
pause