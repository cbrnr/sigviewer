[Setup]
AppId={{3A5B7C8D-1E2F-4A6B-8C9D-0E1F2A3B4C5D}
AppName=SigViewer
AppVersion={#version}
AppVerName=SigViewer {#version}
AppPublisher=SigViewer Developers
AppPublisherURL=https://github.com/cbrnr/sigviewer
AppSupportURL=https://github.com/cbrnr/sigviewer
AppUpdatesURL=https://github.com/cbrnr/sigviewer/releases
DefaultDirName={autopf}\SigViewer
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
UsePreviousAppDir=no
DisableProgramGroupPage=yes
OutputBaseFilename=SigViewer-{#version}
OutputDir=..\..\build
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupIconFile=..\..\src\images\sigviewer.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "..\..\build\sigviewer-windows\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\SigViewer"; Filename: "{app}\sigviewer.exe"
Name: "{autodesktop}\SigViewer"; Filename: "{app}\sigviewer.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\sigviewer.exe"; Description: "{cm:LaunchProgram,SigViewer}"; Flags: nowait postinstall skipifsilent
