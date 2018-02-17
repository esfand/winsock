// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A9F8D191_8403_4A11_938B_3550546D4841__INCLUDED_)
#define AFX_MAINFRM_H__A9F8D191_8403_4A11_938B_3550546D4841__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IOCPServer.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
public:

// Operations
public:
	CIOCPServer		m_iocpServer;

	static void CALLBACK NotifyProc(LPVOID lpParam, ClientContext* pContext, UINT nCode);
	void Activate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileSend();
	afx_msg void OnCheck1();
	//}}AFX_MSG

	LRESULT OnDataInMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientConnect(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A9F8D191_8403_4A11_938B_3550546D4841__INCLUDED_)
