/*
QxAdmin: remotely controls QuakeWorld, Quake 2, Quake 3: Arena
         servers from a Win32 machine.

Copyright (C) 1999  Pierre-Andre Cote

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Version: 0.9 - 1999/05/06 - Initial release.
Version: 1.0 - 1999/05/23
	Always default port check box
	Always console variables check box
	Server infos scrollbar memory
	Right-click pop-up menu to kick player
	Q3:A colors control characters are removed
	Server port Spin control
Version: 1.01 - 1999/05/26
	Auto-update
Version: 1.02 - 1999/05/26
	auto-update spin disable bug
	disable cvars button in QW
Version: 1.03 - 1999/06/10
	repaired kick right-click menu command
Version: 1.1 - 1999/06/21
	sped-up cvars update
	support for rlogview
Version: 1.11 - 1999/06/26
	repaired server key edit window(resource conflict with rlogview)

Pierre-Andre Cote
pierreandre_cote@yahoo.com

*/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0346E88C_57B9_11D1_B40D_00A0241C20B6__INCLUDED_)
#define AFX_STDAFX_H__0346E88C_57B9_11D1_B40D_00A0241C20B6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define MAX_SERVER_HISTORY 10
#define MAX_COMMAND_HISTORY 20

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0346E88C_57B9_11D1_B40D_00A0241C20B6__INCLUDED_)
