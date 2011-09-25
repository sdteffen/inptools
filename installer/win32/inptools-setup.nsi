# Inptools - work with EPANET INP files
#
# Copyright (C) 2009-2011 Steffen Macke <sdteffen@sdteffen.de>
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# NOTE: this .NSI script is designed for NSIS v2.0

Name Inptools

SetCompressor lzma

# Defines
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.2.3
!define COMPANY "Steffen Macke"
!define URL http://epanet.de/inptools

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER Inptools
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI.nsh
!include Library.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ..\..\COPYING
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English
!insertmacro MUI_LANGUAGE German
!define INPTOOLS_DEFAULT_LANGFILE "locale\en.nsh"
!include "langmacros.nsh"  
!insertmacro INPTOOLS_MACRO_INCLUDE_LANGFILE "GERMAN" "locale\de.nsh"


# Installer attributes
OutFile inptools-setup-0.2.3-1.exe
InstallDir $PROGRAMFILES\Inptools
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.2.3.0
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductName Inptools
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyName "${COMPANY}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyWebsite "${URL}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileDescription ""
VIAddVersionKey /LANG=${LANG_ENGLISH} LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

# Installer sections
Section -Main SEC0000
    SetOutPath $INSTDIR\bin
    SetOverwrite on
	File ..\..\build\win32\bin\inptools-about.exe
	File ..\..\build\win32\bin\inptools-file-dialog.exe
	File ..\..\src\Release\inp2shp.exe
    File ..\..\src\Release\inpproj.exe
    File ..\..\src\Release\epanet2csv.exe
	File ..\..\src\Release\epanet2.exe
	File ..\..\build\win32\bin\epanet2.dll
	File ..\..\build\win32\bin\epanet2d.exe
	File ..\..\build\win32\bin\epanet2w.exe
	File ..\..\src\Release\epanet2i.dll
	File "C:\gtk\bin\intl.dll"

	IntCmp $LANGUAGE ${LANG_GERMAN} install_german_locale no_locale_install no_locale_install
	install_german_locale:
	SetOutPath $INSTDIR\locale
	SetOutPath $INSTDIR\locale\de
	SetOutPath $INSTDIR\locale\de\LC_MESSAGES
	File ..\..\po\epanet2.mo
	File /oname=inptools.mo ..\..\po\de.mo
	SetOutPath $INSTDIR\doc\de
	File ..\..\doc\de\inptools.chm
	File ..\..\doc\de\inptools.pdf
	
    no_locale_install:
	
	SetOutPath $INSTDIR\doc
	SetOutPath $INSTDIR\doc\en
	File ..\..\doc\en\inptools.chm
	File ..\..\doc\en\inptools.pdf

    WriteRegStr HKEY_CLASSES_ROOT ".inp" "" "Inptools.inp"
    WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp" "" ""
    WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools" "subcommands" ""
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools" "Icon" "$INSTDIR\bin\inptools-about.exe,0"
	
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd1" "" "$(INPTOOLS_OPEN_WITH_EPANET)"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd1" "Icon" "c:\program files (x86)\EPANET2\EPANET2W.EXE,0"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd1\command" "" '$INSTDIR\epanet2w.exe "%1"'
	
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd2" "" "$(INPTOOLS_CREATE_GERMAN_REPORT)"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd2\command" "" '"$INSTDIR\bin\inptools-file-dialog.exe" "Text files (*.txt)\n*.txt\nAll files (*.*)\n*.*\n" "$INSTDIR\bin\epanet2.exe" "%1"'
	
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd2" "" "$(INPTOOLS_PROJECT_GK3_WGS84)"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd2\command" "" '"$INSTDIR\bin\inptools-file-dialog.exe" "EPANET INP files (*.inp)\n*.inp\nAll files (*.*)\n*.*\n" "$INSTDIR\bin\inpproj.exe" "%1"'
	
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd8" "" "$(INPTOOLS_ABOUT)"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd8" "Icon" "$INSTDIR\bin\inptools-about.exe,0"
	WriteRegStr HKEY_CLASSES_ROOT "Inptools.inp\Shell\Inptools\Shell\cmd8\command" "" '$INSTDIR\bin\inptools-about.exe'
  
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
SectionEnd

Section -post SEC0001
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall-inptools.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
	SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall-inptools.exe
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\English Documentation.lnk" $INSTDIR\doc\en\inptools.chm
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Deutsche Dokumentation.lnk" $INSTDIR\doc\de\inptools.chm
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\English Documentation (PDF).lnk" $INSTDIR\doc\en\inptools.pdf
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Deutsche Dokumentation (PDF).lnk" $INSTDIR\doc\de\inptools.pdf
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall-inptools.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall-inptools.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
	Delete /REBOOTOK $INSTDIR\bin\inptools-about.exe
	Delete /REBOOTOK $INSTDIR\bin\inptools-file-dialog.exe
    Delete /REBOOTOK $INSTDIR\bin\inpproj.exe
	Delete /REBOOTOK $INSTDIR\bin\epanet2.exe
	Delete /REBOOTOK $INSTDIR\bin\epanet2w.exe
	Delete /REBOOTOK $INSTDIR\bin\epanet2d.exe
	Delete /REBOOTOK $INSTDIR\bin\epanet2.dll
	Delete /REBOOTOK $INSTDIR\bin\epanet2i.dll
	Delete /REBOOTOK $INSTDIR\bin\epanet2csv.exe
	Delete /REBOOTOK $INSTDIR\bin\intl.dll

	Delete /REBOOTOK $INSTDIR\locale\de\LC_MESSAGES\epanet2.mo
	Delete /REBOOTOK $INSTDIR\locale\de\LC_MESSAGES\inptools.mo

	Delete /REBOOTOK $INSTDIR\doc\de\inptools.pdf
	Delete /REBOOTOK $INSTDIR\doc\en\inptools.pdf
	Delete /REBOOTOK $INSTDIR\doc\en\inptools.chm
	Delete /REBOOTOK $INSTDIR\doc\de\inptools.chm
	
    DeleteRegValue HKEY_CLASSES_ROOT ".inp" ""
    DeleteRegValue HKEY_CLASSES_ROOT ".inp" "Content Type"
    DeleteRegKey /IfEmpty HKEY_CLASSES_ROOT ".inp"
    DeleteRegValue HKEY_CLASSES_ROOT "inpFile" ""
    DeleteRegKey /IfEmpty HKEY_CLASSES_ROOT "inpFile"

    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\English Documentation (PDF).lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\English Documentation.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Deutsche Dokumentation (PDF).lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Deutsche Dokumentation.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall-inptools.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
	RmDir /REBOOTOK $INSTDIR\doc\de
	RmDir /REBOOTOK $INSTDIR\doc\en
	RmDir /REBOOTOK $INSTDIR\doc
	RmDir /REBOOTOK $INSTDIR\locale\de\LC_MESSAGES
	RmDir /REBOOTOK $INSTDIR\locale\de
	RmDir /REBOOTOK $INSTDIR\locale
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR\bin
    RmDir /REBOOTOK $INSTDIR
	
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

# Installer functions
Function .onInit
	!insertmacro MUI_LANGDLL_DISPLAY
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd

