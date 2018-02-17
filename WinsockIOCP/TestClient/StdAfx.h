// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F8CD0C1F_36FC_4540_98B2_E54007756D2C__INCLUDED_)
#define AFX_STDAFX_H__F8CD0C1F_36FC_4540_98B2_E54007756D2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

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

#endif // !defined(AFX_STDAFX_H__F8CD0C1F_36FC_4540_98B2_E54007756D2C__INCLUDED_)
