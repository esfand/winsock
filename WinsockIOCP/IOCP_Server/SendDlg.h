#if !defined(AFX_SENDDLG_H__F561BF61_B3DC_488A_A512_5A8EEB52A902__INCLUDED_)
#define AFX_SENDDLG_H__F561BF61_B3DC_488A_A512_5A8EEB52A902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendDlg dialog

class CSendDlg : public CDialog
{
// Construction
public:
	CSendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendDlg)
	enum { IDD = IDD_DIALOG1 };
	CIPAddressCtrl	m_ipCtrl;
	CString	m_strData;
	//}}AFX_DATA

	CString		m_strIP;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendDlg)
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDDLG_H__F561BF61_B3DC_488A_A512_5A8EEB52A902__INCLUDED_)
