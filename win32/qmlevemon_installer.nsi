!if ${NSIS_PTR_SIZE} > 4
  !define BITS 64
  !define NAMESUFFIX " (64 bit)"
!else
  !define BITS 32
  !define NAMESUFFIX ""
!endif

OutFile "qmlevemon-setup.exe"
Unicode true
SetCompressor /SOLID lzma
InstallDir $PROGRAMFILES${BITS}\QMLEVEMon
; Registry key to check for directory (so if you install again, it will overwrite the old one automatically)
InstallDirRegKey HKLM "Software\QMLEVEMon" "Install_Dir"
RequestExecutionLevel admin

!include "MUI2.nsh"

; Names
Name "QMLEVEMon"
Caption "QMLEVEMon ${VERSION}${NAMESUFFIX} Setup"

!define REG_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMLEVEMon"

; Interface Settings
!define MUI_ABORTWARNING

;!define MUI_ICON "..\img\app_icon\app_icon.ico"
;!define MUI_UNICON "..\img\app_icon\app_icon.ico"

;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis3-branding.bmp"
;!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\nsis3-branding.bmp"

; Pages
!define MUI_WELCOMEPAGE_TITLE "Welcome to the QMLEVEMon Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of QML EVEMon, EVE Online Character Monitor.$\r$\n$\r$\nWindows version, {BITS}-bit.$\r$\n$\r$\n$_CLICK"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_LINK "Visit the QML EVEMon site for the latest news."
!define MUI_FINISHPAGE_LINK_LOCATION "https://qmlevemon.eve-wh.space/"
!define MUI_FINISHPAGE_RUN "$INSTDIR\QMLEVEMon.exe"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages
!insertmacro MUI_LANGUAGE "English"


;--------------------------------
; Installer version information
;!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
;VIProductVersion ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD}
VIProductVersion 0.2.0.0
VIAddVersionKey "FileVersion" "0.2"
VIAddVersionKey "FileDescription" "qmlevemon setup"
VIAddVersionKey "LegalCopyright" "https://qmlevemon.eve-wh.space/License"
;!endif


Section "QMLEVEMon"
    SectionIn RO
    ; Set output path to the installation directory.
    SetOutPath $INSTDIR
    ; Add all output directory to installed files list
    File /r "..\build\out\*.*"
    
    WriteRegExpandStr HKLM "${REG_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegExpandStr HKLM "${REG_UNINST_KEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayName" "QMLEVEMon (EVE Online Character Monitor)"
    WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayIcon" "$INSTDIR\QMLEVEMon.exe,0"
    WriteRegStr HKLM "${REG_UNINST_KEY}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "${REG_UNINST_KEY}" "URLInfoAbout" "https://qmlevemon.eve-wh.space/"
    WriteRegDWORD HKLM "${REG_UNINST_KEY}" "NoModify" "1"
    WriteRegDWORD HKLM "${REG_UNINST_KEY}" "NoRepair" "1"
    WriteUninstaller "uninstall.exe"
SectionEnd


; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
    CreateDirectory "$SMPROGRAMS\QMLEVEMon"
    CreateShortcut "$SMPROGRAMS\QMLEVEMon\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
    CreateShortcut "$SMPROGRAMS\QMLEVEMon\QMLEVEMon.lnk" "$INSTDIR\QMLEVEMon.exe" "" "$INSTDIR\QMLEVEMon.exe" 0
SectionEnd


; Uninstaller
Section "Uninstall"
    ; Remove registry keys
    DeleteRegKey HKLM "${REG_UNINST_KEY}"
    DeleteRegKey HKLM "SOFTWARE\QMLEVEMon"

    ; Remove files and uninstaller
    Delete "$INSTDIR\*.*"
    Delete $INSTDIR\uninstall.exe

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\QMLEVEMon\*.*"

    ; Remove directories used
    RMDir "$SMPROGRAMS\QMLEVEMon"
    RMDir "$INSTDIR"
SectionEnd
