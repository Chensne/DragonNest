#include "StdAfx.h"
#include "DnCharSelectListDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnAuthTask.h"
#include "DnTableDB.h"
#include "DnSimpleTooltipDlg.h"
#include "DnInterfaceString.h"
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_MOD_SELECT_CHAR

CDnCharSelectListDlg::CDnCharSelectListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pCharacterListBox( NULL )
, m_pStaticCharCount( NULL )
#ifdef PRE_ADD_TOTAL_LEVEL_SKILL
, m_pTotalLevelInfo( NULL )
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
#ifdef PRE_CHARLIST_SORTING
, m_pCharacterListBoxSortComboBox( NULL )
#endif // PRE_CHARLIST_SORTING
{
}

CDnCharSelectListDlg::~CDnCharSelectListDlg(void)
{
#ifdef PRE_ADD_NEWCOMEBACK
	m_vecCharListData.clear();
#endif // PRE_ADD_NEWCOMEBACK
	m_pCharacterListBox->RemoveAllItems();
}

void CDnCharSelectListDlg::Initialize( bool bShow )
{
#ifdef _ADD_NEWLOGINUI
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charselectdlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_characterDlg.ui" ).c_str(), bShow );
#endif
}

void CDnCharSelectListDlg::InitialUpdate()
{
	m_pCharacterListBox = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_LIST" );
	m_pStaticCharCount = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );

#ifdef _ADD_NEWLOGINUI
	GetControl<CEtUIStatic>("ID_STATIC_NOTICE")->Show(false);
	GetControl<CEtUIStatic>("ID_TEXT_NOTICE")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_NEW1")->Show(IsNewCharacter());
#else
	//rlkt_test
	GetControl<CEtUIStatic>("ID_STATIC_NEW")->Show(true);
	GetControl<CEtUIButton>("ID_BT_DARK")->Show(true);
#endif

//#if defined(PRE_ADD_23829)
// �⺻������ �� ��ư�� Hide���� ���´�. [2010/12/17 semozz]
	CEtUIButton *pButton = GetControl<CEtUIButton>( "ID_BT_PASSCREATE" );
	if( pButton )
	{
		pButton->Show( false );
	}
//#endif // PRE_ADD_23829

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_pTotalLevelInfo = GetControl<CEtUIStatic>("ID_TOTAL_LEVEL");
	if (m_pTotalLevelInfo)
		m_pTotalLevelInfo->SetText(L"");
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
#ifdef PRE_CHARLIST_SORTING
	m_pCharacterListBoxSortComboBox = GetControl<CEtUIComboBox>( "ID_COMBOBOX_SORT" );
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1226 ), NULL, 1 );	// ���� �������
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1227 ), NULL, 2 );	// ĳ���� ���� �������
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1228 ), NULL, 3 );	// ĳ���͸� �������
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1229 ), NULL, 4 );	// ���뺰
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1230 ), NULL, 5 );	// ������
	m_pCharacterListBoxSortComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1231 ), NULL, 6 );	// �����ֱ� ������ �������
	CDnLoginTask* pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( pLoginTask )
		m_pCharacterListBoxSortComboBox->SetSelectedByValue( pLoginTask->GetCharacterListSortCommand() );	// default
#endif // PRE_CHARLIST_SORTING
}

void CDnCharSelectListDlg::Process( float fElapsedTime )
{
	if(!IsShow()) return;

#ifdef PRE_ADD_DWC
	// DWC ĳ�� ���� ��ư
	CEtUIButton* pDWCButton = GetControl<CEtUIButton>("ID_BT_DWC");
	if(pDWCButton)
	{
		bool bIsDWCSession = GetDWCTask().IsDWCRankSession();
		pDWCButton->Show(bIsDWCSession);
	}	
#endif // PRE_ADD_DWC

	CEtUIDialog::Process( fElapsedTime );
}

void CDnCharSelectListDlg::SetCharacterList( std::vector<TCharListData>& vecCharListData, int nMaxCharCount )
{
//	int nBaseCreateCharCount = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Base_CreateCharCount );
	int nMaxCreateCharCount = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Login_Max_CreateCharCount );
#ifdef PRE_ADD_DWC
	for(int i = 0 ; i < (int)vecCharListData.size() ; ++i)
	{
		if(vecCharListData[i].cAccountLevel == eAccountLevel::AccountLevel_DWC )
		{
			nMaxCreateCharCount++;
			break;
		}
	}
#endif // PRE_ADD_DWC

	if( nMaxCreateCharCount < nMaxCharCount || nMaxCharCount < static_cast<int>( vecCharListData.size() ) )
		return;

#ifdef PRE_ADD_NEWCOMEBACK
	m_vecCharListData = vecCharListData;
#endif // PRE_ADD_NEWCOMEBACK

	int nCreatableCount = nMaxCharCount - static_cast<int>( vecCharListData.size() );

	m_pCharacterListBox->RemoveAllItems();

	// ĳ���� ���� ����
	std::vector<TCharListData>::iterator iter = vecCharListData.begin();
	for( ; iter != vecCharListData.end(); iter++ )
	{
		CDnCharacterInfoDlg* pItem = m_pCharacterListBox->AddItem<CDnCharacterInfoDlg>();
		if( pItem )
		{
			if( (*iter).bDeleteFlag )
				pItem->SetCharacterInfo( SLOT_DELETE_WAIT, &(*iter), m_pCharacterListBox->GetSize() - 1 );
			else
				pItem->SetCharacterInfo( SLOT_PLAYER, &(*iter), m_pCharacterListBox->GetSize() - 1 );
		}
	}

	// �� ���� ����
	for( int i=0; i<nCreatableCount; i++ )
	{
		CDnCharacterInfoDlg* pItem = m_pCharacterListBox->AddItem<CDnCharacterInfoDlg>();
		if( pItem )
			pItem->SetCharacterInfo( SLOT_ENABLE, NULL, m_pCharacterListBox->GetSize() - 1 );
	}

	// ���� �Ұ� ���� ����
	int nSlotCount = m_pCharacterListBox->GetSize();
	int nDisableSlotCount = nMaxCreateCharCount - nSlotCount;
	if( nDisableSlotCount > 0 )
	{
		for( int i=0; i<nDisableSlotCount; i++ )
		{
			CDnCharacterInfoDlg* pItem = m_pCharacterListBox->AddItem<CDnCharacterInfoDlg>();
			if( pItem )
				pItem->SetCharacterInfo( SLOT_DISABLE, NULL, m_pCharacterListBox->GetSize() - 1 );
		}
	}

	WCHAR wszStr[256];
	swprintf_s( wszStr, L"%d / %d", static_cast<int>( vecCharListData.size() ), nMaxCharCount );
	m_pStaticCharCount->SetText( wszStr );
}

void CDnCharSelectListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{

		
		//RLKT_TEST add dark class
		if( IsCmdControl( "ID_BT_OK" ) ) 
		{
			pLoginTask->ChangeState( CDnLoginTask::CharCreate_SelectClass );
#ifdef PRE_ADD_DWC
			GetInterface().SetDWCCharSelect(false);
#endif
			return;
		}else if( IsCmdControl( "ID_BT_DARK" ) )  //dark avenger+silver //rlkt_test
		{
			pLoginTask->ChangeState( CDnLoginTask::CharCreate_SelectClassDark ); //aici
			return;
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) ) 
		{
			if( pLoginTask->GetSelectActorSlot() == -1 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_11, MB_OK, MESSAGEBOX_11, pLoginTask );
				return;
			}

			GetInterface().MessageBox( MESSAGEBOX_6, MB_YESNO, MESSAGEBOX_6, this, false, true );
			return;
		}
#if defined(PRE_ADD_23829)
		else if( IsCmdControl( "ID_BT_PASSCREATE" ) ) 
		{
			GetInterface().OpenSecurityCreateDlg();
			return;
		}
#endif // PRE_ADD_23829
#if defined(PRE_ADD_DWC)
		else if( IsCmdControl("ID_BT_DWC") )
		{
			std::vector<TCharListData> vCharList = pLoginTask->GetMyCharListData();
			int	nSize = (int)vCharList.size();
			
			//bool bHaveHighLevelChar = false;
			//for(int i = 0 ; i < nSize ; ++i)
			//{
			//	if(vCharList[i].cAccountLevel == eAccountLevel::AccountLevel_DWC)
			//		continue;

			//	if(vCharList[i].cLevel >= GetDWCTask().GetDWCCharCreateLvLimit())
			//	{
			//		bHaveHighLevelChar = true;
			//		break;
			//	}
			//}

			//// DWCĳ���� �����ϱ� ���ؼ�, ������ ������ ����ĳ���� �Ѱ��� �־����.
			//if(vCharList.empty() || !bHaveHighLevelChar)
			//{
			//	GetInterface().MessageBox(120260); // mid: DWC ĳ���͸� �������, ���� ĳ���Ͱ� �ּ� �Ѱ� �̻� �־�� �մϴ�.
			//	return;
			//}

			// DWCĳ���� �Ѱ��� ������ �������� �ʴ´�
			for(int i = 0 ; i < nSize ; ++i)
			{				
				if( vCharList[i].cAccountLevel == AccountLevel_DWC )
				{
					GetInterface().MessageBox(120259); // mid: �̹� DWC ĳ���Ͱ� �����Ǿ� �ֽ��ϴ�.
					return;
				}
			}

			pLoginTask->ChangeState( CDnLoginTask::CharCreate_DWC );
			return;
		}
#endif

	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( strstr( pControl->GetControlName(), "ID_LISTBOXEX_LIST" ) )
		{
			for( int i=0; i<m_pCharacterListBox->GetSize(); i++ )
			{
				CDnCharacterInfoDlg* pCharacterInfoDlg = m_pCharacterListBox->GetItem<CDnCharacterInfoDlg>( i );
				if( pCharacterInfoDlg )
					pCharacterInfoDlg->SetSelect( false );
			}

			int nSelectIndex = m_pCharacterListBox->GetSelectedIndex();

			pLoginTask->SetCharSelect( nSelectIndex );
#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
			GetInterface().ResetPreviewCostumeRadioButton();
#endif // PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

#ifdef PRE_ADD_DWC
			CDnLoginTask* pLoginTask = static_cast<CDnLoginTask *>(CTaskManager::GetInstance().GetTask("LoginTask"));
			if(pLoginTask && CDnDWCTask::IsActive())
			{
				std::vector<TCharListData> vCharList = pLoginTask->GetMyCharListData();
				if(!vCharList.empty())
				{
					if( nSelectIndex >= 0 && nSelectIndex < (int)vCharList.size())
					{
						bool bResult = (vCharList[nSelectIndex].cAccountLevel == AccountLevel_DWC);
						GetDWCTask().SetDWCChar(bResult);
					}
				}
			}
#endif // PRE_ADD_DWC

			CDnCharacterInfoDlg* pCharacterInfoDlg = m_pCharacterListBox->GetItem<CDnCharacterInfoDlg>( nSelectIndex );
			CEtUIButton* pCharacterDeleteButton = GetControl<CEtUIButton>( "ID_BT_CANCEL" );
			if( pCharacterInfoDlg && pCharacterDeleteButton )
			{
				if( pCharacterInfoDlg->GetCharacterSlotType() == SLOT_PLAYER )
					pCharacterDeleteButton->Enable( true );
				else
					pCharacterDeleteButton->Enable( false );

				pCharacterInfoDlg->SetSelect( true );
			}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
			UpdateTotalLevelInfo(nSelectIndex);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
		}
	}
#ifdef PRE_CHARLIST_SORTING
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser )
	{
		if( IsCmdControl( "ID_COMBOBOX_SORT" ) && m_pCharacterListBoxSortComboBox->IsOpenedDropDownBox() == false )
		{
			SComboBoxItem* pItem = m_pCharacterListBoxSortComboBox->GetSelectedItem();
			if( pItem )
			{
				m_pCharacterListBoxSortComboBox->ClearText();
				m_pCharacterListBoxSortComboBox->SetText( pItem->strText );
				pLoginTask->SortCharacterList( (CDnLoginTask::eCharacterListSortCommand)pItem->nValue );
			}
		}
	}
#endif // PRE_CHARLIST_SORTING
}

void CDnCharSelectListDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED && nID == MESSAGEBOX_6 )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			//2�� ������ ������ 
			CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask( "AuthTask" );
			if( pAuthTask )
			{
				if( pAuthTask->GetSecondAuthPW() )
				{
					GetInterface().OpenSecurityCheckDlg( CDnInterface::InterfaceTypeEnum::Login , 1 );
					return; 
				}
				else 
				{
					CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
					if( pTask ) 
					{
						int nValue[4] = { 0, };
						pTask->SetAuthPassword( 0, nValue );
						pTask->DeleteCharacter();
					}
				}
			}
		}
	}

	switch( nID ) 
	{	
#ifdef PRE_ADD_COMEBACK
	case 1:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) {
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
				}
			}
		}
		break;
#endif

#ifdef PRE_ADD_NEWCOMEBACK
	case 7741:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 )
					GetInterface().OpenSecurityCheckDlg( CDnInterface::InterfaceTypeEnum::Login , 0 );					
			}
		}
		break;
#endif // PRE_ADD_NEWCOMEBACK

	default:
		break;
	}
}

#if defined(PRE_ADD_23829)
void CDnCharSelectListDlg::Show2ndPassCreateButton( bool bShow )
{
	CEtUIButton *pButton = GetControl<CEtUIButton>( "ID_BT_PASSCREATE" );
	if( pButton )
	{
		pButton->Show( bShow );
	}
}
#endif // PRE_ADD_23829

void CDnCharSelectListDlg::SelectCharIndex( int nSelectIndex )
{
	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask )
		return;

	if( m_pCharacterListBox == NULL || nSelectIndex >= m_pCharacterListBox->GetSize() )
		return;

	m_pCharacterListBox->SelectItem( nSelectIndex );
	pLoginTask->SetCharSelect( nSelectIndex );
}

bool CDnCharSelectListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pTask ) 
		return false;

	if( uMsg == WM_LBUTTONDBLCLK )
	{
		if( !IsMouseInDlg() ) return false;
		POINT MousePoint;
		SUICoord uiCoordsBase[2];

		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		fMouseX = MousePoint.x / GetScreenWidth();
		fMouseY = MousePoint.y / GetScreenHeight();

		int nSelectIndex = m_pCharacterListBox->GetSelectedIndex();
		CDnCharacterInfoDlg* pCharacterInfoDlg = m_pCharacterListBox->GetItem<CDnCharacterInfoDlg>( nSelectIndex );
		if( pCharacterInfoDlg )
		{
			SUICoord uiCoord;
			pCharacterInfoDlg->GetDlgCoord( uiCoord );
			uiCoord.fWidth -= 0.01f;
			if( uiCoord.IsInside( fMouseX, fMouseY ) ) 
			{
				if( pTask->GetSelectActorSlot() == -1 )
				{
					GetInterface().MessageBox( MESSAGEBOX_11, MB_OK, MESSAGEBOX_11, pTask );
					return false;
				}

				if( pTask->CheckSelectCharDeleteWait() )
					return false;
#if defined(PRE_ADD_DWC)
				if(GetDWCTask().IsDWCRankSession() == false && GetDWCTask().IsDWCChar() ) // DWC������ �ƴ϶��
				{
					// mid: �ݷδ�ȸ �Ⱓ�� �ƴմϴ�. �ش� ĳ���� ����Ҽ������ϴ�.
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120270));
					return false;
				}
#endif
		
#ifdef PRE_ADD_NEWCOMEBACK
				// ��ȯ���� ���.
				CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
									
				int selectedIdx = m_pCharacterListBox->GetSelectedIndex();
				if( pTask && pTask->GetComeback() &&
					selectedIdx < (int)m_vecCharListData.size() 
#if defined(PRE_ADD_DWC)
					&& GetDWCTask().IsDWCChar() == false
#endif // PRE_ADD_DWC
					)
					//selectedIdx < (int)m_vecCharListData.size() && m_vecCharListData[ selectedIdx ].bCombackEffectItem )				
				{	
					if( m_pCharacterListBox && ValidReward() )
					{						
						GetInterface().OpenComebackRewardDlg( true );
						GetInterface().SelectedCharIndex( m_pCharacterListBox->GetSelectedIndex() );
					}
					else
					{
						GetInterface().MessageBox( 7741, MB_OKCANCEL, 7741, this ); // "�ش� ĳ���ʹ� ��ȯ�� ���� ���� ����� �ƴϱ� ������, ������ ���� ���� �� �����ϴ�."
					}										
				}

#ifdef PRE_ADD_GAMEQUIT_REWARD
				// �������� �����Ӻ������.
				else if( pTask && pTask->GetAccountRegTime() && pTask->GetNewbieReward() )
				{												
					GetInterface().ShowGameQuitRewardComboDlg( true );
					GetInterface().SelectedGameQuitRewardCharIndex( m_pCharacterListBox->GetSelectedIndex() );								
				}
#endif // PRE_ADD_GAMEQUIT_REWARD

				// �Ϲ�����.
				else
				{
					GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
				}
#else

	#ifdef PRE_ADD_COMEBACK
					CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
					if( pTask->GetComeback() )
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7740 ), MB_OKCANCEL, 1, this ); // "���� �� ĳ���ͷ� ���� �����ðڽ��ϱ�? (�����Ͻø� ������ �Ұ����մϴ�. �����ϰ� �������ּ���.)"						
					else
						GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
	#else
					GetInterface().OpenSecurityCheckDlg(CDnInterface::InterfaceTypeEnum::Login , 0 );
	#endif // PRE_ADD_COMEBACK

#endif // PRE_ADD_NEWCOMEBACK

				return true;
			}
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
void CDnCharSelectListDlg::UpdateTotalLevelInfo(int nSelectIndex)
{
	int nTotalLevel = 0;

	CDnCharacterInfoDlg* pSelectedInfoDlg = m_pCharacterListBox->GetItem<CDnCharacterInfoDlg>(nSelectIndex);
	int nSelectedWorldID = pSelectedInfoDlg ? pSelectedInfoDlg->GetWorldID() : -1;

	if (pSelectedInfoDlg)
	{
		int nListCount = m_pCharacterListBox->GetSize();
		for (int i = 0; i < nListCount; ++i)
		{
			CDnCharacterInfoDlg* pInfoDlg = m_pCharacterListBox->GetItem<CDnCharacterInfoDlg>(i);
			if (pInfoDlg && nSelectedWorldID == pInfoDlg->GetWorldID())
			{
#ifdef PRE_ADD_DWC
				if(pInfoDlg->IsDWCChar())
					continue;
#endif
				nTotalLevel += pInfoDlg->GetCharLevel();
			}			
		}
	}


	WCHAR wszTotalLevelInfo[128] = {0, };
	_snwprintf_s(wszTotalLevelInfo, _countof(wszTotalLevelInfo), _TRUNCATE, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1832 ), nTotalLevel);
	if (m_pTotalLevelInfo)
		m_pTotalLevelInfo->SetText(wszTotalLevelInfo);
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_NEWCOMEBACK
int CDnCharSelectListDlg::GetSelectCharIndex()
{
	if( m_pCharacterListBox )
		return m_pCharacterListBox->GetSelectedIndex();
	return 0;
}

bool CDnCharSelectListDlg::ValidReward()
{
	int nCharIndex = m_pCharacterListBox->GetSelectedIndex();

	if( m_vecCharListData.empty() || (int)m_vecCharListData.size() <= nCharIndex )
		return false;

	TCharListData & aCharData = m_vecCharListData[ nCharIndex ];

	// LevelupEventTable.
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TLEVELUPEVENT ); 
	if( !pTable )
		return false;

	DNTableCell * pCell = NULL;

	bool bRes = false;

	int size = pTable->GetItemCount();

	// LevelupEventTable //	
	for( int i=0; i<size; ++i )
	{
		int nID = pTable->GetItemID( i );

		pCell = pTable->GetFieldFromLablePtr( nID, "_EventType" );
		if( !pCell || ( pCell && pCell->GetInteger() != 5 ) )
			continue;

		pCell = pTable->GetFieldFromLablePtr( nID, "_PCLevel" );
		if( pCell && pCell->GetInteger() != aCharData.cLevel )
		{
			bRes = true;
			break;
		}
	}

	return bRes;
}

#endif // PRE_ADD_NEWCOMEBACK






//////////////////////////////////////////////////////////////////////////
// class CDnCharacterInfoDlg

CDnCharacterInfoDlg::CDnCharacterInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
, m_pStaticLevel( NULL )
, m_pStaticName( NULL )
, m_pStaticServer( NULL )
, m_pStaticVillage( NULL )
, m_pStaticJobIcon( NULL )
, m_pStaticBackground( NULL )
, m_pStaticCreate( NULL )
, m_pStaticCreateText( NULL )
, m_pStaticDelete( NULL )
, m_pStaticDeleteText( NULL )
, m_pStaticDeleteTime( NULL )
, m_pButtonDeleteCancel( NULL )
, m_pStaticDisable( NULL )
, m_nSlotIndex( -1 )
, m_eSlotType( SLOT_DISABLE )
, m_ExpectTime( 0 )
, m_cJob( 0 )
, m_cAccountLevel( 0 )
#ifdef PRE_ADD_NEWCOMEBACK
, m_pStaticComebackLight( NULL )
, m_pStaticComebackTooltip( NULL )
#endif // PRE_ADD_NEWCOMEBACK
#ifdef PRE_ADD_DWC
, m_pStaticDWCBoard(NULL)
, m_pStaticDWCToolTip(NULL)
#endif // PRE_ADD_DWC
{
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_nCharLevel = 0;
	m_nWorldID = -1;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}

CDnCharacterInfoDlg::~CDnCharacterInfoDlg( void )
{
}

void CDnCharacterInfoDlg::Initialize( bool bShow )
{
#ifdef _ADD_NEWLOGINUI
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charselectlistdlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_characterListDlg.ui" ).c_str(), bShow );
#endif
}

void CDnCharacterInfoDlg::InitialUpdate()
{
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_LV" );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );
	m_pStaticServer = GetControl<CEtUIStatic>( "ID_TEXT_SERVER" );
	m_pStaticVillage = GetControl<CEtUIStatic>( "ID_TEXT_VILLAGE" );
	m_pStaticJobIcon = GetControl<CDnJobIconStatic>( "ID_STATIC_CLASS" );
	m_pStaticBackground = GetControl<CEtUIStatic>( "ID_STATIC_NAMEBAR" );
	m_pStaticCreate = GetControl<CEtUIStatic>( "ID_STATIC_CREATE" );
	m_pStaticCreateText = GetControl<CEtUIStatic>( "ID_TEXT_CREATE" );
	m_pStaticDelete = GetControl<CEtUIStatic>( "ID_STATIC_DELETE" );
	m_pStaticDeleteText = GetControl<CEtUIStatic>( "ID_TEXT_DELETE" );
	m_pStaticDeleteTime = GetControl<CEtUIStatic>( "ID_TEXT_DELETECOUNT" );
	m_pButtonDeleteCancel = GetControl<CEtUIButton>( "ID_BT_CANCELDELETE" );
	m_pStaticDisable = GetControl<CEtUIStatic>( "ID_STATIC_COVER" );

// #78053 ��ȯ�� �������� ������.
#ifdef PRE_ADD_NEWCOMEBACK
	m_pStaticComebackLight = GetControl<CEtUIStatic>( "ID_STATIC_RETURNBOARD" );
	m_pStaticComebackTooltip = GetControl<CEtUIStatic>( "ID_STATIC_RETURN" );
#else
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>( "ID_STATIC_RETURNBOARD" );
	if( pStatic )
		pStatic->Show( false );
	pStatic = GetControl<CEtUIStatic>( "ID_STATIC_RETURN" );
	if( pStatic )
		pStatic->Show( false );
#endif // PRE_ADD_NEWCOMEBACK

	EtColor color( m_pStaticJobIcon->GetTextureColor() );
	color.a = 0.5f;
	m_pStaticJobIcon->SetTextureColor( color, 0 );

#ifdef PRE_ADD_DWC
	m_pStaticDWCBoard = GetControl<CEtUIStatic>("ID_STATIC_DWCBOARD");
	m_pStaticDWCBoard->Show(false);


#endif // PRE_ADD_DWC
	m_pStaticDWCToolTip = GetControl<CEtUIStatic>("ID_STATIC_DWC_TOOLTIP");
	m_pStaticDWCToolTip->Show(false);

	ResetControl();
}

void CDnCharacterInfoDlg::SetCharacterInfo( eCharacterSlotType eSlotType, TCharListData* pTCharListData, int nSlotIndex )
{
	m_eSlotType = eSlotType;
	m_nSlotIndex = nSlotIndex;

	if( eSlotType != SLOT_ENABLE && eSlotType != SLOT_DISABLE && pTCharListData == NULL )
		return;
		
	ResetControl();
	switch( eSlotType )
	{
		case SLOT_PLAYER:
			{
				wchar_t wszTemp[ 256 ] = { 0 };
				swprintf_s( wszTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), pTCharListData->cLevel );
				m_pStaticLevel->SetText( wszTemp );
				m_pStaticLevel->Show( true );
				m_pStaticName->SetText( pTCharListData->wszCharacterName );
				m_pStaticName->Show( true );
				m_pStaticServer->SetText( CGlobalInfo::GetInstance().GetServerName( pTCharListData->cWorldID ) );
				m_pStaticServer->Show( true );

				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
				if( !pSox ) return;

				int nMapNameID = 0;
				if( pTCharListData->nMapID > 0 )
					nMapNameID = pSox->GetFieldFromLablePtr( pTCharListData->nMapID, "_MapNameID" )->GetInteger();
				else
					nMapNameID = 1305004;

				m_pStaticVillage->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMapNameID ) );
				m_pStaticVillage->Show( true );
				m_pStaticJobIcon->SetIconID( pTCharListData->cJob );
				m_pStaticJobIcon->Show( true );
				m_pStaticBackground->Show( true );
				m_cJob = pTCharListData->cJob;
				m_pStaticJobIcon->SetTooltipText( DN_INTERFACE::STRING::GetClassString( m_cJob ) );

				//[_debug]  ��ʱȥ��DWC1
#if 0
				m_cAccountLevel = pTCharListData->cAccountLevel;
#endif


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
				SetCharLevel(pTCharListData->cLevel);
				SetWorldID((int)pTCharListData->cWorldSetID);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_NEWCOMEBACK
				m_pStaticComebackLight->Show( pTCharListData->bCombackEffectItem );
				m_pStaticComebackTooltip->Show( pTCharListData->bCombackEffectItem );
				EnableComebackUI( pTCharListData->bCombackEffectItem );
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC				
				bool bIsDWCChar = (pTCharListData->cAccountLevel == AccountLevel_DWC);
				m_pStaticDWCBoard->Show(bIsDWCChar);				
				m_pStaticDWCToolTip->Show(bIsDWCChar);				
#endif // PRE_ADD_DWC
			}
			break;
		case SLOT_DELETE_WAIT:
			{
				wchar_t wszTemp[ 256 ] = { 0 };
				swprintf_s( wszTemp, 256, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), pTCharListData->cLevel );
				m_pStaticLevel->SetText( wszTemp );
				m_pStaticLevel->Show( true );
				m_pStaticName->SetText( pTCharListData->wszCharacterName );
				m_pStaticName->Show( true );
				m_pStaticJobIcon->SetIconID( pTCharListData->cJob );
				m_pStaticJobIcon->Show( true );

				m_pStaticDelete->Show( true );
				SetRemainTime( pTCharListData->DeleteDate );
				m_pButtonDeleteCancel->Show( true );
				m_pStaticDeleteText->Show( true );
				m_pStaticDeleteTime->Show( true );
				m_cJob = pTCharListData->cJob;
				m_pStaticJobIcon->SetTooltipText( DN_INTERFACE::STRING::GetClassString( m_cJob ) );

				//[_debug]  ��ʱȥ��DWC2
#if 0
				m_cAccountLevel = pTCharListData->cAccountLevel;
#endif


#ifdef PRE_ADD_NEWCOMEBACK
				m_pStaticComebackLight->Show( pTCharListData->bCombackEffectItem );
				m_pStaticComebackTooltip->Show( pTCharListData->bCombackEffectItem );
				EnableComebackUI( pTCharListData->bCombackEffectItem );
#endif // PRE_ADD_NEWCOMEBACK

			}
			break;
		case SLOT_ENABLE:
			{
				m_pStaticCreate->Show( true );
				m_pStaticCreateText->Show( true );
			}
			break;
		case SLOT_DISABLE:
			{
				m_pStaticDisable->Show( true );
			}
			break;
	}
}

void CDnCharacterInfoDlg::ResetControl()
{
	m_pStaticLevel->Show( false );
	m_pStaticName->Show( false );
	m_pStaticServer->Show( false );
	m_pStaticVillage->Show( false );
	m_pStaticJobIcon->Show( false );
	m_pStaticBackground->Show( false );
	m_pStaticCreate->Show( false );
	m_pStaticCreateText->Show( false );
	m_pStaticDelete->Show( false );
	m_pStaticDeleteText->Show( false );
	m_pStaticDeleteTime->Show( false );
	m_pButtonDeleteCancel->Show( false );
	m_pStaticDisable->Show( false );
	m_cJob = 0;

#ifdef PRE_ADD_NEWCOMEBACK
	m_pStaticComebackLight->Show( false );
    m_pStaticComebackTooltip->Show( false );
#endif // PRE_ADD_NEWCOMEBACK
}

void CDnCharacterInfoDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	if( !IsShow() ) return;

	if( m_eSlotType == SLOT_DELETE_WAIT )
		UpdateDeleteRemainTime();

/*	if( m_cJob > 0 )
	{
		float fMouseX, fMouseY;
		GetScreenMouseMovePoints( fMouseX, fMouseY );
		fMouseX -= GetXCoord();
		fMouseY -= GetYCoord();
		bool bMouseEnter(false);
		SUICoord uiCoord;
		m_pStaticJobIcon->GetUICoord( uiCoord );
		if( uiCoord.IsInside( fMouseX, fMouseY ) )
		{
			CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
			if( pSimpleTooltipDlg )
			{
				pSimpleTooltipDlg->ShowTooltipDlg( m_pStaticJobIcon, true, DN_INTERFACE::STRING::GetClassString( m_cJob ), 0xffffffff, true );
			}
		}
	}*/
}

void CDnCharacterInfoDlg::SetRemainTime( __time64_t RemainTime )
{
	m_ExpectTime = RemainTime;
	UpdateDeleteRemainTime();
}

void CDnCharacterInfoDlg::UpdateDeleteRemainTime()
{


	/////////////////////////////////////////////////////
	//[_debug5]  ��ɫɾ��ʱ�������ʱ�ɵ�

#if 1
	tm *ptmTemp = localtime( &m_ExpectTime );
#else
	tm *ptmTemp = NULL;
#endif

	/////////////////////////////////////////////////////

	if( ptmTemp == NULL ) 
	{
		m_pStaticDeleteTime->SetText( L"" );
		return;
	}
	
	__time64_t CurTime;
	time( &CurTime );

	tm ExpectTime = *localtime( &m_ExpectTime );
	tm CurrentTime = *localtime( &CurTime );

	__int64 nExpectTime = ( GetDateValue( 1900 + ExpectTime.tm_year, ExpectTime.tm_mon + 1, ExpectTime.tm_mday ) * 24 * 60 ) + ( ExpectTime.tm_hour * 60 ) + ExpectTime.tm_min;
	__int64 nCurrentTime = ( GetDateValue( 1900 + CurrentTime.tm_year, CurrentTime.tm_mon + 1, CurrentTime.tm_mday ) * 24 * 60 ) + ( CurrentTime.tm_hour * 60 ) + CurrentTime.tm_min;
	__int64 nRemainTime = nExpectTime - nCurrentTime;
	if( nRemainTime < 0 ) 
	{
		m_pStaticDeleteTime->SetText( L"" );
		return;
	}

	WCHAR wszTemp[256];
	if( nRemainTime >= 60 * 24 ) 
	{
		int nDay = (int)( nRemainTime / ( 60 * 24 ) );
		int nHour = (int)( ( nRemainTime % ( 60 * 24 ) ) / 60 );
		int nMin = (int)( ( nRemainTime % ( 60 * 24 ) )  % 60 );
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 116 ), nDay, nHour, nMin );
	}
	else 
	{
		int nHour = (int)( nRemainTime / 60 );
		int nMin = (int)( nRemainTime % 60 );
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1986 ), nHour, nMin );
	}

	m_pStaticDeleteTime->SetText( wszTemp );
}

void CDnCharacterInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BT_CANCELDELETE" ) ) 
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 113 ), MB_YESNO, m_nDialogID, this );
		}
	}
}

void CDnCharacterInfoDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) ) 
		{
			CDnLoginTask* pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask )
				pTask->ReviveCharacter( m_nSlotIndex );
			m_pButtonDeleteCancel->Enable( false );
		}
	}
}

void CDnCharacterInfoDlg::SetSelect( bool bSelect )
{
#ifdef PRE_ADD_NEWCOMEBACK
	bool bShow = ( bSelect && IsComebackUser() );
	GetInterface().SetComebackUser( bShow );
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	bShow = ( bSelect && IsDWCChar() );
	GetInterface().SetDWCCharSelect( bShow );
#endif // PRE_ADD_DWC

	if( m_pStaticJobIcon == NULL )
		return;

	EtColor color( m_pStaticJobIcon->GetTextureColor() );
	if( bSelect )
	{
		color.a = 1.0f;
	}
	else
	{
		color.a = 0.5f;
	}

	m_pStaticJobIcon->SetTextureColor( color, 0 );
}


#ifdef PRE_ADD_NEWCOMEBACK
bool CDnCharacterInfoDlg::IsComebackUser()
{
	if( m_pStaticComebackLight )
		return m_pStaticComebackLight->IsShow();

	return false;
}

void CDnCharacterInfoDlg::EnableComebackUI( bool bComeback )
{
	if( bComeback )
	{
		char * strUI[32] = { "ID_STATIC_NAMEBAR", "ID_STATIC_CREATE", "ID_STATIC_DELETE" };
		CEtUIStatic * pStatic = NULL;
		for( int i=0; i<3; ++i )
		{
			pStatic = GetControl<CEtUIStatic>( strUI[i] );
			if( pStatic )
				pStatic->Show( false );
		}		
	}
}

#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
bool CDnCharacterInfoDlg::IsDWCChar()
{
	bool bIsDWC = false;
	if( m_cAccountLevel == eAccountLevel::AccountLevel_DWC )
		bIsDWC = true;

	return bIsDWC;
}
#endif

#endif // PRE_MOD_SELECT_CHAR

bool CDnCharSelectListDlg::IsNewCharacter()
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TCHARCREATE);
	if (!pSox) return false;

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		int NewBanner = pSox->GetFieldFromLablePtr(i, "_NewBanner")->GetInteger();
		if (NewBanner == 1)
		{
			return true;
		}
	}

	return false;
}