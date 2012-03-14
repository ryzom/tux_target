; Define your application name
!define APPNAME "mtp-target"
!define APPVERSION "1.2.2"
!define APPOLDVERSION "1.2.1"
!define APPNAMEANDVERSION "${APPNAME} ${APPVERSION}"

Function IsMtptInstalledUpdate
 Push $R0
 ClearErrors
 ReadRegStr $R0 HKCU "Software\${APPNAME}" "version"
 IfErrors lbl_na
   StrCmp $R0 "${APPOLDVERSION}" lbl_end
     MessageBox MB_OK|MB_ICONSTOP "This update if for version ${APPOLDVERSION}" 
     Quit 
 lbl_na:
   MessageBox MB_OK|MB_ICONSTOP "Mtp Target not installed." 
   Quit 
 lbl_end:
 Pop $R0
FunctionEnd

Function IsMtptInstalledPatch
 Push $R0
 ClearErrors
 ReadRegStr $R0 HKCU "Software\${APPNAME}" "version"
 IfErrors lbl_na
   StrCmp $R0 "${APPOLDVERSION}" lbl_end
     MessageBox MB_OK|MB_ICONSTOP "This patch if for version ${APPVERSION}" 
     Quit 
 lbl_na:
   MessageBox MB_OK|MB_ICONSTOP "Mtp Target not installed." 
   Quit 
 lbl_end:
 Pop $R0
FunctionEnd
