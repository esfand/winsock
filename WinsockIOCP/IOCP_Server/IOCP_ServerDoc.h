// IOCP_ServerDoc.h : interface of the CIOCP_ServerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOCP_SERVERDOC_H__25F7D389_D749_457E_A02D_4A281B4B88BF__INCLUDED_)
#define AFX_IOCP_SERVERDOC_H__25F7D389_D749_457E_A02D_4A281B4B88BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CIOCP_ServerDoc : public CDocument
{
protected: // create from serialization only
	CIOCP_ServerDoc();
	DECLARE_DYNCREATE(CIOCP_ServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIOCP_ServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIOCP_ServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIOCP_ServerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOCP_SERVERDOC_H__25F7D389_D749_457E_A02D_4A281B4B88BF__INCLUDED_)
