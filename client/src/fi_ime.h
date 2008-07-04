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

#ifndef NL_FI_IME_H
#define NL_FI_IME_H


//
// Includes
//

#include "nel/misc/types_nl.h"
#include "nel/misc/ucstring.h"

#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <windows.h>
// In \PlatformSDK\Include:
// Internals of old IME need to be accessed as they do not provide a sufficient API
// (can't be placed in .cpp because INPUTCONTEXT is a struct therefore can't be forward declarated :(
#	include <dimm.h>
#endif

/**
 * Unicode buffer
 */
class CUnicodeBuffer
{
public:
	CUnicodeBuffer( uint initialSize=1 ) { if ( initialSize != 0 ) _UString.reserve( 0 ); }
	void setText( ucchar *src ) { _UString = (ucchar*)src; }
	void clear() { _UString.clear(); }
	bool empty() const { return _UString.empty(); }
	uint size() const { return _UString.size(); }
	const ucchar& operator[] ( uint index ) const { return (const ucchar&)_UString[index]; } // no check
	void pop_back() { _UString.resize(_UString.size()-1); }

	ucstring	_UString;
};


#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

/**
 * Fully integrated IME
 * \author Olivier Cado
 * \author Nevrax France
 * \date 2005
 */
class CFullIME
{
public:
	/// Constructor
	CFullIME() {}

	/// Init
	static void		init();

	/// Release
	static void		release();

	///	Associate/disassociate the IME context with the window
	void			onFocus( bool hasFocus );

	/// Submit a char to build an IME char
	void			submitChar( ucchar compositionChar );

	/// Static message procedure
    static bool		staticMsgProc( uint32 uMsg, uint32 wParam, uint32 lParam );

	/// Non-static message procedure
	bool msgProc( uint32 uMsg, uint32 wParam, uint32 lParam );

	/// (TEMP)
	void displayIMEInfo();

    enum IMESTATE { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

	ucstring indicator() const;
	ucstring composition() const;

private:

#ifdef NL_OS_WINDOWS

    enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };

    struct CCandList
    {
        WCHAR awszCandidate[MAX_CANDLIST][256];
        CUnicodeBuffer HoriCand; // Candidate list string (for horizontal candidate window)
        int   nFirstSelected; // First character position of the selected string in HoriCand
        int   nHoriSelectedLen; // Length of the selected string in HoriCand
        DWORD dwCount;       // Number of valid entries in the candidate list
        DWORD dwSelection;   // Currently selected candidate entry relative to page top
        DWORD dwPageSize;
        int   nReadingError; // Index of the error character
        bool  bShowWindow;   // Whether the candidate list window is visible
        RECT  rcCandidate;   // Candidate rectangle computed and filled each time before rendered
    };

    struct CInputLocale
    {
        HKL   m_hKL;            // Keyboard layout
        WCHAR m_wszLangAbb[3];  // Language abbreviation
        WCHAR m_wszLang[64];    // Localized language name
    };

	// IMM32 DLL handle
    static HINSTANCE _HDllImm32;
	// Version DLL handle
    static HINSTANCE _HDllVer;
	// Default input context
    static HIMC      _HImcDef;

    // Function pointers: IMM32
    static INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
    static BOOL (WINAPI * _ImmUnlockIMC)( HIMC );
    static LPVOID (WINAPI * _ImmLockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmUnlockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmDisableTextFrameService)( DWORD );
    static LONG (WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
	static LONG (WINAPI * _ImmSetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD, LPVOID, DWORD );
    static DWORD (WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
    static HIMC (WINAPI * _ImmGetContext)( HWND );
    static BOOL (WINAPI * _ImmReleaseContext)( HWND, HIMC );
    static HIMC (WINAPI * _ImmAssociateContext)( HWND, HIMC );
    static BOOL (WINAPI * _ImmGetOpenStatus)( HIMC );
    static BOOL (WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
    static BOOL (WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
    static HWND (WINAPI * _ImmGetDefaultIMEWnd)( HWND );
    static UINT (WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
    static UINT (WINAPI * _ImmGetVirtualKey)( HWND );
    static BOOL (WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
    static BOOL (WINAPI * _ImmIsIME)( HKL );

    // Function pointers: Newer IMEs
    static UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
    static BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

	// Function pointers: Version library imports
    static BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
    static BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
    static DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

    // Application-wide data
    static HKL     _HklCurrent;			  // Current keyboard layout of the process
    static bool    _IsVerticalCand;       // Indicates that the candidates are listed vertically
    static LPWSTR  _CurrIndicator;        // Points to an indicator string that corresponds to current input locale
    static WCHAR   _Indicators[5][3];     // String to draw to indicate current input locale
    static bool    _IsInsertOnType;       // Insert the character as soon as a key is pressed (Korean behavior)
    static HINSTANCE _HDllIme;            // Instance handle of the current IME module
    static IMESTATE  _ImeState;			  // IME global state
    static bool    s_bEnableImeSystem;    // Whether the IME system is active
    static POINT   s_ptCompString;        // Composition string position. Updated every frame.
    static int     _CompCaret;            // Caret position of the composition string
    static int     s_nFirstTargetConv;    // Index of the first target converted char in comp string.  If none, -1.
    static CUnicodeBuffer _CompString;        // Buffer to hold the composition string (we fix its length)
    static BYTE    _CompStringAttr[MAX_COMPSTRING_SIZE];
    static DWORD   _CompStringClause[MAX_COMPSTRING_SIZE];
    static WCHAR   _ReadingString[32];    // Used only with horizontal reading window (why?)
    static CCandList _CandList;           // Data relevant to the candidate list
    static bool    _IsReadingWindowVisible; // Indicates whether reading window is visible
    static bool    _IsHorizontalReading;  // Indicates whether the reading window is vertical or horizontal
    static bool    _IsChineseIME;
    static vector< CInputLocale > _Locales; // Array of loaded keyboard layout on system

    static WORD getLanguage() { return LOWORD( _HklCurrent ); }
    static WORD getPrimaryLanguage() { return PRIMARYLANGID( LOWORD( _HklCurrent ) ); }
    static WORD getSubLanguage() { return SUBLANGID( LOWORD( _HklCurrent ) ); }
    static DWORD getImeId( UINT uIndex = 0 );

	// Load the new IME when switching input locale
	static void	setupImeApi();

	/// Get current locale and update related data if changed
    static void checkInputLocale();

	/// Get current locale and update related data and ime state if changed
    static void checkToggleState();

	/// Reset the composition string
    static void resetCompositionString();

	/// Truncate composition string by sending keystrokes to the window
    void truncateCompString( bool bUseBackSpace = true, int iNewStrLen = 0 );
	
	/// Send the current composition string to the application by sending keystroke messages
    void sendCompString();

	/// Output current composition string then clean up the composition task
    void finalizeString( bool bSend );

	/// Determine whether the reading window should be vertical or horizontal
    static void getReadingWindowOrientation( DWORD dwId );

	/// Obtain the reading string upon WM_IME_NOTIFY/INM_PRIVATE notification
	static void getPrivateReadingString();
#endif //NL_OS_WINDOWS
};

#endif
