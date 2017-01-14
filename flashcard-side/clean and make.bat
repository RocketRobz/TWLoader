@echo off
make clean
make
copy "flashcard-side.nds" "../7zfile/flashcardroot/_nds/twloader.nds"
pause