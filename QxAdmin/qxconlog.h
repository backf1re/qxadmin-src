#if !defined(AFX_QXCONLOG_H__CAA33F00_2323_11D3_9551_F63D9C385028__INCLUDED_)
#define AFX_QXCONLOG_H__CAA33F00_2323_11D3_9551_F63D9C385028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// qxconlog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQxConLog dialog

class CQxConLog : public CDialog
{
// Construction
public:
	CQxConLog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQxConLog)
	enum { IDD = IDD_QXCONLOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQxConLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	

	int qxconsrv_sock;			//QxConsoleServer socket, 0 if not connected

	enum { WM_QXCONSRVINPUT = WM_USER+292 };

	// Generated message map functions
	//{{AFX_MSG(CQxConLog)
	afx_msg void OnQxConConnect();
	afx_msg void OnQxConHide();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnQxConSvrInput(WPARAM, LPARAM lParam);
	void OnGetMinMaxInfo(HWND, LPMINMAXINFO pMMI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QXCONLOG_H__CAA33F00_2323_11D3_9551_F63D9C385028__INCLUDED_)
