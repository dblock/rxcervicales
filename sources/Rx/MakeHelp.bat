@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by RX.HPJ. >"hlp\Rx.hm"
echo. >>"hlp\Rx.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Rx.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Rx.hm"
echo. >>"hlp\Rx.hm"
echo // Prompts (IDP_*) >>"hlp\Rx.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Rx.hm"
echo. >>"hlp\Rx.hm"
echo // Resources (IDR_*) >>"hlp\Rx.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Rx.hm"
echo. >>"hlp\Rx.hm"
echo // Dialogs (IDD_*) >>"hlp\Rx.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Rx.hm"
echo. >>"hlp\Rx.hm"
echo // Frame Controls (IDW_*) >>"hlp\Rx.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Rx.hm"
REM -- Make help for Project RX


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\Rx.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\Rx.hlp" goto :Error
if not exist "hlp\Rx.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\Rx.hlp" Debug
if exist Debug\nul copy "hlp\Rx.cnt" Debug
if exist Release\nul copy "hlp\Rx.hlp" Release
if exist Release\nul copy "hlp\Rx.cnt" Release
echo.
goto :done

:Error
echo hlp\Rx.hpj(1) : error: Problem encountered creating help file

:done
echo.
