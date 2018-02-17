// SendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IOCP_Server.h"
#include "SendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendDlg dialog


CSendDlg::CSendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendDlg)
	m_strData = _T("");
	//}}AFX_DATA_INIT
}


void CSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDlg)
	DDX_Control(pDX, IDC_IPADDRESS, m_ipCtrl);
	DDX_Text(pDX, IDC_EDIT, m_strData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendDlg, CDialog)
	//{{AFX_MSG_MAP(CSendDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendDlg message handlers

void CSendDlg::OnOk() 
{
	UpdateData(TRUE);
	
	m_ipCtrl.GetWindowText(m_strIP);

	EndDialog(TRUE);
}
