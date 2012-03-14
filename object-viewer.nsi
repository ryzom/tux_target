; Modern interface settings
!include "MUI.nsh"
!include "object-viewer.nsh"

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

Section "object viewer" Section1
	; Set Section properties
	SectionIn RO
	SetOverwrite on
	
	SetOutPath "$INSTDIR\object-viewer"
	FILE "..\nel\lib\nel_drv_opengl_win_rd.dll"
;	FILE "..\nel\lib\nel_drv_direct3d_win_rd.dll"
	FILE "..\nel\lib\object_viewer_rd.dll"
	FILE "..\nel\tools\3d\object_viewer_exe\ReleaseDebug\object_viewer.exe"
	FILE "object_viewer.cfg"
	FILE "..\wpkg\bin\stlport_vc645.dll"
	; Set Section Files and Shortcuts
	CreateDirectory "$SMPROGRAMS\${APPNAME}"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\object viewer.lnk" "$INSTDIR\object-viewer\object_viewer.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\About.lnk" "http://mtp-target.dyndns.org/wiki/index.php/Main/ObjectViewer"
	
	SetOutPath "$INSTDIR\object-viewer\data"
	FILE "client\cache\entity_pingoo_close.shape"
	FILE "client\cache\entity_pingoo_open.shape"
	FILE "..\mtp-target-original-data\texture\ping_ball_blue.tga"
	FILE "..\mtp-target-original-data\texture\ping_ball_blue.psd"



	;WriteUninstaller "$INSTDIR\Uninst.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"

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
;	!insertmacro MUI_DESCRIPTION_TEXT ${Section2} "Server"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	RmDir /r "$INSTDIR\object-viewer"

	;Delete Start Menu Shortcuts
	Delete "$SMPROGRAMS\${APPNAME}\*.*"
  RmDir "$SMPROGRAMS\${APPNAME}"

  RMDir "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\${APPNAME}"

SectionEnd

BrandingText "plopida"

; eof