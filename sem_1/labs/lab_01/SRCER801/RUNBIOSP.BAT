echo off
if exist biosp.exe goto run
if exist srfiles.zip goto unzip
echo  ******** Can not locate BIOSP.EXE program ********
goto exit
:unzip
echo _______ One-Time decompression of BIOS Pre-Processor files
pkunzip srfiles.zip
:run
biosp %1 %2 %3 -b
if errorlevel 16 goto exit
echo _________  Copying BIOS Pre-Proceesor output file to BIOS.DEF 
if errorlevel 4 goto mainbios
if errorlevel 2 goto diskbios
if errorlevel 1 goto videobios
copy ubios.def bios.def
goto runsr
:mainbios
copy mbios.def bios.def
goto runsr
:diskbios
copy dbios.def bios.def
goto runsr
:videobios
copy vbios.def bios.def
:runsr
echo _________  Loading SOURCER
sr bios.def %1 %2 %3
:exit

