; Modern interface settings
!include "MUI.nsh"
!include "mtp-target.nsh"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "${APPNAME}-update-${APPOLDVERSION}-${APPVERSION}.exe"


!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_NOAUTOCLOSE
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

ShowInstDetails show

Section "base" Section1
	call IsMtptInstalledUpdate
	; Set Section properties
	SectionIn RO
	SetOverwrite on
	
	SetOutPath "$INSTDIR\client"
	FILE "mtp-target.pat"
	Rename "$INSTDIR\client\mtp-target.exe" "$INSTDIR\client\mtp-target_old.exe" 
	InitPluginsDir
	DetailPrint "Updating $INSTDIR\client\mtp-target.exe..."
	vpatch::vpatchfile "$INSTDIR\client\mtp-target.pat" "$INSTDIR\client\mtp-target_old.exe" "$INSTDIR\client\mtp-target.exe"
	Pop $R0
	;DetailPrint "Result: $R0"
	Delete "$INSTDIR\client\mtp-target.pat"
	StrCmp $R0 "OK" patch_ok
	Rename "$INSTDIR\client\mtp-target_old.exe" "$INSTDIR\client\mtp-target.exe" 
	Abort $R0
	Goto patch_error
	patch_ok:
	Delete "$INSTDIR\client\mtp-target_old.exe"
	patch_error:
	;FILE "client\ReleaseDebug\mtp-target.exe"
	FILE "client\mtp_target_default.cfg"
	;FILE "..\nel\lib\nel_drv_opengl_win_rd.dll"
	;FILE "..\nel\lib\nel_drv_direct3d_win_rd.dll"


	; Set Section Files and Shortcuts
	;CreateDirectory "$SMPROGRAMS\${APPNAME}"
	;CreateShortCut "$SMPROGRAMS\${APPNAME}\Play Mtp-Target.lnk" "$INSTDIR\mtp-target.exe"
	;CreateShortCut "$SMPROGRAMS\${APPNAME}\About.lnk" "http://mtptarget.free.fr/" "" "$INSTDIR\mtp-target.exe" 0
	
	
	SetOutPath "$INSTDIR\client\data"
	;FILE "client\data\font\*.ttf"
	
	WriteUninstaller "$INSTDIR\Uninst.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	
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
SectionEnd

Section "server" Section2
	call IsMtptInstalledUpdate
	; Set Section properties
	SetOverwrite on
	
	SetOutPath "$INSTDIR\server"
	;FILE "server\ReleaseDebug\mtp_target_service.exe"
	FILE "server\mtp_target_service_default.cfg"
	;FILE "..\wpkg\bin\stlport_vc645.dll"

	; Set Section Files and Shortcuts
	;CreateDirectory "$SMPROGRAMS\${APPNAME}"
	;CreateShortCut "$SMPROGRAMS\mtp-target\Launch dedicated server.lnk" "$INSTDIR\mtp_target_service.exe" "" "$INSTDIR\mtp_target_service.exe" 0

SectionEnd


Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKCU "Software\${APPNAME}" "version" "${APPVERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	;WriteUninstaller "$INSTDIR\uninstall.exe"

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

	Delete "$INSTDIR\*.*" 
	Delete "$INSTDIR\cache\*.*" 
	RmDir "$INSTDIR\cache"
	Delete "$INSTDIR\data\*.*" 
	RmDir "$INSTDIR\data"
	Delete "$INSTDIR\replay\*.*" 
	RmDir "$INSTDIR\replay"

	;Delete Start Menu Shortcuts
	Delete "$SMPROGRAMS\${APPNAME}\*.*"
  RmDir "$SMPROGRAMS\${APPNAME}"

  RMDir "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\${APPNAME}"

SectionEnd

BrandingText "plopida"

; eof