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

// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "qxadmin.h"
#include "qxadminDlg.h"
#include "EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog


CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)	: CDialog(CEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditDlg)
	m_value = _T("");
	m_key = _T("");
	//}}AFX_DATA_INIT

	m_key = ((CQxAdminDlg *)AfxGetMainWnd())->edit_key;
	m_value = ((CQxAdminDlg *)AfxGetMainWnd())->edit_value;
}


void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlg)
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDX_Text(pDX, IDC_SERVER_INFO_KEY, m_key);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDlg message handlers

void CEditDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	((CQxAdminDlg *)AfxGetMainWnd())->edit_key = m_key;
	((CQxAdminDlg *)AfxGetMainWnd())->edit_value = m_value;
		
}
