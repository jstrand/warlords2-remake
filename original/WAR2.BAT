@echo off
if exist war2.ini goto doneinit
	echo 1 >war2.ini
	install
:doneinit

if "%1" == ""    goto startit
goto donestart
:startit
start
if NOT ERRORLEVEL 0 goto exit
:donestart

warlord2 %1 %2 %3 %4
:exit

