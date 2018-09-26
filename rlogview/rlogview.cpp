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
// rlogview.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "rlogview.h"
#include "rlogviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CrlogviewApp

BEGIN_MESSAGE_MAP(CrlogviewApp, CWinApp)
	//{{AFX_MSG_MAP(CrlogviewApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CrlogviewApp construction

CrlogviewApp::CrlogviewApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CrlogviewApp object

CrlogviewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CrlogviewApp initialization

BOOL CrlogviewApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	AfxInitRichEdit();

	CRlogviewDlg dlg;
	m_pMainWnd = &dlg;
	
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
