/*
rlogview: remotely view log files updates.

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

Version: 0.87 - 1999/06/21 - Initial release

Pierre-Andre Cote
pierreandre_cote@yahoo.com

*/
// rlogview.h : main header file for the RLOGVIEW application
//

#if !defined(AFX_RLOGVIEW_H__E1B228E3_2441_11D3_9551_F63D9C385028__INCLUDED_)
#define AFX_RLOGVIEW_H__E1B228E3_2441_11D3_9551_F63D9C385028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CrlogviewApp:
// See rlogview.cpp for the implementation of this class
//

class CrlogviewApp : public CWinApp
{
public:
	CrlogviewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CrlogviewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CrlogviewApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RLOGVIEW_H__E1B228E3_2441_11D3_9551_F63D9C385028__INCLUDED_)
