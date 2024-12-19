// PreviewPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "PreviewPaneView.h"


// CPreviewPaneView

IMPLEMENT_DYNCREATE(CPreviewPaneView, CFormView)

CPreviewPaneView::CPreviewPaneView()
	: CFormView(CPreviewPaneView::IDD)
{

}

CPreviewPaneView::~CPreviewPaneView()
{
}

void CPreviewPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlListBox);
}

BEGIN_MESSAGE_MAP(CPreviewPaneView, CFormView)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CPreviewPaneView::OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST1, &CPreviewPaneView::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CPreviewPaneView 진단입니다.

#ifdef _DEBUG
void CPreviewPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPreviewPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPreviewPaneView 메시지 처리기입니다.

void CPreviewPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if( m_ctrlListBox )
	{
		CRect rcRect;
		GetClientRect( &rcRect );
		rcRect.left += 10;
		rcRect.right -= 10;
		rcRect.top += 30;
		rcRect.bottom -= 10;
		m_ctrlListBox.MoveWindow( &rcRect );
	}
}

void CPreviewPaneView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	SetActiveWindow();      // activate us first !
	UINT nFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	TCHAR szFileName[_MAX_PATH];
	char szFullFileName[256];
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);

		CString strFileName = szFileName;
		int nLength = strFileName.GetLength();

		if( nLength > 3 )
		{
			bool bEffFile = false;
			if( strFileName[nLength-3] == 'e' && strFileName[nLength-2] == 'f' && strFileName[nLength-1] == 'f' ) bEffFile = true;

			if( bEffFile )
			{
				_GetFullFileName( szFullFileName, _countof(szFullFileName), szFileName );

				int nEffectDataIndex = -1;
				std::map< std::string, int >::iterator iter;
				iter = m_mapEffectData.find( szFullFileName );
				if( iter == m_mapEffectData.end() )
				{
					nEffectDataIndex = EternityEngine::LoadEffectData( szFileName );
					m_mapEffectData.insert( std::make_pair( szFullFileName, nEffectDataIndex ) );
					m_ctrlListBox.AddString( szFullFileName );
				}
			}
			else
			{
				MessageBox( "eff파일만 로드 가능합니다.", "경고", MB_OK );
			}
		}
	}
	DragFinish(hDropInfo);

	CFormView::OnDropFiles(hDropInfo);
}

void CPreviewPaneView::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ctrlListBox.ResetContent();
	std::map< std::string, int >::iterator iter = m_mapEffectData.begin();
	for( ; iter != m_mapEffectData.end(); ++iter )
	{
		EternityEngine::DeleteEffectData( iter->second );
	}
	m_mapEffectData.clear();
}

void CPreviewPaneView::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strName;
	m_ctrlListBox.GetText( m_ctrlListBox.GetCurSel(), strName );
	std::string szKey = strName;

	std::map< std::string, int >::iterator iter;
	iter = m_mapEffectData.find( szKey );
	if( iter != m_mapEffectData.end() )
	{
		MatrixEx Cross;
		EtEffectObjectHandle hHandle = EternityEngine::CreateEffectObject( iter->second, Cross );
	}
}
