﻿; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{3DF66435-0FE1-4E20-B7CE-72C48D809045}
AppName=もじのべんきょう
AppVerName=もじのべんきょう 2.0
AppPublisher=片山博文MZ
VersionInfoProductName=もじのべんきょう
VersionInfoDescription=もじのべんきょう
VersionInfoCompany=片山博文MZ
VersionInfoVersion=2.0
VersionInfoCopyright=Copyright (C) 2009-2020 Katayama Hirofumi MZ. All rights reserved.
AppPublisherURL=https://katahiromz.web.fc2.com/mojiben/
AppSupportURL=https://katahiromz.web.fc2.com/mojiben/
AppUpdatesURL=https://katahiromz.web.fc2.com/mojiben/
DefaultDirName={pf}\Moji No Benkyou
DefaultGroupName=もじのべんきょう
DisableProgramGroupPage=yes
OutputDir=.
OutputBaseFilename=mojiben-2.0-setup
Compression=lzma
SolidCompression=yes
ShowLanguageDialog=auto

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: ja; MessagesFile: "compiler:Languages\Japanese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "build\mojiben1.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\mojiben2.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\mojiben3.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\mojiben4.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\読んでね"; Filename: "{app}\README.txt"
Name: "{group}\ライセンス"; Filename: "{app}\LICENSE.txt"
Name: "{group}\(1) ひらがなとかたかな"; Filename: "{app}\mojiben1.exe"
Name: "{group}\(2) アルファベット"; Filename: "{app}\mojiben2.exe"
Name: "{group}\(3) すうじとかず"; Filename: "{app}\mojiben3.exe"
Name: "{group}\(4) 小学校一年生でならうかん字"; Filename: "{app}\mojiben4.exe"
Name: "{group}\アンインストール"; Filename: "{uninstallexe}"
Name: "{commondesktop}\もじべん (1) ひらがなとかたかな"; Filename: "{app}\mojiben1.exe"; Tasks: desktopicon
Name: "{commondesktop}\もじべん (2) アルファベット"; Filename: "{app}\mojiben2.exe"; Tasks: desktopicon
Name: "{commondesktop}\もじべん (3) すうじとかず"; Filename: "{app}\mojiben3.exe"; Tasks: desktopicon
Name: "{commondesktop}\もじべん (4) 小学校一年生でならうかん字"; Filename: "{app}\mojiben4.exe"; Tasks: desktopicon