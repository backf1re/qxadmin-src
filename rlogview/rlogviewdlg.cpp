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
Version: 0.90 - 1999/06/28
	Erase log text if > 32K
	Set socket in blocking mode in OnSrvInput

Pierre-Andre Cote
pierreandre_cote@yahoo.com

TODO:	'Save...': (save as?) default file name should be the filename of the server
"Shell_NotifyIcon"
*/
/* rlogviewDlg.cpp : implementation file */

#include "stdafx.h"
#include "rlogview.h"
#include "rlogviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRlogviewDlg dialog

CRlogviewDlg::CRlogviewDlg(CWnd* pParent /*=NULL*/)	: CDialog(CRlogviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRlogviewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	minsize_pt.x = minsize_pt.y = 0;
}

void CRlogviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRlogviewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRlogviewDlg, CDialog)
	//{{AFX_MSG_MAP(CRlogviewDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_EN_ERRSPACE(IDC_LOG_TEXT, OnErrspaceLogText)
	ON_EN_MAXTEXT(IDC_LOG_TEXT, OnMaxtextLogText)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SRVINPUT, OnSvrInput)
	ON_MESSAGE(WM_GETMINMAXINFO, OnGetMinMaxInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRlogviewDlg message handlers

BOOL CRlogviewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//set defaults in dialog
	((CSpinButtonCtrl *)GetDlgItem(IDC_PORT_SPIN_R))->SetRange32(0, 65535);

	GetDlgItem(IDC_PASSWORD)->SetFocus();
	
	SetWindowText("[not connected] - rlogview 0.91");

	sock = 0;

	//get options from .ini file
	CString profile_name = AfxGetApp()->m_pszHelpFilePath;
	profile_name.Replace(".HLP", ".INI");
	char option[1024];

	if(GetPrivateProfileString("rlogoptions", "address", "", option, 1023, profile_name))
		GetDlgItem(IDC_ADDRESS)->SetWindowText(option);
	else
		GetDlgItem(IDC_ADDRESS)->SetWindowText("localhost");

	if(GetPrivateProfileString("rlogoptions", "port", "", option, 1023, profile_name))
		GetDlgItem(IDC_PORT)->SetWindowText(option);
	else
		GetDlgItem(IDC_PORT)->SetWindowText("55920");

	if(GetPrivateProfileString("rlogoptions", "key", "", option, 1023, profile_name))
		GetDlgItem(IDC_KEY)->SetWindowText(option);

	LOGFONT lf;
	if(GetPrivateProfileStruct("rlogoptions", "font", &lf, sizeof(lf), profile_name)) {
		font.CreateFontIndirect(&lf);
		GetDlgItem(IDC_LOG_TEXT)->SetFont(&font);
	}

	RECT dlg_rect, con_rect, log_text_rect;

	//move main dialog and log text window
	if(GetPrivateProfileStruct("rlogoptions", "windowpos", &dlg_rect, sizeof(dlg_rect), profile_name)) {	
		MoveWindow(&dlg_rect);
		GetWindowRect(&dlg_rect);
		GetDlgItem(IDC_LOG_TEXT)->GetWindowRect(&log_text_rect);
		log_text_rect.bottom = dlg_rect.bottom - 12;
		log_text_rect.right = dlg_rect.right - 12;
		ScreenToClient(&log_text_rect);
		GetDlgItem(IDC_LOG_TEXT)->MoveWindow(&log_text_rect);
	}

	//set minimum dialog size
	GetWindowRect(&dlg_rect);
	GetDlgItem(IDC_CONNECT)->GetWindowRect(&con_rect);
	GetDlgItem(IDC_LOG_TEXT)->GetWindowRect(&log_text_rect);

	minsize_pt.x = con_rect.right - dlg_rect.left + 13;
	minsize_pt.y = log_text_rect.top - dlg_rect.top + 32;

	//init random seed for OnGenerate()
	srand(time(0));

	//set log text length limit to a 'big' number
	//On WinNT: max: 4Gig(or 2? MBCS...)
	//On Win9x: max: 32K MBCS(a way to disable MBCS?)
	((CEdit *)GetDlgItem(IDC_LOG_TEXT))->LimitText(0);
	//((CRichEditCtrl *)GetDlgItem(IDC_LOG_TEXT))->LimitText(100000);

	return FALSE;
}

void CRlogviewDlg::OnPaint() 
{
	if (IsIconic())	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else
		CDialog::OnPaint();
}

HCURSOR CRlogviewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//encode data for transmition
//this simply XOR data with 'key'
char *CRlogviewDlg::encode(char *data, unsigned long int datalen)
{
	unsigned long int i;
	char *key_p = (char *)&key;

	//encode data
	for(i = 0; i < datalen; i++)
		data[i] ^= key_p[i % 4];

	data[datalen] = 0;	//null terminate

	return data;
}

//decode data encoded by encode()
char *CRlogviewDlg::decode(char *data, unsigned long int datalen)
{
	unsigned long int i;
	char *key_p = (char *)&key;

	//decode data
	for(i = 0; i < datalen; i++)
		data[i] ^= key_p[i % 4];

	data[datalen] = 0;	//null terminate

	return data;
}

//NEW encode/decode functions, it was to prevent some bug, but setting the socket in
// blocking mode in OnSrvInput() prevent the bug from happening
/*
unsigned long int CRlogviewDlg::encode(char *data, unsigned long int datalen)
{
	unsigned long int i, ndatalen, pad_num;	//pad_num: number of padding bytes(0-3)
	unsigned long int *datai = (unsigned long int *)data;
	unsigned long int mod =  datalen % 4;

	//move data 4 bytes down
	memmove(data+4, data, datalen);
	
	//calculate number of padding bytes
	pad_num = mod == 0 ? 0 : 4 - mod;
	
	//put data length in the first 4 bytes of data(in network order)
	ndatalen = htonl(datalen + pad_num);
	memcpy(data, (unsigned long int *)&ndatalen, 4);
	datalen += 4;

	//append padding
	memset(data + datalen, '\xff', pad_num);
	datalen += pad_num;

	//encode data
	for(i = 1; i < datalen / 4; i++)
		datai[i] ^= key;

	//append number of padding bytes + 1
	memset(data + datalen++, pad_num+1, 1);
	memset(data + datalen, 0, 1);	//null terminated(?)

	return datalen;
}

char *CRlogviewDlg::decode(char *data, unsigned long int datalen)
{
	unsigned int i, blocklen;
	unsigned int *datai = (unsigned int *)data;

	blocklen = ntohl(*datai);

	memmove(data, data+4, blocklen+1);

	if(blocklen % 4 != 0)
		MessageBox("Error decoding data! Attempting to continue...");
	//decode data
	for(i = 0; i < blocklen / 4; i++)
		datai[i] ^= key;

	//remove padding
	data[blocklen - data[blocklen] + 1] = 0;
	
	if(blocklen + 5 > datalen)
		MessageBox("Data have been truncated somewhere...\n");
	if(blocklen + 5 < datalen)
		MessageBox("Two or more rlogviewd packets in the same packet...\n");

	return data;
}
*/

//WM_SRVINPUT message handler
//this is called indirectly by WSAAsyncSelect when there's data ready
// or the connection has been reset(FD_READ or FD_CLOSE)
//for now if the server send updates too fast for the network
// TCP can join packets at will and that will fuck decode() because
// decode need the footer at the end, not in the middle...
// Setting the socket in blocking mode prevent this...(I think)
void CRlogviewDlg::OnSvrInput(WPARAM, LPARAM lParam)
{
	int retval;
	CString reply;
	CEdit *logtext_edit = (CEdit *)GetDlgItem(IDC_LOG_TEXT);
//	CRichEditCtrl *logtext_edit = (CRichEditCtrl *)GetDlgItem(IDC_LOG_TEXT);

	//set socket in blocking mode
	if(WSAAsyncSelect(sock, this->m_hWnd, WM_SRVINPUT, 0) != 0)
		MessageBox("WSAAsyncSelect");
	unsigned long sockcmd = 0;
	if(ioctlsocket(sock, FIONBIO, &sockcmd) == SOCKET_ERROR)
		MessageBox("ioctlsocket");

	//check for Connection reset
	if(WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
		MessageBox("rlogviewd server closed the connection.");
		SetWindowText("[not connected] - rlogview 0.91");
		GetDlgItem(IDC_CONNECT)->SetWindowText("Connect");
		closesocket(sock);
		sock = 0;
		return;
	}
	
	//receive data from server
	char *buffer = new char[65536];
	if((retval = recv(sock, buffer, 65535, 0)) < 0)
		MessageBox("Error receiving data from rlogviewd server.");
	else {
//this commented code is to be used with the new encode/decode funtions that is not used
//		unsigned int blocklen = ntohl(*(unsigned int *)buffer);
//		CString cstr;
//		cstr.Format("blocklen = %u", blocklen);
//		MessageBox(cstr);
//		if((retval = recv(sock, buffer, blocklen, 0)) < 0)
//			MessageBox("Error receiving data from rlogviewd server.");
//		else {
//			if((unsigned int)retval < blocklen)
//				MessageBox("Got not all the data that I need.");
			CString reply = decode(buffer, retval);
			for(int i=0; i < reply.GetLength(); i++) {
				if((reply[i] == 0xA) && i && (reply[i-1] != 0xD))
					reply.Insert(i++, 0xD);
			}

			int len = logtext_edit->GetWindowTextLength();
			logtext_edit->SetSel(len, len);
			logtext_edit->ReplaceSel(reply);

//		}
	}

	delete buffer;
	
	//async select/socket in blocking mode
	if(WSAAsyncSelect(sock, this->m_hWnd, WM_SRVINPUT, FD_READ | FD_CLOSE) != 0)
		MessageBox("WSAAsyncSelect");
}

//Connect to rlogviewd Server
//Disconnect if already connected
void CRlogviewDlg::OnConnect() 
{
	int retval;
	struct sockaddr_in saddr;
	char buffer[4096];
	const int YIPI = ('Y'<<24)+('I'<<16)+('P'<<8)+'I';	//successfull login message
	const int PASS = ('P'<<24)+('A'<<16)+('S'<<8)+'S';	//login message
	CString password, address, port_cstr;
	struct hostent *hostname;

	BeginWaitCursor();

	//if sock != 0 we are already connected. disconnect and change button text
	if(sock) {
		GetDlgItem(IDC_CONNECT)->SetWindowText("Connect");
		SetWindowText("[not connected] - rlogview 0.91");
		closesocket(sock);
		sock = 0;
		return;
	}

	//get key check for validity
	char key_str[9];
	GetDlgItem(IDC_KEY)->GetWindowText(key_str, 9);
	//check for invalid chars not a-z, A-Z and 0-9
	for(unsigned int i=0; i < strlen(key_str); i++) {
		key_str[i] = toupper(key_str[i]);
		if(key_str[i] < '0' || key_str[i] > 'F' || key_str[i] < 'A' && key_str[i] > '9') {
			MessageBox("Hash key can only contains these characters: a-f, A-F and 0-9");
			return;
		}
	}
	key = ntohl(strtoul(key_str, NULL, 16));
	if(!key) {
		MessageBox("Hash key is invalid!");
		return;
	}

	//get address check for emptyness
	GetDlgItem(IDC_ADDRESS)->GetWindowText(address);
	retval = address.Find(':');
	if(retval > 0)
		address = address.Left(retval);
	if(address.IsEmpty()) {
		MessageBox("No Address!");
		return;
	}
	//get port check for emptyness
	GetDlgItem(IDC_PORT)->GetWindowText(port_cstr);
	if(port_cstr.IsEmpty()) {
		MessageBox("No Port!");
		return;
	}
	//get password check for emptyness
	GetDlgItem(IDC_PASSWORD)->GetWindowText(password);
	if(password.IsEmpty()) {
		MessageBox("No Password!");
		return;
	}

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
	saddr.sin_port = htons(atoi(port_cstr.GetBuffer(0)));

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		MessageBox("Error creating socket.");
		return;
	}

	if(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		MessageBox("Error connecting to rlogviewd server.");
		closesocket(sock);
		sock = 0;
		return;
	}

	//send 'encoded' login message
	sprintf(buffer, "PASS %s", password.GetBuffer(0));
	retval = strlen(buffer);
	encode(buffer, retval);
	if(send(sock, buffer, retval, 0) == -1) {
		MessageBox("Error logging in rlogviewd server.");
		closesocket(sock);
		sock = 0;
		return;
	}

	//wait for server response
	if((retval = recv(sock, buffer, 2048, 0)) < 0) {
		MessageBox("Error logging in rlogviewd server.");
		closesocket(sock);
		sock = 0;
	} else {
		decode(buffer, retval);
		if(*(unsigned int *)buffer != ntohl(YIPI)) {
			if(*(unsigned int *)buffer == ntohl(PASS)) {
				//we received back a login message, that's bad: wrong key
				MessageBox("Wrong rlogviewd server hash key!");
				closesocket(sock);
				sock = 0;
				return;
			}
			MessageBox("Wrong rlogviewd server password!");
			closesocket(sock);
			sock = 0;
			return;
		}
		//login successful, display log file name of the server in title bar
		CString dlg_title;
		dlg_title.Format("[%s] - rlogview 0.91", buffer+5);
		SetWindowText(dlg_title);

		//non-blocking select: will indirectly call OnQxConSrvInput() on data reception
		if(WSAAsyncSelect(sock, this->m_hWnd, WM_SRVINPUT, FD_READ | FD_CLOSE) != 0)
			MessageBox("WSAAsyncSelect");

		// no more message box...
		//MessageBox("Successfully logged in rlogviewd server.");
		GetDlgItem(IDC_CONNECT)->SetWindowText("Disconnect");
	}

	EndWaitCursor();
}

void CRlogviewDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(!IsWindowVisible())		//GetDlgItem will bork if this function is called before
		return;					//being made visible(?). OnSize is called before InitDialog.

	//resize log text edit box
	CRect rect;
	GetDlgItem(IDC_LOG_TEXT)->GetWindowRect(&rect);
	ScreenToClient(rect);
	GetDlgItem(IDC_LOG_TEXT)->MoveWindow(rect.left, rect.top, cx-rect.left-8, cy-rect.top-8);
}

//limit window size
void CRlogviewDlg::OnGetMinMaxInfo(HWND, LPMINMAXINFO pMMI)
{
	pMMI->ptMinTrackSize = minsize_pt;
}

//save log text
void CRlogviewDlg::OnSave() 
{
	char filename[1024] = "log.txt";

	CFileDialog save_dlg(FALSE);

	save_dlg.m_ofn.lpstrFile = filename;
	save_dlg.m_ofn.nMaxFile = 1023;

	//display save as dialog
	if(save_dlg.DoModal() == IDCANCEL)
		return;

	//save file
	HANDLE hfile;
	hfile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile == INVALID_HANDLE_VALUE) {
		MessageBox("Error creating file!");
		return;
	}

	unsigned long int i;
	CString log_text;
	GetDlgItem(IDC_LOG_TEXT)->GetWindowText(log_text);
	if(WriteFile(hfile, log_text.GetBuffer(0), log_text.GetLength(), &i, NULL) == 0)
		MessageBox("Error writing file!");

	CloseHandle(hfile);
}

//display a font selection dialog and set font of log text
void CRlogviewDlg::OnFont() 
{
//	CHARFORMAT cf;
//	CRichEditCtrl *logtext = (CRichEditCtrl *)GetDlgItem(IDC_LOG_TEXT);
//	logtext->GetDefaultCharFormat(cf);

	CFontDialog font_dlg;
	CFont *pfont;

	//init font_dlg with current font

	pfont = GetDlgItem(IDC_LOG_TEXT)->GetFont();
	pfont->GetLogFont(font_dlg.m_cf.lpLogFont);

//	font_dlg.m_cf.lpLogFont->lfHeight = -(((cf.yHeight-45)/15) + 3);
//	font_dlg.m_cf.lpLogFont->lfCharSet = cf.bCharSet;
//	font_dlg.m_cf.lpLogFont->lfPitchAndFamily = cf.bPitchAndFamily;
//	strcpy(font_dlg.m_cf.lpLogFont->lfFaceName, cf.szFaceName);

	font_dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;

	if(font_dlg.DoModal() == IDCANCEL)
		return;

	font.DeleteObject();
	font.CreateFontIndirect(font_dlg.m_cf.lpLogFont);
	GetDlgItem(IDC_LOG_TEXT)->SetFont(&font);
}

//save some options in rlogview.ini or QxAdmin.ini
void CRlogviewDlg::OnClose() 
{	
	CString cstr;
	CString profile_name = AfxGetApp()->m_pszHelpFilePath;
	profile_name.Replace(".HLP", ".INI");

	GetDlgItem(IDC_ADDRESS)->GetWindowText(cstr);
	WritePrivateProfileString("rlogoptions", "address", cstr, profile_name);
	GetDlgItem(IDC_PORT)->GetWindowText(cstr);
	WritePrivateProfileString("rlogoptions", "port", cstr, profile_name);
	GetDlgItem(IDC_KEY)->GetWindowText(cstr);
	WritePrivateProfileString("rlogoptions", "key", cstr, profile_name);

	LOGFONT lf;
	CFont *pfont;
	pfont = GetDlgItem(IDC_LOG_TEXT)->GetFont();
	pfont->GetLogFont(&lf);
	WritePrivateProfileStruct("rlogoptions", "font", &lf, sizeof(lf), profile_name);

	CRect rect;
	GetWindowRect(&rect);
	WritePrivateProfileStruct("rlogoptions", "windowpos", &rect, sizeof(rect), profile_name);

	CDialog::OnClose();
}

//put a random hex number in key edit box
void CRlogviewDlg::OnGenerate() 
{
	unsigned int random_key;
	char rand_str[10];

	random_key = rand() << 16;
	random_key += rand();

	sprintf(rand_str, "%08x", random_key);

	GetDlgItem(IDC_KEY)->SetWindowText(rand_str);
}	

//Log text Edit box can only contains 32K of text(LimitText() is useless it seems)
//This function erase empty the edit box
void CRlogviewDlg::OnErrspaceLogText() 
{
//	CString cstr;
//	cstr.Format("log_text len = %u", GetDlgItem(IDC_LOG_TEXT)->GetWindowTextLength());
//	MessageBox(cstr);
	GetDlgItem(IDC_LOG_TEXT)->SetWindowText("========Log text erased. Can't have more than 32Kbytes of text========\r\n");
}

void CRlogviewDlg::OnMaxtextLogText() 
{
	MessageBox("OnMaxtextLogText()");
}

void CRlogviewDlg::OnTest() 
{
	char buf[8192];
	strcpy(buf, "\001\001\001\001\001\001");
	int retval = send(sock, buf, strlen(buf), 0);
	
}
