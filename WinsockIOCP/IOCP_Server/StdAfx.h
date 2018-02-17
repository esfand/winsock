// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7868572A_8057_429D_905B_FA2FC0A98261__INCLUDED_)
#define AFX_STDAFX_H__7868572A_8057_429D_905B_FA2FC0A98261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


#include "Buffer.h"


#define WM_CLIENT_CONNECT	WM_APP + 0x1001
#define WM_CLIENT_CLOSE		WM_APP + 0x1002

#define WM_CLIENT_NOTIFY	WM_APP + 0x1003

#define WM_DATA_IN_MSG		WM_APP + 0x1004
#define WM_DATA_OUT_MSG		WM_APP + 0x1005


// string message allocator for posting messages between windows...
static char* AllocBuffer(CString strMsg)
{
	int nLen = strMsg.GetLength();
	char *pBuffer = new char[nLen+1]; 
	
	strcpy(pBuffer,(const char*)strMsg);

	ASSERT(pBuffer != NULL);
	return pBuffer;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7868572A_8057_429D_905B_FA2FC0A98261__INCLUDED_)
