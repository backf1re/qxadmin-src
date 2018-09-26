/*qxconlog.cpp : implementation file

  TODO: encode all data from/to server ?
*/

#include "stdafx.h"
#include "qxadmin.h"
#include "qxconlog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQxConLog dialog


CQxConLog::CQxConLog(CWnd* pParent /*= NULL*/) : CDialog(CQxConLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQxConLog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	//Create is called in CQxAdminDlg::InitDialog (with a parent window...)
	
	qxconsrv_sock = 0;
}


void CQxConLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQxConLog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQxConLog, CDialog)
	//{{AFX_MSG_MAP(CQxConLog)
	ON_BN_CLICKED(IDC_QXCON_CONNECT, OnQxConConnect)
	ON_BN_CLICKED(IDC_QXCON_HIDE, OnQxConHide)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_QXCONSRVINPUT, OnQxConSvrInput)
	ON_MESSAGE(WM_GETMINMAXINFO, OnGetMinMaxInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQxConLog message handlers

BOOL CQxConLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_QXCON_PORT)->SetWindowText("55920");
	((CSpinButtonCtrl *)GetDlgItem(IDC_QXCON_PORT_SPIN))->SetRange32(0, 65535);

	GetDlgItem(IDC_QXCON_PASSWORD)->SetFocus();
	
	return FALSE;
}

char *endecode(char *msg) {
	const char KEY = '\xA4';
	unsigned int i;
	for(i = 0; i < strlen(msg); i++)
		msg[i] ^= KEY;
	return msg;
}

//Connect to QxConsole Server
void CQxConLog::OnQxConConnect()
{

	int retval;
	struct sockaddr_in saddr;
	char buffer[4096];
	const int YIPI = ('Y'<<24)+('I'<<16)+('P'<<8)+'I';	//successfull login message
	CString password, address, port_cstr;
	struct hostent *hostname;

	BeginWaitCursor();

	//if qxconsrv_sock != 0 we are already connected. disconnect and change button text
	if(qxconsrv_sock) {
		GetDlgItem(IDC_QXCON_CONNECT)->SetWindowText("Connect");
		closesocket(qxconsrv_sock);
		qxconsrv_sock = 0;
		return;
	}

	//parse address from QxAdmin main dialog
	GetParent()->GetDlgItem(IDC_SERVER_ADDRESS)->GetWindowText(address);
	retval = address.Find(':');
	if(retval > 0)
		address = address.Left(retval);

	//build server address struct
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(address);
	if(saddr.sin_addr.s_addr == -1) {
		hostname = gethostbyname(address.GetBuffer(0));
		if(!hostname) {
			MessageBox("Error looking up hostname.");
			return;
		}
		saddr.sin_addr = *(struct in_addr *)hostname->h_addr;
	}
	GetDlgItem(IDC_QXCON_PORT)->GetWindowText(port_cstr);
	saddr.sin_port = htons(atoi(port_cstr.GetBuffer(0)));

	qxconsrv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(qxconsrv_sock == -1) {
		MessageBox("Error creating socket.");
		return;
	}

	if(connect(qxconsrv_sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		MessageBox("Error connecting to QxConsole Server.");
		closesocket(qxconsrv_sock);
		qxconsrv_sock = 0;
		return;
	}

	GetDlgItem(IDC_QXCON_PASSWORD)->GetWindowText(password);
	sprintf(buffer, "PASS %s", password.GetBuffer(0));
//	GetDlgItem(IDC_STATUS)->SetWindowText("Logging in QxConsoleServer...");

	//send 'encoded' login message
	if(send(qxconsrv_sock, endecode(buffer), strlen(buffer), 0) == -1) {
		MessageBox("Error logging in QxConsole Server.");
		closesocket(qxconsrv_sock);
		qxconsrv_sock = 0;
		return;
	}

	//wait for server response
	if((retval = recv(qxconsrv_sock, buffer, 4096, 0)) < 0) {
		MessageBox("Error logging in QxConsoleServer.");
		closesocket(qxconsrv_sock);
		qxconsrv_sock = 0;
	} else {
		buffer[retval] = 0;
		endecode(buffer);
		if(*(unsigned int *)buffer != ntohl(YIPI)) {
			MessageBox("Wrong QxConsole Server password.");
			closesocket(qxconsrv_sock);
			qxconsrv_sock = 0;
			return;
		}
		//non-blocking select: will indirectly call OnQxConSrvInput() on data reception
		if(WSAAsyncSelect(qxconsrv_sock, this->m_hWnd, WM_QXCONSRVINPUT, FD_READ | FD_CLOSE) != 0)
			MessageBox("WSAAsyncSelect");
		MessageBox("Successfully logged in QxConsoleServer.");
		GetDlgItem(IDC_QXCON_CONNECT)->SetWindowText("Disconnect");
	}
//	GetDlgItem(IDC_STATUS)->SetWindowText("Ready.");

	EndWaitCursor();
}

//WM_QXCONSRVINPUT message handler
//this is called indirectly by WSAAsyncSelect when there's data ready
// or the connection has been reset(FD_READ, FD_CLOSE)
void CQxConLog::OnQxConSvrInput(WPARAM, LPARAM lParam)
{
	int retval;
	CString reply;
	CEdit *logtext_edit = (CEdit *)GetDlgItem(IDC_LOG_TEXT);

	//check for Connection reset
	if(WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
		MessageBox("QxConsole Server closed the connection.");
		GetDlgItem(IDC_QXCON_CONNECT)->SetWindowText("Connect");
		closesocket(qxconsrv_sock);
		qxconsrv_sock = 0;
		return;
	}

	//receive data from server
	char *buffer = new char[65536];
	if((retval = recv(qxconsrv_sock, buffer, 65536, 0)) < 0)
		MessageBox("recv");
	else {
		buffer[retval] = 0;
		CString reply = endecode(buffer);
		for(int i=0; i < reply.GetLength(); i++) {
			if(reply[i] == 0xA)
				reply.Insert(i++, 0xD);
		}

		int len = logtext_edit->GetWindowTextLength();
		logtext_edit->SetSel(len, len);
		logtext_edit->ReplaceSel(reply);
	}

	delete buffer;
}

void CQxConLog::OnQxConHide() 
{
	ShowWindow(SW_HIDE);
}

void CQxConLog::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;

	CDialog::OnSize(nType, cx, cy);

	if(!::IsWindowVisible(m_hWnd))		//GetDlgItem will bork if this function is called before
		return;							//being made visible(?)

	//resize log text edit box
	GetDlgItem(IDC_LOG_TEXT)->GetWindowRect(&rect);
	ScreenToClient(rect);
	GetDlgItem(IDC_LOG_TEXT)->MoveWindow(rect.left, rect.top, cx-rect.left-8, cy-rect.top-8);

}

//limit minimum window size
void CQxConLog::OnGetMinMaxInfo(HWND, LPMINMAXINFO pMMI)
{
	POINT pt;

	pt.x = 580;
	pt.y = 100;
	pMMI->ptMinTrackSize = pt;
}
