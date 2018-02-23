// ServerShutdownDlg.h : header file
//

#if !defined(AFX_SERVERSHUTDOWNDLG_H__66169577_AD59_49B9_9CE0_D46B08D16814__INCLUDED_)
#define AFX_SERVERSHUTDOWNDLG_H__66169577_AD59_49B9_9CE0_D46B08D16814__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JetByteTools\Win32Tools\ManualResetEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CServerShutdownDlg dialog

class CServerShutdownDlg : public CDialog
{
// Construction
public:
	CServerShutdownDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CServerShutdownDlg)
	enum { IDD = IDD_SERVERSHUTDOWN_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerShutdownDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServerShutdownDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnShutdown();
	afx_msg void OnPauseResume();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private :

   JetByteTools::Win32::CManualResetEvent m_shutdownEvent;
   JetByteTools::Win32::CManualResetEvent m_pauseResumeEvent;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERSHUTDOWNDLG_H__66169577_AD59_49B9_9CE0_D46B08D16814__INCLUDED_)
