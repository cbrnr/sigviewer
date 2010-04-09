;Include Modern UI

  !include "MUI.nsh"

;General

  ;Name and file
  Name "SigViewer"
  OutFile "sigviewer-0.3.0-win32.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\SigViewer"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\SigViewer" ""

;Interface Settings

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "src\images\sigviewer_installer.bmp"
  !define MUI_ABORTWARNING
  
;Pages

  !insertmacro MUI_PAGE_LICENSE "COPYING"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  
;Installer Sections

Section "SigViewer" SecSigViewer

  SetOutPath "$INSTDIR"
  
  File "bin\sigviewer.exe"
  File "bin\mingwm10.dll"
  File "bin\libgcc_s_dw2-1.dll"
  File "bin\QtCore4.dll"
  File "bin\QtGui4.dll"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\SigViewer" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Start Menu Entries" SecSMEntries
  
  CreateDirectory $SMPROGRAMS\SigViewer

  CreateShortCut "$SMPROGRAMS\SigViewer\SigViewer.lnk" "$INSTDIR\sigviewer.exe" ""

  CreateShortCut "$SMPROGRAMS\SigViewer\Uninstall SigViewer.lnk" "$INSTDIR\Uninstall.exe"

SectionEnd


;Descriptions

  ;Language strings
  LangString DESC_SecSigViewer ${LANG_ENGLISH} "Installs the program with all required components."

  LangString DESC_SecSMEntries ${LANG_ENGLISH} "Adds shortcuts to the start menu."


  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSigViewer} $(DESC_SecSigViewer)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSMEntries} $(DESC_SecSMEntries)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

 
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\sigviewer.exe"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  RMDir "$INSTDIR"
  
  RMDir /r $SMPROGRAMS\SigViewer

  DeleteRegKey /ifempty HKCU "Software\SigViewer"

SectionEnd
