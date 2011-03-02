;Include Modern UI

  !include "MUI.nsh"
  !define VERSION 0.5.1

;General

  ;Name and file
  Name "SigViewer"
  OutFile "sigviewer-${VERSION}-win32.exe"


  ;Default installation folder
  InstallDir "$PROGRAMFILES\SigViewer"
  
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
  
  File "bin\release\sigviewer.exe"
  File "bin\release\mingwm10.dll"
  File "bin\release\libgcc_s_dw2-1.dll"
  File "bin\release\QtCore4.dll"
  File "bin\release\QtGui4.dll"
  File "bin\release\QtXml4.dll"
  
  ;Store uninstall information in Add/Remove Programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "DisplayName" "SigViewer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "Publisher" "Graz University of Technology"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "DisplayIcon" "$\"$INSTDIR\sigviewer.exe$\""
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer" "EstimatedSize" "15000"
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Start Menu Entries" SecSMEntries
  SetShellVarContext all
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
  SetShellVarContext all
  ;ADD YOUR OWN FILES HERE...
  
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\sigviewer.exe"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtXml4.dll"
  RMDir "$INSTDIR"
  
  Delete "$SMPROGRAMS\SigViewer\SigViewer.lnk"
  Delete "$SMPROGRAMS\SigViewer\Uninstall SigViewer.lnk"
  RMDir /r $SMPROGRAMS\SigViewer

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SigViewer"

SectionEnd
