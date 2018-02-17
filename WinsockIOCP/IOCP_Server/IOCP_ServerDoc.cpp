// IOCP_ServerDoc.cpp : implementation of the CIOCP_ServerDoc class
//

#include "stdafx.h"
#include "IOCP_Server.h"

#include "IOCP_ServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerDoc

IMPLEMENT_DYNCREATE(CIOCP_ServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CIOCP_ServerDoc, CDocument)
	//{{AFX_MSG_MAP(CIOCP_ServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerDoc construction/destruction

CIOCP_ServerDoc::CIOCP_ServerDoc()
{
	// TODO: add one-time construction code here

}

CIOCP_ServerDoc::~CIOCP_ServerDoc()
{
}

BOOL CIOCP_ServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerDoc serialization

void CIOCP_ServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerDoc diagnostics

#ifdef _DEBUG
void CIOCP_ServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIOCP_ServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerDoc commands
