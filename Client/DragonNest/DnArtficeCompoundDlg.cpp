#include "Stdafx.h"
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "DnArtficeCompoundDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnItemCompoundRenewalTab.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnArtficeCompoundDlg::CDnArtficeCompoundDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCompoundBase( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pCheckBoxRare(NULL)
, m_pCheckBoxEpic(NULL)
, m_pCheckBoxUnique(NULL)
, m_pCheckBoxLegend(NULL)
, m_pComboBoxJob(NULL)
, m_pStaticTextAccesariesList( NULL )
, m_pTreeCtrlAccessories(NULL)
, m_pTreeCtrlSkillList(NULL)
, m_nSelectJobType( -1 )
, m_bInitialize( true )
, m_pAccesaries( NULL )
, m_bRemote( false )
{

}

CDnArtficeCompoundDlg::~CDnArtficeCompoundDlg()
{
	m_pTreeCtrlAccessories->DeleteAllItems();

	CompoundGroupListClear();
}


void CDnArtficeCompoundDlg::InitialUpdate()
{
	m_pCheckBoxRare   =  GetControl<CEtUICheckBox>("ID_CHECKBOX_RARE");
	m_pCheckBoxEpic   =  GetControl<CEtUICheckBox>("ID_CHECKBOX_EPIC");
	m_pCheckBoxUnique =  GetControl<CEtUICheckBox>("ID_CHECKBOX_UNIQUE");
	m_pCheckBoxLegend  =  GetControl<CEtUICheckBox>("ID_CHECKBOX_MAGIC");

	m_pComboBoxJob    =  GetControl<CEtUIComboBox>("ID_COMBOBOX_CLASS");

	m_pTreeCtrlAccessories = GetControl<CEtUITreeCtl>("ID_TREE_ITEM");
	m_pTreeCtrlSkillList = GetControl<CEtUITreeCtl>("ID_TREE_SETITEM");

	m_pStaticTextAccesariesList = GetControl<CEtUIStatic>("ID_TEXT_SETITEM");
	m_pStaticTextAccesariesList->ClearText();
	m_pStaticTextAccesariesList->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2145 ));	// 2124 : * 스킬 레벨업 효과 

	char szControlName[48];

	CEtUICheckBox* pCheckBox = NULL;
	CEtUIStatic* pStaticTextLevel= NULL;

	m_pCheckBoxRare->SetChecked(true );
	m_pCheckBoxEpic->SetChecked(true );
	m_pCheckBoxUnique->SetChecked(true );
	m_pCheckBoxLegend->SetChecked(true );

	m_pCheckBoxLegend->Enable( false );
	m_pCheckBoxRare->Enable( false );
	m_pCheckBoxUnique->Enable( false );

	CEtUIStatic* pTextMagic = GetControl<CEtUIStatic>("ID_TEXT_MAGIC");
	pTextMagic->Enable( false );
	CEtUIStatic* pTextRare = GetControl<CEtUIStatic>("ID_TEXT_RARE");
	pTextRare->Enable( false );
	CEtUIStatic* pTextUnique = GetControl<CEtUIStatic>("ID_TEXT_UNIQUE");
	pTextUnique->Enable( false );

	for( int i = 0; i < MAXLEVEL; ++i )
	{
		sprintf_s( szControlName, _countof( szControlName ), "ID_CHECKBOX_LEVEL%d", i );
		pCheckBox = GetControl<CEtUICheckBox>( szControlName );	
		pCheckBox->SetChecked( true );
		pCheckBox->Show( ( i >= 8 ) ? false : true );//rlkt_test

		sprintf_s( szControlName, _countof( szControlName ), "ID_TEXT_LEVEL%d", i );
		pStaticTextLevel = GetControl<CEtUIStatic>( szControlName );	
		pStaticTextLevel->SetText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2164 ), ( ( i+2 ) * 10 ) ) );
		pStaticTextLevel->Show(( i >= 8 ) ? false : true);//rlkt_test


		if( i < 8 ) //rlkt_test
		{
			m_vecCheckBoxLevel.push_back( pCheckBox );
			m_vecStaticLevel.push_back( pStaticTextLevel );
			m_nVecLevelList.push_back( i < 3 ? ( i+3 ) * 8 : ( i+2 ) * 10 );    
		} 
	}

	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 46 ), NULL, 1 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 47 ), NULL, 2 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 49 ), NULL, 3 );
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 48 ), NULL, 4 );
#ifdef PRE_ADD_ACADEMIC
#ifndef PRE_REMOVE_ACADEMIC
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 305 ), NULL, 5 );
#endif
#endif
#if defined( PRE_ADD_KALI ) && !defined( PRE_REMOVE_KALI )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 306 ), NULL, 6 );
#endif
//[OK]
#if defined( PRE_ADD_ASSASSIN ) && !defined( PRE_REMOVE_ASSASSIN )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 307 ), NULL, 7 );
#endif

#if defined( PRE_ADD_LENCEA )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 308 ), NULL, 8 );
#endif
#if defined( PRE_ADD_MACHINA )
	m_pComboBoxJob->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 376 ), NULL, 9 );
#endif

	if( CDnActor::s_hLocalActor )
		m_pComboBoxJob->SetSelectedByIndex( CDnActor::s_hLocalActor->GetClassID() - 1 );
	
	RefreshCheckBoxLevel( CDnActor::s_hLocalActor->GetLevel(), m_bRemote );

	if( SetAccessoriesList() )
		RefreshAccessoriesList();
	

	SetCompoundTable( CDnCompoundBase::COMPOUND_ARTFICE_DIALOG );	
	RefreshSkillList();

	m_bInitialize = false;

}
void CDnArtficeCompoundDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompound_Tab01Dlg.ui" ).c_str(), bShow );
}

void CDnArtficeCompoundDlg::SetCompoundTable( int nType )
{
	CompoundGroupListClear();

	if( !CDnActor::s_hLocalActor ) return;
	if( nType != CDnCompoundBase::COMPOUND_ARTFICE_DIALOG ) return;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMCOMPOUNDGROUP );
	DNTableFileFormat* pCompoundSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if( !pSox || !pCompoundSox ) return;

	std::vector<int> nVecList;
	pSox->GetItemIDListFromField( "_MainCategory", nType, nVecList );

	if( nVecList.empty() )
		return;

	SetItemStruct* pSetListStruct = NULL;

	// 직업의 갯수만큼 미리 로드
	for( DWORD i = 0; i < PLAYER_MAX; ++i )
	{
		pSetListStruct =  new SetItemStruct( i + 1 );
		if( pSetListStruct ) m_VecSetList.push_back( pSetListStruct );
	}

	for( DWORD i=0; i<nVecList.size(); i++ )
	{
		CDnCompoundBase::ItemGroupStruct *pStruct = new CDnCompoundBase::ItemGroupStruct;
		pStruct->nGroupTableID = nVecList[i];
		pStruct->nCompoundMainCategory = pSox->GetFieldFromLablePtr( nVecList[i], "_MainCategory" )->GetInteger();
		pStruct->nCompoundGroupID = pSox->GetFieldFromLablePtr( nVecList[i], "_CompoundGroupID" )->GetInteger();			
		pStruct->nCompoundGroupType = pSox->GetFieldFromLablePtr( nVecList[i], "_CompoundGroupType" )->GetInteger();		
		pStruct->nPartsID = pSox->GetFieldFromLablePtr( nVecList[i], "_PartsNameID" )->GetInteger();							
		pStruct->nItemID = pSox->GetFieldFromLablePtr( nVecList[i], "_TooltipItemID" )->GetInteger();
		pStruct->nLevel = pSox->GetFieldFromLablePtr( nVecList[i], "_Level" )->GetInteger();
		pStruct->nRank = pSox->GetFieldFromLablePtr( nVecList[i], "_Rank" )->GetInteger();
		pStruct->nJob = pSox->GetFieldFromLablePtr( nVecList[i], "_JobClass" )->GetInteger();
		pStruct->nActivateLevel = pSox->GetFieldFromLablePtr( nVecList[i], "_ActivateLevel" )->GetInteger();
		pStruct->bIsPreview = pSox->GetFieldFromLablePtr( nVecList[i], "_PreviewState" )->GetBool();
		pStruct->nPreviewDescUIStringID = pSox->GetFieldFromLablePtr( nVecList[i], "_PreviewDescription" )->GetInteger();
		
		// 성민님이 스킬 ID를 만들어주실듯.. 
		//pStruct->nCompooudSkillID = pSox->GetFieldFromLablePtr( nVecList[i], "_PreviewDescription" )->GetInteger();

		pCompoundSox->GetItemIDListFromField( "_CompoundGroupID", pStruct->nCompoundGroupID, pStruct->nVecCompoundList );	 		

		if( pStruct->nJob > 0 &&  ( pStruct->nJob - 1 )  < (int)m_VecSetList.size() )
			m_VecSetList[ pStruct->nJob - 1 ]->Add( pStruct );

	}	
}

bool CDnArtficeCompoundDlg::IsExistItem( const int& nExistitem)
{
	std::vector<FilterTypeStruct>::iterator itor = m_VecSkillList.begin();
	std::vector<FilterTypeStruct>::iterator itorEnd =  m_VecSkillList.end();
	FilterTypeStruct SetItem;
	for( ; itor != itorEnd; ++itor )
	{
		SetItem = (*itor);
		if( SetItem.nUniqueID == nExistitem )
			return true;
	}

	return false;
}

void CDnArtficeCompoundDlg::CompoundGroupListClear()
{
	for( DWORD i = 0; i < m_VecSetList.size(); ++i )
		SAFE_DELETE_VEC( m_VecSetList[i]->m_SetItemList );	
}

bool CDnArtficeCompoundDlg::RefreshSkillList()
{
	if( m_nSelectJobType < 0  || 
		m_nSelectJobType >= (int)m_VecSetList.size() || 
		m_VecSetList.empty() )
		return false;

	m_pTreeCtrlSkillList->DeleteAllItems();
	m_pVecResultList.clear();

	if( m_Rank.nEpic == 0 ) return false;

	std::vector< CDnCompoundBase::ItemGroupStruct* > VecList = m_VecSetList[m_nSelectJobType]->m_SetItemList;
	DNTableFileFormat* pCompoundSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if(  !pCompoundSox ) return false;

	CDnCompoundBase::ItemGroupStruct* pCompoundItem;
	CTreeItem *pSelectedPartsItem = m_pTreeCtrlAccessories->GetSelectedItem();		
	CTreeItem* pItem;
	CTreeItem* pRemoteSelect = NULL;

	int nSelectControlType = 0;
	int nSkillGrade = 0;
	int nID = 0;
	
	if( pSelectedPartsItem ) nSelectControlType = pSelectedPartsItem->GetItemValueInt();

	std::map< int, int > mapJobGrade;
	std::map< int, int >::iterator itor, itorSec, itorEnd;
	bool bPass = false;
	
	CTreeItem* pSubTitleItem = NULL;
	//CTreeItem* pPrimary = m_pTreeCtrlSkillList->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 65 ) ); // 1차 직업 스킬
	//if( pPrimary ) pPrimary->SetItemValueInt( 64 );
	//else return false;

	//CTreeItem* pSecondary = m_pTreeCtrlSkillList->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 310 ) ); // 2차 직업 스킬
	//if( pSecondary ) pSecondary->SetItemValueInt( 310 );
	//else return false;

	//CTreeItem* pTertiary = m_pTreeCtrlSkillList->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 313 ) ); // 3차 직업 스킬
	//if( pTertiary ) pTertiary->SetItemValueInt( 311 );
	//else return false;

	// 악세서리는 모두 에픽이기 때문에 등급 체크 필요 없다
	for( DWORD i = 0; i < (int)VecList.size(); ++i )
	{
		pCompoundItem = VecList[i];
		if( ! pCompoundItem ) continue;

		// 파츠 체크 ( Title or SubTitle ) 
		if( pSelectedPartsItem && pSelectedPartsItem->GetItemValueInt() != pCompoundItem->nPartsID ) continue;
		
		// 레벨 체크
		if( ! CheckCompoundItemLevel( pCompoundItem->nLevel ) ) continue;

		// 1, 2, 3차에 대한 테이블이 생기면 지우자
		for( int j = 0; j < (int)pCompoundItem->nVecCompoundList.size(); ++j )
		{
			//if( mapJobGrade.size() == 3 )
			//	break;

			nID    = pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Explanation" )->GetInteger();	// 헤뷔슬래쉬 + 1 뭐 이런거
			nSkillGrade  = pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Kind" )->GetInteger();			// 직업의 1차 2차 3차 

			itor = mapJobGrade.find( nSkillGrade );
			if( itor == mapJobGrade.end() )
				mapJobGrade[nSkillGrade] = nSkillGrade; // 맵은 들어갈때마다 정렬. 
			else 
				continue;	
		}

		m_pVecResultList.push_back( pCompoundItem );
	}
	
	itor = mapJobGrade.begin(),	itorEnd = mapJobGrade.end();
	for( itor; itor != itorEnd; ++itor )
	{
		pSubTitleItem = m_pTreeCtrlSkillList->FindItemInt( itor->second );
		if( pSubTitleItem == NULL )
		{
			pSubTitleItem = m_pTreeCtrlSkillList->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, itor->second ) );
			pSubTitleItem->SetItemValueInt( itor->second );
			pSubTitleItem->SetTextColor( textcolor::GOLDENROD );
		}
	}

	for( int i = 0; i < (int)m_pVecResultList.size(); ++i )
	{
		pCompoundItem = m_pVecResultList[i];
		if( ! pCompoundItem ) continue;

		for( int j = 0; j < (int)pCompoundItem->nVecCompoundList.size(); ++j )
		{
			nID    = pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Explanation" )->GetInteger();	// 헤뷔슬래쉬 + 1 뭐 이런거
			nSkillGrade  = pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Kind" )->GetInteger();			
			//nGrade = ( pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Kind" )->GetInteger() == 1000001412 ? true : false ); 단계 파싱 
			// 성민님에게 요청해서 새로하나 파자 
			
			itor = mapJobGrade.find( nSkillGrade );
			if( itor != mapJobGrade.end() && m_pTreeCtrlSkillList->FindItemInt( nID ) ==  NULL )
			{
				pSubTitleItem = m_pTreeCtrlSkillList->FindItemInt( nSkillGrade );
				if( ! pSubTitleItem ) return false;
				pItem = m_pTreeCtrlSkillList->AddChildItem( pSubTitleItem, CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nID ) );
				pItem->SetItemValueInt( nID );	
			}
			//else if( (itor++)->second == nSkillGrade && m_pTreeCtrlSkillList->FindItemInt( nID  ) ==  NULL)
			//{
			//	pItem = m_pTreeCtrlSkillList->AddChildItem( pSecondary, CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nID ) );
			//	pItem->SetItemValueInt( nID );	
			//}
			//else if( (itor++)->second == nSkillGrade && m_pTreeCtrlSkillList->FindItemInt( nID ) == NULL )
			//{
			//	pItem = m_pTreeCtrlSkillList->AddChildItem( pTertiary, CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nID ) );
			//	pItem->SetItemValueInt( nID );	
			//}

			if( m_bRemote && m_RemoteCompoundItem.nVecCompoundList.empty() == false 
				&& m_RemoteCompoundItem.nVecCompoundList[0] == pCompoundItem->nVecCompoundList[j] )
			{
				CTreeItem* pSelectItem = m_pTreeCtrlSkillList->FindItemInt( nID );
				if( pSelectItem ) m_pTreeCtrlSkillList->SetSelectItem( pSelectItem );	
			}
		}
	}

	if( m_bRemote )	ProcessCommand( EVENT_TREECONTROL_SELECTION, false, m_pTreeCtrlSkillList, 0 );

	m_pTreeCtrlSkillList->ExpandAll();

	return true;
}

bool CDnArtficeCompoundDlg::CheckCompoundItemLevel( const int& nLevel )
{
	for( DWORD i = 0; i < m_nVecLevelList.size(); ++i )
	{
		if( m_nVecLevelList[i] == nLevel )
			return true;
	}

	return false;
}

int CDnArtficeCompoundDlg::GetTypeFromPartsID( const int& nPartsID )
{
	switch( nPartsID )
	{
	case 5 : case 6 : case 9 : case 10 : case 11: 
		return ITEMTYPE_WEAPON;
	case 0 : case 1 : case 2 :  case 3 : case 4 :
		return ITEMTYPE_PARTS;
	default:
		return ITEMTYPE_NORMAL;
	}
}

bool CDnArtficeCompoundDlg::RefreshAccessoriesList()
{
	if( m_nSelectJobType < 0 || m_nSelectJobType >= CDnSetItemCompoundDlg::PLAYER_MAX )
		return false;

	//if( m_pAccesaries ) m_pTreeCtrlAccessories->SetSelectItem( m_pAccesaries );

	return true;
}

bool CDnArtficeCompoundDlg::SetAccessoriesList()
{	
	m_pTreeCtrlAccessories->DeleteAllItems();

	m_pAccesaries = m_pTreeCtrlAccessories->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2144 ) );	// 2144 : 악세사리
	if( m_pAccesaries ) 
	{ 
		m_pAccesaries->SetItemValueInt( 2144 );
		m_pAccesaries->SetTextColor( textcolor::GOLDENROD );
	}

	CTreeItem* pNecklace = m_pTreeCtrlAccessories->AddChildItem( m_pAccesaries,  CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2227 ) ); // 2227 : 목걸이 
	if( pNecklace ) pNecklace->SetItemValueInt( 2227 );
	CTreeItem* pEarring= m_pTreeCtrlAccessories->AddChildItem( m_pAccesaries, CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2228 ) ); // 2228 : 귀걸이 
	if( pEarring ) pEarring->SetItemValueInt( 2228 );
	CTreeItem* pRing= m_pTreeCtrlAccessories->AddChildItem( m_pAccesaries, CTreeItem::typeOpen, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2229 ) ); // 2229 : 반지 
	if( pRing ) pRing->SetItemValueInt( 2229 );

	m_pTreeCtrlAccessories->ExpandAll();

	if( m_bRemote )
	{
		if( pNecklace->GetItemValueInt() == m_RemoteCompoundItem.nPartsID )
			m_pTreeCtrlAccessories->SetSelectItem( pNecklace );	
		else if( pEarring->GetItemValueInt() == m_RemoteCompoundItem.nPartsID )
			m_pTreeCtrlAccessories->SetSelectItem( pEarring );	
		else if( pRing->GetItemValueInt() == m_RemoteCompoundItem.nPartsID )
			m_pTreeCtrlAccessories->SetSelectItem( pRing );	
	}
	else 
		m_pTreeCtrlAccessories->SetSelectItem( pNecklace );

   if( ! m_bRemote )	m_nSelectJobType = ( CDnActor::s_hLocalActor->GetClassID() - 1 );


	return true;
}

void CDnArtficeCompoundDlg::RefreshCheckBoxLevel( int nLevel, bool bRemote )
{
	for( DWORD i = 0; i < m_vecCheckBoxLevel.size(); ++i )
	{
		m_nVecLevelList[i] = i < 3 ? ( i+3 ) * 8 : ( i+2 ) * 10; 
		if( bRemote )
		{
			if( m_nVecLevelList[i] == nLevel )
			{
				m_vecCheckBoxLevel[i]->SetChecked( true, false );
			}
			else
			{
				m_vecCheckBoxLevel[i]->SetChecked( false, false );
				m_nVecLevelList[i] = 0;
			}
		}
		else 
		{
			if( m_nVecLevelList[i] >= nLevel )
			{
				m_vecCheckBoxLevel[i]->SetChecked( true, false );
			}
			else
			{
				m_vecCheckBoxLevel[i]->SetChecked( false, false );
				m_nVecLevelList[i] = 0;
			}
		}
	}
}

void CDnArtficeCompoundDlg::SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData )
{
	m_nSelectJobType = CompoundGroupData.nJob - 1;
	m_pComboBoxJob->SetSelectedByIndex( m_nSelectJobType );

	m_bRemote = true;
	CDnCompoundBase::eRARE == CompoundGroupData.nRank ? m_pCheckBoxRare->SetChecked( true, false ) : m_pCheckBoxRare->SetChecked( false, false );
	CDnCompoundBase::eEPIC== CompoundGroupData.nRank ? m_pCheckBoxEpic->SetChecked( true, false ) : m_pCheckBoxEpic->SetChecked( false, false );
	CDnCompoundBase::eUNIQUE == CompoundGroupData.nRank ? m_pCheckBoxUnique->SetChecked( true, false ) : m_pCheckBoxUnique->SetChecked( false, false );
	CDnCompoundBase::eLEGEND == CompoundGroupData.nRank ? m_pCheckBoxLegend->SetChecked( true, false ) : m_pCheckBoxLegend->SetChecked( false, false );

	//m_Rank.nLegend == CompoundGroupData.nRank ? m_pCheckBoxLegend->SetChecked( true, false ) : m_pCheckBoxLegend->SetChecked( false, false );

	m_RemoteCompoundItem = CompoundGroupData;

	RefreshCheckBoxLevel( CompoundGroupData.nLevel, m_bRemote );
	SetAccessoriesList();
	RefreshAccessoriesList();
	RefreshSkillList();
}

void CDnArtficeCompoundDlg::Reset()
{
	m_bRemote = false;
	m_nSelectJobType = ( CDnActor::s_hLocalActor->GetClassID() - 1 );
	m_pComboBoxJob->SetSelectedByIndex( m_nSelectJobType );
	
	m_pCheckBoxRare->SetChecked( true, false );
	m_pCheckBoxEpic->SetChecked( true, false );
	m_pCheckBoxUnique->SetChecked( true, false );
	m_pCheckBoxLegend->SetChecked( true, false );

	m_Rank.nLegend = eLEGEND;
	m_Rank.nRare = eRARE;
	m_Rank.nEpic = eEPIC;
	m_Rank.nUnique = eUNIQUE;
	m_Rank.nLegend = eLEGEND;


	RefreshCheckBoxLevel( CDnActor::s_hLocalActor->GetLevel(), m_bRemote );	
	SetAccessoriesList();
	RefreshAccessoriesList();
	RefreshSkillList();
	TREEITEM_LIST pTreeList = m_pTreeCtrlSkillList->GetTravalTreeItem();
	if( pTreeList.empty() == false )
	{
		CTreeItem* pBeginItem = (*pTreeList.begin());
		if( pBeginItem ) m_pTreeCtrlSkillList->SetSelectItem( (*pBeginItem->GetTreeItemList().begin() ) );
	}
	//
}

void CDnArtficeCompoundDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;
	
	CEtUIDialog::Show( bShow );
}
void CDnArtficeCompoundDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_MAGIC")  )
			m_Rank.nLegend = m_pCheckBoxLegend->IsChecked() ? eLEGEND : 0;

		if( IsCmdControl("ID_CHECKBOX_RARE")  )
			m_Rank.nRare = m_pCheckBoxRare->IsChecked() ? eRARE : 0;

		else if( IsCmdControl("ID_CHECKBOX_EPIC")  )
			m_Rank.nEpic = m_pCheckBoxEpic->IsChecked() ? eEPIC : 0;

		else if( IsCmdControl("ID_CHECKBOX_UNIQUE")  )
			m_Rank.nUnique =  m_pCheckBoxUnique->IsChecked() ? eUNIQUE : 0;

		if( strstr( pControl->GetControlName(), "ID_CHECKBOX_LEVEL") )
		{
			for( DWORD i = 0; i < m_vecCheckBoxLevel.size(); ++i )
			{
				// 세트 아이템 레벨 증가 : 24, 32, 40, 50, 60	
				if( m_vecCheckBoxLevel[i]->IsChecked() )	
					m_nVecLevelList[i] = i < 3 ? ( i+3 ) * 8 : ( i+2 ) * 10;   
				else 
				{
					m_nVecLevelList[i] = 0;
				}
			}
		}
		if( m_bRemote )
			Reset();
		else
			RefreshSkillList();
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED ) 
	{
		if( IsCmdControl( "ID_COMBOBOX_CLASS" ) ) 
		{
			if( m_pComboBoxJob->GetSelectedIndex() != m_nSelectJobType )  
				m_nSelectJobType = m_pComboBoxJob->GetSelectedIndex(); 

			if( m_bRemote )
				Reset();
			else
			{
				RefreshSkillList();
				m_pParentDialog->ProcessCommand( EVENT_COMBOBOX_SELECTION_CHANGED, false, m_pComboBoxJob, 0 );
			}
		}
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION )
	{	
		if( IsCmdControl("ID_TREE_ITEM") )
		{		
			//if( m_bRemote )
			//	Reset();
			//else
				RefreshSkillList();
		}
		if( IsCmdControl("ID_TREE_SETITEM") )
		{	
			CDnItemCompoundRenewalTabDlg* pCompoundTabDlg = static_cast<CDnItemCompoundRenewalTabDlg*>( m_pParentDialog );
			CTreeItem* pPartsItem;
			CTreeItem* pSkillList;

			pPartsItem  = m_pTreeCtrlAccessories->GetSelectedItem();
			pSkillList  = m_pTreeCtrlSkillList->GetSelectedItem();
			if( pPartsItem && pCompoundTabDlg || pSkillList && pCompoundTabDlg ) 
			{
				pCompoundTabDlg->UpdateGroupList( CDnCompoundBase::COMPOUND_ARTFICE_DIALOG );
			}		
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

}


void CDnArtficeCompoundDlg::UpdateGroupList( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList  )
{
	if( m_nSelectJobType < 0  || 
		m_nSelectJobType >= (int)m_VecSetList.size() || 
		m_pVecResultList.empty() )
		return;

	DNTableFileFormat* pCompoundSox = GetDNTable( CDnTableDB::TITEMCOMPOUND );
	if(  !pCompoundSox ) return;

	std::vector< CDnCompoundBase::ItemGroupStruct* > VecList = m_pVecResultList;//m_VecSetList[m_nSelectJobType]->m_SetItemList;

	CDnCompoundBase::ItemGroupStruct* pCompoundItem;
	CTreeItem* pSelectedSkillItem = m_pTreeCtrlSkillList->GetSelectedItem();

	int nSkillListCapacity = 0;
	int nSkillID = 0;

	if( pSelectedSkillItem == NULL )
		return;

	// 여기서는 등급 체크를 할 필요가 없다. 왜냐 ? 세트 아이템 리스트를 보여주는 트리 컨트롤에서 이미
	// 등급에 해당 하지 않는 세트 아이템 리스트들이 걸러진다. 

	// VecList는 한 직업의 리스트임으로 직업 체크 패스 
	int nEquipmentType = 0;
	for( DWORD i = 0; i < VecList.size(); ++i )
	{
		pCompoundItem = VecList[i];

		if( ! pCompoundItem )
			continue;

		nSkillListCapacity = (int)pCompoundItem->nVecCompoundList.size();
		pCompoundItem->nVecResultCompoundList.clear();

		for( int j = 0; j < nSkillListCapacity; ++j )
		{
			nSkillID = pCompoundSox->GetFieldFromLablePtr ( pCompoundItem->nVecCompoundList[j], "_Explanation" )->GetInteger();	// 헤뷔슬래쉬 + 1 뭐 이런거
			if( pSelectedSkillItem->GetItemValueInt() ==  nSkillID )	
			{
				pCompoundItem->nVecResultCompoundList.push_back( pCompoundItem->nVecCompoundList[j] );
			}
		}

		pVecResultList.push_back( pCompoundItem );
	}

}
#endif 





