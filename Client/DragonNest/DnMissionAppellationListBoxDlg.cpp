#include "StdAfx.h"
#include "DnMissionAppellationListBoxDlg.h"
#include "DnMissionTask.h"
#include "DnMissionListItem.h"
#include "DnAppellationTask.h"
#include "DnTableDB.h"
#include "DnUIString.h"
#include "DnInterface.h"
#include "DnSimpleTooltipDlg.h"
#include "DnAppellationDlg.h"
#include "DnMainMenuDlg.h"
#include "DnCharStatusDlg.h"
#include "DnMissionDlg.h"
#include "DnAppellationBookDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL


CDnMissionAppellationListBoxDlg::CDnMissionAppellationListBoxDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEx(NULL)
, m_pComboBox(NULL)
, m_pAppellationButton(NULL)
{
	m_vecMissionInfoList.clear();
}

CDnMissionAppellationListBoxDlg::~CDnMissionAppellationListBoxDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
	SAFE_DELETE_PVEC(m_vecMissionInfoList);
}

void CDnMissionAppellationListBoxDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionTiltle_AppellationDlg.ui" ).c_str(), bShow );
}

void CDnMissionAppellationListBoxDlg::InitialUpdate()
{
	m_pAppellationButton = GetControl<CEtUIButton>("ID_BT_APPELLATION");
	
	m_pListBoxEx		 = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_APPELLATION");
	m_pListBoxEx->Show(false);
	
	m_pComboBox			 = GetControl<CEtUIComboBox>("ID_COMBOBOX_LIST");

	// MsgProc에서, 히든인지 체크하기 위해 Value저장.
	std::vector<CDnAppellationTask::stAppellationCollection>& pVecAppellationCollection = CDnAppellationTask::GetInstance().GetAppellationCollectionList();
	if(pVecAppellationCollection.empty()) return;
	WCHAR wszStr[256] = {0,};
	for(int i = 0 ; i < (int)pVecAppellationCollection.size() ; ++i)
	{
		// 히든 미션인지 , 공개미션인지 체크.
		int nAppellation_Type = E_APPELLATION_HIDE;
		if(pVecAppellationCollection[i].m_bOpen) nAppellation_Type = E_APPELLATION_SHOW;

		swprintf_s(wszStr, pVecAppellationCollection[i].m_strCollectionTitle.c_str() );
		m_pComboBox->AddItem(wszStr, NULL, nAppellation_Type);
	}
}

void CDnMissionAppellationListBoxDlg::Show( bool bShow )
{
	if( bShow == IsShow() )
		return;

	if( bShow )
	{
		m_pListBoxEx->Show(bShow);
		RefreshInfo();
	}
	else
	{
		m_pListBoxEx->Show(false);
		m_pListBoxEx->RemoveAllItems();
	}

	CEtUIDialog::Show( bShow );
}

bool CDnMissionAppellationListBoxDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			if(!m_pComboBox)
				return bRet;

			float fX , fY = .0f;
			POINT PointMouse;
			PointMouse.x = short(LOWORD(lParam));
			PointMouse.y = short(HIWORD(lParam));
			PointToFloat(PointMouse, fX, fY);
	
			if( IsMouseInDlg() && m_pComboBox->IsInside(fX, fY) )
			{
				int nAppellationType = -1;
				m_pComboBox->GetSelectedValue(nAppellationType);
				if(nAppellationType == E_APPELLATION_HIDE)
				{
					CDnSimpleTooltipDlg* pToolTip = GetInterface().GetSimpleTooltipDialog();
					if(pToolTip)
					{
						WCHAR wszTemp[512] = { 0, };
						wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9186 ) );
						pToolTip->ShowTooltipDlg(m_pComboBox, true, wszTemp, 0xffffffff, true);
					}
				}
			}

			if( IsMouseInDlg() && m_pAppellationButton->IsInside(fX, fY) )
			{
				CDnSimpleTooltipDlg* pToolTip = GetInterface().GetSimpleTooltipDialog();
				if(pToolTip)
				{
					WCHAR wszTemp[512] = { 0, };
					wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9192 ) );
					pToolTip->ShowTooltipDlg(m_pAppellationButton, true, wszTemp, 0xffffffff, true);
				}
			}
		}
		break;
	}

	return bRet;
}

void CDnMissionAppellationListBoxDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_BT_APPELLATION"))
		{
			// 미션창 Off
			CDnMissionDlg *pDlg = (CDnMissionDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::MISSION_DIALOG );
			if( pDlg ) pDlg->Show(false);
			
			// 칭호 컬렉션 On
			CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if( !pCharStatusDlg ) return;
			pCharStatusDlg->Show(true);

			CDnAppellationDlg* pAppDlg = pCharStatusDlg->GetAppellationDlg();
			if( !pAppDlg ) return;
			pAppDlg->Show(true);
			pAppDlg->ChangeDialogMode( CDnAppellationDlg::eDialogMode::CollectionBook );
			pAppDlg->GetAppellationTabDlg()->SetCheckCollectionBtn(true);

			// 페이지 이동
			int nSelectedIndex = m_pComboBox->GetSelectedIndex();
			pAppDlg->GetAppellationBookDlg()->SelectContents(nSelectedIndex);

		}
	}
	else if(nCommand == EVENT_COMBOBOX_SELECTION_CHANGED)
	{
		// 리스트 박스 새로 업데이트.
		if(m_pComboBox)
		{		
			RefreshInfo();
		}
	}
	else if(nCommand == EVENT_LISTBOX_SELECTION)
	{
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


CDnMissionTask::MissionInfoStruct* CDnMissionAppellationListBoxDlg::CreateMission(int nArrayIndex)
{
	DNTableFileFormat* pSox				= GetDNTable( CDnTableDB::TMISSION );
	DNTableFileFormat* pCashCommodity	= GetDNTable( CDnTableDB::TCASHCOMMODITY );
	if(!pSox || !pCashCommodity) return NULL;

	int nItemID = pSox->GetItemID( nArrayIndex );
	if( nItemID == -1 ) return NULL;

	CDnMissionTask::MissionInfoStruct *pStruct = new CDnMissionTask::MissionInfoStruct;
	pStruct->nArrayIndex	= nArrayIndex;
	pStruct->nNotifierID	= nItemID;
	pStruct->MainCategory	= (CDnMissionTask::MainCategoryEnum) pSox->GetFieldFromLablePtr( nItemID, "_MainCategory" )->GetInteger();
	pStruct->szSubCategory	= pSox->GetFieldFromLablePtr( nItemID, "_SubCategoryID" )->GetString();
	int		nNameID			= pSox->GetFieldFromLablePtr( nItemID, "_TitleNameID" )->GetInteger();
	char*	szParam			= pSox->GetFieldFromLablePtr( nItemID, "_TitleNameIDParam" )->GetString();
	MakeUIStringUseVariableParam( pStruct->szTitle, nNameID, szParam );
	
	nNameID = pSox->GetFieldFromLablePtr( nItemID, "_SubTitleNameID" )->GetInteger();
	szParam = pSox->GetFieldFromLablePtr( nItemID, "_SubTitleNameIDParam" )->GetString();
	MakeUIStringUseVariableParam( pStruct->szSubTitle, nNameID, szParam );
	
	pStruct->nIcon			= pSox->GetFieldFromLablePtr( nItemID, "_Icon" )->GetInteger();
	pStruct->nAchieveIcon	= pSox->GetFieldFromLablePtr( nItemID, "_AchieveIcon" )->GetInteger();
	
	int nMailID = pSox->GetFieldFromLablePtr( nItemID, "_MailID" )->GetInteger();
	for( int i = 0 ; i < 3 ; i++ ) 
	{
		pStruct->nRewardItemID[i]	 = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemID, false, i );
		pStruct->nRewardItemCount[i] = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemCount, false, i );
	}
	for( int i = 0 ; i < 3 ; i++ ) 
	{
		pStruct->nRewardCashItemSN[i] = CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardItemID, true, i );
		
		// 캐쉬템은 cashcommodity 테이블에 갯수가 지정되어 있음.
		if( 0 < pStruct->nRewardCashItemSN[i] )
			pStruct->nRewardCashItemCount[i] = pCashCommodity->GetFieldFromLablePtr( pStruct->nRewardCashItemSN[i], "_Count" )->GetInteger();
		else
			pStruct->nRewardCashItemCount[i] = 0;
	}

	pStruct->nCounter				= 0;
	pStruct->bAchieve				= false;
	pStruct->nRewardCoin			= CDnTableDB::GetInstance().GetMailInfo( nMailID, CDnTableDB::MailInfoRewardCoin );
	pStruct->nRewardAppellationID	= pSox->GetFieldFromLablePtr( nItemID, "_RewardAppellation" )->GetInteger();
	pStruct->nRewardPoint			= pSox->GetFieldFromLablePtr( nItemID, "_RewardPoint" )->GetInteger();
	pStruct->nAchieveSystemMsgID	= pSox->GetFieldFromLablePtr( nItemID, "_SystemMsg" )->GetInteger();
	pStruct->nType					= pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();

	m_vecMissionInfoList.push_back(pStruct);
	return pStruct;
}

void CDnMissionAppellationListBoxDlg::RefreshInfo()
{
	if( !CDnMissionTask::IsActive() || !CDnAppellationTask::IsActive() )
		return;
	
	std::vector<CDnAppellationTask::stAppellationCollection>& pVecAppellationCollection = CDnAppellationTask::GetInstance().GetAppellationCollectionList();
	if( pVecAppellationCollection.empty() ) return;
	
	int nCnt = m_pComboBox->GetSelectedIndex();
	if( (int)pVecAppellationCollection.size() < nCnt )	return;

	DNTableFileFormat* pMissionSox = GetDNTable( CDnTableDB::TMISSION);
	if(!pMissionSox) return;

	m_pListBoxEx->RemoveAllItems();
	std::map<int,int>::iterator itAppellationID = pVecAppellationCollection[nCnt].m_mapAppellationIndex.begin();
	for( ; itAppellationID != pVecAppellationCollection[nCnt].m_mapAppellationIndex.end() ; ++itAppellationID )
	{
		int nItemID = -1;
		CDnMissionTask::MissionInfoStruct* sMissionInfo = NULL;

		for(int j = 0 ; j < pMissionSox->GetItemCount() ; ++j)
		{
			nItemID = pMissionSox->GetItemID(j);
			if( nItemID == -1 ) continue;

			if( (*itAppellationID).first == pMissionSox->GetFieldFromLablePtr( nItemID , "_RewardAppellation")->GetInteger() )
			{
				sMissionInfo = CreateMission(j);
				break;
			}
		}
		if( !sMissionInfo ) continue;
		
		if( CDnAppellationTask::GetInstance().IsExistAppellation( (*itAppellationID).first - 1 ) )
			sMissionInfo->bAchieve = true;

		// 히든 미션이면서, 습득하지 못했다면 건너뛴다.
		if( pVecAppellationCollection[nCnt].m_bOpen == E_APPELLATION_HIDE && !sMissionInfo->bAchieve )
			continue;

		CDnMissionListItem *pItem = m_pListBoxEx->AddItem<CDnMissionListItem>();
		pItem->SetInfo( sMissionInfo );
#ifdef PRE_MOD_MISSION_HELPER
		pItem->ShowCheckBox( false );
#endif
	}
}

#endif