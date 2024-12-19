// NetTestView.cpp : CNetTestView Ŭ������ ����
//

#include "stdafx.h"
#include "NetTest.h"

#include "NetTestDoc.h"
#include "NetTestView.h"

#include "AddressDlg.h"

#include "VillageProtocol.h"
#include "Village_Structure.h"
#include "Village_ReadPacket.h"
#include "Village_WritePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetTestView

IMPLEMENT_DYNCREATE(CNetTestView, CFormView)

BEGIN_MESSAGE_MAP(CNetTestView, CFormView)
	ON_COMMAND(ID_CONNECT_ADDRESS, &CNetTestView::OnConnectAddress)
	ON_MESSAGE( EN_EDIT_CHANGE, &CNetTestView::OnEditChange )
END_MESSAGE_MAP()

// CNetTestView ����/�Ҹ�

CNetTestView::CNetTestView()
	: CFormView(CNetTestView::IDD)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CNetTestView::~CNetTestView()
{
}

void CNetTestView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ChatLine);
	DDX_Control(pDX, IDC_EDIT2, m_ChatWindow);
}

BOOL CNetTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CNetTestView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

void CNetTestView::Connect( CString &szAddress, int nPort, CString &szID, CString &szPassword ) 
{ 
	if( IsTextUnicode( szAddress.GetBuffer(), szAddress.GetLength() * sizeof( TCHAR ), NULL ) )
	{
		WideCharToMultiByte( CP_ACP, 0, szAddress.GetBuffer(), -1, m_szAddress, 1024, NULL, NULL );
	}
	else
	{
		strcpy( m_szAddress, ( LPSTR )szAddress.GetBuffer() );
	}
	m_nPort = nPort; 
	m_Session.BeginUdp( nPort );

//	m_VillageSession.Begin( m_szAddress, nPort, szID.GetBuffer(), szPassword.GetBuffer() );
}

void CNetTestView::IdleProcess()
{
	m_Session.Polling();

	DWORD dwProtocol, dwPacketLength;
	USHORT usPort;
	BYTE Packet[ 4096 ];
	char szAddress[ 128 ];

//	dwProtocol = 0;
//	usPort = ( USHORT )m_nPort;
	if( m_Session.ReadFromPacket( dwProtocol, szAddress, usPort, Packet, dwPacketLength ) )
	{
		Packet[ dwPacketLength ] = 0;
		Packet[ dwPacketLength + 1 ] = 0;
		m_ChatWindow.ReplaceSel( ( LPCTSTR )Packet );
	}
}


// CNetTestView ����

#ifdef _DEBUG
void CNetTestView::AssertValid() const
{
	CFormView::AssertValid();
}

void CNetTestView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CNetTestDoc* CNetTestView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetTestDoc)));
	return (CNetTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CNetTestView �޽��� ó����

void CNetTestView::OnConnectAddress()
{
	CAddressDlg Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		Connect( Dlg.m_szAddress, Dlg.m_nPort, Dlg.m_szID, Dlg.m_szPassword );
	}
}

LRESULT CNetTestView::OnEditChange( WPARAM wParam, LPARAM lParam )
{
	DWORD dwPacketLength;

	dwPacketLength = ( DWORD )( _tcslen( ( LPCTSTR )wParam ) * sizeof( TCHAR ) );
	m_Session.WriteToPacket( m_szAddress, ( USHORT )m_nPort, 10, ( BYTE * )wParam, dwPacketLength );

	return 1;
}

