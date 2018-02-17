// IOCP_ServerView.h : interface of the CIOCP_ServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOCP_SERVERVIEW_H__3E1ED9C9_2AEB_4210_8E70_576E98B35E82__INCLUDED_)
#define AFX_IOCP_SERVERVIEW_H__3E1ED9C9_2AEB_4210_8E70_576E98B35E82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIOCP_ServerDoc;
struct ClientContext;
class CIOCP_ServerView : public CFormView
{
protected: // create from serialization only
	CIOCP_ServerView();
	DECLARE_DYNCREATE(CIOCP_ServerView)

// Attributes
public:
	//{{AFX_DATA(CIOCP_ServerView)
	enum { IDD = IDD_IOCP_FORM };
	CListCtrl	m_list;
	CString	m_sActive;
	CString	m_sBusy;
	CString	m_sWorkers;
	//}}AFX_DATA

	CIOCP_ServerDoc* GetDocument();
// Operations
public:
	void RemoveFromList(ClientContext* pContext);
	void AddToList(ClientContext* pContext);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIOCP_ServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIOCP_ServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIOCP_ServerView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCheck1();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IOCP_ServerView.cpp
inline CIOCP_ServerDoc* CIOCP_ServerView::GetDocument()
   { return (CIOCP_ServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOCP_SERVERVIEW_H__3E1ED9C9_2AEB_4210_8E70_576E98B35E82__INCLUDED_)
