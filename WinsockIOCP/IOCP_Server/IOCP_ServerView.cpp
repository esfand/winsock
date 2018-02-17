// IOCP_ServerView.cpp : implementation of the CIOCP_ServerView class
//

#include "stdafx.h"
#include "IOCP_Server.h"

#include "IOCP_ServerDoc.h"
#include "IOCP_ServerView.h"
#include "MainFrm.h"
#include "IOCPServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerView

IMPLEMENT_DYNCREATE(CIOCP_ServerView, CFormView)

BEGIN_MESSAGE_MAP(CIOCP_ServerView, CFormView)
	//{{AFX_MSG_MAP(CIOCP_ServerView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CIOCP_ServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIOCP_ServerView)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_BCNT, m_sBusy);
	DDX_Text(pDX, IDC_WCNT, m_sWorkers);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerView construction/destruction

CIOCP_ServerView::CIOCP_ServerView() : CFormView(CIOCP_ServerView::IDD)
{
	//{{AFX_DATA_INIT(CIOCP_ServerView)
	m_sActive = _T("");
	m_sBusy = _T("");
	m_sWorkers = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CIOCP_ServerView::~CIOCP_ServerView()
{
}

BOOL CIOCP_ServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerView drawing

void CIOCP_ServerView::OnDraw(CDC* pDC)
{
	CIOCP_ServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CIOCP_ServerView::OnInitialUpdate()
{

	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_list.InsertColumn(0, "Client", LVCFMT_LEFT, 110);
	m_list.InsertColumn(1, "Msgs Received", LVCFMT_LEFT, 110);
	m_list.InsertColumn(2, "Msgs Sent", LVCFMT_LEFT, 110);

	SetTimer(IDT_TIMER, 1000, NULL);




	// TODO: You may populate your ListView with items by directly accessing
}

/////////////////////////////////////////////////////////////////////////////
// CIOCP_ServerView diagnostics

#ifdef _DEBUG
void CIOCP_ServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CIOCP_ServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CIOCP_ServerDoc* CIOCP_ServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIOCP_ServerDoc)));
	return (CIOCP_ServerDoc*)m_pDocument;
}
#endif //_DEBUG



void CIOCP_ServerView::AddToList(ClientContext* pContext)
{
	int nCnt = m_list.GetItemCount();

	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	
	BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
	
	CString strHost =  inet_ntoa(sockAddr.sin_addr);


	int i = m_list.InsertItem(nCnt, strHost, 0);
	m_list.SetItemData(nCnt, (DWORD) pContext);

}

void CIOCP_ServerView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	
}


void CIOCP_ServerView::RemoveFromList(ClientContext* pContext)
{
	int nCnt = m_list.GetItemCount();
	for (int i=0; i < nCnt; i++)
	{
		ClientContext* pItemContext = (ClientContext*) m_list.GetItemData(i);
		if (pItemContext == pContext)
		{
			m_list.DeleteItem(i);
			break;
		}		
	}
}

void CIOCP_ServerView::OnTimer(UINT nIDEvent) 
{
	int nCnt = m_list.GetItemCount();
	for (int i=0; i < nCnt; i++)
	{
		ClientContext* pItemContext = (ClientContext*) m_list.GetItemData(i);
		if (pItemContext)
		{

			CString sNum;

			sNum.Format("%d",pItemContext->m_nMsgIn);
			m_list.SetItemText(i,1, sNum);

			sNum.Format("%d",pItemContext->m_nMsgOut);
			m_list.SetItemText(i,2, sNum);
		}

	}


	// This is purely for testing purposes
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		
	CIOCPServer* pServer = &pFrame->m_iocpServer;

	m_sWorkers.Format("%d",pServer->m_nCurrentThreads);
	m_sBusy.Format("%d",pServer->m_nBusyThreads);

	UpdateData(FALSE);
	

	CFormView::OnTimer(nIDEvent);
}

void CIOCP_ServerView::OnCheck1() 
{
	// This is purely for testing purposes
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		
	CIOCPServer* pServer = &pFrame->m_iocpServer;
	pServer->SetEchoMode();
	
}
