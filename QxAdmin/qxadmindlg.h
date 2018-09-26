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

// qxadminDlg.h : header file
//

#if !defined(AFX_QXADMINDLG_H__0346E88A_57B9_11D1_B40D_00A0241C20B6__INCLUDED_)
#define AFX_QXADMINDLG_H__0346E88A_57B9_11D1_B40D_00A0241C20B6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../libqxadm/libqxadm.h"
#include "../rlogview/rlogviewDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CQxAdminDlg dialog

class CQxAdminDlg : public CDialog
{
// Construction
public:
	CQxAdminDlg(CWnd* pParent = NULL);				// standard constructor
	CString edit_key, edit_value;
	int ping_sorted, name_sorted, points_sorted;	//0: not sorted, 1: asc. sorted, -1: desc: sorted
	
// Dialog Data
	//{{AFX_DATA(CQxAdminDlg)
	enum { IDD = IDD_QXADMIN };
	CEdit	m_reply;
	CComboBox	m_rcon_command_list;
	CComboBox	m_server_address_list;
	CListCtrl	m_players;
	CListCtrl	m_keys_values;
	CString	m_server_port;
	CString	m_status;
	CString	m_rcon_command;
	CString	m_rcon_password;
	CString	m_players_infos_string;
	CString	m_server_address;
	CString	m_reply_text;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQxAdminDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	HICON m_hIcon;
	CString maxclients;
	CQServer server;
	int server_list_hack;
	//CQxConLog qxconclient;
	CRlogviewDlg rlogview;
	CFont font;

	// Generated message map functions
	//{{AFX_MSG(CQxAdminDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnQuery();
	afx_msg void OnRconSend();
	afx_msg void OnRclickKeysValues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnDblclkKeysValues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelendokServerAddressList();
	afx_msg void OnRconstatus();
	afx_msg void OnDropdownServerAddressList();
	afx_msg void OnColumnclickPlayers(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelendcancelServerAddress();
	afx_msg int OnGetConvars();
	afx_msg void OnSetfocusServerAddress();
	afx_msg void OnSetfocusRconCommand();
	afx_msg void OnQw();
	afx_msg void OnQ2();
	afx_msg void OnQ3();
	afx_msg void OnPlayerKick();
	afx_msg void OnRclickPlayers(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAutoUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeAutoUpdateTime();
	afx_msg void OnAutodetect();
	afx_msg void OnErrspaceReply();
	afx_msg void OnQxconsrv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void error(CString msg);
	int detect_server_type();
	int parse_server_infos(CString &server_infos);//char *server_infos);
	int set_server_data();
	char *convert_crappy_name(char *name);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QXADMINDLG_H__0346E88A_57B9_11D1_B40D_00A0241C20B6__INCLUDED_)
