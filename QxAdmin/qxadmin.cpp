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

// qxadmin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "qxadmin.h"
#include "qxadminDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQxAdminApp

BEGIN_MESSAGE_MAP(CQxAdminApp, CWinApp)
	//{{AFX_MSG_MAP(CQxAdminApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQxAdminApp construction

CQxAdminApp::CQxAdminApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQxAdminApp object

CQxAdminApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CQxAdminApp initialization

BOOL CQxAdminApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CQxAdminDlg dlg;
	m_pMainWnd = &dlg;
	char str[128];	//for itoa
	int item, retval=1;

	//Servers and commands history will be saved in the qxadmin.ini file in
	// the same directory as QxAdmin.exe
	CString cstr = m_pszHelpFilePath;
	cstr.Replace(".HLP", ".INI");
	m_pszProfileName = cstr.GetBuffer(0);
	cstr.ReleaseBuffer();

	//Get servers and commands history from qxadmin.ini
	for(item = 0; item < MAX_SERVER_HISTORY; item++)
		ini_servers[item] = GetProfileString("servers", itoa(item,str,10));
	if(item != MAX_SERVER_HISTORY)
		ini_servers[item] = "";

	for(item = 0; item < MAX_COMMAND_HISTORY; item++)
		ini_commands[item] = GetProfileString("commands", itoa(item,str,10));
	if(item != MAX_COMMAND_HISTORY)
		ini_commands[item] = "";

	auto_update_time = GetProfileInt("options", "auto_update_time", 15);

	dlg.DoModal();

	//Save servers and commands history in qxadmin.ini
	for(item = 0; retval != 0 && item < MAX_SERVER_HISTORY && !ini_servers[item].IsEmpty(); item++)
		retval = WriteProfileString("servers", itoa(item,str,10), ini_servers[item]);

	for(item = 0; retval != 0 && item < MAX_COMMAND_HISTORY && !ini_commands[item].IsEmpty(); item++)
		retval = WriteProfileString("commands", itoa(item,str,10), ini_commands[item]);

	//save the auto update time in qxadmin.ini
	WriteProfileInt("options", "auto_update_time", auto_update_time);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
