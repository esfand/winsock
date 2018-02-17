// IOCP_Server.h : main header file for the IOCP_SERVER application
//

#if !defined(AFX_IOCP_SERVER_H__B206A39D_49A4_4C46_82A5_2922B012C52D__INCLUDED_)
#define AFX_IOCP_SERVER_H__B206A39D_49A4_4C46_82A5_2922B012C52D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerApp:
// See IOCP_Server.cpp for the implementation of this class
//

class CIOCP_ServerApp : public CWinApp
{
public:
	CIOCP_ServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIOCP_ServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CIOCP_ServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOCP_SERVER_H__B206A39D_49A4_4C46_82A5_2922B012C52D__INCLUDED_)
