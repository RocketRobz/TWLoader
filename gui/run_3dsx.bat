@echo off
set /p ip="Enter 3DS IP: "
3dslink -a %ip% TWLoader.3dsx -r 2
pause