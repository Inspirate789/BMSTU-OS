@echo off
echo :
echo :  Runs DUMPNE to add any found exports into the specified .dat file
echo :  _________________________________________________________________
echo :
if "%1"=="" goto info
if "%2"=="" goto info
echo  Examining all specified files %1.  Please wait.
echo :
for %%f in (%1) do dumpne -quiet -exports %%f >> %2.dat
goto exit
:info
echo :  Command line:
echo :     EXPORTS [path\wildcards]  [target DAT file]
echo :
echo :  Example:
echo :     exports \windows\system\*.exe  win31
echo :
echo :  Example results:
echo :     All of the .EXE files in the directory \WINDOWS\SYSTEM are examined
echo :     for exports, and any found are added to the file WIN31.DAT.
echo :
:exit
