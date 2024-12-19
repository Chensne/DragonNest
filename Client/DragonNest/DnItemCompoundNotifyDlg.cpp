#include "StdAfx.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnItemCompoundNotifyDlg.h"
#include "DnMainMenuDlg.h"
#include "DnItemCompoundTab2Dlg.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
#include "DnPotentialJewelDlg.h"
#endif

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "DnCompoundBase.h"
#include "DnItemCompoundRenewalTab.h"
#endif

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND

int CDnItemCompoundNotifyDlg::s_nNotifyItemIndex = 0;
int CDnItemCompoundNotifyDlg::s_nNotifyItemCompoundID = 0;

CDnItemCompoundNotifyDlg::CDnItemCompoundNotifyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{   
	m_pButtonCover = NULL;
	m_pItemSlotButton = NULL;
	m_pNotifyItem = NULL;
	m_pStaticItemName = NULL;
	m_pProgressBar = NULL;
	s_nNotifyItemIndex = 0;
	s_nNotifyItemCompoundID = 0;
}

CDnItemCompoundNotifyDlg::~CDnItemCompoundNotifyDlg(void)
{
	SAFE_DELETE( m_pNotifyItem );
}


void CDnItemCompoundNotifyDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompoundHighLvSumInfoDlg.ui" ).c_str(), bShow );
}

void CDnItemCompoundNotifyDlg::InitialUpdate()
{
	m_pStaticItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pProgressBar = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR2");
	m_pProgressBar->SetProgress( 0.f );
	m_pProgressBar->Show( false );

	m_pButtonCover = GetControl<CEtUIButton>("ID_BT_COMPOUND");
	m_pItemSlotButton = GetControl<CDnItemSlotButton>( "ID_BT_ITEM" );
	m_pItemSlotButton->SetSlotType( ST_ITEM_NOTIFY_COMPOUND );
	m_pItemSlotButton->Show(false);
	m_pItemSlotButton->Enable(false);

}

void CDnItemCompoundNotifyDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
		if(!pPotentialJewelDlg) return;

		bool bIsShowPotentialDlg = pPotentialJewelDlg->IsShow();
		m_pButtonCover->Enable( !bIsShowPotentialDlg );
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

		if( strcmp( pControl->GetControlName(), "ID_BT_COMPOUND" ) == 0 ) 
		{
			OpenCompoundShopFromCompoundIndex( GetCompoundIndex() );
		}

		if( strcmp( pControl->GetControlName(), "ID_BT_ITEM") == 0 )
		{
			OpenCompoundShopFromCompoundIndex( GetCompoundIndex() );
		}
	}
}

void CDnItemCompoundNotifyDlg::Show( bool bShow ) 
{ 
	CDnCustomDlg::Show( bShow );
}

void CDnItemCompoundNotifyDlg::SetItemNotify( int nItemIndex , int nOptionIndex )
{
	SAFE_DELETE( m_pNotifyItem );
	m_pItemSlotButton->ResetSlot();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if( pSox )
	{
		int nMakeItemIndex = pSox->GetFieldFromLablePtr( nOptionIndex, "_SuccessItemID1" )->GetInteger();
		
		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( nMakeItemIndex, 1, itemInfo ) == true )
		{
			int nPotentialID = pSox->GetFieldFromLablePtr( nOptionIndex, "_CompoundPotentialID" )->GetInteger();
			itemInfo.Item.cOption = nPotentialID;

			m_pNotifyItem = CDnItemTask::GetInstance().CreateItem( itemInfo );
			m_pItemSlotButton->SetItem( m_pNotifyItem , CDnSlotButton::ITEM_ORIGINAL_COUNT );
			m_pItemSlotButton->Show( true );
			m_pItemSlotButton->Enable( true );
			m_pButtonCover->Show( false );

			s_nNotifyItemIndex = nItemIndex;
			s_nNotifyItemCompoundID = nOptionIndex;

			RefreshItemPercentage();
		}
	}
}

void CDnItemCompoundNotifyDlg::RefreshItemPercentage()
{
	if( s_nNotifyItemCompoundID == 0 ) return;

	DNTableFileFormat* pCompoundSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if( pCompoundSox && pCompoundSox->IsExistItem(s_nNotifyItemCompoundID) )
	{
		int nResultItemID = 0;
		int nItemOption = 0;
		float nCurItemCount = 0;
		float nMaxNeedItemCount = 0;

		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
		GetItemTask().GetCompoundInfo( CDnItemCompoundNotifyDlg::GetCompoundIndex() , &CompoundInfo );

		bool bPossible = true;
		for( int k=0; k<5; k++ ) 
		{
			if( CompoundInfo.aiItemID[k] > 0 && CompoundInfo.aiItemCount[k] > 0 )
			{
				int nItemCount = 0;

				if( CompoundInfo.abResultItemIsNeedItem[k] )
				{
					nItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[k] , CompoundInfo.iCompoundPreliminaryID );
				}
				else
				{
					nItemCount = GetItemTask().GetCharInventory().GetItemCount( CompoundInfo.aiItemID[k] );
				}

				if( nItemCount > CompoundInfo.aiItemCount[k] )
					nItemCount = CompoundInfo.aiItemCount[k];

				nCurItemCount += nItemCount;
				nMaxNeedItemCount += CompoundInfo.aiItemCount[k];
			}
		}

		nResultItemID = pCompoundSox->GetFieldFromLablePtr( s_nNotifyItemCompoundID, "_SuccessItemID1" )->GetInteger();;
		nItemOption = pCompoundSox->GetFieldFromLablePtr( s_nNotifyItemCompoundID, "_CompoundPotentialID" )->GetInteger();

		std::wstring wstResult =  CDnItem::GetItemFullName( nResultItemID , nItemOption );
		std::wstring wstPercentage = FormatW( L" %d%%" , (int)( (nCurItemCount / nMaxNeedItemCount) * 100.f ) );
		wstResult += wstPercentage;
	
		m_pStaticItemName->ClearText();
		m_pStaticItemName->ClearPartColorText();

		m_pStaticItemName->SetText( wstResult );

		DNTableFileFormat* pItemSox = GetDNTable( CDnTableDB::TITEM );
		if( pItemSox )
		{
			eItemRank eType = (eItemRank)pItemSox->GetFieldFromLablePtr( nResultItemID , "_Rank" )->GetInteger();
			m_pStaticItemName->SetTextColor( DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( eType ) );
		}
		
		m_pStaticItemName->SetPartColorText( wstPercentage , textcolor::LIGHTSKYBLUE , D3DCOLOR_ARGB(0, 255, 255, 255) );
	
		float fProgressPercentage = 0.f;
		if( nMaxNeedItemCount > 0 )
		{
			fProgressPercentage = (nCurItemCount / nMaxNeedItemCount) * 100.f;
		}

		m_pProgressBar->Show( true );
		m_pProgressBar->SetProgress( fProgressPercentage );
	}
}

void CDnItemCompoundNotifyDlg::ResetItemNotify()
{
	SAFE_DELETE( m_pNotifyItem );
	m_pItemSlotButton->ResetSlot();
	m_pButtonCover->Show( true );

	m_pProgressBar->Show( false );
	m_pItemSlotButton->Show( false );
	m_pItemSlotButton->Enable( false );
	m_pStaticItemName->ClearText();
	s_nNotifyItemIndex = 0;
	s_nNotifyItemCompoundID = 0;
}


bool CDnItemCompoundNotifyDlg::IsMatchedNotifyItem( int nCompoundIndex )
{
	if( s_nNotifyItemCompoundID == nCompoundIndex )
	{
		return true;
	}

	return false;
}

void CDnItemCompoundNotifyDlg::OpenCompoundShopFromCompoundIndex( int nCompoundIndex )
{
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
	CDnItemCompoundRenewalTabDlg *pDlg = (CDnItemCompoundRenewalTabDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG);
#else
	CDnItemCompoundTab2Dlg *pDlg = (CDnItemCompoundTab2Dlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG);
#endif 
	if( pDlg )
	{
		int COMPOUND_ITEM_SHOP_ID = 1001;
		int COMPOUND_GROUP_TYPE = 0;
		int COMPOUND_LEVEL = 0;
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
		CDnCompoundBase::ItemGroupStruct pStruct;
		memset( &pStruct, 0, sizeof( CDnCompoundBase::ItemGroupStruct ) );
#endif 

		DNTableFileFormat* pSoxCompound = GetDNTable( CDnTableDB::TITEMCOMPOUND );
		DNTableFileFormat* pSoxCompoundGroup = GetDNTable( CDnTableDB::TITEMCOMPOUNDGROUP );
		if( nCompoundIndex > 0 && pSoxCompound && pSoxCompound )
		{
			if( pSoxCompound->IsExistItem( nCompoundIndex ) )
			{
				int nCompoundGroupIndex = pSoxCompound->GetFieldFromLablePtr( nCompoundIndex , "_CompoundGroupID" )->GetInteger();
				int nCompoundGroupUnique = -1;

				for( int i=0; i<pSoxCompoundGroup->GetItemCount(); i++ )
				{
					int nItemID = pSoxCompoundGroup->GetItemID( i );
					if( nCompoundGroupIndex == pSoxCompoundGroup->GetFieldFromLablePtr( nItemID, "_CompoundGroupID" )->GetInteger() )
					{
						nCompoundGroupUnique = nItemID;
						break;
					}
				}
				if( pSoxCompoundGroup->IsExistItem( nCompoundGroupUnique ) )
				{
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
					pStruct.nGroupTableID = nCompoundGroupUnique;
					pStruct.nCompoundMainCategory = pSoxCompoundGroup->GetFieldFromLablePtr(nCompoundGroupUnique, "_MainCategory" )->GetInteger();
					pStruct.nCompoundGroupID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_CompoundGroupID" )->GetInteger();			
					pStruct.nCompoundGroupType = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_CompoundGroupType" )->GetInteger();		
					pStruct.nPartsID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_PartsNameID" )->GetInteger();							
					pStruct.nItemID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_TooltipItemID" )->GetInteger();
					pStruct.nLevel = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_Level" )->GetInteger();
					pStruct.nRank = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_Rank" )->GetInteger();
					pStruct.nJob = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_JobClass" )->GetInteger();
					pStruct.nActivateLevel = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_ActivateLevel" )->GetInteger();
					pStruct.bIsPreview = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_PreviewState" )->GetBool();
					pStruct.nPreviewDescUIStringID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_PreviewDescription" )->GetInteger();
					pStruct.nCompoundSetItemID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_SetID" )->GetInteger();
					pStruct.nSuffixGrade = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_ListID" )->GetInteger();

					pSoxCompound->GetItemIDListFromField( "_CompoundGroupID", pStruct.nCompoundGroupID, pStruct.nVecCompoundList );
#else
					COMPOUND_ITEM_SHOP_ID = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique , "_ListID" )->GetInteger();
					COMPOUND_GROUP_TYPE = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique , "_CompoundGroupType" )->GetInteger();
					COMPOUND_LEVEL = pSoxCompoundGroup->GetFieldFromLablePtr( nCompoundGroupUnique, "_Level" )->GetInteger();
#endif 
				}
			}
		}

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
		// 접미사 제작창 UI를 모두 초기화 상태로 만든다음에 
		// 파츠선택과, 탭 고유의 선택창에 포커스가 가도록 한다. 
		pStruct.nVecResultCompoundList.clear();
		for( int i = 0; i < (int)pStruct.nVecCompoundList.size(); ++i )
		{
			if( pStruct.nVecCompoundList[i] == GetCompoundIndex() )
			{
				pStruct.nVecResultCompoundList.push_back( GetCompoundIndex() );
				pStruct.nVecCompoundList = pStruct.nVecResultCompoundList;
				break;
			}
		}
		pDlg->Reset();
		pDlg->SetForceOpenMode();
		pDlg->SetRemoteCompoundDlg( pStruct );	
		GetInterface().GetMainMenuDialog()->ToggleShowDialog( CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG );	

#else
		pDlg->SetCompoundListID( COMPOUND_ITEM_SHOP_ID );
		pDlg->SelectCompoundPartsType( COMPOUND_GROUP_TYPE );
		pDlg->RefreshCompoundGroupByLevel( COMPOUND_LEVEL );
		pDlg->SetForceOpenMode( !pDlg->IsShow() );
		GetInterface().GetMainMenuDialog()->ToggleShowDialog( CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG );
		pDlg->SelectCompoundItemFromIndex( GetCompoundIndex() );
#endif 
	}
}

void CDnItemCompoundNotifyDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	RefreshItemPercentage();
}

#endif
