// This file is part of Mtp Target.
// Copyright (C) 2008 Vialek
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Vianney Lecroart - gpl@vialek.com

#include "stdpch.h"

#include "fi_ime.h"
#include "3d_task.h"
#include "font_manager.h"
#include "chat_task.h"

#ifndef NL_OS_WINDOWS

void CFullIME::init() { }
void CFullIME::release() { }
void CFullIME::onFocus( bool hasFocus ) { }
void CFullIME::submitChar( ucchar compositionChar ) { }
bool CFullIME::staticMsgProc( uint32 uMsg, uint32 wParam, uint32 lParam ) { return false; }
bool CFullIME::msgProc( uint32 uMsg, uint32 wParam, uint32 lParam ) { return false; }
void  CFullIME::displayIMEInfo() { }
ucstring CFullIME::indicator() const { return ucstring(""); }
ucstring CFullIME::composition() const { return ucstring(""); }

#else

// Adapted from the DX9 CustomUI example

#define IMM32_DLLNAME "\\imm32.dll"
#define VER_DLLNAME "\\version.dll"

// IME constants
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )   // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )   // New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )   // New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )   // New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )   // New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )   // New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )   // New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41 ( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )   // MSPY1.5  // SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42 ( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )   // MSPY2    // Win2k/WinME
#define IMEID_CHS_VER53 ( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )   // MSPY3    // WinXP

// Function pointers: IMM32
INPUTCONTEXT* (WINAPI * CFullIME::_ImmLockIMC)( HIMC ) = NULL;
BOOL (WINAPI * CFullIME::_ImmUnlockIMC)( HIMC ) = NULL;
LPVOID (WINAPI * CFullIME::_ImmLockIMCC)( HIMCC ) = NULL;
BOOL (WINAPI * CFullIME::_ImmUnlockIMCC)( HIMCC ) = NULL;
BOOL (WINAPI * CFullIME::_ImmDisableTextFrameService)( DWORD ) = NULL;
LONG (WINAPI * CFullIME::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD ) = NULL;
LONG (WINAPI * CFullIME::_ImmSetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD, LPVOID, DWORD ) = NULL;
DWORD (WINAPI * CFullIME::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD ) = NULL;
HIMC (WINAPI * CFullIME::_ImmGetContext)( HWND ) = NULL;
BOOL (WINAPI * CFullIME::_ImmReleaseContext)( HWND, HIMC ) = NULL;
HIMC (WINAPI * CFullIME::_ImmAssociateContext)( HWND, HIMC ) = NULL;
BOOL (WINAPI * CFullIME::_ImmGetOpenStatus)( HIMC ) = NULL;
BOOL (WINAPI * CFullIME::_ImmSetOpenStatus)( HIMC, BOOL ) = NULL;
BOOL (WINAPI * CFullIME::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD ) = NULL;
HWND (WINAPI * CFullIME::_ImmGetDefaultIMEWnd)( HWND ) = NULL;
UINT (WINAPI * CFullIME::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT ) = NULL;
UINT (WINAPI * CFullIME::_ImmGetVirtualKey)( HWND ) = NULL;
BOOL (WINAPI * CFullIME::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD ) = NULL;
BOOL (WINAPI * CFullIME::_ImmSetConversionStatus)( HIMC, DWORD, DWORD ) = NULL;
BOOL (WINAPI * CFullIME::_ImmSimulateHotKey)( HWND, DWORD ) = NULL;
BOOL (WINAPI * CFullIME::_ImmIsIME)( HKL ) = NULL;

// Function pointers: Newer IMEs
UINT (WINAPI * CFullIME::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) = NULL;
BOOL (WINAPI * CFullIME::_ShowReadingWindow)( HIMC, BOOL ) = NULL;

// Function pointers: Version library imports
BOOL (APIENTRY * CFullIME::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT ) = NULL;
BOOL (APIENTRY * CFullIME::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID ) = NULL;
DWORD (APIENTRY * CFullIME::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD ) = NULL;

HINSTANCE CFullIME::_HDllImm32;
HINSTANCE CFullIME::_HDllVer;
HKL       CFullIME::_HklCurrent;
bool      CFullIME::_IsVerticalCand;  // Indicates that the candidates are listed vertically
WCHAR     CFullIME::_Indicators[5][3] = // String to draw to indicate current input locale
            {
                L"En",
                L"\x7B80",
                L"\x7E41",
                L"\xAC00",
                L"\x3042",
            };
LPWSTR    CFullIME::_CurrIndicator   // Points to an indicator string that corresponds to current input locale
            = CFullIME::_Indicators[0];
bool      CFullIME::_IsInsertOnType = false;     // Insert the character as soon as a key is pressed (Korean behavior)
HINSTANCE CFullIME::_HDllIme;           // Instance handle of the current IME module
HIMC      CFullIME::_HImcDef;           // Default input context
CFullIME::IMESTATE  CFullIME::_ImeState = IMEUI_STATE_OFF;
//bool      CFullIME::s_bEnableImeSystem;  // Whether the IME system is active
POINT     CFullIME::s_ptCompString;      // Composition string position. Updated every frame.
int       CFullIME::_CompCaret;
int       CFullIME::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CUnicodeBuffer CFullIME::_CompString = CUnicodeBuffer( MAX_COMPSTRING_SIZE );
BYTE      CFullIME::_CompStringAttr[MAX_COMPSTRING_SIZE];
DWORD     CFullIME::_CompStringClause[MAX_COMPSTRING_SIZE];
WCHAR     CFullIME::_ReadingString[32];
CFullIME::CCandList CFullIME::_CandList;       // Data relevant to the candidate list
bool      CFullIME::_IsReadingWindowVisible; // Indicates whether reading window is visible
bool      CFullIME::_IsHorizontalReading; // Indicates whether the reading window is vertical or horizontal
bool      CFullIME::_IsChineseIME;
vector< CFullIME::CInputLocale > CFullIME::_Locales; // Array of loaded keyboard layout on system

#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

//#define PLACEHOLDERPROC( APIName ) \
//   _##APIName = Dummy_##APIName
#define PLACEHOLDERPROC( APIName ) \
    _##APIName = NULL

/*
 * Tested OK
 */
/*static*/ void CFullIME::init()
{
    FARPROC Temp;

    char dllPath[MAX_PATH+1] = "";
    if( !::GetSystemDirectoryA( dllPath, MAX_PATH+1 ) )
        return;
	::strncat( dllPath, IMM32_DLLNAME, MAX_PATH );
    _HDllImm32 = LoadLibraryA( dllPath );
    if( _HDllImm32 )
    {
        GETPROCADDRESS( _HDllImm32, ImmLockIMC, Temp );
        GETPROCADDRESS( _HDllImm32, ImmUnlockIMC, Temp );
        GETPROCADDRESS( _HDllImm32, ImmLockIMCC, Temp );
        GETPROCADDRESS( _HDllImm32, ImmUnlockIMCC, Temp );
        GETPROCADDRESS( _HDllImm32, ImmDisableTextFrameService, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetCompositionStringW, Temp );
        GETPROCADDRESS( _HDllImm32, ImmSetCompositionStringW, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetCandidateListW, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetContext, Temp );
        GETPROCADDRESS( _HDllImm32, ImmReleaseContext, Temp );
        GETPROCADDRESS( _HDllImm32, ImmAssociateContext, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetOpenStatus, Temp );
        GETPROCADDRESS( _HDllImm32, ImmSetOpenStatus, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetConversionStatus, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetDefaultIMEWnd, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetIMEFileNameA, Temp );
        GETPROCADDRESS( _HDllImm32, ImmGetVirtualKey, Temp );
        GETPROCADDRESS( _HDllImm32, ImmNotifyIME, Temp );
        GETPROCADDRESS( _HDllImm32, ImmSetConversionStatus, Temp );
        GETPROCADDRESS( _HDllImm32, ImmSimulateHotKey, Temp );
        GETPROCADDRESS( _HDllImm32, ImmIsIME, Temp );
    }

    if( !::GetSystemDirectoryA( dllPath, MAX_PATH+1 ) )
        return;
    ::strncat( dllPath, VER_DLLNAME, MAX_PATH );
    _HDllVer = LoadLibraryA( dllPath );
    if( _HDllVer )
    {
        GETPROCADDRESS( _HDllVer, VerQueryValueA, Temp );
        GETPROCADDRESS( _HDllVer, GetFileVersionInfoA, Temp );
        GETPROCADDRESS( _HDllVer, GetFileVersionInfoSizeA, Temp );
    }

    // Save the default input context
	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
    _HImcDef = _ImmGetContext( hWnd );
    _ImmReleaseContext( hWnd, _HImcDef );

    // Populate _Locales with the list of keyboard layouts (was in WM_ACTIVATEAPP)
    UINT cKL = GetKeyboardLayoutList( 0, NULL );
    _Locales.clear();
    HKL *phKL = new HKL[cKL];
    if( phKL )
    {
        GetKeyboardLayoutList( cKL, phKL );
        for( UINT i = 0; i < cKL; ++i )
        {
            CInputLocale Locale;

            // Filter out East Asian languages that are not IME.
            if( ( PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_CHINESE ||
                  PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_JAPANESE ||
                  PRIMARYLANGID( LOWORD( phKL[i] ) ) == LANG_KOREAN ) &&
                  !_ImmIsIME( phKL[i] ) )
                  continue;

            // If this language is already in the list, don't add it again.
            bool bBreak = false;
            for( int e = 0; e < (int)_Locales.size(); ++e )
                if( LOWORD( _Locales[e].m_hKL ) ==
                    LOWORD( phKL[i] ) )
                {
                    bBreak = true;
                    break;
                }
            if( bBreak )
                break;

            Locale.m_hKL = phKL[i];
            WCHAR wszDesc[128] = L"";
            switch( PRIMARYLANGID( LOWORD( phKL[i] ) ) )
            {
                // Simplified Chinese
                case LANG_CHINESE:
                    switch( SUBLANGID( LOWORD( phKL[i] ) ) )
                    {
                        case SUBLANG_CHINESE_SIMPLIFIED:
                            wcsncpy( Locale.m_wszLangAbb, _Indicators[INDICATOR_CHS], 3 );
                            break;
                        case SUBLANG_CHINESE_TRADITIONAL:
                            wcsncpy( Locale.m_wszLangAbb, _Indicators[INDICATOR_CHT], 3 );
                            break;
                        default:    // unsupported sub-language
                            GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                            Locale.m_wszLangAbb[0] = wszDesc[0];
                            Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                            Locale.m_wszLangAbb[2] = L'\0';
                            break;
                    }
                    break;
                // Korean
                case LANG_KOREAN:
                    wcsncpy( Locale.m_wszLangAbb, _Indicators[INDICATOR_KOREAN], 3 );
                    break;
                // Japanese
                case LANG_JAPANESE:
                    wcsncpy( Locale.m_wszLangAbb, _Indicators[INDICATOR_JAPANESE], 3 );
                    break;         
                default:
                    // A non-IME language.  Obtain the language abbreviation
                    // and store it for rendering the indicator later.
                    GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
                    Locale.m_wszLangAbb[0] = wszDesc[0];
                    Locale.m_wszLangAbb[1] = towlower( wszDesc[1] );
                    Locale.m_wszLangAbb[2] = L'\0';
                    break;
            }

            GetLocaleInfoW( MAKELCID( LOWORD( phKL[i] ), SORT_DEFAULT ), LOCALE_SLANGUAGE, wszDesc, 128 );
            wcsncpy( Locale.m_wszLang, wszDesc, 64 );

            _Locales.push_back( Locale );
        }
        delete[] phKL;
    }

}


/*
 *
 */
/*static*/ void CFullIME::setupImeApi()
{
    char imeFile[MAX_PATH + 1];

    _GetReadingString = NULL;
    _ShowReadingWindow = NULL;
    if( _ImmGetIMEFileNameA( _HklCurrent, imeFile, sizeof(imeFile)/sizeof(imeFile[0]) - 1 ) == 0 )
        return;

    if( _HDllIme ) FreeLibrary( _HDllIme );
    _HDllIme = LoadLibraryA( imeFile );
    if ( !_HDllIme )
        return;
    _GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
        ( GetProcAddress( _HDllIme, "GetReadingString" ) );
    _ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
        ( GetProcAddress( _HDllIme, "ShowReadingWindow" ) );
}

/*
 * Tested OK with no _HDllIme
 */
/*static*/ void CFullIME::release()
{
    if( _HDllImm32 )
    {
        PLACEHOLDERPROC( ImmLockIMC );
        PLACEHOLDERPROC( ImmUnlockIMC );
        PLACEHOLDERPROC( ImmLockIMCC );
        PLACEHOLDERPROC( ImmUnlockIMCC );
        PLACEHOLDERPROC( ImmDisableTextFrameService );
        PLACEHOLDERPROC( ImmGetCompositionStringW );
		PLACEHOLDERPROC( ImmSetCompositionStringW );
        PLACEHOLDERPROC( ImmGetCandidateListW );
        PLACEHOLDERPROC( ImmGetContext );
        PLACEHOLDERPROC( ImmReleaseContext );
        PLACEHOLDERPROC( ImmAssociateContext );
        PLACEHOLDERPROC( ImmGetOpenStatus );
        PLACEHOLDERPROC( ImmSetOpenStatus );
        PLACEHOLDERPROC( ImmGetConversionStatus );
        PLACEHOLDERPROC( ImmGetDefaultIMEWnd );
        PLACEHOLDERPROC( ImmGetIMEFileNameA );
        PLACEHOLDERPROC( ImmGetVirtualKey );
        PLACEHOLDERPROC( ImmNotifyIME );
        PLACEHOLDERPROC( ImmSetConversionStatus );
        PLACEHOLDERPROC( ImmSimulateHotKey );
        PLACEHOLDERPROC( ImmIsIME );

        FreeLibrary( _HDllImm32 );
        _HDllImm32 = NULL;
    }
    if( _HDllIme )
    {
        PLACEHOLDERPROC( GetReadingString );
        PLACEHOLDERPROC( ShowReadingWindow );

        FreeLibrary( _HDllIme );
        _HDllIme = NULL;
    }
    if( _HDllVer )
    {
        PLACEHOLDERPROC( VerQueryValueA );
        PLACEHOLDERPROC( GetFileVersionInfoA );
        PLACEHOLDERPROC( GetFileVersionInfoSizeA );

        FreeLibrary( _HDllVer );
        _HDllVer = NULL;
    }
}

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

//--------------------------------------------------------------------------------------
//  GetImeId( UINT uIndex )
//      returns 
//  returned value:
//  0: In the following cases
//      - Non Chinese IME input locale
//      - Older Chinese IME
//      - Other error cases
//
//  Otherwise:
//      When uIndex is 0 (default)
//          bit 31-24:  Major version
//          bit 23-16:  Minor version
//          bit 15-0:   Language ID
//      When uIndex is 1
//          pVerFixedInfo->dwFileVersionLS
//
//  Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//  
/*static*/ DWORD CFullIME::getImeId( UINT uIndex )
{
    static HKL hklPrev = 0;
    static DWORD dwID[2] = { 0, 0 };  // Cache the result
    
    DWORD   dwVerSize;
    DWORD   dwVerHandle;
    LPVOID  lpVerBuffer;
    LPVOID  lpVerData;
    UINT    cbVerData;
    char    szTmp[1024];

    if( uIndex >= sizeof( dwID ) / sizeof( dwID[0] ) )
        return 0;

    if( hklPrev == _HklCurrent )
        return dwID[uIndex];

    hklPrev = _HklCurrent;  // Save for the next invocation

    // Check if we are using an older Chinese IME
    if( !( ( _HklCurrent == _CHT_HKL ) || ( _HklCurrent == _CHT_HKL2 ) || ( _HklCurrent == _CHS_HKL ) ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Obtain the IME file name
    if ( !_ImmGetIMEFileNameA( _HklCurrent, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
    {
        dwID[0] = dwID[1] = 0;
        return dwID[uIndex];
    }

    // Check for IME that doesn't implement reading string API
    if ( !_GetReadingString )
    {
        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) )
        {
            dwID[0] = dwID[1] = 0;
            return dwID[uIndex];
        }
    }

    dwVerSize = _GetFileVersionInfoSizeA( szTmp, &dwVerHandle );
    if( dwVerSize )
    {
        lpVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
        if( lpVerBuffer )
        {
            if( _GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) )
            {
                if( _VerQueryValueA( lpVerBuffer, "\\", &lpVerData, &cbVerData ) )
                {
                    DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                    dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                    if( _GetReadingString
                        ||
                        ( getLanguage() == LANG_CHT &&
                          ( dwVer == MAKEIMEVERSION(4, 2) || 
                            dwVer == MAKEIMEVERSION(4, 3) || 
                            dwVer == MAKEIMEVERSION(4, 4) || 
                            dwVer == MAKEIMEVERSION(5, 0) ||
                            dwVer == MAKEIMEVERSION(5, 1) ||
                            dwVer == MAKEIMEVERSION(5, 2) ||
                            dwVer == MAKEIMEVERSION(6, 0) ) )
                        ||
                        ( getLanguage() == LANG_CHS &&
                          ( dwVer == MAKEIMEVERSION(4, 1) ||
                            dwVer == MAKEIMEVERSION(4, 2) ||
                            dwVer == MAKEIMEVERSION(5, 3) ) )
                      )
                    {
                        dwID[0] = dwVer | getLanguage();
                        dwID[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                    }
                }
            }
            HeapFree( GetProcessHeap(), 0, lpVerBuffer );
        }
    }

    return dwID[uIndex];
}

/*
 *
 */
/*static*/ void CFullIME::checkInputLocale()
{
    static HKL hklPrev = 0;
    _HklCurrent = GetKeyboardLayout( 0 );
    if ( hklPrev == _HklCurrent )
        return;

    hklPrev = _HklCurrent;
    switch ( getPrimaryLanguage() )
    {
        // Simplified Chinese
        case LANG_CHINESE:
            _IsVerticalCand = true;
            switch ( getSubLanguage() )
            {
                case SUBLANG_CHINESE_SIMPLIFIED:
                    _CurrIndicator = _Indicators[INDICATOR_CHS];
                    _IsVerticalCand = (getImeId() == 0);
                    break;
                case SUBLANG_CHINESE_TRADITIONAL:
                    _CurrIndicator = _Indicators[INDICATOR_CHT];
                    break;
                default:    // unsupported sub-language
                    _CurrIndicator = _Indicators[INDICATOR_NON_IME];
                    break;
            }
            break;
        // Korean
        case LANG_KOREAN:
            _CurrIndicator = _Indicators[INDICATOR_KOREAN];
            _IsVerticalCand = false;
            break;
        // Japanese
        case LANG_JAPANESE:
            _CurrIndicator = _Indicators[INDICATOR_JAPANESE];
            _IsVerticalCand = true;
            break;
        default:
            // A non-IME language.  Obtain the language abbreviation
            // and store it for rendering the indicator later.
            _CurrIndicator = _Indicators[INDICATOR_NON_IME];
    }

    // If non-IME, use the language abbreviation.
    if( _CurrIndicator == _Indicators[INDICATOR_NON_IME] )
    {
        WCHAR lang[5];
        GetLocaleInfoW( MAKELCID( LOWORD( _HklCurrent ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, lang, 5 );
        _CurrIndicator[0] = lang[0];
        _CurrIndicator[1] = towlower( lang[1] );
    }
}

/*
 *
 */
/*static*/ void CFullIME::checkToggleState()
{
    checkInputLocale();
    bool bIme = _ImmIsIME( _HklCurrent ) != 0;
    _IsChineseIME = (getPrimaryLanguage() == LANG_CHINESE) && bIme;

    HIMC hImc;
	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
    if( NULL != (hImc = _ImmGetContext( hWnd )) )
    {
        if( _IsChineseIME )
        {
            DWORD dwConvMode, dwSentMode;
            _ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
            _ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
        }
        else
        {
            _ImeState = ( bIme && _ImmGetOpenStatus( hImc ) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }
        _ImmReleaseContext( hWnd, hImc );
    }
    else
        _ImeState = IMEUI_STATE_OFF;
}

/*
 *
 */
void CFullIME::resetCompositionString()
{
    _CompCaret = 0;
    _CompString.setText( (ucchar*)L"" );
    ZeroMemory( _CompStringAttr, sizeof(_CompStringAttr) );
}

/*
 *
 */
void CFullIME::truncateCompString( bool bUseBackSpace, int iNewStrLen )
{
    if( !_IsInsertOnType )
        return;

    int cc = (int) _CompString.size();
    if ( ! ( iNewStrLen == 0 || iNewStrLen >= cc ) )
	{
		nlwarning( "NewStrLen == 0 || iNewStrLen >= cc (%d)", iNewStrLen );
		return;
	}

	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();

    // Send right arrow keystrokes to move the caret
    //   to the end of the composition string.
    for (int i = 0; i < cc - _CompCaret; ++i )
        SendMessage( hWnd, WM_KEYDOWN, VK_RIGHT, 0 ); // TODO: Support DirectInput mode... (handle all SendMessage* and PostMessage*)
    SendMessage( hWnd, WM_KEYUP, VK_RIGHT, 0 );

    if( bUseBackSpace /*|| _InsertMode*/ )
        iNewStrLen = 0;

    // The caller sets bUseBackSpace to false if there's possibility of sending
    // new composition string to the app right after this function call.
    // 
    // If the app is in overwriting mode and new comp string is 
    // shorter than current one, delete previous comp string 
    // till it's same long as the new one. Then move caret to the beginning of comp string.
    // New comp string will overwrite old one.
    if( iNewStrLen < cc )
    {
        for( int i = 0; i < cc - iNewStrLen; ++i )
        {
            SendMessage( hWnd, WM_KEYDOWN, VK_BACK, 0 );  // Backspace character
            SendMessageW( hWnd, WM_CHAR, VK_BACK, 0 );
        }
        SendMessage( hWnd, WM_KEYUP, VK_BACK, 0 );
    }
    else
        iNewStrLen = cc;

    // Move the caret to the beginning by sending left keystrokes
    for (int i = 0; i < iNewStrLen; ++i )
        SendMessage( hWnd, WM_KEYDOWN, VK_LEFT, 0 );
    SendMessage( hWnd, WM_KEYUP, VK_LEFT, 0 );
}

/*
 * MARCHE PAS.
 * En DirectInput disabled, ça submit juste les caractères non catchés par l'IME (le '?' rajouté, 
 * les chiffres...)
 * En DirectInput enabled, rien n'a l'air submitté.
 * Trouver un moyen de controler l'IME en DirectInput! Ou désactiver DirectInput pour le clavier!
 */
void CFullIME::submitChar( ucchar compositionChar )
{
	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
	HIMC hImc;
    if( NULL == (hImc = _ImmGetContext( hWnd )) )
        return;
	ucchar compString[MAX_COMPSTRING_SIZE];
	if ( _ImmGetCompositionStringW( hImc, GCS_COMPSTR, compString, MAX_COMPSTRING_SIZE*2 ) < 0 )
	{
	    _ImmReleaseContext( hWnd, hImc );
		return;
	}
	uint len = 1; //wcslen( (unsigned short*)compString );
	ucstring toAdd;
	toAdd = compositionChar;
	_CompString._UString += toAdd;
	_ImmSetCompositionStringW( hImc, SCS_SETSTR, (void*)_CompString._UString.c_str(), (_CompString._UString.length()+1)*2, NULL, 0 );
    _ImmReleaseContext( hWnd, hImc );
}

/*
 *
 */
void CFullIME::sendCompString()
{
//ace	CInputHandlerManager *pIH= CInputHandlerManager::instance();

//	int len = (int)_CompString.size();
//	for( int i = 0; i < len; ++i )
//	{
        //msgProc( WM_CHAR, (WPARAM)_CompString[i], 0 ); // TODO: Support direct input mode...

		//HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
		//SendMessage( hWnd, WM_IME_CHAR, (WPARAM)_CompString[i], 0 );

//ace		CEventChar eventChar( _CompString[i], noKeyButton, NULL );
//ace		eventChar.setRaw( false );
//ace		pIH->process( eventChar );
//	}
//	CChatTask::instance().addToInput(_CompString._UString);
	C3DTask::instance().kbAddToString(_CompString._UString);
}

/*
 *
 */
void CFullIME::finalizeString( bool bSend )
{
	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();

    HIMC hImc;
    if( NULL == (hImc = _ImmGetContext( hWnd )) )
        return;

    static bool bProcessing = false;
    if ( bProcessing )    // avoid infinite recursion
    {
        nlwarning( "Reentrant finalizeString" );
        _ImmReleaseContext( hWnd, hImc );
        return;
    }
    bProcessing = true;

    if( !_IsInsertOnType && bSend )
    {
        // Send composition string to app.
        LONG lLength = _CompString.size();
        // In case of CHT IME, don't send the trailing double byte space, if it exists.
        if( getLanguage() == LANG_CHT
            && _CompString[lLength - 1] == 0x3000 )
        {
            _CompString.pop_back();
        }
        sendCompString();
    }

    resetCompositionString();
    // Clear composition string in IME
    _ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
    // the following line is necessary as Korean IME doesn't close cand list
    // when comp string is canceled.
    _ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 ); 
    _ImmReleaseContext( hWnd, hImc );
    bProcessing = false;
}

/*
 *
 */
void CFullIME::getReadingWindowOrientation( DWORD dwId )
{
    _IsHorizontalReading = ( _HklCurrent == _CHS_HKL ) || ( _HklCurrent == _CHT_HKL2 ) || ( dwId == 0 );
    if( !_IsHorizontalReading && ( dwId & 0x0000FFFF ) == LANG_CHT )
    {
        WCHAR wszRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = dwId & 0xFFFF0000;
        wcsncpy( wszRegPath, L"software\\microsoft\\windows\\currentversion\\", MAX_PATH );
        wcsncat( wszRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? L"MSTCIPH" : L"TINTLGNT", MAX_PATH );
        LONG lRc = RegOpenKeyExW( HKEY_CURRENT_USER, wszRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExW( hKey, L"Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    _IsHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }
}

/*
 *
 */
void CFullIME::getPrivateReadingString()
{
    DWORD dwId = getImeId();
    if( !dwId )
    {
        _IsReadingWindowVisible = false;
        return;
    }

	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();

    HIMC hImc;
    hImc = _ImmGetContext( hWnd );
    if( !hImc )
    {
        _IsReadingWindowVisible = false;
        return;
    }

    DWORD dwReadingStrLen = 0;
    DWORD dwErr = 0;
    WCHAR *pwszReadingStringBuffer = NULL;  // Buffer for when the IME supports GetReadingString()
    WCHAR *wstr = 0;
    bool bUnicodeIme = false;  // Whether the IME context component is Unicode.
    INPUTCONTEXT *lpIC = NULL;

    if( _GetReadingString )
    {
        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        dwReadingStrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        if( dwReadingStrLen )
        {
            wstr = pwszReadingStringBuffer = (LPWSTR)HeapAlloc( GetProcessHeap(), 0, sizeof(WCHAR) * dwReadingStrLen );
            if( !pwszReadingStringBuffer )
            {
                // Out of memory. Exit.
                _ImmReleaseContext( hWnd, hImc );
                return;
            }

            // Obtain the reading string
            dwReadingStrLen = _GetReadingString( hImc, dwReadingStrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
        }

        _IsHorizontalReading = !bVertical;
        bUnicodeIme = true;
    }
    else
    {
        // IMEs that doesn't implement Reading String API

        lpIC = _ImmLockIMC( hImc );
        
        LPBYTE p = 0;
        switch( dwId )
        {
            case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
            case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
            case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
                dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
                wstr = (WCHAR *)( p + 56 );
                bUnicodeIme = true;
                break;

            case IMEID_CHT_VER50: // 5.0.x.x // WinME
                p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
                if( !p ) break;
                p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
                wstr = (WCHAR *)(p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = false;
                break;

            case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
            case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
            case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
                if( !p ) break;
                p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = true;
                break;

            // the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
            case IMEID_CHS_VER41:
            {
                int nOffset;
                nOffset = ( getImeId( 1 ) >= 0x00000002 ) ? 8 : 7;

                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 7*4 + 16*2*4);
                dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
                dwErr = min( dwErr, dwReadingStrLen );
                wstr = (WCHAR *)(p + 6*4 + 16*2*1);
                bUnicodeIme = true;
                break;
            }

            case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
            {
                OSVERSIONINFOW osi;
                osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
                GetVersionExW( &osi );

                int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(WCHAR) : sizeof(char);
                p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
                if( !p ) break;
                dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
                dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
                wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
                bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
            }
        }   // switch
    }

    // Copy the reading string to the candidate list first
    _CandList.awszCandidate[0][0] = 0;
    _CandList.awszCandidate[1][0] = 0;
    _CandList.awszCandidate[2][0] = 0;
    _CandList.awszCandidate[3][0] = 0;
    _CandList.dwCount = dwReadingStrLen;
    _CandList.dwSelection = (DWORD)-1; // do not select any char
    if( bUnicodeIme )
    {
        UINT i;
        for( i = 0; i < dwReadingStrLen; ++i ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && _CandList.dwSelection == (DWORD)-1 )
            {
                // select error char
                _CandList.dwSelection = i;
            }

            _CandList.awszCandidate[i][0] = wstr[i];
            _CandList.awszCandidate[i][1] = 0;
        }
        _CandList.awszCandidate[i][0] = 0;
    }
    else
    {
        char *p = (char *)wstr;
        DWORD i, j;
        for( i = 0, j = 0; i < dwReadingStrLen; ++i, ++j ) // dwlen > 0, if known IME
        {
            if( dwErr <= i && _CandList.dwSelection == (DWORD)-1 )
            {
                _CandList.dwSelection = j;
            }
            // Obtain the current code page
            WCHAR wszCodePage[8];
            UINT uCodePage = CP_ACP;  // Default code page
            if( GetLocaleInfoW( MAKELCID( getLanguage(), SORT_DEFAULT ),
                                LOCALE_IDEFAULTANSICODEPAGE,
                                wszCodePage,
                                sizeof(wszCodePage)/sizeof(wszCodePage[0]) ) )
            {
                uCodePage = wcstoul( wszCodePage, NULL, 0 );
            }
            MultiByteToWideChar( uCodePage, 0, p + i, IsDBCSLeadByteEx( uCodePage, p[i] ) ? 2 : 1,
                                 _CandList.awszCandidate[j], 1 );
            if( IsDBCSLeadByteEx( uCodePage, p[i] ) )
                ++i;
        }
        _CandList.awszCandidate[j][0] = 0;
        _CandList.dwCount = j;
    }
    if( !_GetReadingString )
    {
        _ImmUnlockIMCC( lpIC->hPrivate );
        _ImmUnlockIMC( hImc );
        getReadingWindowOrientation( dwId );
    }
    _ImmReleaseContext( hWnd, hImc );

    if( pwszReadingStringBuffer )
        HeapFree( GetProcessHeap(), 0, pwszReadingStringBuffer );

    // Copy the string to the reading string buffer
    if( _CandList.dwCount > 0 )
        _IsReadingWindowVisible = true;
    else
        _IsReadingWindowVisible = false;
    if( _IsHorizontalReading )
    {
        _CandList.nReadingError = -1;
        _ReadingString[0] = 0;
        for( UINT i = 0; i < _CandList.dwCount; ++i )
        {
            if( _CandList.dwSelection == i )
                _CandList.nReadingError = lstrlenW( _ReadingString );
            wcsncat( _ReadingString, _CandList.awszCandidate[i], 32 );
        }
    }

    _CandList.dwPageSize = MAX_CANDLIST;
}

/*
 *
 */
void CFullIME::onFocus( bool hasFocus )
{
	HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();

	if ( hasFocus )
	{
		//if( s_bEnableImeSystem )
		{
			_ImmAssociateContext( hWnd, _HImcDef );
			checkToggleState();
		}
		/*else
			_ImmAssociateContext( hWnd, NULL );*/

		// Set up the IME global state according to the current instance state
		HIMC hImc;
		if( NULL != ( hImc = _ImmGetContext( hWnd ) ) ) 
		{
			/*if( !s_bEnableImeSystem )
				s_ImeState = IMEUI_STATE_OFF;*/

			_ImmReleaseContext( hWnd, hImc );
			checkToggleState();
		}
	}
	else
	{
		finalizeString( false );  // Don't send the comp string as to match RichEdit behavior

		_ImmAssociateContext( hWnd, NULL );
	}
}

/*
 *
 */
bool CFullIME::staticMsgProc( uint32 uMsg, uint32 wParam, uint32 lParam )
{
    HIMC hImc;

   // if( !s_bEnableImeSystem )
   //     return false;

    switch( uMsg )
    {
        case WM_INPUTLANGCHANGE:
            nldebug( "WM_INPUTLANGCHANGE" );
            {
                UINT uLang = getPrimaryLanguage();
                checkToggleState();
                if ( uLang != getPrimaryLanguage() )
                {
                    // Korean IME always inserts on keystroke.  Other IMEs do not.
//ace                    _IsInsertOnType = (getPrimaryLanguage() == LANG_KOREAN);
                }

                // IME changed.  Setup the new IME.
                setupImeApi();
                if( _ShowReadingWindow )
                {
					HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
                    if ( NULL != ( hImc = _ImmGetContext( hWnd ) ) )
                    {
                        _ShowReadingWindow( hImc, false );
                        _ImmReleaseContext( hWnd, hImc );
                    }
                }
            }
            return true;

        case WM_IME_SETCONTEXT:
            //nldebug( "WM_IME_SETCONTEXT" );
			{
				//
				// We don't want anything to display, so we have to clear this
				//
				lParam = 0;

				// Hide the default IME window
				//HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
				//ShowWindow( _ImmGetDefaultIMEWnd( hWnd ), SW_HIDE );

				//if ( _ImmIsUIMessage( _ImmGetDefaultIMEWnd((HWND)hWnd), WM_IME_SETCONTEXT, wParam, lParam) == TRUE ) 
				//	return true;
			}

        // Handle WM_IME_STARTCOMPOSITION here since
        // we do not want the default IME handler to see
        // this when our fullscreen app is running.
        case WM_IME_STARTCOMPOSITION:
            //nldebug( "WM_IME_STARTCOMPOSITION" );
            resetCompositionString();
            // Since the composition string has its own caret, we don't render
            // the edit control's own caret to avoid double carets on screen.
            //s_bHideCaret = true; // TODO
            return true;

        case WM_IME_COMPOSITION:
            nldebug( "WM_IME_COMPOSITION" );
            return true;
    }

    return false;
}

bool CFullIME::msgProc( uint32 uMsg, uint32 wParam, uint32 lParam )
{
	//  if( !m_bEnabled || !m_bVisible )
	//    return false;

    bool trappedData;
    bool *trapped = &trappedData;

    HIMC hImc;
    static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;

    *trapped = false;
    //if( !s_bEnableImeSystem )
    //    return true;

    switch( uMsg )
    {
        //
        //  IME Handling
        //
        case WM_IME_COMPOSITION:
            nldebug( "WM_IME_COMPOSITION" );
            {
                LONG lRet;  // Returned count in CHARACTERS
                WCHAR wszCompStr[MAX_COMPSTRING_SIZE];

				HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();
                *trapped = true;
                if( NULL == (hImc = _ImmGetContext( hWnd )) )
                {
                    break;
                }

                // Get the caret position in composition string
                if ( lParam & GCS_CURSORPOS )
                {
                    _CompCaret = _ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
                    if( _CompCaret < 0 )
                        _CompCaret = 0; // On error, set caret to pos 0.
                }

                // ResultStr must be processed before composition string.
                //
                // This is because for some IMEs, such as CHT, pressing Enter
                // to complete the composition sends WM_IME_COMPOSITION with both
                // GCS_RESULTSTR and GCS_COMPSTR.  Retrieving the result string
                // gives the correct string, while retrieving the comp string
                // (GCS_COMPSTR) gives empty string.  GCS_RESULTSTR should be
                // handled first so that the application receives the string.  Then
                // GCS_COMPSTR can be handled to clear the comp string buffer.

                if ( lParam & GCS_RESULTSTR )
                {
                    nldebug( "  GCS_RESULTSTR" );
                    lRet = _ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);
                        wszCompStr[lRet] = 0;  // Force terminate
                        truncateCompString( false, (int)wcslen( wszCompStr ) );
                        _CompString.setText( (ucchar*)wszCompStr );
                        sendCompString();
                        resetCompositionString();
                    }
                }

                //
                // Reads in the composition string.
                //
                if ( lParam & GCS_COMPSTR )
                {
                    nldebug( "  GCS_COMPSTR" );
                    //////////////////////////////////////////////////////
                    // Retrieve the latest user-selected IME candidates
                    lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
                    if( lRet > 0 )
                    {
                        lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
                        wszCompStr[lRet] = 0;  // Force terminate
                        //
                        // Remove the whole of the string
                        //
                        truncateCompString( false, (int)wcslen( wszCompStr ) );

                        _CompString.setText( (ucchar*)wszCompStr );

                        // Older CHT IME uses composition string for reading string
                        if ( getLanguage() == LANG_CHT && !getImeId() )
                        {
                            if( ! _CompString.empty() )
                            {
                                _CandList.dwCount = 4;             // Maximum possible length for reading string is 4
                                _CandList.dwSelection = (DWORD)-1; // don't select any candidate

                                // Copy the reading string to the candidate list
                                for( int i = 3; i >= 0; --i )
                                {
                                    if( i > (int)_CompString.size() - 1 )
                                        _CandList.awszCandidate[i][0] = 0;  // Doesn't exist
                                    else
                                    {
                                        _CandList.awszCandidate[i][0] = _CompString[i];
                                        _CandList.awszCandidate[i][1] = 0;
                                    }
                                }
                                _CandList.dwPageSize = MAX_CANDLIST;
                                // Clear comp string after we are done copying
								_CompString.clear();
                                //ZeroMemory( (LPVOID)_CompString.GetBuffer(), 4 * sizeof(WCHAR) );
                                _IsReadingWindowVisible = true;
                                getReadingWindowOrientation( 0 );
                                if( _IsHorizontalReading )
                                {
                                    _CandList.nReadingError = -1;  // Clear error

                                    // create a string that consists of the current
                                    // reading string.  Since horizontal reading window
                                    // is used, we take advantage of this by rendering
                                    // one string instead of several.
                                    //
                                    // Copy the reading string from the candidate list
                                    // to the reading string buffer.
                                    _ReadingString[0] = 0;
                                    for( UINT i = 0; i < _CandList.dwCount; ++i )
                                    {
                                        if( _CandList.dwSelection == i )
                                            _CandList.nReadingError = lstrlenW( _ReadingString );
                                        wcsncat( _ReadingString, _CandList.awszCandidate[i], 32 );
                                    }
                                }
                            }
                            else
                            {
                                _CandList.dwCount = 0;
                                _IsReadingWindowVisible = false;
                            }
                        }

                        if( _IsInsertOnType )
                        {
                            // Send composition string to the edit control
                            sendCompString();
                            // Restore the caret to the correct location.
                            // It's at the end right now, so compute the number
                            // of times left arrow should be pressed to
                            // send it to the original position.
                            int nCount = _CompString.size() - _CompCaret;
                            // Send left keystrokes
                            for( int i = 0; i < nCount; ++i )
                                SendMessage( hWnd, WM_KEYDOWN, VK_LEFT, 0 );
                            SendMessage( hWnd, WM_KEYUP, VK_LEFT, 0 );
                        }
                    }

                    //resetCaretBlink();
                }

                // Retrieve comp string attributes
                if( lParam & GCS_COMPATTR )
                {
                    lRet = _ImmGetCompositionStringW( hImc, GCS_COMPATTR, _CompStringAttr, sizeof( _CompStringAttr ) );
                    if( lRet > 0 )
                        _CompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?
                }

                // Retrieve clause information
                if( lParam & GCS_COMPCLAUSE )
                {
                    lRet = _ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, _CompStringClause, sizeof( _CompStringClause ) );
                    _CompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
                }

                _ImmReleaseContext( hWnd, hImc );
            }
            break;

        case WM_IME_ENDCOMPOSITION:
            nldebug( "WM_IME_ENDCOMPOSITION" );
            truncateCompString();
            resetCompositionString();
            // We can show the edit control's caret again.
            //s_bHideCaret = false;
            // Hide reading window
            _IsReadingWindowVisible = false;
            break;

        case WM_IME_NOTIFY:
            //nldebug( "WM_IME_NOTIFY %u", wParam );
            switch( wParam )
            {
                case IMN_SETCONVERSIONMODE:
                    nldebug( "  IMN_SETCONVERSIONMODE" );
                case IMN_SETOPENSTATUS:
                    nldebug( "  IMN_SETOPENSTATUS" );
                    checkToggleState();
                    break;

                case IMN_OPENCANDIDATE:
                case IMN_CHANGECANDIDATE: // TODO: when changing page (PgUp, PgDn), does not seem to update the candidate list
                {
                    nldebug( wParam == IMN_CHANGECANDIDATE ? "  IMN_CHANGECANDIDATE" : "  IMN_OPENCANDIDATE" );

					HWND hWnd = (HWND)C3DTask::instance().driver().getDisplay();

                    _CandList.bShowWindow = true;
                    *trapped = true;
                    if( NULL == ( hImc = _ImmGetContext( hWnd ) ) )
                        break;

                    LPCANDIDATELIST lpCandList = NULL;
                    DWORD dwLenRequired;

                    _IsReadingWindowVisible = false;
                    // Retrieve the candidate list
                    dwLenRequired = _ImmGetCandidateListW( hImc, 0, NULL, 0 );
                    if( dwLenRequired )
                    {
                        lpCandList = (LPCANDIDATELIST)HeapAlloc( GetProcessHeap(), 0, dwLenRequired );
                        dwLenRequired = _ImmGetCandidateListW( hImc, 0, lpCandList, dwLenRequired );
                    }

                    if( lpCandList )
                    {
                        // Update candidate list data
                        _CandList.dwSelection = lpCandList->dwSelection;
                        _CandList.dwCount = lpCandList->dwCount;

                        int nPageTopIndex = 0;
                        _CandList.dwPageSize = min( lpCandList->dwPageSize, (DWORD)MAX_CANDLIST );
                        if( getPrimaryLanguage() == LANG_JAPANESE )
                        {
                            // Japanese IME organizes its candidate list a little
                            // differently from the other IMEs.
                            nPageTopIndex = ( _CandList.dwSelection / _CandList.dwPageSize ) * _CandList.dwPageSize;
                        }
                        else
                            nPageTopIndex = lpCandList->dwPageStart;

                        // Make selection index relative to first entry of page
                        _CandList.dwSelection = ( getLanguage() == LANG_CHS && !getImeId() ) ? (DWORD)-1
                                                 : _CandList.dwSelection - nPageTopIndex;

                        ZeroMemory( _CandList.awszCandidate, sizeof(_CandList.awszCandidate) );
                        for( UINT i = nPageTopIndex, j = 0;
                            (DWORD)i < lpCandList->dwCount && j < _CandList.dwPageSize;
                            i++, j++ )
                        {
                            // Initialize the candidate list strings
                            LPWSTR pwsz = _CandList.awszCandidate[j];
                            // For every candidate string entry,
                            // write [index] + Space + [String] if vertical,
                            // write [index] + [String] + Space if horizontal.
                            *pwsz++ = (WCHAR)( L'0' + ( (j + 1) % 10 ) );  // Index displayed is 1 based
                            if( _IsVerticalCand )
                                *pwsz++ = L' ';
                            WCHAR *pwszNewCand = (LPWSTR)( (LPBYTE)lpCandList + lpCandList->dwOffset[i] );
                            while ( *pwszNewCand )
                                *pwsz++ = *pwszNewCand++;
                            if( !_IsVerticalCand )
                                *pwsz++ = L' ';
                            *pwsz = 0;  // Terminate
                        }

                        // Make dwCount in s_CandList be number of valid entries in the page.
                        _CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
                        if( _CandList.dwCount > lpCandList->dwPageSize )
                            _CandList.dwCount = lpCandList->dwPageSize;

                        HeapFree( GetProcessHeap(), 0, lpCandList );
                        _ImmReleaseContext( hWnd, hImc );

                        // Korean and old Chinese IME can't have selection.
                        // User must use the number hotkey or Enter to select
                        // a candidate.
                        if( getPrimaryLanguage() == LANG_KOREAN ||
                            getLanguage() == LANG_CHT && !getImeId() )
                        {
                            _CandList.dwSelection = (DWORD)-1;
                        }

                        // Initialize s_CandList.HoriCand if we have a
                        // horizontal candidate window.
                        if( !_IsVerticalCand )
                        {
                            WCHAR wszCand[256] = L"";

                            _CandList.nFirstSelected = 0;
                            _CandList.nHoriSelectedLen = 0;
                            for( UINT i = 0; i < MAX_CANDLIST; ++i )
                            {
                                if( _CandList.awszCandidate[i][0] == L'\0' )
                                    break;

                                WCHAR wszEntry[32];
								_snwprintf( wszEntry, 32, L"%s ", (WCHAR*)_CandList.awszCandidate[i] );

                                // If this is the selected entry, mark its char position.
                                if( _CandList.dwSelection == i )
                                {
                                    _CandList.nFirstSelected = lstrlenW( wszCand );
                                    _CandList.nHoriSelectedLen = lstrlenW( wszEntry ) - 1;  // Minus space
                                }
                                wcsncat( wszCand, wszEntry, 256 );
                            }
                            wszCand[lstrlenW(wszCand) - 1] = L'\0';  // Remove the last space
                            _CandList.HoriCand.setText( (ucchar*)wszCand );
                        }
                    }
                    break;
                }

                case IMN_CLOSECANDIDATE:
                {
                    nldebug( "  IMN_CLOSECANDIDATE" );
                    _CandList.bShowWindow = false;
                    if( !_IsReadingWindowVisible )
                    {
						if ( _IsVerticalCand )
						{
							_CandList.dwCount = 0;
							ZeroMemory( _CandList.awszCandidate, sizeof(_CandList.awszCandidate) );
						}
						else
						{
							_CandList.HoriCand.clear(); // was missing in CustomUI!!
						}
                    }
                    *trapped = true;
                    break;
                }

                case IMN_PRIVATE:
                    nldebug( "  IMN_PRIVATE" );
                    {
                        if( !_CandList.bShowWindow )
                            getPrivateReadingString();

                        // Trap some messages to hide reading window
                        DWORD dwId = getImeId();
                        switch( dwId )
                        {
                            case IMEID_CHT_VER42:
                            case IMEID_CHT_VER43:
                            case IMEID_CHT_VER44:
                            case IMEID_CHS_VER41:
                            case IMEID_CHS_VER42:
                                if( ( lParam == 1 ) || ( lParam == 2 ) )
                                {
                                    *trapped = true;
                                }
                                break;

                            case IMEID_CHT_VER50:
                            case IMEID_CHT_VER51:
                            case IMEID_CHT_VER52:
                            case IMEID_CHT_VER60:
                            case IMEID_CHS_VER53:
                                if( (lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28) )
                                {
                                    *trapped = true;
                                }
                                break;
                        }
                    }
                    break;

                default:
                    *trapped = true;
                    break;
            }
            break;

        // When Text Service Framework is installed in Win2K, Alt+Shift and Ctrl+Shift combination (to switch input
        // locale / keyboard layout) doesn't send WM_KEYUP message for the key that is released first. We need to check
        // if these keys are actually up whenever we receive key up message for other keys.
        case WM_KEYUP:
        case WM_SYSKEYUP: // TODO !!!! :(
            if ( !( lAlt & 0x80000000 ) && wParam != VK_MENU && ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, ( lAlt & 0x01ff0000 ) | 0xC0000001 );
            }   
            else if ( !( lCtrl & 0x80000000 ) && wParam != VK_CONTROL && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, ( lCtrl & 0x01ff0000 ) | 0xC0000001 );
            }
            else if ( !( lShift & 0x80000000 ) && wParam != VK_SHIFT && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
            {
                PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, ( lShift & 0x01ff0000 ) | 0xC0000001 );
            }
            // fall through WM_KEYDOWN / WM_SYSKEYDOWN
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            switch ( wParam )
            {
                case VK_MENU:
                    lAlt = lParam;
                    break;
                case VK_SHIFT:
                    lShift = lParam;
                    break;
                case VK_CONTROL:
                    lCtrl = lParam;
                    break;
            }
            //break;
            // Fall through to default case
            // so we invoke the parent.

        default:;

    }  // switch

    return *trapped;
}

// TODO: handle click on the composition window
//bool CDXUTIMEEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )

/*void CFullIME::renderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, float fElapsedTime, bool bReading )
{
    RECT rc;
    UINT nNumEntries = bReading ? 4 : MAX_CANDLIST;
    D3DCOLOR TextColor, TextBkColor, SelTextColor, SelBkColor;
    int nX, nXFirst, nXComp;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

    if( bReading )
    {
        TextColor = m_ReadingColor;
        TextBkColor = m_ReadingWinColor;
        SelTextColor = m_ReadingSelColor;
        SelBkColor = m_ReadingSelBkColor;
    } else
    {
        TextColor = m_CandidateColor;
        TextBkColor = m_CandidateWinColor;
        SelTextColor = m_CandidateSelColor;
        SelBkColor = m_CandidateSelBkColor;
    }

    // For Japanese IME, align the window with the first target converted character.
    // For all other IMEs, align with the caret.  This is because the caret
    // does not move for Japanese IME.
    if ( GetLanguage() == LANG_CHT && !GetImeId() )
        nXComp = 0;
    else
    if( GetPrimaryLanguage() == LANG_JAPANESE )
        s_CompString.CPtoX( s_nFirstTargetConv, FALSE, &nXComp );
    else
        s_CompString.CPtoX( s_nCompCaret, FALSE, &nXComp );

    // Compute the size of the candidate window
    int nWidthRequired = 0;
    int nHeightRequired = 0;
    int nSingleLineHeight = 0;

    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            if( s_CandList.awszCandidate[i][0] == L'\0' )
                break;
            SetRect( &rc, 0, 0, 0, 0 );
            m_pDialog->CalcTextRect( s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );
            nWidthRequired = max( nWidthRequired, rc.right - rc.left );
            nSingleLineHeight = max( nSingleLineHeight, rc.bottom - rc.top );
        }
        nHeightRequired = nSingleLineHeight * nNumEntries;
    } else
    {
        // Horizontal window
        SetRect( &rc, 0, 0, 0, 0 );
        if( bReading )
            m_pDialog->CalcTextRect( s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
        else
            m_pDialog->CalcTextRect( s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );
        nWidthRequired = rc.right - rc.left;
        nSingleLineHeight = nHeightRequired = rc.bottom - rc.top;
    }

    // Now that we have the dimension, calculate the location for the candidate window.
    // We attempt to fit the window in this order:
    // bottom, top, right, left.

    bool bHasPosition = false;

    // Bottom
    SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y + m_rcText.bottom - m_rcText.top,
                  s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y + m_rcText.bottom - m_rcText.top + nHeightRequired );
    // if the right edge is cut off, move it left.
    if( rc.right > m_pDialog->GetWidth() )
    {
        rc.left -= rc.right - m_pDialog->GetWidth();
        rc.right = m_pDialog->GetWidth();
    }
    if( rc.bottom <= m_pDialog->GetHeight() )
        bHasPosition = true;

    // Top
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp, s_ptCompString.y - nHeightRequired,
                      s_ptCompString.x + nXComp + nWidthRequired, s_ptCompString.y );
        // if the right edge is cut off, move it left.
        if( rc.right > m_pDialog->GetWidth() )
        {
            rc.left -= rc.right - m_pDialog->GetWidth();
            rc.right = m_pDialog->GetWidth();
        }
        if( rc.top >= 0 )
            bHasPosition = true;
    }

    // Right
    if( !bHasPosition )
    {
        int nXCompTrail;
        s_CompString.CPtoX( s_nCompCaret, TRUE, &nXCompTrail );
        SetRect( &rc, s_ptCompString.x + nXCompTrail, 0,
                      s_ptCompString.x + nXCompTrail + nWidthRequired, nHeightRequired );
        if( rc.right <= m_pDialog->GetWidth() )
            bHasPosition = true;
    }

    // Left
    if( !bHasPosition )
    {
        SetRect( &rc, s_ptCompString.x + nXComp - nWidthRequired, 0,
                      s_ptCompString.x + nXComp, nHeightRequired );
        if( rc.right >= 0 )
            bHasPosition = true;
    }

    if( !bHasPosition )
    {
        // The dialog is too small for the candidate window.
        // Fall back to render at 0, 0.  Some part of the window
        // will be cut off.
        rc.left = 0;
        rc.right = nWidthRequired;
    }

    // If we are rendering the candidate window, save the position
    // so that mouse clicks are checked properly.
    if( !bReading )
        s_CandList.rcCandidate = rc;

    // Render the elements
    m_pDialog->DrawRect( &rc, TextBkColor );
    if( ( s_bVerticalCand && !bReading ) ||
        ( !s_bHorizontalReading && bReading ) )
    {
        // Vertical candidate window
        for( UINT i = 0; i < nNumEntries; ++i )
        {
            // Here we are rendering one line at a time
            rc.bottom = rc.top + nSingleLineHeight;
            // Use a different color for the selected string
            if( s_CandList.dwSelection == i )
            {
                m_pDialog->DrawRect( &rc, SelBkColor );
                m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
            } else
                m_Elements.GetAt( 1 )->FontColor.Current = TextColor;

            m_pDialog->DrawText( s_CandList.awszCandidate[i], m_Elements.GetAt( 1 ), &rc );

            rc.top += nSingleLineHeight;
        }
    } else
    {
        // Horizontal candidate window
        m_Elements.GetAt( 1 )->FontColor.Current = TextColor;
        if( bReading )
            m_pDialog->DrawText( s_wszReadingString, m_Elements.GetAt( 1 ), &rc );
        else
            m_pDialog->DrawText( s_CandList.HoriCand.GetBuffer(), m_Elements.GetAt( 1 ), &rc );

        // Render the selected entry differently
        if( !bReading )
        {
            int nXLeft, nXRight;
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected, FALSE, &nXLeft );
            s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen, FALSE, &nXRight );

            rc.right = rc.left + nXRight;
            rc.left += nXLeft;
            m_pDialog->DrawRect( &rc, SelBkColor );
            m_Elements.GetAt( 1 )->FontColor.Current = SelTextColor;
            m_pDialog->DrawText( s_CandList.HoriCand.GetBuffer() + s_CandList.nFirstSelected,
                                m_Elements.GetAt( 1 ), &rc, false, s_CandList.nHoriSelectedLen );
        }
    }
}*/


//--------------------------------------------------------------------------------------
/*void CFullIME::renderComposition( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    RECT rcCaret = { 0, 0, 0, 0 };
    int nX, nXFirst;
    m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
    m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );
    CDXUTElement *pElement = m_Elements.GetAt( 1 );

    // Get the required width
    RECT rc = { m_rcText.left + nX - nXFirst, m_rcText.top,
                m_rcText.left + nX - nXFirst, m_rcText.bottom };
    m_pDialog->CalcTextRect( s_CompString.GetBuffer(), pElement, &rc );

    // If the composition string is too long to fit within
    // the text area, move it to below the current line.
    // This matches the behavior of the default IME.
    if( rc.right > m_rcText.right )
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

    // Save the rectangle position for processing highlighted text.
    RECT rcFirst = rc;

    // Update s_ptCompString for RenderCandidateReadingWindow().
    s_ptCompString.x = rc.left; s_ptCompString.y = rc.top;

    D3DCOLOR TextColor = m_CompColor;
    // Render the window and string.
    // If the string is too long, we must wrap the line.
    pElement->FontColor.Current = TextColor;
    const WCHAR *pwszComp = s_CompString.GetBuffer();
    int nCharLeft = s_CompString.GetTextSize();
    for( ; ; )
    {
        // Find the last character that can be drawn on the same line.
        int nLastInLine;
        int bTrail;
        s_CompString.XtoCP( m_rcText.right - rc.left, &nLastInLine, &bTrail );
        int nNumCharToDraw = min( nCharLeft, nLastInLine );
        m_pDialog->CalcTextRect( pwszComp, pElement, &rc, nNumCharToDraw );

        // Draw the background
        // For Korean IME, blink the composition window background as if it
        // is a cursor.
        if( GetPrimaryLanguage() == LANG_KOREAN )
        {
            if( m_bCaretOn )
            {
                m_pDialog->DrawRect( &rc, m_CompWinColor );
            }
            else
            {
                // Not drawing composition string background. We
                // use the editbox's text color for composition
                // string text.
                TextColor = m_Elements.GetAt(0)->FontColor.States[DXUT_STATE_NORMAL];
            }
        } else
        {
            // Non-Korean IME. Always draw composition background.
            m_pDialog->DrawRect( &rc, m_CompWinColor );
        }

        // Draw the text
        pElement->FontColor.Current = TextColor;
        m_pDialog->DrawText( pwszComp, pElement, &rc, false, nNumCharToDraw );

        // Advance pointer and counter
        nCharLeft -= nNumCharToDraw;
        pwszComp += nNumCharToDraw;
        if( nCharLeft <= 0 )
            break;

        // Advance rectangle coordinates to beginning of next line
        OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );
    }

    // Load the rect for the first line again.
    rc = rcFirst;

    // Inspect each character in the comp string.
    // For target-converted and target-non-converted characters,
    // we display a different background color so they appear highlighted.
    int nCharFirst = 0;
    nXFirst = 0;
    s_nFirstTargetConv = -1;
    BYTE *pAttr;
    const WCHAR *pcComp;
    for( pcComp = s_CompString.GetBuffer(), pAttr = s_abCompStringAttr;
          *pcComp != L'\0'; ++pcComp, ++pAttr )
    {
        D3DCOLOR bkColor;

        // Render a different background for this character
        int nXLeft, nXRight;
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), FALSE, &nXLeft );
        s_CompString.CPtoX( int(pcComp - s_CompString.GetBuffer()), TRUE, &nXRight );

        // Check if this character is off the right edge and should
        // be wrapped to the next line.
        if( nXRight - nXFirst > m_rcText.right - rc.left )
        {
            // Advance rectangle coordinates to beginning of next line
            OffsetRect( &rc, m_rcText.left - rc.left, rc.bottom - rc.top );

            // Update the line's first character information
            nCharFirst = int(pcComp - s_CompString.GetBuffer());
            s_CompString.CPtoX( nCharFirst, FALSE, &nXFirst );
        }

        // If the caret is on this character, save the coordinates
        // for drawing the caret later.
        if( s_nCompCaret == int(pcComp - s_CompString.GetBuffer()) )
        {
            rcCaret = rc;
            rcCaret.left += nXLeft - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        // Set up color based on the character attribute
        if( *pAttr == ATTR_TARGET_CONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetColor;
            bkColor = m_CompTargetBkColor;
        }
        else
        if( *pAttr == ATTR_TARGET_NOTCONVERTED )
        {
            pElement->FontColor.Current = m_CompTargetNonColor;
            bkColor = m_CompTargetNonBkColor;
        }
        else
        {
            continue;
        }

        RECT rcTarget = { rc.left + nXLeft - nXFirst, rc.top, rc.left + nXRight - nXFirst, rc.bottom };
        m_pDialog->DrawRect( &rcTarget, bkColor );
        m_pDialog->DrawText( pcComp, pElement, &rcTarget, false, 1 );

        // Record the first target converted character's index
        if( -1 == s_nFirstTargetConv )
            s_nFirstTargetConv = int(pAttr - s_abCompStringAttr);
    }

    // Render the composition caret
    if( m_bCaretOn )
    {
        // If the caret is at the very end, its position would not have
        // been computed in the above loop.  We compute it here.
        if( s_nCompCaret == s_CompString.GetTextSize() )
        {
            s_CompString.CPtoX( s_nCompCaret, FALSE, &nX );
            rcCaret = rc;
            rcCaret.left += nX - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        m_pDialog->DrawRect( &rcCaret, m_CompCaretColor );
    }
}*/


//--------------------------------------------------------------------------------------
/*void CFullIME::renderIndicator( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    CDXUTElement *pElement = m_Elements.GetAt( 9 );
    pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

    m_pDialog->DrawSprite( pElement, &m_rcIndicator );
    RECT rc = m_rcIndicator;
    InflateRect( &rc, -m_nSpacing, -m_nSpacing );
    pElement->FontColor.Current = s_ImeState == IMEUI_STATE_ON && s_bEnableImeSystem ? m_IndicatorImeColor : m_IndicatorEngColor;
    RECT rcCalc = { 0, 0, 0, 0 };
    // If IME system is off, draw English indicator.
    WCHAR *pwszIndicator = s_bEnableImeSystem ? s_wszCurrIndicator : s_aszIndicator[0];

    m_pDialog->CalcTextRect( pwszIndicator, pElement, &rcCalc );
    m_pDialog->DrawText( pwszIndicator, pElement, &rc );
}*/


//--------------------------------------------------------------------------------------
/*9void CFullIME::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
    if( m_bVisible == false )
        return;

    // If we have not computed the indicator symbol width,
    // do it.
    if( !m_nIndicatorWidth )
    {
        for( int i = 0; i < 5; ++i )
        {
            RECT rc = { 0, 0, 0, 0 };
            m_pDialog->CalcTextRect( s_aszIndicator[i], m_Elements.GetAt( 9 ), &rc );
            m_nIndicatorWidth = max( m_nIndicatorWidth, rc.right - rc.left );
        }
        // Update the rectangles now that we have the indicator's width
        UpdateRects();
    }

    // Let the parent render first (edit control)
    CDXUTEditBox::Render( pd3dDevice, fElapsedTime );

    CDXUTElement* pElement = GetElement( 1 );
    if( pElement )
    {
        _CompString.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
        _CandList.HoriCand.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
    }

    //
    // Now render the IME elements
    //
    if( m_bHasFocus )
    {
        // Render the input locale indicator
        RenderIndicator( pd3dDevice, fElapsedTime );

        // Display the composition string.
        // This method should also update s_ptCompString
        // for RenderCandidateReadingWindow.
        RenderComposition( pd3dDevice, fElapsedTime );

        // Display the reading/candidate window. RenderCandidateReadingWindow()
        // uses s_ptCompString to position itself.  s_ptCompString must have
        // been filled in by RenderComposition().
        if( s_bShowReadingWindow )
            // Reading window
            RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, true );
        else
        if( s_CandList.bShowWindow )
            // Candidate list window
            RenderCandidateReadingWindow( pd3dDevice, fElapsedTime, false );
    }
}*/

/*
 * TODO: don't mix IME processing and display (UI)
 */
void CFullIME::displayIMEInfo()
{
//	TextContext->setShaded(true);
//	TextContext->setFontSize(14);
//	TextContext->setColor(CRGBA(255,255,255,255));
//	TextContext->setHotSpot(NL3D::UTextContext::TopLeft);

	// Language indicator
	float line = 7.0f;
//	TextContext->printAt(0.45f, line, ucstring( (ucchar*)_CurrIndicator ) );
	CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, ucstring( (ucchar*)_CurrIndicator ));

	if ( _ImeState != CFullIME::IMEUI_STATE_ON )
		return;

	static uint counter = 100;
	--counter;
	if ( counter == 0 )
		counter = 100;

	// Composition string
	sint32 lineStep = -1;
	line -= lineStep;
	bool isCaretVisible = (counter < 20);
	ucstring compositionString =
		_CompString._UString.substr( 0, _CompCaret ) +
		ucstring( isCaretVisible ? "_" : " " ) +
		_CompString._UString.substr( _CompCaret );
//	TextContext->printAt(0.45f, line, compositionString );
	CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, compositionString);
	line -= lineStep;

	// Reading string
	if ( _IsReadingWindowVisible )
	{
		if ( _IsHorizontalReading )
		{
			//TextContext->printAt(0.45f, line, ucstring( (ucchar*)_ReadingString ) );
			CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, ucstring( (ucchar*)_ReadingString ));
			line -= lineStep;
		}
		else
		{
			for ( uint i=0; i!=MAX_CANDLIST; ++i )
			{
		        if( _CandList.awszCandidate[i][0] == L'\0' )
	                break;
				//TextContext->printAt(0.45f, line, ucstring( (ucchar*)_CandList.awszCandidate[i] ) );
				CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, ucstring( (ucchar*)_CandList.awszCandidate[i] ));
				line -= lineStep;
			}
		}
	}

	// Candidate list
	if ( !_IsVerticalCand )
	{
		//TextContext->printAt(0.45f, line, _CandList.HoriCand._UString );
		CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, _CandList.HoriCand._UString);
		line -= lineStep;
	}
	else if ( _CandList.bShowWindow )
	{
		for ( uint i=0; i!=_CandList.dwCount; ++i ) // TODO: more candidates (dwPageStart...)
		{
//			if ( i == _CandList.dwSelection ) // TODO (not set)
//				TextContext->setColor(CRGBA(0,255,0,255));
//			else
//				TextContext->setColor(CRGBA(255,255,255,255));
//			TextContext->printAt(0.45f, line, ucstring( (ucchar*)_CandList.awszCandidate[i] ) );
			CFontManager::instance().print(CFontManager::TCDebug, 0.0f, line, ucstring( (ucchar*)_CandList.awszCandidate[i] ));
			line -= lineStep;
		}
	}
}

ucstring CFullIME::indicator() const
{
	return ucstring((ucchar*)_CurrIndicator);
}

ucstring CFullIME::composition() const
{
	return ucstring(_CompString._UString);
}

#endif
