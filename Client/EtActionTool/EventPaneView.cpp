// EventPaneView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MainFrm.h"
#include "EtActionTool.h"
#include "EventPaneView.h"

#include "UserMessage.h"
#include "ActionBase.h"
#include "ActionElement.h"
#include "ActionSignal.h"
#include "AddSignalDlg.h"
#include "SignalItem.h"
#include "SignalManager.h"
#include "GlobalValue.h"
#include "PaneDefine.h"
#include "SignalCustomRender.h"
#include "../Common/Utility/CSVWriter.h"


// CEventPaneView

IMPLEMENT_DYNCREATE(CEventPaneView, CSequenceView)

CEventGridItemBase::CEventGridItemBase()
{
	pSignal = NULL;
}

CEventGridItemBase::~CEventGridItemBase()
{
}


CEventPaneView::CEventPaneView()
{
	m_bActivate = false;
	m_pObject = NULL;
#ifdef _SELECT_MULTIPLE_SIGNAL
#else
	m_pPrevSelectItem = NULL;
	m_pCopyCutBuffer = NULL;
#endif
	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );

	TCHAR pBuffer[MAX_PATH]={0};
	SHGetSpecialFolderPath( GetSafeHwnd(), pBuffer, CSIDL_PERSONAL, 0 );
	wsprintf( pBuffer, "%s\\copyandpastebuffer.signal", pBuffer );

	m_CopyAndPastePath = CString( pBuffer );
}

CEventPaneView::~CEventPaneView()
{
	SAFE_DELETE( m_pContextMenu );
}

void CEventPaneView::DoDataExchange(CDataExchange* pDX)
{
	CSequenceView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEventPaneView, CSequenceView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_REFRESH_PANE, OnRefresh )
	ON_MESSAGE( UM_EVENT_PANE_UPDATE_FRAME, OnUpdateFrame )
	ON_WM_HSCROLL()
	ON_COMMAND(ID_EVENTPANECONTEXT_INSERTSIGNAL, &CEventPaneView::OnEventpanecontextInsertsignal)
	ON_COMMAND(ID_EVENTPANECONTEXT_REMOVESIGNAL, &CEventPaneView::OnEventpanecontextRemovesignal)
	ON_COMMAND(ID_EVENTPANECONTEXT_COPYSIGNAL, &CEventPaneView::OnEventpanecontextCopysignal)
	ON_COMMAND(ID_EVENTPANECONTEXT_CUTSIGNAL, &CEventPaneView::OnEventpanecontextCutsignal)
	ON_COMMAND(ID_EVENTPANECONTEXT_PASTE, &CEventPaneView::OnEventpanecontextPaste)
	ON_COMMAND(ID_EVENTPANECONTEXT_EXPORTSIGNAL, &CEventPaneView::OnEventpanecontextExportSignal)
	ON_COMMAND(ID_EVENTPANECONTEXT_IMPORTSIGNAL, &CEventPaneView::OnEventpanecontextImportSignal)
	ON_MESSAGE(UM_EVENT_PANE_RESETCOPYCUT, OnResetCopyCut)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EVENTPANECONTEXT_FULLRANGE, &CEventPaneView::OnEventpanecontextFullrange)
	ON_COMMAND(ID_EVENTPANECONTEXT_EXPORTEXCEL, &CEventPaneView::OnEventpanecontextExportexcel)
END_MESSAGE_MAP()


// CEventPaneView 진단입니다.

#ifdef _DEBUG
void CEventPaneView::AssertValid() const
{
	CSequenceView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventPaneView::Dump(CDumpContext& dc) const
{
	CSequenceView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventPaneView 메시지 처리기입니다.

void CEventPaneView::OnInitialUpdate()
{
	CSequenceView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	SetEnable( false );
}

void CEventPaneView::OnSize(UINT nType, int cx, int cy)
{
	CSequenceView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CEventPaneView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

LRESULT CEventPaneView::OnRefresh( WPARAM wParam, LPARAM lParam )
{
	m_pObject = (CActionBase *)wParam;

	if( m_pObject == NULL ) {
		SetEnable( false );
		SetSliderRange(0, 0);
		RefreshSlider();
		RefreshScroll();
		Reset();
		Invalidate();

		return S_OK;
	}
#ifdef _SELECT_MULTIPLE_SIGNAL
	m_pPrevSelectItems.clear();
#else
	m_pPrevSelectItem = NULL;
#endif

	CActionElement *pElement = (CActionElement *)m_pObject;
	pElement->SetPlay();
	SetEnable( true );
	SetSliderRange( 0, pElement->GetLength() );
	Reset();

	int nMaxYOrder = 8;
	bool bMax = false;
	for( DWORD i=0; i<pElement->GetChildCount(); i++ ) {
		CActionSignal *pSignal = (CActionSignal *)pElement->GetChild(i);
		CSignalItem *pSignalItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( pSignal->GetSignalIndex() );
		CEventGridItemBase *pGridItem = (CEventGridItemBase *)AddSignal( pSignal->GetStartFrame(), pSignal->GetEndFrame(), pSignal->GetYOrder(), pSignalItem->GetInitialsColor(), pSignalItem->GetBackgroundColor(), pSignalItem->IsHasLength() ? pSignalItem->GetName() : (CString)pSignalItem->GetName()[0] );
		pGridItem->pSignal = pSignal;
		pGridItem->pSignal->SetFocus( false );
		pGridItem->bModifyLength = pSignalItem->IsHasLength();
		if( pGridItem->nYOrder > nMaxYOrder ) nMaxYOrder = pGridItem->nYOrder;
		if( pGridItem->nYOrder == 8 ) bMax = true;
	}
	if( nMaxYOrder == 8 ) {
		if( bMax ) m_dwAdditionalGridHeightCount = 1;
		else m_dwAdditionalGridHeightCount = 0;
	}
	else if( nMaxYOrder > 8 ) m_dwAdditionalGridHeightCount = nMaxYOrder - 7;
	//	SetGridHeightCount( nMaxYOrder );
#ifdef _SELECT_MULTIPLE_SIGNAL
	ClearSelectedSignals();
#else
	m_nFocusSignalIndex = -1;
#endif

	RefreshSlider();
	RefreshScroll();
	Invalidate();

	return S_OK;
}

LRESULT CEventPaneView::OnUpdateFrame( WPARAM wParam, LPARAM lParam )
{
	if( !m_pObject ) return S_OK;

	CActionElement *pElement = (CActionElement *)m_pObject;
	if( (DWORD)pElement->GetCurFrame() == GetSliderPos() ) return S_OK;
	SetSliderPos( (int)pElement->GetCurFrame() );
	return S_OK;
}

void CEventPaneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CSequenceView::OnHScroll(nSBCode, nPos, pScrollBar);
	if( pScrollBar && m_pObject && GetSlider() ) {
		if( pScrollBar->m_hWnd == GetSlider()->m_hWnd ) {
			DWORD dwIndex = GetSliderPos();
			CActionElement *pElement = (CActionElement *)m_pObject;
			if( !pElement->IsPause() ) pElement->Pause();
			pElement->SetCurFrame( (float)dwIndex );
		}
	}
}

CSequenceView::CSequenceSignalBase *CEventPaneView::CreateSignal()
{
	return new CEventGridItemBase;
}

bool CEventPaneView::InsertSignal( CSequenceView::CSequenceSignalBase *pItem )
{
	CAddSignalDlg Dlg;
	if( Dlg.DoModal() != IDOK ) return FALSE;
	if( !CGlobalValue::GetInstance().GetControlObject() ) return FALSE;
	if( CGlobalValue::GetInstance().GetControlObject()->GetType() != CActionBase::ELEMENT ) return FALSE;

	CEventGridItemBase *pBase = (CEventGridItemBase *)pItem;
	CSignalItem *pSignal = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( Dlg.m_nSelectSignalIndex );

	CString szElementName = CGlobalValue::GetInstance().GetControlObject()->GetName();

	pBase->pSignal = CGlobalValue::GetInstance().AddSignal( szElementName, Dlg.m_nSelectSignalIndex, pItem->nStartFrame, pItem->nEndFrame, pItem->nYOrder );
	pBase->InitialColor = pSignal->GetInitialsColor();
	pBase->Color = pSignal->GetBackgroundColor();
	if( pSignal->IsHasLength() )
		pBase->szString = pSignal->GetName();
	else pBase->szString = pSignal->GetName()[0];
	pBase->bModifyLength = pSignal->IsHasLength();

	pBase->pSignal->SetFocus( true );
#ifdef _SELECT_MULTIPLE_SIGNAL
	PushBackSelectedSignalIndex((int)m_pVecSignalList.size());
	PushBackPrevSelectItems(pBase);
#else
	m_nFocusSignalIndex = (int)m_pVecSignalList.size();

	m_pPrevSelectItem = pBase;
#endif

	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );

	return TRUE;
}

#ifdef _SELECT_MULTIPLE_SIGNAL
void CEventPaneView::ResetPrevSelectItemSignal()
{
	if (m_pPrevSelectItems.size() > 0)
	{
		std::vector<CEventGridItemBase*>::iterator iter = m_pPrevSelectItems.begin();
		for (; iter != m_pPrevSelectItems.end(); ++iter)
		{
			CEventGridItemBase* pCurPrevItem = (*iter);
			if (pCurPrevItem && pCurPrevItem->pSignal)
			{
				pCurPrevItem->pSignal->SetFocus( false );
				if( pCurPrevItem->pSignal->GetCustomRender() ) {
					pCurPrevItem->pSignal->GetCustomRender()->OnUnSelect();
				}
			}
		}
	}

	m_bSelectEmpty = FALSE;

	CSignalCustomRender * pSignalCustomRender = CGlobalValue::GetInstance().GetSignalCustomRenderObject();

	if( NULL == pSignalCustomRender )
		return;

	pSignalCustomRender->OnUnSelect();
	CGlobalValue::GetInstance().SetSignalCustomRenderObject( NULL );
}
void CEventPaneView::OnSelectSignalArray( std::vector<CSequenceView::CSequenceSignalBase *>& pItemArray )
{
	if( FALSE == IsTabKey() )
	{
		ResetPrevSelectItemSignal();
		m_pPrevSelectItems.clear();
	}

	if (pItemArray.size() <= 0)
	{
		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)CGlobalValue::GetInstance().GetControlObject() );
	}
	else
	{
		std::vector<CSequenceView::CSequenceSignalBase *>::iterator iter = pItemArray.begin();
		for (; iter != pItemArray.end(); ++iter)
		{
			CSequenceView::CSequenceSignalBase* pCurItem = (*iter);
			if (pCurItem)
			{
				CEventGridItemBase *pBase = static_cast<CEventGridItemBase *>(pCurItem);
				if (pBase)
					m_pPrevSelectItems.push_back(pBase);
			}
		}

		if( true == IsSelectedArrayEqualValue() )
		{
			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			if( pWnd )
			{
				std::vector<CSequenceView::CSequenceSignalBase *>::iterator iter = pItemArray.begin();
				CSequenceView::CSequenceSignalBase* pCurItem = (*iter);
				CEventGridItemBase *pBase = static_cast<CEventGridItemBase *>(pCurItem);
				pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
			}

			return;
		}
	}
}

void CEventPaneView::OnClearSelectedSignals()
{
	ResetPrevSelectItemSignal();

	m_pPrevSelectItems.clear();

	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)CGlobalValue::GetInstance().GetControlObject() );
}
#endif // _SELECT_MULTIPLE_SIGNAL

bool CEventPaneView::IsValidPropertyPaneState() const
{
#ifdef _CHECK_CUSTOMFLY
	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if (pWnd)
	{
		LRESULT result = pWnd->SendMessage( UM_PROPERTY_PANE_CHECK_STATE, 0, (LPARAM)CGlobalValue::GetInstance().GetControlObject() );
		if (result != S_OK)
			return false;
	}
#endif

	return true;
}

void CEventPaneView::OnSelectSignal( CSequenceView::CSequenceSignalBase *pItem )
{
#ifdef _SELECT_MULTIPLE_SIGNAL
	if( FALSE == IsTabKey() )
	{
		ResetPrevSelectItemSignal();
		m_pPrevSelectItems.clear();
	}
#else
	if( m_pPrevSelectItem ) {
		m_pPrevSelectItem->pSignal->SetFocus( false );
		if( m_pPrevSelectItem->pSignal->GetCustomRender() ) {
			m_pPrevSelectItem->pSignal->GetCustomRender()->OnUnSelect();
			CGlobalValue::GetInstance().SetSignalCustomRenderObject( NULL );
		}
	}
#endif

	if( pItem == NULL ) {
		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)CGlobalValue::GetInstance().GetControlObject() );
	}
	else {
		CEventGridItemBase *pBase = (CEventGridItemBase *)pItem;

		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );

#ifdef _SELECT_MULTIPLE_SIGNAL
		m_pPrevSelectItems.push_back(pBase);
#endif
		if( pWnd )
		{
#ifdef _SELECT_MULTIPLE_SIGNAL
			if( 1 == m_pPrevSelectItems.size() )
			{
				pBase->pSignal->SetFocus( true );	
				if( pBase->pSignal->GetCustomRender() )
				{
					if( !pBase->pSignal->GetCustomRender()->IsActivate() )
						pBase->pSignal->GetCustomRender()->Initialize();

					pBase->pSignal->GetCustomRender()->OnSelect( true );
				}
				pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
			}
			else if( true == IsSelectedArrayEqualValue() )
			{
				std::vector<CEventGridItemBase *>::iterator iter = m_pPrevSelectItems.begin();
				CEventGridItemBase *pBase = (*iter);
				pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
			}
			else
				pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)CGlobalValue::GetInstance().GetControlObject() );
#else
			pBase->pSignal->SetFocus( true );	
			if( pBase->pSignal->GetCustomRender() )
			{
				if( !pBase->pSignal->GetCustomRender()->IsActivate() )
					pBase->pSignal->GetCustomRender()->Initialize();

				pBase->pSignal->GetCustomRender()->OnSelect( true );
			}
			pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
#endif
		}
	}

#ifdef _SELECT_MULTIPLE_SIGNAL
#else
	m_pPrevSelectItem = (CEventGridItemBase *)pItem;
#endif
}

bool CEventPaneView::OnRemoveSignal( CSequenceView::CSequenceSignalBase *pItem )
{
	CEventGridItemBase *pBase = (CEventGridItemBase *)pItem;
#ifdef _SELECT_MULTIPLE_SIGNAL
	if (pBase == NULL)
		return false;

	std::vector<CEventGridItemBase*>::iterator iter = std::find(m_pPrevSelectItems.begin(), m_pPrevSelectItems.end(), pBase);
	if (iter != m_pPrevSelectItems.end())
		m_pPrevSelectItems.erase(iter);

	m_CopyCutBuffer.Erase(pBase);
#else
	if( m_pPrevSelectItem == pBase ) m_pPrevSelectItem = NULL;
	if( m_pCopyCutBuffer == pBase->pSignal ) m_pCopyCutBuffer = NULL;
#endif
	pBase->pSignal->GetParent()->RemoveChild( pBase->pSignal );
	return true;
}

void CEventPaneView::OnModifySignal( CSequenceView::CSequenceSignalBase *pItem )
{
	CEventGridItemBase *pBase = (CEventGridItemBase *)pItem;
	pBase->pSignal->SetStartFrame( pItem->nStartFrame );
	pBase->pSignal->SetEndFrame( pItem->nEndFrame );
	pBase->pSignal->SetYOrder( pItem->nYOrder );
}

void CEventPaneView::OnEventpanecontextInsertsignal()
{
	// TODO: Add your command handler code here
	int nPos, nYOrder;
	if( CalcSignalPos( m_ContextMousePos.x, m_ContextMousePos.y, nPos, nYOrder ) == false ) return;

	CSequenceSignalBase *pBase = CreateSignal();
	if( pBase == NULL ) return;
	pBase->nStartFrame = nPos;
	pBase->nEndFrame = nPos+1;
	pBase->nYOrder = nYOrder;

	if( InsertSignal( pBase ) == false ) {
		SAFE_DELETE( pBase );
		return;
	}
	m_pVecSignalList.push_back( pBase );
	Invalidate();
}

#ifdef _SELECT_MULTIPLE_SIGNAL
bool CEventPaneView::SCopyCutGridItemInfo::operator==(const CEventGridItemBase& rhs) const
{
	return ((nStartGridX == rhs.nStartFrame) && (nEndGridX == rhs.nEndFrame) && (nYOrder == rhs.nYOrder));
}

BOOL CEventPaneView::SCopyCutInfo::Push( CEventPaneView::SCopyCutGridItemInfo * pAddItem )
{
	if (pAddItem == NULL)
		return FALSE;

	std::list<SCopyCutGridItemInfo>::const_iterator copyIter = 
		std::find(gridItemList.begin(), gridItemList.end(), *pAddItem);
	if (copyIter == gridItemList.end())
	{
		SCopyCutGridItemInfo element;
		element.nYOrder = pAddItem->nYOrder;
		element.nStartGridX = pAddItem->nStartGridX;
		element.nEndGridX = pAddItem->nEndGridX;
		element.pSignal = pAddItem->pSignal;

		gridItemList.push_back(element);
		return TRUE;
	}

	return FALSE;
}

BOOL CEventPaneView::SCopyCutInfo::Push(CEventGridItemBase* pAddItem)
{
	if (pAddItem == NULL)
		return FALSE;

	std::list<SCopyCutGridItemInfo>::const_iterator copyIter = 
		std::find(gridItemList.begin(), gridItemList.end(), *pAddItem);
	if (copyIter == gridItemList.end())
	{
		SCopyCutGridItemInfo element;
		element.nYOrder = pAddItem->nYOrder;
		element.nStartGridX = pAddItem->nStartFrame;
		element.nEndGridX = pAddItem->nEndFrame;
		element.pSignal = pAddItem->pSignal;

		gridItemList.push_back(element);
		return TRUE;
	}

	return FALSE;
}

BOOL CEventPaneView::SCopyCutInfo::Erase(const CEventGridItemBase* pEraseItemIter)
{
	if (pEraseItemIter == NULL)
		return FALSE;

	std::list<SCopyCutGridItemInfo>::iterator foundIter = 
		std::find(gridItemList.begin(), gridItemList.end(), *pEraseItemIter);
	if (foundIter != gridItemList.end())
	{
		gridItemList.erase(foundIter);
		return TRUE;
	}

	return FALSE;
}

std::list<CEventPaneView::SCopyCutGridItemInfo>& CEventPaneView::SCopyCutInfo::GetGridItemList()
{
	return gridItemList;
}

const CEventPaneView::SCopyCutGridItemInfo& CEventPaneView::SCopyCutInfo::GetMostLeftGridItem() const
{
	if (gridItemList.size() <= 1)
		return gridItemList.front();

	int mostLeft = -1;
	std::list<SCopyCutGridItemInfo>::const_iterator iter = gridItemList.begin(), leftIter;
	for (; iter != gridItemList.end(); ++iter)
	{
		const SCopyCutGridItemInfo& curItem = (*iter);
		if (mostLeft < 0 || curItem.nStartGridX < mostLeft)
		{
			mostLeft = curItem.nStartGridX;
			leftIter = iter;
		}
	}

	return (*leftIter);
}

const CEventPaneView::SCopyCutGridItemInfo& CEventPaneView::SCopyCutInfo::GetMostTopGridItem() const
{
	if (gridItemList.size() <= 1)
		return gridItemList.front();

	int mostTop = -1;
	std::list<SCopyCutGridItemInfo>::const_iterator iter = gridItemList.begin(), topIter;
	for (; iter != gridItemList.end(); ++iter)
	{
		const SCopyCutGridItemInfo& curItem = (*iter);
		if (mostTop < 0 || curItem.nYOrder < mostTop)
		{
			mostTop = curItem.nYOrder;
			topIter = iter;
		}
	}

	return (*topIter);
}

BOOL CEventPaneView::OnRemoveSignal(CEventGridItemBase* pItem)
{
	if (pItem == NULL || pItem->pSignal == NULL)
		return FALSE;

	for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) {
		if( m_pVecSignalList[i] == pItem ) {
			m_pVecSignalList.erase( m_pVecSignalList.begin() + i );
			break;
		}
	}

	pItem->pSignal->GetParent()->RemoveChild( pItem->pSignal );

	m_CopyCutBuffer.Erase(pItem);

	return TRUE;
}

void CEventPaneView::CopySignalFromPrevSelectItem()
{
	if (m_pPrevSelectItems.size() > 0)
	{
		m_CopyCutBuffer.Clear();
		bool bMessage = false;
		std::vector<CEventGridItemBase*>::iterator iter = m_pPrevSelectItems.begin();
		for (; iter != m_pPrevSelectItems.end(); ++iter)
		{
			CEventGridItemBase* pGridItem = (*iter);
			if (pGridItem && pGridItem->pSignal)
			{
				if( false == CSignalManager::GetInstance().IsEnableSaveAndPasteSignal( pGridItem->pSignal->GetSignalIndex() ) )
				{
					if( false == bMessage )
					{
						bMessage = true;
						MessageBox( "복사 불가능한 시그널이 포함 되어 있습니다.\n설정 - CopyPaste Signal 에서 확인 하세요." );
					}
					continue;
				}

				BOOL bPushed = m_CopyCutBuffer.Push(pGridItem);
				if (bPushed)
				{
					CRect& copyCutWholeArea = m_CopyCutBuffer.wholeArea;
					if (copyCutWholeArea.left < 0 || pGridItem->nStartFrame < copyCutWholeArea.left)
						copyCutWholeArea.left = pGridItem->nStartFrame;
					if (copyCutWholeArea.right < 0 || pGridItem->nEndFrame > copyCutWholeArea.right)
						copyCutWholeArea.right = pGridItem->nEndFrame;
					if (copyCutWholeArea.top < 0 || pGridItem->nYOrder < copyCutWholeArea.top)
						copyCutWholeArea.top = pGridItem->nYOrder;
					if (copyCutWholeArea.bottom < 0 || pGridItem->nYOrder > copyCutWholeArea.bottom)
						copyCutWholeArea.bottom = pGridItem->nYOrder;
				}
			}
		}
	}
}
#endif // _SELECT_MULTIPLE_SIGNAL

void CEventPaneView::ExportSignal( const char *szFileName )
{
#ifdef _SELECT_MULTIPLE_SIGNAL
	CopySignalFromPrevSelectItem();
#endif

	FILE * fp = NULL;
	fopen_s( &fp, szFileName, "wb" );
	if( fp == NULL )
		return;

	CString szHeader = "Eternity Engine Signal Export File 2.0";

	WriteCString( &szHeader, fp );

	std::list<SCopyCutGridItemInfo> & gridItemList = m_CopyCutBuffer.GetGridItemList();

	int nSize = (int)gridItemList.size();
	fwrite( &nSize, sizeof(int), 1, fp );

	bool bExist = false;
	std::list<SCopyCutGridItemInfo>::iterator Itor = gridItemList.begin();
	for( ; Itor != gridItemList.end(); ++Itor )
	{
		CActionSignal * pSignal = (*Itor).pSignal;

		if( NULL == pSignal )
			bExist = false;
		else
			bExist = true;

		fwrite( &bExist, sizeof(bool), 1, fp );

		if( false == bExist )
			continue;

		fwrite( &(*Itor).nStartGridX, sizeof(int),1 ,fp );
		fwrite( &(*Itor).nEndGridX, sizeof(int),1 ,fp );
		fwrite( &(*Itor).nYOrder, sizeof(int),1 ,fp );

		pSignal->ExportObject( fp, nSize );
	}

	fclose( fp );
}

void CEventPaneView::ImportSignal( const char *szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rb" );
	if( fp == NULL ) return;

	int nCount;
	CString szHeader;
	ReadCString( &szHeader, fp );
	fread( &nCount, sizeof(int), 1, fp );

	int nPos, nYOrder;
	if( szHeader == "Eternity Engine Action Export File 2.0" 
		|| 0 != nCount 
		|| FALSE == m_bSelectEmpty )
	{
		if( true == CalcSignalPos( m_ContextMousePos.x, m_ContextMousePos.y, nPos, nYOrder ) )
		{
			m_CopyCutBuffer.Clear();

			bool bExist = false;

			for( int itr = 0; itr < nCount; ++itr )
			{
				fread( &bExist, sizeof(bool), 1, fp );

				if( false == bExist )
					continue;

				SCopyCutGridItemInfo sData;

				fread( &sData.nStartGridX, sizeof(int), 1, fp );
				fread( &sData.nEndGridX, sizeof(int), 1, fp );
				fread( &sData.nYOrder, sizeof(int), 1, fp );

				sData.pSignal = new CActionSignal;
				sData.pSignal->SetParent( m_pObject );
				if( sData.pSignal->ImportObject( fp ) == false )
				{
					m_pObject->RemoveChild( sData.pSignal );
					continue;
				}

				m_CopyCutBuffer.Push( &sData );
			}
		}
	}
	fclose( fp );
}

bool CEventPaneView::IsSelectedArrayEqualValue()
{
	if( 2 > m_pPrevSelectItems.size() )
		return false;

	std::vector<CEventGridItemBase *>::iterator iter = m_pPrevSelectItems.begin();
	CEventGridItemBase * pCurItem = (*iter);
	const int nSignalIndex = pCurItem->pSignal->GetSignalIndex();
	++iter;

	for (; iter != m_pPrevSelectItems.end(); ++iter)
	{
		CSequenceView::CSequenceSignalBase* pCurItem = (*iter);
		CEventGridItemBase *pBase = static_cast<CEventGridItemBase *>(pCurItem);

		if( nSignalIndex != pBase->pSignal->GetSignalIndex() )
			return false;
	}

	return true;
}

void CEventPaneView::ChangeValueSelectedArraySignal( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	if( false == IsSelectedArrayEqualValue() )
		return;

	std::vector<CEventGridItemBase *>::iterator iter = m_pPrevSelectItems.begin();
	for (; iter != m_pPrevSelectItems.end(); ++iter)
	{
		CSequenceView::CSequenceSignalBase* pCurItem = (*iter);
		CEventGridItemBase *pBase = static_cast<CEventGridItemBase *>(pCurItem);

		pBase->pSignal->OnSelectChangePropertyValue( dwIndex, pVariable );
	}
}

//// bintitle.
//#include "EtResourceMng.h"
//struct SCameraFileHeader
//{
//	char szHeaderString[ 256 ];
//	int nVersion;
//	int nFrame;
//	int nType;
//	int nFOVKeyCount;
//	int nPositionKeyCount;
//	int nRotationKeyCount;
//	float fDefaultFOV;
//	EtVector3 vDefaultPosition;
//	EtQuat qDefaultRotation;
//	char szCameraName[ 32 ];
//};
//
//
//// bintitle - STE_CameraEffect_KeyFrame.
//// .cam 데이타를 읽어들여 카메라KeyFrame길이만큼 Signal의 EndFrame 을 설정한다.
//void CEventPaneView::ChangeValueSelectedSignal( CUnionValueProperty *pVariable, DWORD dwIndex )
//{
//	if( m_pPrevSelectItems.size() > 1 )
//		return;
//
//	CEventGridItemBase * pCurItem = m_pPrevSelectItems[ 0 ];
//	const int nSignalIndex = pCurItem->pSignal->GetSignalIndex();
//
//	// STE_CameraEffect_KeyFrame.
//	if( nSignalIndex != 123 ) 
//		return;
//	
//	// 카메라 읽기.
//	SCameraFileHeader cameraData;
//	//char * pStr = pVariable->GetVariableString();
//	CFileNameString strFile( pVariable->m_szValue );
//	CResMngStream Stream( CEtResourceMng::GetInstance().GetFullName( strFile ).c_str() );	
//	Stream.Read( &cameraData, sizeof(SCameraFileHeader) );	
//	Stream.Seek( ( 1024 - sizeof( SCameraFileHeader ) ), SEEK_CUR );
//	if( strstr( cameraData.szHeaderString, "Eternity Engine Camera File" ) == NULL )
//	{
//		MessageBox( "\".cam\" File Read Error.", "ERROR", MB_OK );
//		return;
//	}	
//	// Signal EndFrame 길이변경.
//	//CEventGridItemBase *pBase = static_cast<CEventGridItemBase *>(pCurItem);	
//	//pBase->pSignal->SetEndFrame( pBase->pSignal->GetEndFrame() + cameraData.nFrame );
//	//pBase->nEnd = pBase->pSignal->GetEndFrame();
//	//Invalidate();
//}

void CEventPaneView::OnEventpanecontextRemovesignal()
{
	// TODO: Add your command handler code here
#ifdef _SELECT_MULTIPLE_SIGNAL
	if (m_pPrevSelectItems.empty() == false)
	{
		std::vector<CEventGridItemBase*>::iterator iter = m_pPrevSelectItems.begin();
		for (; iter != m_pPrevSelectItems.end();)
		{
			CEventGridItemBase* pBase = (*iter);
			if (pBase != NULL)
			{
				if (OnRemoveSignal(pBase))
				{
					iter = m_pPrevSelectItems.erase(iter);
					SAFE_DELETE(pBase);
					continue;
				}
			}

			++iter;
		}

		Invalidate();

		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
	}
#else
	if( m_pPrevSelectItem ) {
		for( DWORD i=0; i<m_pVecSignalList.size(); i++ ) {
			if( m_pVecSignalList[i] == m_pPrevSelectItem ) {
				m_pVecSignalList.erase( m_pVecSignalList.begin() + i );
				break;
			}
		}

		m_pPrevSelectItem->pSignal->GetParent()->RemoveChild( m_pPrevSelectItem->pSignal );
		if( m_pCopyCutBuffer == m_pPrevSelectItem->pSignal ) m_pCopyCutBuffer = NULL;
		SAFE_DELETE( m_pPrevSelectItem );
		m_pPrevSelectItem = NULL;
		Invalidate();

		CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
		if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
	}
#endif
}

void CEventPaneView::OnEventpanecontextCopysignal()
{
	// TODO: Add your command handler code here
#ifdef _SELECT_MULTIPLE_SIGNAL
	ExportSignal( m_CopyAndPastePath );

	//CopySignalFromPrevSelectItem();
#else
	if( m_pPrevSelectItem ) m_pCopyCutBuffer = m_pPrevSelectItem->pSignal;
#endif
}

void CEventPaneView::OnEventpanecontextCutsignal()
{
	// TODO: Add your command handler code here
#ifdef _SELECT_MULTIPLE_SIGNAL
	CopySignalFromPrevSelectItem();
#else
	if( m_pPrevSelectItem ) m_pCopyCutBuffer = m_pPrevSelectItem->pSignal;
#endif
}

#ifdef _SELECT_MULTIPLE_SIGNAL
BOOL CEventPaneView::PasteSignal(const int& nGridStart, const int& nGridEnd, const int& nYOrder, const int& signalIndex, const std::vector<CUnionValueProperty *>* pPropertyList)
{
	int nRealUsingGridStart = nGridStart;
	int nRealUsingGridEnd = nGridEnd;

	int overLength = IsSignalEndOverRangeMax(nRealUsingGridEnd);
	if (overLength > 0)
	{
		if (nRealUsingGridStart - overLength < 0)
		{
			return FALSE;
		}
		else
		{
			nRealUsingGridStart -= overLength;
			nRealUsingGridEnd -= overLength;
		}
	}

	bool bAddGridItem = CanAddSignal(nRealUsingGridStart, nRealUsingGridEnd, nYOrder);
	if (bAddGridItem == false)
		return FALSE;

	CEventGridItemBase *pBase = (CEventGridItemBase *)CreateSignal();
	if( pBase == NULL )
		return FALSE;

	pBase->nStartFrame = nRealUsingGridStart;
	pBase->nEndFrame = nRealUsingGridEnd;
	pBase->nYOrder = nYOrder;

	CSignalItem *pSignal = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( signalIndex );
	if (pSignal == NULL)
		return FALSE;

	CString szElementName = CGlobalValue::GetInstance().GetControlObject()->GetName();

	pBase->pSignal = CGlobalValue::GetInstance().AddSignal( szElementName, signalIndex, pBase->nStartFrame, pBase->nEndFrame, pBase->nYOrder );
	pBase->InitialColor = pSignal->GetInitialsColor();
	pBase->Color = pSignal->GetBackgroundColor();
	if( pSignal->IsHasLength() )
		pBase->szString = pSignal->GetName();
	else pBase->szString = pSignal->GetName()[0];
	pBase->bModifyLength = pSignal->IsHasLength();

	if (pPropertyList && pPropertyList->size() > 0)
	{
		const std::vector<CUnionValueProperty *> *pVecTargetList = pBase->pSignal->GetPropertyList();
		for( DWORD i=0; i<pPropertyList->size(); i++ ) {
			*(*pVecTargetList)[i] = *(*pPropertyList)[i];
		}
	}

	m_pVecSignalList.push_back( pBase );

	return TRUE;
}

void CEventPaneView::PasteCopyCutBuffer()
{
	CRect willPasteArea(m_CopyCutBuffer.wholeArea);
	CPoint currentGridPos;
	MousePosToGridPos( CPoint(m_SelectEmptyMousePos.x, m_SelectEmptyMousePos.y), currentGridPos );
	willPasteArea.MoveToXY(currentGridPos);

	int nRealUsingGridY = currentGridPos.y;

	int overLength = IsSignalEndOverRangeMax(willPasteArea.right);
	if (overLength > 0)
	{
		if (willPasteArea.left - overLength < 0)
		{
			return;
		}
		else
		{
			willPasteArea.left -= overLength;
			willPasteArea.right -= overLength;
		}
	}

	if (IsSignalInArea(willPasteArea))
	{
		int calcYOrder = GetLastSignalYOrder(willPasteArea);
		if (calcYOrder == INT_MIN)
			return;

		nRealUsingGridY = calcYOrder + 1;
	}

	const std::list<SCopyCutGridItemInfo>& copyCutGridItems = m_CopyCutBuffer.GetGridItemList();
	int nPos = 0, nYOrder = 0;

	const SCopyCutGridItemInfo& mostLeft = m_CopyCutBuffer.GetMostLeftGridItem();
	const SCopyCutGridItemInfo& mostTop = m_CopyCutBuffer.GetMostTopGridItem();

	std::list<SCopyCutGridItemInfo>::const_iterator iter = copyCutGridItems.begin();
	for (; iter != copyCutGridItems.end(); ++iter)
	{
		const SCopyCutGridItemInfo& gridItemInfo = (*iter);
		if (gridItemInfo.pSignal)
		{
			int nPasteGridX = willPasteArea.left + (gridItemInfo.nStartGridX - mostLeft.nStartGridX);
			int nPasteGridY = nRealUsingGridY + (gridItemInfo.nYOrder - mostTop.nYOrder);
			int nSignalGridLength = gridItemInfo.nEndGridX - gridItemInfo.nStartGridX;
			if (PasteSignal(nPasteGridX, nPasteGridX + nSignalGridLength, nPasteGridY, gridItemInfo.pSignal->GetSignalIndex(), gridItemInfo.pSignal->GetPropertyList()) == false)
				continue;
		}
	}

	PushBackSelectedSignalIndex((int)m_pVecSignalList.size());
}
#endif // _SELECT_MULTIPLE_SIGNAL

void CEventPaneView::OnEventpanecontextPaste()
{
	// TODO: Add your command handler code here
#ifdef _SELECT_MULTIPLE_SIGNAL
	ImportSignal( m_CopyAndPastePath );

	if (m_CopyCutBuffer.IsEmpty())
		return;

	PasteCopyCutBuffer();

	ClearSelectedSignals();

	Invalidate();
#else // _SELECT_MULTIPLE_SIGNAL
	if( !m_pCopyCutBuffer ) return;
	int nPos, nYOrder;
	if( CalcSignalPos( m_ContextMousePos.x, m_ContextMousePos.y, nPos, nYOrder ) == false ) return;

	CEventGridItemBase *pBase = (CEventGridItemBase *)CreateGridItem();
	if( pBase == NULL ) return;
	pBase->nStartFrame = nPos;
	pBase->nEndFrame = nPos+1;
	pBase->nYOrder = nYOrder;


	CSignalItem *pSignal = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_pCopyCutBuffer->GetSignalIndex() );

	CString szElementName = CGlobalValue::GetInstance().GetControlObject()->GetName();

	pBase->pSignal = CGlobalValue::GetInstance().AddSignal( szElementName, m_pCopyCutBuffer->GetSignalIndex(), pBase->nStart, pBase->nEnd, pBase->nYOrder );
	pBase->InitialColor = pSignal->GetInitialsColor();
	pBase->Color = pSignal->GetBackgroundColor();
	if( pSignal->IsHasLength() )
		pBase->szString = pSignal->GetName();
	else pBase->szString = pSignal->GetName()[0];
	pBase->bModifyLength = pSignal->IsHasLength();

	std::vector<CUnionValueProperty *> *pVecList =	m_pCopyCutBuffer->GetPropertyList();
	std::vector<CUnionValueProperty *> *pVecTargetList = pBase->pSignal->GetPropertyList();
	for( DWORD i=0; i<pVecList->size(); i++ ) {
		*(*pVecTargetList)[i] = *(*pVecList)[i];
	}

	m_pPrevSelectItem = pBase;
	m_nFocusItemIndex = (int)m_pVecSignalList.size();

	m_pVecSignalList.push_back( pBase );
	pBase->pSignal->SetFocus( true );

	Invalidate();

	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
#endif // _SELECT_MULTIPLE_SIGNAL
}

void CEventPaneView::OnEventpanecontextExportSignal()
{
	TCHAR szFilter[] = _T( "signalexport File (*.sigexport)|*.sigexport|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("sigexport"), _T("*.sigexport"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( dlg.DoModal() != IDOK )
		return;

	ExportSignal( dlg.GetPathName() );
}

void CEventPaneView::OnEventpanecontextImportSignal()
{
	TCHAR szFilter[] = _T( "signalexport File (*.sigexport)|*.sigexport|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("sigexport"), _T("*.sigexport"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( dlg.DoModal() != IDOK )
		return;

	ImportSignal( dlg.GetPathName() );
}

void CEventPaneView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here

	CMenu *pMenu = m_pContextMenu->GetSubMenu(2);

	std::vector<int> nVecEnable;
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_INSERTSIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_REMOVESIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_COPYSIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_CUTSIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_EXPORTSIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_IMPORTSIGNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem( ID_EVENTPANECONTEXT_FULLRANGE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	if( CGlobalValue::GetInstance().IsOpenAction() ) {
#ifdef _SELECT_MULTIPLE_SIGNAL
		DWORD selectItemCount = (DWORD)m_pPrevSelectItems.size();
		if (selectItemCount > 1)
		{
			nVecEnable.push_back( ID_EVENTPANECONTEXT_REMOVESIGNAL );
			nVecEnable.push_back( ID_EVENTPANECONTEXT_COPYSIGNAL );
		}
		else if (selectItemCount == 1)
		{
			nVecEnable.push_back( ID_EVENTPANECONTEXT_REMOVESIGNAL );
			nVecEnable.push_back( ID_EVENTPANECONTEXT_COPYSIGNAL );
			nVecEnable.push_back( ID_EVENTPANECONTEXT_EXPORTSIGNAL );

			CEventGridItemBase* pItem = m_pPrevSelectItems[0];
			if (pItem)
			{
				CSignalItem *pSignalItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( pItem->pSignal->GetSignalIndex() );
				if( pSignalItem && pSignalItem->IsHasLength() )
					nVecEnable.push_back( ID_EVENTPANECONTEXT_FULLRANGE );
			}
		}
		else
		{
			m_ContextMousePos = point;
			ScreenToClient( &m_ContextMousePos );
			if( CanAddSignal( m_ContextMousePos.x, m_ContextMousePos.y ) )
			{
				nVecEnable.push_back( ID_EVENTPANECONTEXT_INSERTSIGNAL );
				nVecEnable.push_back( ID_EVENTPANECONTEXT_IMPORTSIGNAL );
			}
		}

		ImportSignal( m_CopyAndPastePath );
		if( m_CopyCutBuffer.IsEmpty() == false ) {
			m_ContextMousePos = point;
			ScreenToClient( &m_ContextMousePos );
			if( CanAddSignal( m_ContextMousePos.x, m_ContextMousePos.y ) )
				nVecEnable.push_back( ID_EVENTPANECONTEXT_PASTE );
		}
#else
		if( m_pPrevSelectItem ) {
			nVecEnable.push_back( ID_EVENTPANECONTEXT_REMOVESIGNAL );
			nVecEnable.push_back( ID_EVENTPANECONTEXT_COPYSIGNAL );
			CSignalItem *pSignalItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_pPrevSelectItem->pSignal->GetSignalIndex() );
			if( pSignalItem && pSignalItem->IsHasLength() )
				nVecEnable.push_back( ID_EVENTPANECONTEXT_FULLRANGE );
		}
		else {
			m_ContextMousePos = point;
			ScreenToClient( &m_ContextMousePos );
			if( IsAddGridItem( m_ContextMousePos.x, m_ContextMousePos.y ) )
				nVecEnable.push_back( ID_EVENTPANECONTEXT_INSERTSIGNAL );
		}
		if( m_pCopyCutBuffer ) {
			m_ContextMousePos = point;
			ScreenToClient( &m_ContextMousePos );
			if( IsAddGridItem( m_ContextMousePos.x, m_ContextMousePos.y ) )
				nVecEnable.push_back( ID_EVENTPANECONTEXT_PASTE );
		}
#endif
	}

	for( DWORD i=0; i<nVecEnable.size(); i++ )
		pMenu->EnableMenuItem( nVecEnable[i], MF_BYCOMMAND | MF_ENABLED );

	pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}

LRESULT CEventPaneView::OnResetCopyCut( WPARAM wParam, LPARAM lParam )
{
#ifdef _SELECT_MULTIPLE_SIGNAL
	ResetPrevSelectItemSignal();

	m_CopyCutBuffer.Clear();
	m_pPrevSelectItems.clear();
#else
	m_pCopyCutBuffer = NULL;
	m_pPrevSelectItem = NULL;
#endif
	return S_OK;
}
void CEventPaneView::OnEventpanecontextFullrange()
{
	// TODO: Add your command handler code here
#ifdef _SELECT_MULTIPLE_SIGNAL
	if (m_pPrevSelectItems.size() == 1)
	{
		CEventGridItemBase* pBase = m_pPrevSelectItems[0];
		if (pBase)
		{
			CSignalItem *pSignalItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( pBase->pSignal->GetSignalIndex() );
			if( pSignalItem && pSignalItem->IsHasLength() ) {
				CActionElement *pElement = (CActionElement *)m_pObject;
				pBase->nStartFrame = 0;
				pBase->nEndFrame = pElement->GetLength();
				pBase->pSignal->SetStartFrame(0);
				pBase->pSignal->SetEndFrame( pElement->GetLength() );

				Invalidate();

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pBase->pSignal );
			}
		}
	}
#else
	if( m_pPrevSelectItem ) {
		CSignalItem *pSignalItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( m_pPrevSelectItem->pSignal->GetSignalIndex() );
		if( pSignalItem && pSignalItem->IsHasLength() ) {
			CActionElement *pElement = (CActionElement *)m_pObject;
			m_pPrevSelectItem->nStart = 0;
			m_pPrevSelectItem->nEnd = pElement->GetLength();
			m_pPrevSelectItem->pSignal->SetStartFrame(0);
			m_pPrevSelectItem->pSignal->SetEndFrame( pElement->GetLength() );

			Invalidate();

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)m_pPrevSelectItem->pSignal );
		}
	}
#endif
}

#ifdef _SELECT_MULTIPLE_SIGNAL
BOOL CEventPaneView::IsInPrevSelectItems(const CEventGridItemBase* pCurItem) const
{
	if (pCurItem == NULL)
		return FALSE;

	std::vector<CEventGridItemBase*>::const_iterator iter = std::find(m_pPrevSelectItems.begin(), m_pPrevSelectItems.end(), pCurItem);
	return ((iter) != m_pPrevSelectItems.end());
}

void CEventPaneView::PushBackPrevSelectItems(CEventGridItemBase* pItem)
{
	if (pItem == NULL || IsInPrevSelectItems(pItem))
		return;

	m_pPrevSelectItems.push_back(pItem);
}

void CEventPaneView::RemovePrevSelectItem(CEventGridItemBase* pItem)
{
	if (pItem == NULL)
		return;

	std::vector<CEventGridItemBase*>::iterator iter = std::find(m_pPrevSelectItems.begin(), m_pPrevSelectItems.end(), pItem);
	if (iter != m_pPrevSelectItems.end())
		m_pPrevSelectItems.erase(iter);
}
#endif // _SELECT_MULTIPLE_SIGNAL


#ifdef _EXPORT_EXCEL_ACTION_SIGNAL
void CEventPaneView::OnEventpanecontextExportexcel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( ! CGlobalValue::GetInstance().IsOpenAction() ) {
		::MessageBox( NULL, "액션파일을 열어주세요", "Error", MB_OK );
		return;
	}

	TCHAR szFileter[] = _T("Excel File (*.csv) | *.csv|All File (*.*)|*.*||");
	CFileDialog dlg( FALSE, _T("csv"), _T("*.csv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING, szFileter, NULL );

	CActionElement* pElement = dynamic_cast<CActionElement*>( CGlobalValue::GetInstance().GetControlObject() );

	if( ! pElement ) 
		return;

	if( ! pElement->IsStop() ) {
		::MessageBox( NULL,"플레이 종료후 사용해 주세요", "경고", MB_OK) ;
		return;
	}

	if( dlg.DoModal() != IDOK ) {
		//::MessageBox(NULL,  dlg.GetPathName() , "Failed", MB_OK);
		return;
	}
	
	std::wstring wsFileName;
	ToWideString( (LPSTR) dlg.GetPathName().GetString(), wsFileName);	

	CSVWriter Writer;

	if( ! Writer.Initialize( wsFileName.c_str(), L";") ){
		::MessageBox(NULL, "could not open file", "Error", MB_OK );
		return;
	}
	
	CActionSignal*	pSignal = NULL;
	char			sTemp[_MAX_PATH] = {0, };
	DWORD			i;

	if (m_pPrevSelectItems.size() == 1) {

		CEventGridItemBase* pBase = m_pPrevSelectItems[0];
		if (pBase) { 
			pSignal = dynamic_cast<CActionSignal*>( pBase->pSignal );

			if( ! pSignal )
				return;
		
			Writer.StartWrite();	

			if( pSignal->GetPropertyList()->size() == 0 )
				return;
				
			fprintf_s( Writer.GetStream(), "액션명,");
			fprintf_s( Writer.GetStream(), "길 이,");
			ZeroMemory( sTemp, _MAX_PATH );
			sprintf_s( sTemp, "%s", pSignal->GetProperty( 0 )->GetCategory() );
			fprintf_s( Writer.GetStream(), "%s,", sTemp );

			for(  i = 0; i < pSignal->GetPropertyList()->size(); i++ ){
				ZeroMemory( sTemp, _MAX_PATH );
				sprintf_s( sTemp, "%s", pSignal->GetProperty( i )->GetDescription() );
				fprintf_s( Writer.GetStream(), "%s,", sTemp );
			}
			fprintf_s( Writer.GetStream(), "\n");
			ZeroMemory( sTemp, _MAX_PATH );
			sprintf_s( sTemp, "%s", pSignal->GetParent()->GetName() );
			fprintf_s( Writer.GetStream(), "%s,", sTemp );
			ZeroMemory( sTemp, _MAX_PATH );
			sprintf_s( sTemp, "%d", (int)( (CActionElement*)pSignal->GetParent() )->GetLength() );
			fprintf_s( Writer.GetStream(), "%s,", sTemp );
			pSignal->ExportSignal( Writer.GetStream(), 1 );
			Writer.AddField(3);
			Writer.EndWrite();
		}
	}

}
#endif

