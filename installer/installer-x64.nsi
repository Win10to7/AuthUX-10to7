!include MUI2.nsh
!include LogicLib.nsh
!include x64.nsh
!include WinVer.nsh

# The install path is hardcoded.
# We need both a x64 and x86 OpenWith.exe and putting both
# in Program Files won't work. Having two directory choices
# would also be a mess.

Unicode true
Name "AuthUX"
Outfile "build\AuthUX-setup-x64.exe"
RequestExecutionLevel admin
ManifestSupportedOS all

!define MUI_ICON "installer.ico"
!define MUI_UNICON "installer.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "welcome.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
!define MUI_UNHEADERIMAGE_BITMAP "header.bmp"
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!macro LANG_LOAD LANGLOAD
    !insertmacro MUI_LANGUAGE "${LANGLOAD}"
    !include "l10n\${LANGLOAD}.nsh"
    !undef LANG
!macroend
 
!macro LANG_STRING NAME VALUE
    LangString "${NAME}" "${LANG_${LANG}}" "${VALUE}"
!macroend

!insertmacro LANG_LOAD "English"

Function .onInit
    # NSIS produces an x86-32 installer. Deny installation if
    # we're not on a x86-64 system running WOW64.
    ${IfNot} ${RunningX64}
        MessageBox MB_OK|MB_ICONSTOP "$(STRING_NOT_X64)"
        Quit
    ${EndIf}
    
    # Need at least Windows 10 2004-22H2.
    ${IfNot} ${AtLeastBuild} 19044
	${OrIfNot} ${AtMostBuild} 19045
        MessageBox MB_OK|MB_ICONSTOP "$(STRING_NOT_WIN8)"
        Quit
    ${EndIf}
	
	StrCpy $INSTDIR "$PROGRAMFILES64\AuthUX"
FunctionEnd

Section "AuthUX" AuthUX
    # Make sure install directories are clean
    RMDir /r "$INSTDIR\"

    # Install x86-64 files
    SetOutPath "$INSTDIR\"
    WriteUninstaller "$INSTDIR\uninstall.exe"
    File "..\x64\Release\AuthUX.dll"
	File "..\x64\Release\AuthUX.pdb"

    # Create Uninstall entry
    SetRegView 64
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "DisplayName" "AuthUX"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "Publisher" "explorer7-team"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "DisplayVersion" "1.0.0"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX" \
                 "NoRepair" 1

    # Make LogonController use our server
    ReadEnvStr $0 "USERNAME"
    AccessControl::SetRegKeyOwner HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" $0
    AccessControl::GrantOnRegKey HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" $0 FullAccess
    WriteRegExpandStr HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" \
        "DllPath" "$INSTDIR\AuthUX.dll"
	
	AccessControl::SetRegKeyOwner HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" $0
    AccessControl::GrantOnRegKey HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" $0 FullAccess
    WriteRegExpandStr HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" \
        "DllPath" "$INSTDIR\AuthUX.dll" 
SectionEnd

Section "Uninstall"
    # Delete files
    RMDir /r "$INSTDIR"

    # Revert to default LogonController server.
    SetRegView 64
    ReadEnvStr $0 "USERNAME"
    AccessControl::SetRegKeyOwner HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" $0
    AccessControl::GrantOnRegKey HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" $0 FullAccess
    WriteRegExpandStr HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LogonUX" \
        "DllPath" "%SystemRoot%\system32\Windows.UI.Logon.dll"
		
	AccessControl::SetRegKeyOwner HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" $0
    AccessControl::GrantOnRegKey HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" $0 FullAccess
    WriteRegExpandStr HKLM "SOFTWARE\Microsoft\WindowsRuntime\ActivatableClassId\Windows.Internal.UI.Logon.Controller.LockScreenHost" \
        "DllPath" "%SystemRoot%\system32\logoncontroller.dll"

    # Delete uninstall entry
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AuthUX"
SectionEnd
