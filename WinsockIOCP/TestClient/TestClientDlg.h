// TestClientDlg.h : header file
//

#if !defined(AFX_TESTCLIENTDLG_H__588F56A3_CE4E_438D_82E2_B86BC5E40EFA__INCLUDED_)
#define AFX_TESTCLIENTDLG_H__588F56A3_CE4E_438D_82E2_B86BC5E40EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestClientDlg dialog

#include "SocketClass.h"
class CTestClientDlg : public CDialog
{
// Construction
public:
	CTestClientDlg(CWnd* pParent = NULL);	// standard constructor
	CSocketClass		m_socket;

// Dialog Data
	//{{AFX_DATA(CTestClientDlg)
	enum { IDD = IDD_TESTCLIENT_DIALOG };
	CListBox	m_list;
	CEdit	m_wndRec;
	CIPAddressCtrl	m_ipAddress;
	CString	m_strRep;
	CString	m_strDataOut;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL m_bFlood;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnect();
	afx_msg void OnSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFlood();
	//}}AFX_MSG

	LRESULT OnRecMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnStatusMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCLIENTDLG_H__588F56A3_CE4E_438D_82E2_B86BC5E40EFA__INCLUDED_)
