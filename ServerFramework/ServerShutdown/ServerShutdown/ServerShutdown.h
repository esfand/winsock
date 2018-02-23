// ServerShutdown.h : main header file for the SERVERSHUTDOWN application
//

#if !defined(AFX_SERVERSHUTDOWN_H__11B18585_EF0E_4B74_B9B7_FC9898611518__INCLUDED_)
#define AFX_SERVERSHUTDOWN_H__11B18585_EF0E_4B74_B9B7_FC9898611518__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CServerShutdownApp:
// See ServerShutdown.cpp for the implementation of this class
//

class CServerShutdownApp : public CWinApp
{
public:
	CServerShutdownApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerShutdownApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CServerShutdownApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERSHUTDOWN_H__11B18585_EF0E_4B74_B9B7_FC9898611518__INCLUDED_)
