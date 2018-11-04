SET SOURCE_DIR=%1
SET BUILD_DIR=%2
SET MAKENSIS=%3
SET WINDEPLOYQT=%4

echo Build dir is: %BUILD_DIR%
echo Source dir is: %SOURCE_DIR%

cd /d "%BUILD_DIR%"

rmdir /s /q installer
mkdir installer
cd installer

copy /y "%SOURCE_DIR%\\win32\\qmlevemon_installer.nsi" .
copy /y "%SOURCE_DIR%\\LICENSE" .\LICENSE.txt
rem copy /y "%SOURCE_DIR%\\src\\img\\app_icon\\app_icon.ico" .
copy /y "%BUILD_DIR%\\QMLEVEMon.exe" .

%WINDEPLOYQT% --release --qmldir "%SOURCE_DIR%/src/qml" --compiler-runtime QMLEVEMon.exe

copy /y %windir%\\system32\\libeay32.dll .
copy /y %windir%\\system32\\ssleay32.dll .

%MAKENSIS% .\qmlevemon_installer.nsi

copy /y qmlevemon-setup.exe ..\
del qmlevemon-setup.exe
