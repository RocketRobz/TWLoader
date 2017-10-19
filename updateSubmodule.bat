@echo off
cd gui/source/pp2d
git pull origin master
cd ../../..
git status
git add gui/source/pp2d
git commit -m "Update pp2d submodule"
pause