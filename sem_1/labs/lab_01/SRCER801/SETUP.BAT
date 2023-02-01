echo off
cls
echo џ
echo                   Sourcer with BIOS & Windows Source - First Time Installation
echo ллллл  л  л  л  ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
echo џ
if exist srin.exe goto loading
echo    The SRIN.EXE file was not found on default drive. Select default drive
echo    from DOS before running INSTALL  (i.e. to install from drive B, at the
echo    DOS prompt type  B: ). The Sourcer disk must be in the default drive.
echo џ
goto done
:loading
echo џ
echo    In a moment, the Sourcer installation will begin.    In most cases the
echo    installation will take less than 2 minutes to complete.
echo џ
echo ФФФФ loading Sourcer Installation/Information program SRIN.  Please Wait.
srin install biosp winp %1 %2
:done

