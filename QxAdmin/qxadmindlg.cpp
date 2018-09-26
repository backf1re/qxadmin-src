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


// qxadminDlg.cpp : implementation file
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
// CQxAdminDlg dialog

CQxAdminDlg::CQxAdminDlg(CWnd* pParent /*=NULL*/) : CDialog(CQxAdminDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQxAdminDlg)
	m_server_port = _T("");
	m_status = _T("");
	m_rcon_command = _T("");
	m_rcon_password = _T("");
	m_players_infos_string = _T("");
	m_server_address = _T("");
	m_reply_text = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQxAdminDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CQxAdminDlg::OnPaint() {
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
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQxAdminDlg::OnQueryDragIcon() { return (HCURSOR) m_hIcon; }

void CQxAdminDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQxAdminDlg)
	DDX_Control(pDX, IDC_REPLY, m_reply);
	DDX_Control(pDX, IDC_RCON_COMMAND, m_rcon_command_list);
	DDX_Control(pDX, IDC_SERVER_ADDRESS, m_server_address_list);
	DDX_Control(pDX, IDC_PLAYERS, m_players);
	DDX_Control(pDX, IDC_KEYS_VALUES, m_keys_values);
	DDX_Text(pDX, IDC_SERVER_PORT, m_server_port);
	DDV_MaxChars(pDX, m_server_port, 5);
	DDX_Text(pDX, IDC_STATUS, m_status);
	DDX_Text(pDX, IDC_RCON_COMMAND, m_rcon_command);
	DDX_Text(pDX, IDC_RCON_PASSWORD, m_rcon_password);
	DDX_Text(pDX, IDC_PLAYERS_INFOS_STRING, m_players_infos_string);
	DDX_CBString(pDX, IDC_SERVER_ADDRESS, m_server_address);
	DDX_Text(pDX, IDC_REPLY, m_reply_text);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQxAdminDlg, CDialog)
	//{{AFX_MSG_MAP(CQxAdminDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDQUERY, OnQuery)
	ON_BN_CLICKED(IDC_RCON_SEND, OnRconSend)
	ON_NOTIFY(NM_RCLICK, IDC_KEYS_VALUES, OnRclickKeysValues)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_KEYS_VALUES, OnDblclkKeysValues)
	ON_CBN_SELENDOK(IDC_SERVER_ADDRESS, OnSelendokServerAddressList)
	ON_BN_CLICKED(IDC_RCONSTATUS, OnRconstatus)
	ON_CBN_DROPDOWN(IDC_SERVER_ADDRESS, OnDropdownServerAddressList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PLAYERS, OnColumnclickPlayers)
	ON_CBN_SELENDCANCEL(IDC_SERVER_ADDRESS, OnSelendcancelServerAddress)
	ON_BN_CLICKED(IDC_GET_CONVARS, OnGetConvars)
	ON_CBN_SETFOCUS(IDC_SERVER_ADDRESS, OnSetfocusServerAddress)
	ON_CBN_SETFOCUS(IDC_RCON_COMMAND, OnSetfocusRconCommand)
	ON_BN_CLICKED(IDC_QW, OnQw)
	ON_BN_CLICKED(IDC_Q2, OnQ2)
	ON_BN_CLICKED(IDC_Q3, OnQ3)
	ON_COMMAND(ID_PLAYER_KICK, OnPlayerKick)
	ON_NOTIFY(NM_RCLICK, IDC_PLAYERS, OnRclickPlayers)
	ON_BN_CLICKED(IDC_AUTO_UPDATE, OnAutoUpdate)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_AUTO_UPDATE_TIME, OnChangeAutoUpdateTime)
	ON_BN_CLICKED(IDC_AUTODETECT, OnAutodetect)
	ON_EN_ERRSPACE(IDC_REPLY, OnErrspaceReply)
	ON_BN_CLICKED(IDC_QXCONSRV, OnQxconsrv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CQxAdminDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);				// Set big icon
	SetIcon(m_hIcon, FALSE);			// Set small icon
	
	//set console box font (courier new 9 points)
	font.CreatePointFont(90, "Courier New");
	GetDlgItem(IDC_REPLY)->SetFont(&font);

	m_keys_values.InsertColumn(0,"Key");
	m_keys_values.InsertColumn(1,"Value");
	m_keys_values.SetColumnWidth(0,87);
	m_keys_values.SetColumnWidth(1,183);

	m_players.InsertColumn(0,"Name");
	m_players.InsertColumn(1,"Points");
	m_players.InsertColumn(2,"Ping");
	//m_players.InsertColumn(3,"Address");
	m_players.SetColumnWidth(0,155);
	m_players.SetColumnWidth(1,45);
	m_players.SetColumnWidth(2,45);
	//m_players.SetColumnWidth(3,70);

	//set server history in dialog
	int item;
	for(item = 0; item < MAX_SERVER_HISTORY && !((CQxAdminApp *)AfxGetApp())->ini_servers[item].IsEmpty(); item++)
		m_server_address_list.AddString(((CQxAdminApp *)AfxGetApp())->ini_servers[item]);
	
	if(item != 0) {
		m_server_address_list.GetLBText(0,m_server_address);
		GetDlgItem(IDC_SERVER_ADDRESS)->SetWindowText(m_server_address);
		set_server_data();
	} else
		((CButton*)GetDlgItem(IDC_AUTODETECT))->SetCheck(1);

	//set rcon command history in dialog
	for(item = 0; item < MAX_COMMAND_HISTORY && !((CQxAdminApp *)AfxGetApp())->ini_commands[item].IsEmpty(); item++)
		m_rcon_command_list.AddString(((CQxAdminApp *)AfxGetApp())->ini_commands[item]);

	if(item != 0) {
		m_rcon_command_list.GetLBText(0,m_rcon_command);
		GetDlgItem(IDC_RCON_COMMAND)->SetWindowText(m_rcon_command);
	}

	//set the saved Auto-update time
	CString time_cstr;
	time_cstr.Format("%d", ((CQxAdminApp *)AfxGetApp())->auto_update_time);
	GetDlgItem(IDC_AUTO_UPDATE_TIME)->SetWindowText(time_cstr);

	GetDlgItem(IDC_STATUS)->SetWindowText("Ready.");
	GetDlgItem(IDC_PLAYERS_INFOS_STRING)->SetWindowText("Players Infos:");

	//setup port spin control
	((CSpinButtonCtrl *)GetDlgItem(IDC_PORT_SPIN))->SetRange32(0, 65535);
	//setup auto-update spin control
	((CSpinButtonCtrl *)GetDlgItem(IDC_AUTO_UPDATE_SPIN))->SetRange32(1, 9999);

	server_list_hack = 0;

	ping_sorted = name_sorted = 0;		//default sort order is points descending
	points_sorted = -1;

	((CButton *)GetDlgItem(IDC_DEFAULT_PORT))->SetCheck(1);	//Always default port by default

	//old remote console log viewer
//	qxconclient.Create(IDD_QXCONLOG, this);

	//new one
	rlogview.Create(IDD_RLOGVIEW, this);

	return TRUE;
}

void CQxAdminDlg::OnClose() {

	if(server_list_hack)
		m_server_address_list.DeleteString(m_server_address_list.GetCount()-1);

	int item;

	//prepare for save servers history in .ini
	for(item=0; item<m_server_address_list.GetCount() && item<MAX_SERVER_HISTORY; item++)
		m_server_address_list.GetLBText(item, ((CQxAdminApp *)AfxGetApp())->ini_servers[item]);

	//prepare for save commands history in .ini
	for(item=0; item<m_rcon_command_list.GetCount() && item<MAX_COMMAND_HISTORY; item++)
		m_rcon_command_list.GetLBText(item, ((CQxAdminApp *)AfxGetApp())->ini_commands[item]);

	//prepare to save the Auto-update time
	CString time_cstr;
	GetDlgItem(IDC_AUTO_UPDATE_TIME)->GetWindowText(time_cstr);
	((CQxAdminApp *)AfxGetApp())->auto_update_time = atoi(time_cstr);

	CDialog::OnClose();
}

// detect server type by sending a status packet to the server
int CQxAdminDlg::detect_server_type() {

	char *data;

	GetDlgItem(IDC_STATUS)->SetWindowText("Auto-detecting server type...");
	server.set_type(QUAKE3);
	if(server.status() != 0)
		return QUAKE3;
	server.set_type(QUAKE2);
	if((data = server.status()) != 0)
		if(data[4] == 'p')			//ffffffffprint
			return QUAKE2;
		else
			return QUAKEWORLD;

	server.set_type(0);
	KillTimer(1);
	return 0;
}


// These 4 functions are called by m_keys_values.SortItems and m_players.SortItems

static int CALLBACK sort_cvars(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	return stricmp(((CListCtrl *)lParamSort)->GetItemText(lParam1, 0), ((CListCtrl *)lParamSort)->GetItemText(lParam2, 0));
}
//CASTING MANIA!!! (that is fun)
static int CALLBACK sort_by_name(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CString cstr1 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam1, 0);
	CString cstr2 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam2, 0);
	return ((CQxAdminDlg *)lParamSort)->name_sorted * stricmp(cstr1, cstr2);
}
static int CALLBACK sort_by_points(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	char *dummy;
	CString cstr1 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam1, 1);
	CString cstr2 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam2, 1);
	return ((CQxAdminDlg *)lParamSort)->points_sorted * (strtol(cstr1, &dummy, 10) - strtol(cstr2, &dummy, 10));
}
static int CALLBACK sort_by_ping(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	char *dummy;
	CString cstr1 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam1, 2);
	CString cstr2 = ((CListCtrl *)(((CQxAdminDlg *)lParamSort)->GetDlgItem(IDC_PLAYERS)))->GetItemText(lParam2, 2);
	return ((CQxAdminDlg *)lParamSort)->ping_sorted * (strtol(cstr1, &dummy, 10) - strtol(cstr2, &dummy, 10));
}

//request public server infos, update dialog(server infos, players infos)
int CQxAdminDlg::OnQuery() {

	int pindex = 0;
	char str[16];

	BeginWaitCursor();

	if(!set_server_data()) {
		KillTimer(1);
		return -1;
	}

	GetDlgItem(IDC_STATUS)->SetWindowText("Getting server infos...");

	// Update players infos box
	if(server.get_players() != -1) {	
		m_players.DeleteAllItems();
		for(pindex=0; (server.players[pindex].name[0] != 0) && (pindex < PLAYERS_NUM); pindex++) {	
			CString player_name = server.players[pindex].name;
			m_players.InsertItem(pindex, convert_crappy_name(player_name.GetBuffer(0)));
			m_players.SetItemText(pindex, 1, itoa(server.players[pindex].points, str, 10));
			m_players.SetItemText(pindex, 2, itoa(server.players[pindex].ping, str, 10));
			m_players.SetItemData(pindex, pindex);			//for sorting
		}
		// players sort
		if(points_sorted != 0)
			m_players.SortItems(sort_by_points, (LPARAM)this);
		else if(ping_sorted != 0)
			m_players.SortItems(sort_by_ping, (LPARAM)this);
		else if(name_sorted != 0)
			m_players.SortItems(sort_by_name, (LPARAM)this);

		// Update server infos box
		int getconvars_success = 0;
		if(((CButton *)GetDlgItem(IDC_ALWAYS_CVARS))->GetCheck() && server.get_type() != QUAKEWORLD) {
			if(OnGetConvars() == 0)
				getconvars_success = 1;
		}
		if(!getconvars_success && (server.get_keys() != -1)) {
			int first_key = m_keys_values.GetTopIndex();	//save scrollbar position
			m_keys_values.DeleteAllItems();
			for(int i=0 ; (server.keys[i].name[0] != 0) && (i < KEYS_NUM) ; i++) {
				m_keys_values.InsertItem(i, server.keys[i].name);
				m_keys_values.SetItemText(i, 1, server.keys[i].value);
			}
			//scroll back to previous position in server infos box
			//hand tuned number: 14 (a way to find the height of a clistctrl item?)
			m_keys_values.Scroll(CSize(0, first_key * 14));
		}
	} else {
		KillTimer(1);
		error("Server did not respond.");
		return -1;
	}

	// update players infos header(title)
	switch(server.get_type()) {
		case QUAKE3:
			maxclients = server.get_key("sv_maxclients"); break;
		case QUAKE2:
		case QUAKEWORLD:
			maxclients = server.get_key("maxclients"); break;
		default:
			maxclients = "0";
	}
	m_players_infos_string.Format("Players (%d/%s) Infos:",pindex == 0 ? 0 : pindex--,maxclients);
	GetDlgItem(IDC_PLAYERS_INFOS_STRING)->SetWindowText(m_players_infos_string);

	//Update combo box drop list

	CString port_str = itoa(server.get_udp_port(), str, 10);
	CString type_str;
	int item;
	switch(server.get_type()) {
		case QUAKEWORLD:
			type_str = "QW"; break;
		case QUAKE2:
			type_str = "Q2"; break;
		case QUAKE3:
			type_str = "Q3"; break;
		default:
			type_str = "??";
	}

	//add composite server address in combo box on the first line
	CString tmp_server_address = m_server_address + ":" + port_str + ":" + type_str;
	if((item = m_server_address_list.FindString(-1,tmp_server_address)) >= 0)
		m_server_address_list.DeleteString(item);
	tmp_server_address += ":" + m_rcon_password;
	m_server_address_list.InsertString(0,tmp_server_address);
	if(m_server_address_list.GetCount() > MAX_SERVER_HISTORY)
		m_server_address_list.DeleteString(MAX_SERVER_HISTORY);

	GetDlgItem(IDC_SERVER_ADDRESS)->SetWindowText(m_server_address);
	GetDlgItem(IDC_STATUS)->SetWindowText("Ready.");

	EndWaitCursor();		//Why 2? Because it doesn't work with 1.
	EndWaitCursor();

	return 0;
}

//send a rcon command
void CQxAdminDlg::OnRconSend() {

	this->UpdateData();

	if(m_rcon_password.IsEmpty()) {
		error("No rcon password!");
		return;
	}
	if(m_rcon_command.IsEmpty()) {
		error("No rcon command!");
		return;
	}
	if(!set_server_data())
		return;

	BeginWaitCursor();
	GetDlgItem(IDC_STATUS)->SetWindowText("Sending rcon command...");

	CString reply = server.rcon(m_rcon_command.GetBuffer(0));
	m_rcon_command.ReleaseBuffer();

	if(reply.IsEmpty()) {
		error("Server did not respond.");
		return;
	}

	//remove "packet" header
	switch(server.get_type()) {
		case QUAKEWORLD:
			reply = reply.GetBuffer(0) + 5; break;	//skip "\xff\xff\xff\xffn";
		case QUAKE2:
		case QUAKE3:
			reply = reply.GetBuffer(0) + 10;		//skip "\xff\xff\xff\xffprint\n";
	}

	//Insert returns
	for(int i=0; i<reply.GetLength(); i++) {
		if(reply[i] == 0xA)
			reply.Insert(i++, 0xD);
	}

	//append reply to console cedit box
	CEdit *reply_wnd = (CEdit *)GetDlgItem(IDC_REPLY);
	int len = reply_wnd->GetWindowTextLength();
	reply_wnd->SetSel(len, len);
	reply_wnd->ReplaceSel(reply);

	OnQuery();

	int item;
	if((item = m_rcon_command_list.FindString(-1,m_rcon_command)) >= 0)
		m_rcon_command_list.DeleteString(item);
	m_rcon_command_list.InsertString(0,m_rcon_command);
	if(m_rcon_command_list.GetCount() > MAX_COMMAND_HISTORY)
		m_rcon_command_list.DeleteString(MAX_COMMAND_HISTORY);

	EndWaitCursor();

	m_rcon_command_list.SetCurSel(0);
}

//pop up a dialog window allowing the user to edit a server key value
void CQxAdminDlg::OnRclickKeysValues(NMHDR* pNMHDR, LRESULT* pResult) {

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	edit_key = m_keys_values.GetItemText(pNMListView->iItem,0);
	edit_value = m_keys_values.GetItemText(pNMListView->iItem,1);

	if(edit_key.IsEmpty() || edit_value.IsEmpty() || (edit_key[0] == '*')) {
		*pResult = 0;
		return;
	}

	CEditDlg editdlg;

	int nResponse = editdlg.DoModal();

	if(nResponse == IDOK) {
		
		this->UpdateData(TRUE);
		if(edit_key == "mapname")
			m_rcon_command = "map " + edit_value;
		else
			switch(server.get_type()) {
				case QUAKEWORLD:
					m_rcon_command.Format("%s \"%s\"", edit_key, edit_value); break;
				default:
					m_rcon_command.Format("set %s \"%s\"", edit_key, edit_value);
			}
		this->UpdateData(FALSE);
		OnRconSend();

	}

	*pResult = 0;
}

void CQxAdminDlg::OnDblclkKeysValues(NMHDR* pNMHDR, LRESULT* pResult) {
	OnRclickKeysValues(pNMHDR,pResult);
	*pResult = 0;
}

void CQxAdminDlg::error(CString msg) {
	MessageBox(msg,"QxAdmin: Error");
	GetDlgItem(IDC_STATUS)->SetWindowText("Ready.");
}

// set-up "server" class from the dialog data
int CQxAdminDlg::set_server_data() {
	char *endptr;

	GetDlgItem(IDC_SERVER_ADDRESS)->GetWindowText(m_server_address);
	if(!parse_server_infos(m_server_address)) {
		error("No server address!");
		KillTimer(1);
		return 0;
	}

	//Get data from dialog
	GetDlgItem(IDC_SERVER_ADDRESS)->GetWindowText(m_server_address);
	GetDlgItem(IDC_SERVER_PORT)->GetWindowText(m_server_port);
	GetDlgItem(IDC_RCON_PASSWORD)->GetWindowText(m_rcon_password);

	if(m_server_port.IsEmpty()) {
		error("No server port!");
		KillTimer(1);
		return 0;
	}

	server.set_address(m_server_address.GetBuffer(0));
	server.set_udp_port((unsigned short)strtol(m_server_port, &endptr, 10));
	server.set_rcon_password(m_rcon_password.GetBuffer(0));

	//set server type, autodetect server if "auto-detect" is selected
	//set types buttons
	if(((CButton*)GetDlgItem(IDC_QW))->GetCheck())
		server.set_type(QUAKEWORLD);
	else if(((CButton*)GetDlgItem(IDC_Q2))->GetCheck())
		server.set_type(QUAKE2);
	else if(((CButton*)GetDlgItem(IDC_Q3))->GetCheck())
		server.set_type(QUAKE3);
	else {												//auto detect

		((CButton*)GetDlgItem(IDC_AUTODETECT))->SetCheck(0);

		server.set_type(detect_server_type());
		switch(server.get_type()) {
			case QUAKEWORLD:
				((CButton*)GetDlgItem(IDC_QW))->SetCheck(1); break;
			case QUAKE2:
				((CButton*)GetDlgItem(IDC_Q2))->SetCheck(1); break;
			case QUAKE3:
				((CButton*)GetDlgItem(IDC_Q3))->SetCheck(1); break;
			default:
				((CButton*)GetDlgItem(IDC_AUTODETECT))->SetCheck(1);
				error("Unable to auto-detect server type.");
				KillTimer(1);
				return 0;
		}
	}

	return 1;
}

//set address,port,type,password controls (controls only! not member variables!)
// from a composite server infos[ address:port:type:rcon_password ]
// if one of the parameter is absent the control is not updated
int CQxAdminDlg::parse_server_infos(CString &server_infos) {

	char address[64]="", port[6]="", type[3]="", password[64]="";

	if(server_infos.IsEmpty()) return 0;

	char *temp = strdup(server_infos.GetBuffer(0));
	sscanf(temp, "%63[^:]:%5[^:]:%2[^:]:%63s", &address, &port, &type, &password);

	if(strlen(address) != 0)
		GetDlgItem(IDC_SERVER_ADDRESS)->SetWindowText(address);
	if(strlen(port) != 0)
		GetDlgItem(IDC_SERVER_PORT)->SetWindowText(port);
	if(strlen(password) != 0)
		GetDlgItem(IDC_RCON_PASSWORD)->SetWindowText(password);

	if(strlen(type) != 0) {
		((CButton*)GetDlgItem(IDC_QW))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_Q2))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_Q3))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_AUTODETECT))->SetCheck(0);
		switch(type[1]) {
			case 'W':
				((CButton*)GetDlgItem(IDC_QW))->SetCheck(1);
				GetDlgItem(IDC_GET_CONVARS)->EnableWindow(FALSE);
				GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(FALSE); break;
			case '2':
				((CButton*)GetDlgItem(IDC_Q2))->SetCheck(1);
				GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
				GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);  break;
			case '3':
				((CButton*)GetDlgItem(IDC_Q3))->SetCheck(1);
				GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
				GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);  break;
			default:
				((CButton*)GetDlgItem(IDC_AUTODETECT))->SetCheck(1);
				GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
				GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);
		}
	}

	free(temp);
	return 1;
}

//If the user select a server from the dropdown list we parse the composite addrese and update
//the dialog, then setup for the "hack"
//The hack: onselendok we add an item at the end of the combolist with the value of the server
//address _only_. This allows to have only the address in the edit box after a successful
//selection. Without this the "auto-selection" will kick in and display the full composite
//server address. To complete the hack, in OnDropdownServerAddressList() the previously added
//address is deleted just before displaying the list of servers.
//[Doing all the drawing could work too.]
void CQxAdminDlg::OnSelendokServerAddressList() {

	CString server_infos;
	int cursel = m_server_address_list.GetCurSel();
	if(cursel == -1)
		return;
	m_server_address_list.GetLBText(cursel, server_infos);
	parse_server_infos(server_infos);

	GetDlgItem(IDC_SERVER_ADDRESS)->GetWindowText(m_server_address);
	m_server_address_list.SetCurSel(m_server_address_list.AddString(m_server_address));

	server_list_hack = 1;

	SetDefID(IDQUERY);
}

void CQxAdminDlg::OnSelendcancelServerAddress() {
	SetDefID(IDQUERY);
}

//delete the previously added(in OnSelendokServerAddressList()) address 
void CQxAdminDlg::OnDropdownServerAddressList() {
	
	this->UpdateData(TRUE);			//empeche la nouvelle adresse de se faire overwriter par
									//losti dauto selection.
	this->UpdateData(FALSE);
	m_reply.LineScroll(30000);

	if(server_list_hack) {
		m_server_address_list.DeleteString(m_server_address_list.GetCount()-1);
		server_list_hack = 0;
	}
}

//send a rcon "status" command
void CQxAdminDlg::OnRconstatus() {
	GetDlgItem(IDC_RCON_COMMAND)->SetWindowText("status");
	OnRconSend();
}

//sort the players list
void CQxAdminDlg::OnColumnclickPlayers(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	for(int item = 0; item < m_players.GetItemCount(); item++)
		m_players.SetItemData(item, item);

	//pNMListView->iSubItem = 0,1,2,3 = name, points, ping, address
	switch(pNMListView->iSubItem) {
		case 0:
			points_sorted = 0;
			ping_sorted = 0;
			if(name_sorted <= 0)	//first sort is ascending
				name_sorted = 1;
			else
				name_sorted = -1;
			m_players.SortItems(sort_by_name, (LPARAM)this); break;
		case 1:
			name_sorted = 0;
			ping_sorted = 0;
			if(points_sorted >= 0)	//first sort is descending
				points_sorted = -1;
			else
				points_sorted = 1;
			m_players.SortItems(sort_by_points, (LPARAM)this); break;
		case 2:
			name_sorted = 0;
			points_sorted = 0;
			if(ping_sorted <= 0)	//first sort is ascending
				ping_sorted = 1;
			else
				ping_sorted = -1;
			m_players.SortItems(sort_by_ping, (LPARAM)this); break;
	}

	*pResult = 0;
}


// Pressing enter after entering an address, query the server
void CQxAdminDlg::OnSetfocusServerAddress() {
	SetDefID(IDQUERY);	
}

// Pressing enter after entering a rcon command, send it
void CQxAdminDlg::OnSetfocusRconCommand() {
	SetDefID(IDC_RCON_SEND);
}

// puts console variables in the server infos box
//this not work for quakeworld(there's no cvarlist command)
int CQxAdminDlg::OnGetConvars() {
	this->UpdateData();

	if(((CButton*)GetDlgItem(IDC_QW))->GetCheck()) {
		error("Console variables not available in QuakeWorld!");
		return -1;
	}
	if(m_rcon_password.IsEmpty()) {
		error("No rcon password!");
		return -1;
	}
	if(!set_server_data())
		return -1;

	char *ptr = server.rcon("cvarlist");

	if(!ptr) {
		error("Server did not respond.");
		return -1;
	}

	char *cvars = strdup(ptr);

	//save scrollbar position
	int first_key = m_keys_values.GetTopIndex();

	m_keys_values.DeleteAllItems();

	int item = 0, type = server.get_type();
	char *pos;
	
	m_keys_values.SetRedraw(FALSE);

	//parse cvarlist reply
	char *cvar = strtok(cvars+10, "\n");
	while(cvar) {
		if(type == QUAKE2)
			cvar += 5;
		else //QUAKE3
			cvar += 8;
		pos = strchr(cvar, '"');
		if(pos != 0) {
			*(pos-1) = 0;
			m_keys_values.InsertItem(item, cvar);
			cvar = pos+1;
			pos = strchr(cvar, '"');
			if(pos != 0) {
				*pos = 0;
				m_keys_values.SetItemText(item, 1, cvar);
			}
			m_keys_values.SetItemData(item, item);
		}
		cvar = strtok(NULL, "\n");
		item++;
	}
	free(cvars);

	m_keys_values.SortItems(sort_cvars, (LPARAM)GetDlgItem(IDC_KEYS_VALUES));

	m_keys_values.SetRedraw(TRUE);
	m_keys_values.Invalidate();

	//scroll back to previous position in server infos box
	//hand tuned number: 14 (a way to find the height of a clistctrl item?)
	m_keys_values.Scroll(CSize(0, first_key * 14));


	return 0;
}

// Converts QW "funny" names and remove Q3 colored names
char *CQxAdminDlg::convert_crappy_name(char *name) {

	unsigned int c, name_len, d;
	char *qw_sane_chars = "     .        ..[]0123456789.<-> !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f[=]+ .       >..[]0123456789.<-> !\"#$%&\xff()*+,-./0123456789:;[=]?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\xff";

	switch(server.get_type()) {
		case QUAKE3:
			name_len = strlen(name);
			for(c = 0, d = 0; d < name_len; c++, d++) {
				if(name[d] == '\x1b')
					d += 2;
				if(d > name_len)
					d--;
				name[c] = name[d];
			}
			name[c] = 0;
			return name;
		case QUAKEWORLD:
			for(c = 0; c < strlen(name); c++)
				name[c] = qw_sane_chars[(unsigned char)name[c]];
			return name;
			break;
		default:
			return name;
	}
}

// Put default port if no port or Always default port is selected
void CQxAdminDlg::OnQw() {
	GetDlgItem(IDC_SERVER_PORT)->GetWindowText(m_server_port);
	if(((CButton *)GetDlgItem(IDC_DEFAULT_PORT))->GetCheck() || m_server_port.IsEmpty())
		GetDlgItem(IDC_SERVER_PORT)->SetWindowText("27500");
	GetDlgItem(IDC_GET_CONVARS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(FALSE);
}

// Put default port if no port or Always default port is selected
void CQxAdminDlg::OnQ2() {
	GetDlgItem(IDC_SERVER_PORT)->GetWindowText(m_server_port);
	if(((CButton *)GetDlgItem(IDC_DEFAULT_PORT))->GetCheck() || m_server_port.IsEmpty())
		GetDlgItem(IDC_SERVER_PORT)->SetWindowText("27910");
	GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);
}

// Put default port if no port or Always default port is selected
void CQxAdminDlg::OnQ3() {
	GetDlgItem(IDC_SERVER_PORT)->GetWindowText(m_server_port);
	if(((CButton *)GetDlgItem(IDC_DEFAULT_PORT))->GetCheck() || m_server_port.IsEmpty())
		GetDlgItem(IDC_SERVER_PORT)->SetWindowText("27960");
	GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);
}

//setup the rcon command to kick the player selected with OnRclickPalyers
void CQxAdminDlg::OnPlayerKick() {

	CString kickee, player_name;				//kickee: name(q2/q3) or id(qw)
	CString sel_name = m_players.GetItemText((int)(m_players.GetFirstSelectedItemPosition())-1, 0);


	if(server.get_type() == QUAKE3) {
		char *p_start_tag = "---\n";
		char *status = server.rcon("status");
		if(!status) return;
		status = strstr(status, p_start_tag);
		if(status == 0)
			return;
		status += strlen(p_start_tag);

		int num, dumint;
		char address_str[256], name[256], lastmsg_str[256];
		status = strtok(status, "\n");
		while(status) {
			sscanf(status, "%d %d %d %s %s %s %d %d %d", &num, &dumint, &dumint, &name, &lastmsg_str, &address_str, &dumint, &dumint);
			convert_crappy_name(name);
			sel_name.TrimRight(' ');
			if(strcmp(sel_name.GetBuffer(0), name) == 0) {
				kickee.Format("%d", num);
				GetDlgItem(IDC_RCON_COMMAND)->SetWindowText(CString("kick ") + kickee);
				OnRconSend();
				break;
			}
//try to parse names with spaces at the end,
// it works only if the name length > 14 chars ==> useless
/*		int posdiff;
		char *namepos, *lastmsgpos;
		namepos = strstr(status, name);
		if(!namepos) return;
		lastmsgpos = strstr(namepos, lastmsgstr);
		if(!lastmsgpos) return;
		posdiff = lastmsgpos + strlen(lastmsgstr) - namepos;
		if(posdiff != 23)
			for(;posdiff > 23; posdiff--)
				strcat(name, " ");
*/
			status = strtok(NULL, "\n");
		}

	} else {
		OnQuery();
		for(int i = 0; i < m_players.GetItemCount() && i < PLAYERS_NUM; i++) {
			player_name = server.players[i].name;	//save player name, cause convert modifies it
			if(strcmp(convert_crappy_name(server.players[i].name), sel_name.GetBuffer(0)) == 0) {
				if(server.get_type() == QUAKEWORLD)
					kickee.Format("%d",  server.players[i].id);
				else
					kickee = player_name;
				GetDlgItem(IDC_RCON_COMMAND)->SetWindowText(CString("kick ") + kickee);
				OnRconSend();
				break;
			}
		}
	}
}


// Display a pop-up menu to kick a player
void CQxAdminDlg::OnRclickPlayers(NMHDR* pNMHDR, LRESULT* pResult) {	

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->iItem == -1)
		return;

	POINT pt = pNMListView->ptAction;
	GetDlgItem(IDC_PLAYERS)->ClientToScreen(&pt);

	//Pop-up player actions menu(only kick for the moment)
	CMenu players_menu;

	players_menu.LoadMenu(IDR_MENU_PLAYERS);
	CMenu *popup = players_menu.GetSubMenu(0);

	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, this);

	CString player_name = m_players.GetItemText(pNMListView->iItem, 0);

	*pResult = 0;
}

//Set the timer
//Disable the auto update time edit box if "Auto-update" is unchecked
void CQxAdminDlg::OnAutoUpdate() {

	CString time_cstr;

	if(((CButton *)GetDlgItem(IDC_AUTO_UPDATE))->GetCheck()) {
		GetDlgItem(IDC_AUTO_UPDATE_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_AUTO_UPDATE_SPIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_AUTO_UPDATE_TIME)->GetWindowText(time_cstr);
		int time = atol(time_cstr);
		if(time > 0)
			SetTimer(1, time * 1000, 0);
	} else {
		GetDlgItem(IDC_AUTO_UPDATE_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_AUTO_UPDATE_SPIN)->EnableWindow(FALSE);
		KillTimer(1);
	}
}

//Query the server at IDC_AUTO_UPDATE_TIME interval
void CQxAdminDlg::OnTimer(UINT nIDEvent) {
	
	if(nIDEvent == 1) {
		if(OnQuery() != 0) {
			((CButton *)GetDlgItem(IDC_AUTO_UPDATE))->SetCheck(0);
			OnAutoUpdate();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

//reset the timer with the new time value
void CQxAdminDlg::OnChangeAutoUpdateTime() {

	CString time_cstr;

	if(((CButton *)GetDlgItem(IDC_AUTO_UPDATE))->GetCheck()) {
		GetDlgItem(IDC_AUTO_UPDATE_TIME)->GetWindowText(time_cstr);
		int time = atol(time_cstr);
		if(time > 0)
			SetTimer(1, time * 1000, 0);
	}
}

void CQxAdminDlg::OnAutodetect() {
	GetDlgItem(IDC_GET_CONVARS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALWAYS_CVARS)->EnableWindow(TRUE);
}

//erase reply text when cedit box is full
//this is crude but you gotta do what you gotta do
void CQxAdminDlg::OnErrspaceReply() {
	GetDlgItem(IDC_REPLY)->SetWindowText("++++BackBuffer erased(there was too much text)++++\r\n\r\n");
}

//show qxcon log file window
void CQxAdminDlg::OnQxconsrv() {
//	qxconclient.ShowWindow(SW_SHOW);
//	qxconclient.SetActiveWindow();
	rlogview.ShowWindow(SW_SHOW);
	rlogview.SetActiveWindow();

}
