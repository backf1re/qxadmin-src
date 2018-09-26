; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CQxConLog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "qxadmin.h"
LastPage=0

ClassCount=4
Class1=CEditDlg
Class2=CQxAdminApp
Class3=CQxAdminDlg

ResourceCount=6
Resource1=IDD_QXCONLOG
Resource2=IDD_QXCONSRV_CONNECT
Resource3=IDD_QXADMIN
Resource4=IDD_EDIT_INFO
Class4=CQxConLog
Resource5=IDD_RLOGVIEW
Resource6=IDR_MENU_PLAYERS

[CLS:CEditDlg]
Type=0
BaseClass=CDialog
HeaderFile=EditDlg.h
ImplementationFile=EditDlg.cpp
LastObject=CEditDlg

[CLS:CQxAdminApp]
Type=0
BaseClass=CWinApp
HeaderFile=qxadmin.h
ImplementationFile=qxadmin.cpp

[CLS:CQxAdminDlg]
Type=0
BaseClass=CDialog
HeaderFile=qxadminDlg.h
ImplementationFile=qxadminDlg.cpp
LastObject=CQxAdminDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_EDIT_INFO]
Type=1
Class=CEditDlg
ControlCount=6
Control1=IDC_VALUE,edit,1350631556
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_SERVER_INFO_KEY,static,1342308352

[MNU:IDR_MENU_PLAYERS]
Type=1
Class=CQxAdminDlg
Command1=ID_PLAYER_KICK
CommandCount=1

[DLG:IDD_QXCONSRV_CONNECT]
Type=1
Class=?
ControlCount=7
Control1=IDC_QXCONSRV_CONNECT,button,1342242817
Control2=IDC_QXCONSRV_PASSWORD,edit,1350631552
Control3=IDC_QXCONSRV_PORT,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_LOGTEXT,edit,1352728772
Control7=IDC_CLOSE,button,1342242816

[DLG:IDD_QXCONLOG]
Type=1
Class=CQxConLog
ControlCount=9
Control1=IDC_QXCON_CONNECT,button,1342242817
Control2=IDC_QXCON_HIDE,button,1342242816
Control3=IDC_QXCON_PASSWORD,edit,1350631584
Control4=IDC_QXCON_PORT,edit,1350639744
Control5=IDC_QXCON_PORT_SPIN,msctls_updown32,1342177463
Control6=IDC_QXCON_FILTER,button,1476460547
Control7=IDC_LOG_TEXT,edit,1352732868
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[DLG:IDD_QXADMIN]
Type=1
Class=CQxAdminDlg
ControlCount=33
Control1=IDC_SERVER_ADDRESS,combobox,1342242818
Control2=IDC_SERVER_PORT,edit,1350631552
Control3=IDC_PORT_SPIN,msctls_updown32,1342177463
Control4=IDC_QW,button,1342242825
Control5=IDC_Q2,button,1342242825
Control6=IDC_Q3,button,1342242825
Control7=IDC_AUTODETECT,button,1342242825
Control8=IDC_RCON_PASSWORD,edit,1350631584
Control9=IDC_RCON_COMMAND,combobox,1344339970
Control10=IDQUERY,button,1342242817
Control11=IDC_RCON_SEND,button,1342242816
Control12=IDC_REPLY,edit,1352732868
Control13=IDC_KEYS_VALUES,SysListView32,1342212101
Control14=IDC_PLAYERS,SysListView32,1342177285
Control15=IDC_STATIC,static,1342308364
Control16=IDC_STATIC,static,1342308364
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATUS,static,1342308352
Control19=IDC_STATIC,static,1342177292
Control20=IDC_STATIC,static,1342308352
Control21=IDC_PLAYERS_INFOS_STRING,static,1342308352
Control22=IDC_STATIC,static,1342181388
Control23=IDC_STATIC,button,1342193671
Control24=IDC_STATIC,static,1342177280
Control25=IDC_RCONSTATUS,button,1342242816
Control26=IDC_GET_CONVARS,button,1342242816
Control27=IDC_DEFAULT_PORT,button,1342242819
Control28=IDC_ALWAYS_CVARS,button,1342243075
Control29=IDC_AUTO_UPDATE,button,1342242819
Control30=IDC_AUTO_UPDATE_TIME,edit,1484857474
Control31=IDC_AUTO_UPDATE_SPIN,msctls_updown32,1476395191
Control32=IDC_STATIC,static,1342308352
Control33=IDC_QXCONSRV,button,1342242816

[CLS:CQxConLog]
Type=0
HeaderFile=qxconlog.h
ImplementationFile=qxconlog.cpp
BaseClass=CDialog
Filter=D
LastObject=CQxConLog
VirtualFilter=dWC

[DLG:IDD_RLOGVIEW]
Type=1
Class=?
ControlCount=14
Control1=IDC_CONNECT,button,1342242817
Control2=IDC_PASSWORD,edit,1350631584
Control3=IDC_PORT,edit,1350639744
Control4=IDC_PORT_SPIN_R,msctls_updown32,1342177463
Control5=IDC_LOG_TEXT,edit,1352732868
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_ADDRESS,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_KEY,edit,1350631552
Control12=IDC_SAVE,button,1342242816
Control13=IDC_GENERATE,button,1342242816
Control14=IDC_FONT,button,1342242816

