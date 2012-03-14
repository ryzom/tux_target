; Define your application name
!define APPNAME "object-viewer"
!define APPVERSION "1.0.1"
!define APPOLDVERSION "1.0.0"
!define APPNAMEANDVERSION "${APPNAME} ${APPVERSION}"

Function IsMtptInstalled
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

