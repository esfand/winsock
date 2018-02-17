// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "IOCP_Server.h"

#include "MainFrm.h"
#include "IOCP_ServerView.h"
#include "SendDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_SEND, OnFileSend)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP


	ON_MESSAGE(WM_DATA_IN_MSG, OnDataInMsg)
	ON_MESSAGE(WM_CLIENT_CLOSE, OnClientClose)
	ON_MESSAGE(WM_CLIENT_CONNECT, OnClientConnect)
	
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


LRESULT CMainFrame::OnDataInMsg(WPARAM wParam, LPARAM lParam)
{
	CIOCP_ServerView* pView = static_cast<CIOCP_ServerView*>(GetActiveView());
	return 0;
}


LRESULT CMainFrame::OnClientClose(WPARAM wParam, LPARAM lParam)
{
	CIOCP_ServerView* pView = static_cast<CIOCP_ServerView*>(GetActiveView());

	ClientContext* pContext = reinterpret_cast<ClientContext*>(lParam);
	ASSERT(pContext);

	if (pView)
		pView->RemoveFromList(pContext);

	return 0;
}

LRESULT CMainFrame::OnClientConnect(WPARAM wParam, LPARAM lParam)
{
	CIOCP_ServerView* pView = static_cast<CIOCP_ServerView*>(GetActiveView());

	ClientContext* pContext = reinterpret_cast<ClientContext*>(lParam);
	ASSERT(pContext);

	if (pView)
		pView->AddToList(pContext);


	return 0;
}


void CMainFrame::OnFileSend() 
{
	CSendDlg dlg;
	if (dlg.DoModal() == TRUE)
	{
		m_iocpServer.Send(dlg.m_strIP, dlg.m_strData);
	}
	
}


void CMainFrame::NotifyProc(LPVOID lpParam, ClientContext* pContext, UINT nCode)
{
	CMainFrame* pFrame = (CMainFrame*) lpParam;
	CIOCP_ServerView* pView = static_cast<CIOCP_ServerView*>(pFrame->GetActiveView());

	switch (nCode)
	{
	case NC_CLIENT_CONNECT:
		pView->AddToList(pContext);
		break;
	case NC_CLIENT_DISCONNECT:
		pView->RemoveFromList(pContext);
		break;
	case NC_TRANSMIT:
		break;
	case NC_RECEIVE:
		break;
	

	}
}

void CMainFrame::Activate()
{
	m_iocpServer.Initialize(NotifyProc, this, 10000, 999);
}


void CMainFrame::OnCheck1() 
{	
	
}
