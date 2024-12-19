#include "StdAfx.h"
#include "DnNpcReputationAlbumDlg.h"
#include "DnNpcReputationDlg.h"
#include "DnNpcReputationBromideDlg.h"
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "ReputationSystemRepository.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_REPUTATION_EXPOSURE
#include "DnSimpleTooltipDlg.h"
#endif

const int REPUTATION_ALBUM_PORTRAIT_SIZE = 118;


CDnNpcReputationAlbumDlg::CDnNpcReputationAlbumDlg( UI_DIALOG_TYPE dialogType/* = UI_TYPE_FOCUS*/, CEtUIDialog *pParentDialog/* = NULL*/, 
											   int nID/* = -1*/, CEtUICallback *pCallback/* = NULL*/ ) 
											   : CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true ),
											   m_iNowPage( 0 ),
											   m_iMaxPage( 0 ),
											   m_pComboSort( NULL ),
											   m_pStaticPage( NULL ),
											   m_pBtnPrevPage( NULL ),
											   m_pBtnNextPage( NULL ),
											   m_iNowSelectedSortMethod( NPC_SORT_DEFAULT ),
											   m_pBromideDlg( NULL ),
											   m_bInitialized( false ),
											   m_bShowFromBromide( false )
{

}

CDnNpcReputationAlbumDlg::~CDnNpcReputationAlbumDlg( void )
{
	SAFE_RELEASE_SPTR( m_hPortraitTexture );
#ifdef PRE_ADD_REPUTATION_EXPOSURE
	SAFE_RELEASE_SPTR( m_hGrayScalePortraitTexture );
#endif
}

void CDnNpcReputationAlbumDlg::Initialize( bool bShow )
{
#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "Tab_ReputeAlbumDlg.ui" ).c_str(), bShow );
#else
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "ReputeAlbumDlg.ui" ).c_str(), bShow );
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT

	SAFE_RELEASE_SPTR( m_hPortraitTexture );
	m_hPortraitTexture = LoadResource( "Repute_Album.dds", RT_TEXTURE, true );

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	SAFE_RELEASE_SPTR( m_hGrayScalePortraitTexture );
	m_hGrayScalePortraitTexture = LoadResource( "Repute_GrayAlbum.dds", RT_TEXTURE, true );
#endif
}

void CDnNpcReputationAlbumDlg::InitialUpdate( void )
{
	// 앨범 텍스쳐 컨트롤 및 이름 static 컨트롤..
	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < NPC_REPUTATION_ALBUM_COUNT_PER_PAGE; ++i )
	{
		sprintf_s( acBuffer, "ID_TEXTUREL_PICTURE%d", i );
		m_aElements[ i ].pPictureCtrl = GetControl<CEtUITextureControl>( acBuffer );

		sprintf_s( acBuffer, "ID_CLICK%d", i );
		m_aElements[ i ].pStaticEventReceiver = GetControl<CEtUIStatic>( acBuffer );
		m_aElements[ i ].pStaticEventReceiver->SetButton( true );

		sprintf_s( acBuffer, "ID_TEXT_NAME%d", i );
		m_aElements[ i ].pName = GetControl<CEtUIStatic>( acBuffer );

		m_aElements[ i ].pPictureCtrl->Show( true );
		m_aElements[ i ].pName->Show( true );
	}

	// 정렬관련 콤보박스 추가.
#ifdef PRE_MOD_REPUTE_NOMALICE2
	m_pComboSort = GetControl<CEtUIComboBox>( "ID_COMBOBOX_SORT" );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3209), NULL, NPC_SORT_DEFAULT );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3210), NULL, NPC_SORT_FAVOR );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3212), NULL, NPC_SORT_NAME );
#else
	m_pComboSort = GetControl<CEtUIComboBox>( "ID_COMBOBOX_SORT" );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3209), NULL, 0 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3210), NULL, 1 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3211), NULL, 2 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3212), NULL, 3 );
#endif

	// 페이지 관련 컨트롤들..
	m_pStaticPage = GetControl<CEtUIStatic>( "ID_BT_PAGE" );
	m_pBtnPrevPage = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pBtnNextPage = GetControl<CEtUIButton>( "ID_BT_NEXT" );

	m_bInitialized = true;

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	m_pStaticAchievePercent = GetControl<CEtUIStatic>("ID_STATIC_COUNT");
#endif

#ifdef PRE_MOD_CORRECT_UISOUND_PLAY
	m_DlgInfo.bSound = false;
#endif 
}

void CDnNpcReputationAlbumDlg::Show( bool bShow )
{
	if( IsShow() == bShow )
		return;

	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		if( false == m_bShowFromBromide )
		{
			m_iNowPage = 0;
			m_iNowSelectedSortMethod = NPC_SORT_DEFAULT;
		}

#ifdef PRE_MOD_REPUTE_NOMALICE2
		m_pComboSort->SetSelectedByValue( m_iNowSelectedSortMethod );
#else
		m_pComboSort->SetSelectedByIndex( m_iNowSelectedSortMethod );
#endif

		// SetSelectedByIndex() 함수를 호출하면 ProcessCommand() 함수 내부에서 호출됨..
		//_UpdateAvailNpcList( m_iNowSelectedSortMethod );
		//RefreshToCurrentPage();
	}
	else
	{
		// 숨겨질때는 브로마이드에서 앨범으로 돌아왔다는 플래그를 초기화.
		m_bShowFromBromide = false;
	}
}

void CDnNpcReputationAlbumDlg::_UpdateAvailAlbumList( int iSortMethod )
{
	m_vlAvailAlbumInfos.clear();
	m_vlPortrait.clear();
	m_vlBromideFileName.clear();
	m_vlReputeTableID.clear();
#ifdef PRE_ADD_REPUTATION_EXPOSURE 
	m_vbIsFoundAlbum.clear();
	m_vIBromideRepute.clear();
	m_vIBromideQuestID.clear();
	m_vIBromideQuestEnumID.clear();
#endif

	// 현재 오픈된 npc 호감도의 정보를 모아둔다.
	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if(!pQuestTask) 
		return;

	DNTableFileFormat* pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	if(!pReputeTable)
		return;

	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if(pReputationRepos)
	{
		for( int iIndex = 0; iIndex < pReputeTable->GetItemCount(); ++iIndex )
		{
			// 퀘스트 클리어된 npc 들만 호감도 처리되므로 모아둔다.
			int iReputeTableItemID = pReputeTable->GetItemID( iIndex );

			// 브로마이드의 갯수가 최대 3개임. 텍스쳐 이름이 있는 경우에 추가해준다.
			S_AVAILALBUM_INFO AvailAlbumInfo;
			AvailAlbumInfo.iReputeTableID = iReputeTableItemID;

			char acBuffer[ 256 ] = { 0 };
			for( int k = 0; k < 3; ++k )
			{			

#ifdef PRE_ADD_REPUTATION_EXPOSURE
				bool bPossibleShowAlbum = false;
				
				sprintf_s( acBuffer, "_BromideQuest%d", k+1 );
				int iOpenQuestID = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetInteger();

				sprintf_s( acBuffer, "_BromideQuestName%d", k+1 );
				int iOpenQuestEnumID = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetInteger();

				sprintf_s( acBuffer, "_BromideS%d", k+1 );
				int iPortraitTextureIndex = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetInteger();

				sprintf_s( acBuffer, "_Bromide%d", k+1 );
				const char* pBromideTextureFileName = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetString();

				sprintf_s( acBuffer, "_BromideRepute%d", k+1 );
				int nBromideRepute = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID, acBuffer)->GetInteger();

				// 내 호감도 불러오기.
				int nNpcID = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID , "_NpcID")->GetInteger();
				int nCurrentMyRepute = (int)pReputationRepos->GetNpcReputation( nNpcID, IReputationSystem::NpcFavor );
				if( nBromideRepute > 0 && nBromideRepute < nCurrentMyRepute )
					bPossibleShowAlbum = true;

				if( 0 < strlen(pBromideTextureFileName) )
				{
					AvailAlbumInfo.vlPortraitIndex.push_back( iPortraitTextureIndex );
					AvailAlbumInfo.vlBromideTextureFileName.push_back( pBromideTextureFileName );
					AvailAlbumInfo.vIBromideRepute.push_back(nBromideRepute);
					AvailAlbumInfo.vbIsFoundAlbum.push_back( bPossibleShowAlbum );
					AvailAlbumInfo.vIBromideQuestID.push_back( iOpenQuestID );
					AvailAlbumInfo.vIBromideQuestEnumID.push_back( iOpenQuestEnumID );
				}
#else
				sprintf_s( acBuffer, "_BromideQuest%d", k+1 );
				int  iOpenQuestID			= pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetInteger();
				bool bCompleletedOpenQuest  = pQuestTask->IsClearQuest( iOpenQuestID );
				
				if( bCompleletedOpenQuest ) // 테스트할때는 이거 빼고.
				{
					sprintf_s( acBuffer, "_BromideS%d", k+1 );
					int iPortraitTextureIndex = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetInteger();

					sprintf_s( acBuffer, "_Bromide%d", k+1 );
					const char* pBromideTextureFileName = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, acBuffer )->GetString();
					if( 0 < strlen(pBromideTextureFileName) )
					{
						AvailAlbumInfo.vlPortraitIndex.push_back( iPortraitTextureIndex );
						AvailAlbumInfo.vlBromideTextureFileName.push_back( pBromideTextureFileName );
					}
				}
#endif // PRE_ADD_REPUTATION_EXPOSURE
			} // end of for

			// 셋중에 하나라도 있어야 실제 표시하기 위한 정보를 추가.
			if( false == AvailAlbumInfo.vlBromideTextureFileName.empty() )
				m_vlAvailAlbumInfos.push_back( AvailAlbumInfo );
		}
	}

	// 정렬 선택되어있다면 정렬처리.
	if( NPC_SORT_DEFAULT != iSortMethod )
	{
		switch( iSortMethod )
		{
		case NPC_SORT_FAVOR:
			{
				struct SortByFavor : public binary_function<const S_AVAILALBUM_INFO, const S_AVAILALBUM_INFO, bool>
				{
					CReputationSystemRepository* pReputationRepos;
					DNTableFileFormat*  pReputeTable;

					SortByFavor( CReputationSystemRepository* _pReputationRepos, DNTableFileFormat*  _pReputeTable ) : pReputationRepos( _pReputationRepos ),
						pReputeTable( _pReputeTable ) {};

					bool operator () ( const S_AVAILALBUM_INFO& AvailAlbumInfoA, const S_AVAILALBUM_INFO& AvailAlbumInfoB )
					{
						int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoA.iReputeTableID, "_NpcID" )->GetInteger();
						int iFavorPercentA = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDA, IReputationSystem::NpcFavor );

						int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoB.iReputeTableID, "_NpcID" )->GetInteger();
						int iFavorPercentB = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDB, IReputationSystem::NpcFavor );

						return iFavorPercentA > iFavorPercentB;
					}
				};

				std::sort( m_vlAvailAlbumInfos.begin(), m_vlAvailAlbumInfos.end(), SortByFavor(pReputationRepos, pReputeTable) );
			}
			break;

		case NPC_SORT_MALICE:
			{
				struct SortByMalice : public binary_function<const S_AVAILALBUM_INFO, const S_AVAILALBUM_INFO, bool>
				{
					CReputationSystemRepository* pReputationRepos;
					DNTableFileFormat*  pReputeTable;

					SortByMalice( CReputationSystemRepository* _pReputationRepos, DNTableFileFormat*  _pReputeTable ) : pReputationRepos( _pReputationRepos ),
						pReputeTable( _pReputeTable ) {};

					bool operator () ( const S_AVAILALBUM_INFO& AvailAlbumInfoA, const S_AVAILALBUM_INFO& AvailAlbumInfoB )
					{
						int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoA.iReputeTableID, "_NpcID" )->GetInteger();
						int iMalicePercentA = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDA, IReputationSystem::NpcMalice );

						int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoB.iReputeTableID, "_NpcID" )->GetInteger();
						int iMalicePercentB = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDB, IReputationSystem::NpcMalice );

						return iMalicePercentA > iMalicePercentB;
					}
				};

				std::sort( m_vlAvailAlbumInfos.begin(), m_vlAvailAlbumInfos.end(), SortByMalice(pReputationRepos, pReputeTable) );
			}
			break;

		case NPC_SORT_NAME:
			{
				struct SortByName : public binary_function<const S_AVAILALBUM_INFO, const S_AVAILALBUM_INFO, bool>
				{
					DNTableFileFormat*  pReputeTable;
					DNTableFileFormat*  pNpcTable;

					SortByName( DNTableFileFormat*  _pNpcTable, DNTableFileFormat*  _pReputeTable ) : pNpcTable(_pNpcTable), pReputeTable(_pReputeTable) {};

					bool operator () ( const S_AVAILALBUM_INFO& AvailAlbumInfoA, const S_AVAILALBUM_INFO& AvailAlbumInfoB )
					{
						int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoA.iReputeTableID, "_NpcID" )->GetInteger();
						int iNpcNameA = pNpcTable->GetFieldFromLablePtr( iNpcTableIDA, "_NameID" )->GetInteger();
						const wchar_t* pNpcNameA = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNpcNameA );

						int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( AvailAlbumInfoB.iReputeTableID, "_NpcID" )->GetInteger();
						int iNpcNameB = pNpcTable->GetFieldFromLablePtr( iNpcTableIDB, "_NameID" )->GetInteger();
						const wchar_t* pNpcNameB = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNpcNameB );

						return (0 > wcscmp( pNpcNameA, pNpcNameB ));
					}
				};

				DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
				std::sort( m_vlAvailAlbumInfos.begin(), m_vlAvailAlbumInfos.end(), SortByName(pNpcTable, pReputeTable) );
			}
			break;
		}
	}

	for( int i = 0; i < (int)m_vlAvailAlbumInfos.size(); ++i )
	{
		S_AVAILALBUM_INFO& AvailAlbumInfo = m_vlAvailAlbumInfos.at( i );
		AvailAlbumInfo.iAlbumListOffset = (int)m_vlPortrait.size();

		for( int k = 0; k < (int)AvailAlbumInfo.vlBromideTextureFileName.size(); ++k )
		{
			m_vlReputeTableID.push_back( AvailAlbumInfo.iReputeTableID );
			m_vlPortrait.push_back( AvailAlbumInfo.vlPortraitIndex.at(k) );
			m_vlBromideFileName.push_back( AvailAlbumInfo.vlBromideTextureFileName.at(k) );
#ifdef PRE_ADD_REPUTATION_EXPOSURE 
			m_vbIsFoundAlbum.push_back( AvailAlbumInfo.vbIsFoundAlbum.at(k));
			m_vIBromideRepute.push_back( AvailAlbumInfo.vIBromideRepute.at(k));
			m_vIBromideQuestID.push_back( AvailAlbumInfo.vIBromideQuestID.at(k));
			m_vIBromideQuestEnumID.push_back( AvailAlbumInfo.vIBromideQuestEnumID.at(k));
#endif
		}
	}

	m_iMaxPage = ((int)m_vlBromideFileName.size()-1) / NPC_REPUTATION_ALBUM_COUNT_PER_PAGE;
}


void CDnNpcReputationAlbumDlg::Clear( void )
{
	for( int i = 0; i < NPC_REPUTATION_ALBUM_COUNT_PER_PAGE; ++i )
		m_aElements[ i ].Reset();
}


void CDnNpcReputationAlbumDlg::UpdatePage( int iPage )
{
	Clear();

	// 퀘스트 태스크가 없으면 안됨..
	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	// 페이지 단위로 앨범을 보여준다.
	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if( pReputationRepos )
	{
		DNTableFileFormat*  pReputeTable =  GetDNTable( CDnTableDB::TREPUTE );
		int iNumBromide = (int)m_vlPortrait.size();

#ifdef PRE_ADD_REPUTATION_EXPOSURE
		// 달성률 계산
		int nAchieveCount = 0;
		for( int i = 0; i < iNumBromide; ++i )
		{
			//if( m_vbIsFoundAlbum[i] )
			if(IsOpenBromide(i))
				nAchieveCount++;
		}

		float fAchievePercent = ( (float)nAchieveCount / (float)iNumBromide ) * 100.0f;
#endif

		for( int i = 0; i < NPC_REPUTATION_ALBUM_COUNT_PER_PAGE; ++i )
		{
			// 퀘스트 깨진 애들만 모여있으므로 그대로 셋팅해주면 끝.
			int iIndex = iPage*NPC_REPUTATION_ALBUM_COUNT_PER_PAGE + i;
			if( iIndex < iNumBromide )
			{ 
				int iReputeTableItemID = m_vlReputeTableID.at( iIndex );
			
				int iPortraitTextureIndex = m_vlPortrait.at( iIndex );
				int iNpcID = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, "_NpcID" )->GetInteger();

				// npc 이름.
				DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
				int iNameUIStringID = pNpcTable->GetFieldFromLablePtr( iNpcID, "_NameID" )->GetInteger();
				m_aElements[ i ].pName->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iNameUIStringID) );

#ifdef PRE_ADD_REPUTATION_EXPOSURE
				// 1. 달성률 보여주기
				if(m_pStaticAchievePercent)
				{
					wchar_t wzStr[255]={0,};
					swprintf(wzStr, L"%s %d%% (%d/%d)", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9108), (int)fAchievePercent, nAchieveCount, iNumBromide);
					m_pStaticAchievePercent->SetText(wzStr);
				}


				// 2. 초상화 텍스쳐 보여주기(작은이미지)
				EtTextureHandle TempTex;
				( IsOpenBromide(iIndex) ) ? TempTex = m_hPortraitTexture : TempTex = m_hGrayScalePortraitTexture ;
				//( m_vbIsFoundAlbum.at( iIndex ) ) ? TempTex = m_hPortraitTexture : TempTex = m_hGrayScalePortraitTexture ;
				
				m_aElements[ i ].pPictureCtrl->SetTexture(  TempTex, REPUTATION_ALBUM_PORTRAIT_SIZE*(iPortraitTextureIndex%4), 
															REPUTATION_ALBUM_PORTRAIT_SIZE*(iPortraitTextureIndex/4), 
															REPUTATION_ALBUM_PORTRAIT_SIZE, 
															REPUTATION_ALBUM_PORTRAIT_SIZE );
#else
				m_aElements[ i ].pPictureCtrl->SetTexture(  m_hPortraitTexture, REPUTATION_ALBUM_PORTRAIT_SIZE*(iPortraitTextureIndex %4), 
															REPUTATION_ALBUM_PORTRAIT_SIZE*(iPortraitTextureIndex/4), 
															REPUTATION_ALBUM_PORTRAIT_SIZE, 
															REPUTATION_ALBUM_PORTRAIT_SIZE );
#endif

				m_aElements[ i ].pPictureCtrl->Show( true );
			}
		}
	}

	m_pStaticPage->SetIntToText( m_iNowPage+1 );
}


void CDnNpcReputationAlbumDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	if( false == IsShow() )
		return;

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() ); 

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_PRIOR" ) )
		{
			RefreshPage(false);
		}
		else
		if( IsCmdControl( "ID_BT_NEXT" ) )
		{
			RefreshPage(true);
		}
		else
		if( strstr( pControl->GetControlName(), "ID_CLICK" ) )
		{
			// 클릭된 스태틱 버튼을 찾는다.
			int iBromideIndexInPage = 0;
			for( int i = 0; i < NPC_REPUTATION_ALBUM_COUNT_PER_PAGE; ++i )
			{
				if( m_aElements[ i ].pStaticEventReceiver == pControl )
				{
					iBromideIndexInPage = i;
					break;
				}
			}

			// 큰 텍스쳐 이름을 얻어옴.
			int iBromideIndex = (m_iNowPage * NPC_REPUTATION_ALBUM_COUNT_PER_PAGE) + iBromideIndexInPage;
#ifdef PRE_ADD_REPUTATION_EXPOSURE

			//if( iBromideIndex < (int)m_vlBromideFileName.size() && m_vbIsFoundAlbum[iBromideIndex])
			if( IsOpenBromide(iBromideIndex) )
#else
			if( iBromideIndex < (int)m_vlBromideFileName.size() )
#endif
			{
				EtTextureHandle hTexture = LoadResource( m_vlBromideFileName.at(iBromideIndex).c_str(), RT_TEXTURE, true );
				
				m_pBromideDlg->SetBromideTexture( hTexture );	// 셋팅된 텍스쳐는 브로마이드 다이얼로그가 닫힐 때 릴리즈 된다.
				m_pBromideDlg->Show( true );					// 큰 이미지를 보여주는 자식 다이얼로그를 보여준다~ 
				this->Show( false );
			}
		}

		// 콤보 박스 아닌 곳 클릭시 콤보 박스 포커스를 없애야 할듯 [2010/11/16 semozz]
		m_pComboSort->Focus(false);
		PopFocusControl();
	}
	else
	if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl( "ID_COMBOBOX_SORT" ) )
		{
			if( m_bInitialized )
			{
				CEtUIComboBox* pComboSort = static_cast<CEtUIComboBox*>( pControl );
#ifdef PRE_MOD_REPUTE_NOMALICE2
				pComboSort->GetSelectedValue(m_iNowSelectedSortMethod);
#else
				m_iNowSelectedSortMethod = pComboSort->GetSelectedIndex();
#endif
				_UpdateAvailAlbumList( m_iNowSelectedSortMethod );
				UpdatePage( m_iNowPage );
			}
		}
	}
}

bool CDnNpcReputationAlbumDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	switch( uMsg )
	{
		// 마우스 휠 기능 추가 [2010/11/16 semozz]
	case WM_MOUSEWHEEL:
		{
			//다이얼로그 위에 있어야 하고, 정렬콤보 박스가 포커스가 아닐때
			if ( IsMouseInDlg() && !m_pComboSort->IsFocus() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;

				//휠을 내릴때 - : 다음 페이지, 올릴때 + : 이전 페이지
				if ( nScrollAmount != 0 )
					RefreshPage( nScrollAmount < 0 );
			}
		}
		break;

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	case WM_MOUSEMOVE:
		{
			POINT MousePT;
			MousePT.x = short(LOWORD(lParam));
			MousePT.y = short(HIWORD(lParam));

			float fMouseX, fMouseY;
			PointToFloat(MousePT, fMouseX, fMouseY);

			ShowBromideHelpTooltip(fMouseX, fMouseY);
		}
		break;
#endif
	}

	return bRet;
}

void CDnNpcReputationAlbumDlg::RefreshPage(bool bInc)
{
	int tempCurrentPage = m_iNowPage;

	if ( false == bInc )
	{
		tempCurrentPage--;
		if( tempCurrentPage < 0 )
			tempCurrentPage = 0;
	}
	else
	{
		tempCurrentPage++;
		if( m_iMaxPage < tempCurrentPage )
			tempCurrentPage = m_iMaxPage;
	}

	if (tempCurrentPage != m_iNowPage)
	{
		m_iNowPage = tempCurrentPage;
		RefreshToCurrentPage();
	}
}

#ifdef PRE_ADD_REPUTATION_EXPOSURE
void CDnNpcReputationAlbumDlg::ShowBromideHelpTooltip(float fX, float fY)
{
	// 클릭된 스태틱 버튼을 찾는다.
	int iBromideIndexInPage = -1;
	for( int i = 0; i < NPC_REPUTATION_ALBUM_COUNT_PER_PAGE; ++i )
	{
		if( m_aElements[i].pStaticEventReceiver->IsInside(fX, fY) && m_aElements[i].pStaticEventReceiver->IsShow() )
		{
			iBromideIndexInPage = i;
			break;
		}
	}

	if(iBromideIndexInPage == -1)
		return;

	// 달성도 미니 툴팁을 보여준다.
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TREPUTE);
	if(!pSox) 
		return;

	// 큰 텍스쳐 이름을 얻어옴.
	int iBromideIndex = (m_iNowPage * NPC_REPUTATION_ALBUM_COUNT_PER_PAGE) + iBromideIndexInPage;
	if( iBromideIndex >= (int)m_vlBromideFileName.size() || iBromideIndex >= (int)m_vbIsFoundAlbum.size() )
		return;

	if( IsOpenBromide(iBromideIndex) )
	{
		// 클릭 시 브로마이드를 볼수있습니다.
		std::wstring wszToolTip = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7940);
		if(!wszToolTip.empty())
		{
			CDnSimpleTooltipDlg* pToolTip = GetInterface().GetSimpleTooltipDialog();
			if(pToolTip)
				pToolTip->ShowTooltipDlg(m_aElements[iBromideIndexInPage].pStaticEventReceiver, true, wszToolTip, 0xffffffff, true);
		}
	}
	else
	{
		// 아직 얻지못한 브로마이드.
		// 수집조건을 보여준다.
		if( iBromideIndex >= (int)m_vlReputeTableID.size() || iBromideIndex >= (int)m_vIBromideRepute.size() )
			return;

		int		nSuccess = 0;
		WCHAR	wszBuff[256] = { 0 , };
		
		// 1차 조건: _BromideQuest가 있는지 체크.
		// 2차 조건: _BromideRepute로 호감도를 체크.
		if( m_vIBromideQuestID[iBromideIndex] > 0 )
		{
			if( !m_vIBromideQuestEnumID.empty() )
			{
				int nBromideReputeStringID = m_vIBromideQuestEnumID[iBromideIndex];
				nSuccess = wsprintf(wszBuff, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nBromideReputeStringID));
			}
		}
		else
		{
			int nBromideReputeMax  = pSox->GetFieldFromLablePtr(m_vlReputeTableID[iBromideIndex] , "_MaxFavor")->GetInteger();
			int nBromideReputeNeed = m_vIBromideRepute[iBromideIndex];

			// 일부값 / 전체값 * 100
			float fPercent = ((float)nBromideReputeNeed / (float)nBromideReputeMax) * 100.0f;
			nSuccess = wsprintf(wszBuff, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7941), (int)fPercent);
		}
		
		if(nSuccess)
		{
			CDnSimpleTooltipDlg* pToolTip = GetInterface().GetSimpleTooltipDialog();
			if(pToolTip)
				pToolTip->ShowTooltipDlg(m_aElements[iBromideIndexInPage].pStaticEventReceiver, true, wszBuff, 0xffffffff, true);
		}
	}
}

bool CDnNpcReputationAlbumDlg::IsOpenBromide(int iBromideIndex)
{
	bool bIsOpenBromide = false;

	if( iBromideIndex < (int)m_vIBromideQuestID.size()  ||
		iBromideIndex < (int)m_vbIsFoundAlbum.size()	|| 
		iBromideIndex < (int)m_vlBromideFileName.size() )
	{
		if(m_vIBromideQuestID[iBromideIndex] > 0)
		{
			// QuestID가 있다면, 미션 체크를 할필요가 없다.
			CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));
			if(!pQuestTask) return false;

			int nQuestID = m_vIBromideQuestID[iBromideIndex];
			bIsOpenBromide = pQuestTask->IsClearQuest(nQuestID);
		}
		else
			bIsOpenBromide = m_vbIsFoundAlbum[iBromideIndex]; // m_vbIsFoundAlbum에는, 현재호감도와 필요한 호감도의 차이를 계산해서 저장해뒀음.
	}

	return bIsOpenBromide;
}
#endif