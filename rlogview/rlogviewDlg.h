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
// rlogviewDlg.h : header file
//

#if !defined(AFX_RLOGVIEWDLG_H__E1B228E5_2441_11D3_9551_F63D9C385028__INCLUDED_)
#define AFX_RLOGVIEWDLG_H__E1B228E5_2441_11D3_9551_F63D9C385028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRlogviewDlg dialog

class CRlogviewDlg : public CDialog
{
// Construction
public:
	CRlogviewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRlogviewDlg)
	enum { IDD = IDD_RLOGVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRlogviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	int sock;			//socket
	POINT minsize_pt;	//dialog minimum size
	CFont font;			//log text font
	unsigned int key;	//encoding hash key

	int count;

	enum { WM_SRVINPUT = WM_USER+292 };

	// Generated message map functions
	//{{AFX_MSG(CRlogviewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnConnect();
	afx_msg void OnSave();
	afx_msg void OnFont();
	afx_msg void OnClose();
	afx_msg void OnGenerate();
	afx_msg void OnErrspaceLogText();
	afx_msg void OnMaxtextLogText();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnSvrInput(WPARAM, LPARAM lParam);					//receive data from server
	void OnGetMinMaxInfo(HWND, LPMINMAXINFO pMMI);			//limit minimum dialog size
	char *encode(char *data, unsigned long int datalen);	//encode outgoing data
	char *decode(char *data, unsigned long int datalen);	//decode incoming data

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RLOGVIEWDLG_H__E1B228E5_2441_11D3_9551_F63D9C385028__INCLUDED_)
