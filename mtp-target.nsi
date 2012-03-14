; Modern interface settings
!include "MUI.nsh"
!include "mtp-target.nsh"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "${APPNAME}-${APPVERSION}.exe"


!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "client" Section1
	; Set Section properties
	SectionIn RO
	SetOverwrite on
	
	SetOutPath "$INSTDIR\client"
	FILE "client\ReleaseDebug\mtp-target.exe"
	FILE "client\mtp_target_default.cfg"
	FILE "client\imagehlp.dll"
	FILE "client\fmod.dll"
	FILE "..\nel\lib\nel_drv_opengl_win_rd.dll"
	FILE "..\nel\lib\nel_drv_direct3d_win_rd.dll"
	FILE "..\wpkg\bin\stlport_vc645.dll"
	; Set Section Files and Shortcuts
	CreateDirectory "$SMPROGRAMS\${APPNAME}"
	CreateShortCut "$DESKTOP\mtp-target.lnk" "$INSTDIR\client\mtp-target.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Play Mtp-Target.lnk" "$INSTDIR\client\mtp-target.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\About.lnk" "http://mtptarget.free.fr/" "" "$INSTDIR\client\mtp-target.exe" 0
	
	; back up old value of .opt
	!define Index "Line${__LINE__}"
	  ReadRegStr $1 HKCR ".mtr" ""
	  StrCmp $1 "" "${Index}-NoBackup"
	    StrCmp $1 "MtpTargetReplayFile" "${Index}-NoBackup"
	    WriteRegStr HKCR ".mtr" "backup_val" $1
	"${Index}-NoBackup:"
	  WriteRegStr HKCR ".mtr" "" "MtpTargetReplayFile"
	  ReadRegStr $0 HKCR "MtpTargetReplayFile" ""
	  StrCmp $0 "" 0 "${Index}-Skip"
		WriteRegStr HKCR "MtpTargetReplayFile" "" "Mtp-Target Replay File"
		WriteRegStr HKCR "MtpTargetReplayFile\shell" "" "open"
		WriteRegStr HKCR "MtpTargetReplayFile\DefaultIcon" "" "$INSTDIR\client\mtp-target.exe,0"
	"${Index}-Skip:"
	;  WriteRegStr HKCR "MtpTargetReplayFile\shell\open\command" "" \
	;    '$INSTDIR\client\mtp-target.exe "%1"'
	;  WriteRegStr HKCR "MtpTargetReplayFile\shell\edit" "" "Edit Options File"
	;  WriteRegStr HKCR "MtpTargetReplayFile\shell\edit\command" "" \
	;    '$INSTDIR\client\mtp-target.exe "%1"'
	!undef Index
	; Rest of script


	SetOutPath "$INSTDIR\client\data"
	FILE /r "client\data\*.*"

	;FILE "client\data\font\*.*"
	;FILE "client\data\gui\*.*"
	;FILE "client\data\particule\*.*"
	;FILE "client\data\shape\*.*"
	;FILE "client\data\sound\*.*"
	;FILE "client\data\texture\*.*"
	;FILE "client\data\lua\*.*"

	SetOutPath "$INSTDIR\client\cache"
	FILE "server\data\level\*.lua"
	FILE "server\data\lua\*.lua"
	FILE "server\data\misc\*.lua"
	FILE "server\data\module\*.lua"
	FILE "server\data\particle\*.ps"
	FILE "server\data\shape\*.shape"
	FILE "server\data\texture\*.dds"
	FILE "server\data\texture\*.tga"


	;WriteUninstaller "$INSTDIR\Uninst.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

Section "server" Section2
	; Set Section properties
	SetOverwrite on
	
	SetOutPath "$INSTDIR\server"
	FILE "server\ReleaseDebug\mtp_target_service.exe"
	FILE "server\mtp_target_service_default.cfg"
	FILE "client\imagehlp.dll"
	FILE "..\nel\lib\nel_drv_opengl_win_rd.dll"
	FILE "..\nel\lib\nel_drv_direct3d_win_rd.dll"
	FILE "..\wpkg\bin\stlport_vc645.dll"

	; Set Section Files and Shortcuts
	CreateDirectory "$SMPROGRAMS\${APPNAME}"
	CreateShortCut "$SMPROGRAMS\mtp-target\Launch dedicated server.lnk" "$INSTDIR\server\mtp_target_service.exe"

	SetOutPath "$INSTDIR\server\data"
	FILE /r "server\data\*.lua"
	FILE /r "server\data\*.ps"
	FILE /r "server\data\*.shape"
	FILE /r "server\data\*.dds"
	FILE /r "server\data\*.tga"
	
	;FILE "server\data\level\*.lua"
	;FILE "server\data\lua\*.lua"
	;FILE "server\data\misc\*.lua"
	;FILE "server\data\module\*.lua"
	;FILE "server\data\particle\*.ps"
	;FILE "server\data\shape\*.shape"
	;FILE "server\data\texture\*.dds"
	;FILE "server\data\texture\*.tga"


SectionEnd


Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKCU "Software\${APPNAME}" "version" "${APPVERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} "Client"
	!insertmacro MUI_DESCRIPTION_TEXT ${Section2} "Server"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	RmDir /r "$INSTDIR\client"
	RmDir /r "$INSTDIR\server"

	;Delete Start Menu Shortcuts
	Delete "$SMPROGRAMS\${APPNAME}\*.*"
	Delete "$DESKTOP\mtp-target.lnk" 
  RmDir "$SMPROGRAMS\${APPNAME}"

  RMDir "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\${APPNAME}"

SectionEnd

BrandingText "plopida"

; eof