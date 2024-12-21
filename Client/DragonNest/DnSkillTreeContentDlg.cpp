#include "StdAfx.h"
#include "DnSkillTreeContentDlg.h"
#include "DnSkillTreeDlg.h"
#include "DnQuickSlotButton.h"
#include "DnSkillAcquireDlg.h"
#include "DnSkillLevelUpDlg.h"
#include "DnSkill.h"
#include "DnSkillTask.h"
#include "DnSkillTreeSystem.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnUnlockSkillDlg.h"
#include "DnMainMenuDlg.h"
#include "DnGuildWarTask.h"
#include "DnTooltipDlg.h"
#include "DnSimpleTooltipDlg.h"
#include "DnMessageBox.h"
#include "DnSkillTreeButtonDlg.h"
#include "DnLocalPlayerActor.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#include "DnTotalLevelSkillActivateListDlg.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//#undef PRE_ADD_SKILL_LEVELUP_RESERVATION

const float TREE_DOWN_LINE_X_START = 5.0f;		// 트리라인 텍스쳐에서 실제 사용하는 UV 의 텍스쳐의 오프셋
const float TREE_LEFT_LINE_Y_START = 5.0f;
const float TREE_RIGHT_LINE_Y_START = 6.0f;

const float TREE_LINE_THICK = 5.0f;				// 트리라인 텍스쳐에서 실제 사용하는 UV 의 텍스쳐의 두께
const float TREE_LINE_WIDTH_RATIO = 5.0f;		// 퀵슬롯버튼의 너비 대비 라인 두께. 너비를 이 값으로 나눈 값을 라인 두께로 사용합니다.

const float ARROW_DOWN_X_START = 3.0f;
const float ARROW_DOWN_Y_START = 5.0f;
const float ARROW_DOWN_WIDTH = 11.0f;
const float ARROW_DOWN_HEIGHT = 7.0f;

const float ARROW_LEFT_X_START = 4.0f;
const float ARROW_LEFT_Y_START = 3.0f;
const float ARROW_LEFT_WIDTH = 7.0f;
const float ARROW_LEFT_HEIGHT = 11.0f;

const float ARROW_RIGHT_X_START = 5.0f;
const float ARROW_RIGHT_Y_START = 2.0f;
const float ARROW_RIGHT_WIDTH = 7.0f;
const float ARROW_RIGHT_HEIGHT = 11.0f;

const float SHOW_TOOLTIP_TIME = 1.0f;
const float SHOW_SKILL_BOOK_TOOLTIP_TIME = 6.f;

CDnSkillTreeContentDlg::CDnSkillTreeContentDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pSkillAcquireDlg( NULL )
, m_pSkillLevelUpDlg( NULL )
, m_pSkillTreeDlg( NULL )
, m_iSelectedSlotIndex( -1 )
, m_iDragSoundIndex( -1 )
, m_bUnlockByMoneyMode( false )
, m_pTreeTextureCtrl( NULL )
, m_bGuildWarSkillMode( false )
, m_pSkillUnlockDlg(NULL)
{
}

CDnSkillTreeContentDlg::~CDnSkillTreeContentDlg(void)
{
	if( m_iDragSoundIndex == -1 ) 
		CEtSoundEngine::GetInstance().RemoveSound( m_iDragSoundIndex );

	SAFE_RELEASE_SPTR( m_hTreeTexture );
#ifdef DRAW_AUTO_TREE_LINE
	SAFE_RELEASE_SPTR( m_hLineDownTexture );
	SAFE_RELEASE_SPTR( m_hLineLeftTexture );
	SAFE_RELEASE_SPTR( m_hLineRightTexture );
	SAFE_RELEASE_SPTR( m_hArrowDownTexture );
	SAFE_RELEASE_SPTR( m_hArrowLeftTexture );
	SAFE_RELEASE_SPTR( m_hArrowRightTexture );
#endif

	SAFE_DELETE( m_pSkillUnlockDlg );
	SAFE_DELETE( m_pSkillAcquireDlg );
	SAFE_DELETE( m_pSkillLevelUpDlg );
}

void CDnSkillTreeContentDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeContentDlg.ui" ).c_str(), bShow );

#ifdef DRAW_AUTO_TREE_LINE
	m_hLineDownTexture = LoadResource( "tree_line_down.dds", RT_TEXTURE, true );
	m_hLineLeftTexture = LoadResource( "tree_line_left.dds", RT_TEXTURE, true );
	m_hLineRightTexture = LoadResource( "tree_line_right.dds", RT_TEXTURE, true );
	m_hArrowDownTexture = LoadResource( "tree_arrow_down.dds", RT_TEXTURE, true );
	m_hArrowLeftTexture = LoadResource( "tree_arrow_left.dds", RT_TEXTURE, true );
	m_hArrowRightTexture = LoadResource( "tree_arrow_right.dds", RT_TEXTURE, true );
#endif

	SetElementDialog( true );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10005 );
	if( strlen( szFileName ) > 0 )
	{
		m_iDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

void CDnSkillTreeContentDlg::InitialUpdate( void )
{
	_ASSERT( MAX_SKILL_SLOT_COUNT == (int)m_vlSkillSlot.size() && 
			 "최대 스킬 슬롯의 갯수가 R:/GameRes/Resource/UI/Skill/SkillTreeContentDlg.ui에 있는 슬롯의 갯수와 맞지 않습니다." );

	m_pTreeTextureCtrl = GetControl<CEtUITextureControl>( "ID_TEXTUREL_TREE" );
	m_pTreeTextureCtrl->Show( false );

	char acControlName[ 64 ] = { 0 };
	for( int i = 0; i < MAX_SKILL_SLOT_COUNT; ++i )
	{
		S_SKILL_SLOT_UI& SkillSlot = m_vlSkillSlot.at( i );
		
		sprintf_s( acControlName, 64, "ID_SLOT_%d", i );
		SkillSlot.pSlotBase = GetControl<CEtUIStatic>( acControlName );

		sprintf_s( acControlName, 64, "ID_PASIVESLOT_%d", i );
		SkillSlot.pPassiveSlotBase = GetControl<CEtUIStatic>( acControlName );

		sprintf_s( acControlName, 64, "ID_SKILL_LEVEL_%02d", i );
		SkillSlot.pSkillLevel = GetControl<CEtUIStatic>( acControlName );

		// 스킬 레벨 툴팁 인덱스 셋팅.
		SkillSlot.pSkillLevel->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1814 ) );

		sprintf_s( acControlName, 64, "ID_BUTTON_LOCK%d", i );
		SkillSlot.pSkillLockButton = GetControl<CDnSkillAcquireButton>( acControlName );
		SkillSlot.pSkillLockButton->SetBlink( true );

		sprintf_s( acControlName, 64, "ID_BUTTON_SPECIAL%d", i );
		SkillSlot.pExclusiveSkillLockButton = GetControl<CDnSkillAcquireButton>( acControlName );
		SkillSlot.pSkillLockButton->SetBlink( true );
		SkillSlot.pExclusiveSkillLockButton->Show( false );

		sprintf_s( acControlName, 64, "ID_SKILLUP_%02d", i );
		SkillSlot.pSkillUpButton = GetControl<CEtUIButton>( acControlName );

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		sprintf_s( acControlName, 64, "ID_SKILLDOWN_%d", i );
		SkillSlot.pSkillDownButton = GetControl<CEtUIButton>( acControlName );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		sprintf_s( acControlName, 64, "ID_STATIC_LIGHT%d", i );
		SkillSlot.pHighlightStatic = GetControl<CEtUIStatic>( acControlName );

		// 그냥 슬롯 인덱스별로 버튼 아이디 할당.
		SkillSlot.pSkillUpButton->SetButtonID( i );
		SkillSlot.pSkillLockButton->SetButtonID( i );
		SkillSlot.pExclusiveSkillLockButton->SetButtonID( i );
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		SkillSlot.pSkillDownButton->SetButtonID( i );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	}

	m_pSkillUnlockDlg = new CDnSkillUnlockDlg( UI_TYPE_MODAL, NULL, SKILL_UNLOCK_DIALOG, this );
	m_pSkillUnlockDlg->Initialize( false );

	m_pSkillAcquireDlg = new CDnSkillAcquireDlg( UI_TYPE_MODAL, NULL, SKILL_ACQUIRE_DIALOG, this );
	m_pSkillAcquireDlg->Initialize( false );

	m_pSkillLevelUpDlg = new CDnSkillLevelUpDlg( UI_TYPE_MODAL, NULL, SKILL_LEVELUP_DIALOG, this );
	m_pSkillLevelUpDlg->Initialize( false );

	// 잡 테이블에서 영문 이름 읽어옴.
#ifndef DRAW_AUTO_TREE_LINE
	DNTableFileFormat*  pJobTable = GetDNTable( CDnTableDB::TJOB );
	for( int i = 0; i < pJobTable->GetItemCount(); ++i )
	{
		int iItemID = pJobTable->GetItemID( i );
		const char* pJobName = pJobTable->GetFieldFromLablePtr( iItemID, "_EnglishName" )->GetString();

		if( strlen( pJobName ) )
		{
			m_mapJobUIStatic.insert( make_pair( iItemID, string(pJobName) ) );
			//string strUIStaticName( "ID_" );
			//strUIStaticName.append( pJobName );
			//m_mapJobUIStatic.insert( make_pair( iItemID, strUIStaticName ) );

			//// 모든 직업 컨트롤 HIDE
			//CEtUIStatic* pControl = GetControl<CEtUIStatic>( strUIStaticName.c_str() );
			//if( pControl )
			//{
			//	pControl->Show( false );
			//	m_vlpTreeLineStatics.push_back( pControl );
			//}
		}
	}
#endif
}

void CDnSkillTreeContentDlg::InitCustomControl( CEtUIControl *pControl )
{
	if( typeid(*pControl) != typeid(CDnQuickSlotButton) )
		return;

	CDnQuickSlotButton *pSkillSlotButton(NULL);
	pSkillSlotButton = static_cast<CDnQuickSlotButton*>(pControl);

	pSkillSlotButton->SetSlotType(ST_SKILL);
	pSkillSlotButton->SetSlotIndex( (int)m_vlSkillSlot.size() );

	S_SKILL_SLOT_UI SkillSlot;
	SkillSlot.pSkillQuickSlot = pSkillSlotButton;
	m_vlSkillSlot.push_back( SkillSlot );
}

void CDnSkillTreeContentDlg::UpdateGuildWarSkillTree( bool bSetGuildSkill )
{
	_ASSERT( MAX_SKILL_SLOT_COUNT == (int)m_vlSkillSlot.size() );

	// 슬롯의 값들을 리셋함. 나중엔 스킬의 자리가 옮겨지지 않기 때문에 락 언락만 업데이트 해주면 될 듯.
	int iNumSkillSlot = (int)m_vlSkillSlot.size();
	for( int iSlot = 0; iSlot < iNumSkillSlot; ++iSlot )
	{
		S_SKILL_SLOT_UI& SkillSlot = m_vlSkillSlot.at( iSlot );
		SkillSlot.Reset();
	}
	SAFE_RELEASE_SPTR( m_hTreeTexture );	// 스킬 트리 라인 삭제

	vector<DnSkillHandle> vlhGuildWarSkillList = GetSkillTask().GetGuildWarSkillList();
	vector<DnSkillHandle>::iterator iter = vlhGuildWarSkillList.begin();

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLTREE );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid BattleGroundSkillTree" );
		return;
	}

	int nRemainPoint = GetGuildWarTask().GetGuildWarSkillLeavePoint();
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	nRemainPoint -= GetSkillTask().GetReservationGuildSkillCount();
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

	for( ; iter != vlhGuildWarSkillList.end(); iter++ )
	{
		DnSkillHandle hSkill = (*iter);
		if( hSkill == NULL ) continue;

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		DnSkillHandle hReservationSkill = GetSkillTask().GetReservationGuildSkill( hSkill->GetClassID() );
		if( CDnSkill::Identity() != hReservationSkill )
			hSkill = hReservationSkill;
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		int nItemID = pSox->GetItemIDFromField( "_SkillTableID", hSkill->GetClassID() );
		int iSlot = pSox->GetFieldFromLablePtr( nItemID, "_TreeSlotIndex" )->GetInteger();

		S_SKILL_SLOT_UI& SkillSlotUI = m_vlSkillSlot.at( iSlot );
		SkillSlotUI.pSkillQuickSlot->SetQuickItem( (*iter).GetPointer() );

		// 길드전 스킬을 선택 할 수 있는 조건 일 경우에만 오픈
		if( bSetGuildSkill )
		{
			if( nRemainPoint <= 0 )
			{
				SkillSlotUI.pSkillUpButton->Show( false );
				SkillSlotUI.pSkillUpButton->Enable( false );
			}
			else
			{
				SkillSlotUI.pSkillUpButton->Show( true );
				SkillSlotUI.pSkillUpButton->Enable( true );
			}
		}
		else
		{
			SkillSlotUI.pSkillUpButton->Show( false );
			SkillSlotUI.pSkillUpButton->Enable( false );
		}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		if( true == GetSkillTask().IsReservationGuildSkillDown( hSkill->GetClassID() ) )
		{
			SkillSlotUI.pSkillDownButton->Show( true );
			SkillSlotUI.pSkillDownButton->Enable( true );
		}
		else
		{
			SkillSlotUI.pSkillDownButton->Show( false );
			SkillSlotUI.pSkillDownButton->Enable( false );
		}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		// 레벨이 최대 레벨 보다 낮으면 스킬업 버튼 오픈
		if( hSkill->GetLevel() < hSkill->GetMaxLevel() )
			SkillSlotUI.pSkillUpButton->Show( true );
		else
			SkillSlotUI.pSkillUpButton->Show( false );

		// 최대 레벨이 1인 단일 스킬의 경우 획득하지 않았을 경우 스킬업 버튼 오픈
		if( !hSkill->IsAcquired() && hSkill->GetMaxLevel() == 1 )
			SkillSlotUI.pSkillUpButton->Show( true );

#ifdef PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN
		std::wstring szTemp;
#else
		wchar_t szTemp[256] = { 0 };
#endif // PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN
		if( hSkill->IsAcquired() )
		{
#ifdef PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN
			//rlkt 2016
			//szTemp = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), hSkill->GetLevel());
			szTemp = FormatW(L"%d/%d", hSkill->GetLevel(), hSkill->GetMaxLevel());
#else
			swprintf_s( szTemp, _countof(szTemp), L"%d/%d", hSkill->GetLevel(), hSkill->GetMaxLevel() );
#endif

//			if( bSetGuildSkill )
				SkillSlotUI.pSkillQuickSlot->SetForceUsable( true );
//			else
//				SkillSlotUI.pSkillQuickSlot->SetForceUnUsable( true );

#ifdef PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN
			SkillSlotUI.pSkillLevel->SetText( szTemp.c_str() );
#else
			SkillSlotUI.pSkillLevel->SetText( szTemp );
#endif

			if( hSkill->GetMaxLevel() == 1 )
				SkillSlotUI.pSkillUpButton->Show( false );
		}
		else
			SkillSlotUI.pSkillQuickSlot->SetForceUnUsable( true );

		SkillSlotUI.pSlotBase->Show( true );
		SkillSlotUI.pPassiveSlotBase->Show( false );
		SkillSlotUI.pSkillQuickSlot->Show( true );
		SkillSlotUI.pSkillLevel->Show( true );
		SkillSlotUI.pSkillLockButton->Show( false );

		if( SkillSlotUI.pSkillUpButton->IsShow() )
		{
#ifdef PRE_FIX_GUILDWARSKILLTREE_BUFFEROVERRUN
			szTemp = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5117 ), 1);
			SkillSlotUI.pSkillUpButton->SetTooltipText( szTemp.c_str() );
#else
			swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5117 ), 1 );
			SkillSlotUI.pSkillUpButton->SetTooltipText( szTemp );
#endif
		}
		else
			SkillSlotUI.pSkillUpButton->SetTooltipText( L"" );

		this->ShowTooltipDlg( SkillSlotUI.pSkillUpButton, false );
	}

	m_bGuildWarSkillMode = true;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_bTotalLevelSkillMode = false;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}

void CDnSkillTreeContentDlg::UpdateSkillTree( void )
{
	// TODO: 추후에 락/언락 스킬 구분해줘야 함.

	// 정해진 슬롯 최대 갯수와 UI 에서 셋팅된 갯수가 맞아야 함
	_ASSERT( MAX_SKILL_SLOT_COUNT == (int)m_vlSkillSlot.size() );

	// 슬롯의 값들을 리셋함. 나중엔 스킬의 자리가 옮겨지지 않기 때문에 락 언락만 업데이트 해주면 될 듯.
	int iNumSkillSlot = (int)m_vlSkillSlot.size();
	for( int iSlot = 0; iSlot < iNumSkillSlot; ++iSlot )
	{
		S_SKILL_SLOT_UI& SkillSlot = m_vlSkillSlot.at( iSlot );
		SkillSlot.Reset();
	}

	for( int iSlot = 0; iSlot < MAX_SKILL_SLOT_COUNT; ++iSlot )
	{
		CDnSkillTask::S_SKILLTREE_SLOT_INFO SlotInfo;
		GetSkillTask().GetSkillTreeSlotInfo( m_pSkillTreeDlg->GetSelectedJobID(), iSlot, &SlotInfo );

		if( SlotInfo.hSkill )
			Update_SkillSlotUI( SlotInfo, iSlot );
	}

	// exclusive 가 걸린 궁극기 스킬의 깃발은 보라색으로 표시..
	// 결과적으로 스킬 락 버튼의 결과를 그대로 따라간다.
	for( int iSlot = 0; iSlot < MAX_SKILL_SLOT_COUNT; ++iSlot )
	{
		CDnSkillTask::S_SKILLTREE_SLOT_INFO SlotInfo;
		GetSkillTask().GetSkillTreeSlotInfo( m_pSkillTreeDlg->GetSelectedJobID(), iSlot, &SlotInfo );

		S_SKILL_SLOT_UI& SkillSlotUI = m_vlSkillSlot.at( iSlot );
		SkillSlotUI.pExclusiveSkillLockButton->Show( false );

		// exclusive id 를 궁극기에서 사용하지 않으므로 글로벌 그룹 ID 로 기준 변경.
		// 추후에 궁극기를 구분하도록 아예 테이블에 필드를 넣어야 할 수도 있다.
		if( SlotInfo.hSkill 
			&& 0 < SlotInfo.hSkill->GetGlobalSkillGroupID() 
#if defined( PRE_MOD_GLOBAL_SKILLICON )
			&& SlotInfo.NodeRenderInfo.pAcquireCondition->iNeedLevel >= 40	// 이부분 너무 임시로 때우고 있어서 추후에 꼭 명확히 하는 작업이 필요합니다.
#endif	// PRE_MOD_GLOBAL_SKILLICON
			)
		{
			SkillSlotUI.pExclusiveSkillLockButton->Show( SkillSlotUI.pSkillLockButton->IsShow() );
			SkillSlotUI.pExclusiveSkillLockButton->Enable( SkillSlotUI.pSkillLockButton->IsEnable() );
			SkillSlotUI.pExclusiveSkillLockButton->SetBlink( true );
			SkillSlotUI.pExclusiveSkillLockButton->SetTooltipText( SkillSlotUI.pSkillLockButton->GetTooltipText().c_str() );

			// 기존의 빨간 깃발은 숨긴다.
			SkillSlotUI.pSkillLockButton->Show( false );
		}
	}

	// 맞는 스킬트리 라인 스태틱을 보여줌.
#ifndef DRAW_AUTO_TREE_LINE
	//for( int i = 0; i < (int)m_vlpTreeLineStatics.size(); ++i )
	//	m_vlpTreeLineStatics.at( i )->Show( false );

	//map<int, string>::iterator iter = m_mapJobUIStatic.find( m_pSkillTreeDlg->GetSelectedJobID() );
	//if( m_mapJobUIStatic.end() != iter )
	//{
	//	CEtUIStatic* pControl = GetControl<CEtUIStatic>( iter->second.c_str() );
	//	if( pControl )
	//	{
	//		pControl->Show( true );
	//		MoveToHead( pControl );		// 스태틱 컨트롤을 뒤로 빼서 포커스가 가지 않도록 처리.
	//	}
	//}

	map<int, string>::iterator iter = m_mapJobUIStatic.find( m_pSkillTreeDlg->GetSelectedJobID() );
	if( m_mapJobUIStatic.end() != iter )
	{
		string strTextureFileName = iter->second + ".dds";
		SAFE_RELEASE_SPTR( m_hTreeTexture );
		m_hTreeTexture = LoadResource( strTextureFileName.c_str(), RT_TEXTURE, true );

		m_pTreeTextureCtrl->SetTexture( m_hTreeTexture, 0, 0, 512, 512 );
		m_pTreeTextureCtrl->Show( true );
		MoveToHead( m_pTreeTextureCtrl );		// 스태틱 컨트롤을 뒤로 빼서 포커스가 가지 않도록 처리.
	}
#endif
	m_bGuildWarSkillMode = false;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_bTotalLevelSkillMode = false;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

}

void CDnSkillTreeContentDlg::OnSkillLevelUp( bool bSuccess )
{
	if( bSuccess )
	{
		if( m_iSelectedSlotIndex >= 0 )
			m_vlSkillSlot[ m_iSelectedSlotIndex ].pSkillQuickSlot->SetCompleteCoolTime( 0.5f );

		m_iSelectedSlotIndex = -1;

		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1134 ) );
	}
	else
	{
	}
}

bool CDnSkillTreeContentDlg::OnMouseMove( float fX, float fY )
{
	bool bRetVal = false;

	// 이 다이얼로그를 감싸는 다이얼로그 영역 밖으로 마우스가 나갔다면 이 다이얼로그도 동일하게 
	// 포커스를 소멸시킨다.
	if( !m_pSkillTreeDlg )
		return bRetVal;
	
	SUICoord FrameDlgCoord;
	m_pSkillTreeDlg->GetDlgCoord( FrameDlgCoord );
	FrameDlgCoord.SetPosition( m_pSkillTreeDlg->GetXCoord(), m_pSkillTreeDlg->GetYCoord() );
	if( FrameDlgCoord.IsInside( s_fScreenMouseX, s_fScreenMouseY ) )
	{
		bRetVal = CDnCustomDlg::OnMouseMove( fX, fY );
		ShowSlotInformation( fX, fY );
	}
	else
	{
		m_bMouseInDialog = false;
		ReleaseMouseEnterControl();
	}

	return bRetVal;
}

#ifdef DRAW_AUTO_TREE_LINE
void CDnSkillTreeContentDlg::_DrawTreeLeftLine( const SUICoord FromRight, const SUICoord ToLeft, bool bRenderArrow/* = true*/ )
{
	float fDistance = FromRight.fX - (ToLeft.fX+ToLeft.fWidth);

	// TODO: UV 값은 고정일 테니까 한번만 계산하던지 하자.
	SUICoord LeftLineUVCoord( 0.0f, TREE_LEFT_LINE_Y_START / float(m_hLineLeftTexture->Width()),
							  1.0f, TREE_LINE_THICK / float(m_hLineLeftTexture->Width()) );
	SUICoord LeftLineCoord( 0.0f, 0.0f, 0.0f, 0.0f );

	LeftLineCoord.fWidth = fDistance;
	if( bRenderArrow )
		LeftLineCoord.fWidth -= (float)TREE_LINE_THICK / DEFAULT_UI_SCREEN_WIDTH;

	LeftLineCoord.fHeight = (TREE_LINE_THICK / DEFAULT_UI_SCREEN_HEIGHT);

	LeftLineCoord.fX = ToLeft.fX + ToLeft.fWidth;
	if( bRenderArrow )
		LeftLineCoord.fX += (ARROW_LEFT_WIDTH/DEFAULT_UI_SCREEN_WIDTH);

	LeftLineCoord.fY = ToLeft.fY + ToLeft.fHeight/2.0f - (LeftLineCoord.fHeight/2.0f);

	DrawSprite( m_hLineLeftTexture, LeftLineUVCoord, 0xffffffff, LeftLineCoord );

	if( bRenderArrow )
	{
		// 화살표 끝 표시.
		SUICoord ArrowLeftUVCoord( ARROW_LEFT_X_START / float(m_hArrowLeftTexture->Width()), 
								   ARROW_LEFT_Y_START / float(m_hArrowLeftTexture->Height()), 
								   ARROW_LEFT_WIDTH / float(m_hArrowLeftTexture->Width()), 
								   ARROW_LEFT_HEIGHT / float(m_hArrowLeftTexture->Height()));
		SUICoord ArrowLeftCoord( 0.0f, 0.0f, 0.0f, 0.0f );

		ArrowLeftCoord.fWidth = ARROW_LEFT_WIDTH / DEFAULT_UI_SCREEN_WIDTH;
		ArrowLeftCoord.fHeight = ARROW_LEFT_HEIGHT / DEFAULT_UI_SCREEN_HEIGHT;
		ArrowLeftCoord.fX = ToLeft.fX + ToLeft.fWidth;
		ArrowLeftCoord.fY = ToLeft.fY + ToLeft.fHeight/2.0f - (ArrowLeftCoord.fHeight/2.0f);

		DrawSprite( m_hArrowLeftTexture, ArrowLeftUVCoord, 0xffffffff, ArrowLeftCoord );
	}
}

void CDnSkillTreeContentDlg::_DrawTreeRightLine( const SUICoord FromLeft, const SUICoord ToRight, bool bRenderArrow/* = true*/ )
{
	float fDistance = ToRight.fX - (FromLeft.fX+FromLeft.fWidth);

	SUICoord RightLineUVCoord( 0.0f, TREE_RIGHT_LINE_Y_START / float(m_hLineRightTexture->Width()),
							   1.0f, TREE_LINE_THICK / float(m_hLineRightTexture->Width()) );
	SUICoord RightLineCoord( 0.0f, 0.0f, 0.0f, 0.0f );

	RightLineCoord.fWidth = fDistance;
	if( bRenderArrow )
		RightLineCoord.fWidth -= (float)ARROW_RIGHT_WIDTH/DEFAULT_UI_SCREEN_WIDTH;

	RightLineCoord.fHeight = (TREE_LINE_THICK / DEFAULT_UI_SCREEN_HEIGHT);
	RightLineCoord.fX = FromLeft.fX + FromLeft.fWidth;
	RightLineCoord.fY = FromLeft.fY + FromLeft.fHeight/2.0f - (RightLineCoord.fHeight/2.0f);

	DrawSprite( m_hLineRightTexture, RightLineUVCoord, 0xffffffff, RightLineCoord );

	if( bRenderArrow )
	{
		// 화살표 끝 표시.
		SUICoord ArrowRightUVCoord( ARROW_RIGHT_X_START / float(m_hArrowRightTexture->Width()), 
									ARROW_RIGHT_Y_START / float(m_hArrowRightTexture->Height()), 
									ARROW_RIGHT_WIDTH / float(m_hArrowRightTexture->Width()), 
									ARROW_RIGHT_HEIGHT / float(m_hArrowRightTexture->Height()));
		SUICoord ArrowRightCoord( 0.0f, 0.0f, 0.0f, 0.0f );

		ArrowRightCoord.fWidth = ARROW_RIGHT_WIDTH / DEFAULT_UI_SCREEN_WIDTH;
		ArrowRightCoord.fHeight = ARROW_RIGHT_HEIGHT / DEFAULT_UI_SCREEN_HEIGHT;
		ArrowRightCoord.fX = ToRight.fX - (ARROW_RIGHT_WIDTH / DEFAULT_UI_SCREEN_WIDTH);
		ArrowRightCoord.fY = ToRight.fY + ToRight.fHeight/2.0f - (ArrowRightCoord.fHeight/2.0f);

		DrawSprite( m_hArrowRightTexture, ArrowRightUVCoord, 0xffffffff, ArrowRightCoord );
	}
}

void CDnSkillTreeContentDlg::_DrawTreeDownLine( const SUICoord FromUp, const SUICoord ToDown, bool bRenderArrow/* = true*/ )
{
	float fDistance = ToDown.fY - (FromUp.fY+FromUp.fHeight);

	SUICoord DownLineUVCoord( TREE_DOWN_LINE_X_START / float(m_hLineDownTexture->Width()), 0.0f, 
							  TREE_LINE_THICK / float(m_hLineDownTexture->Width()), 1.0f );
	SUICoord DownLineCoord( 0.0f, 0.0f, 0.0f, 0.0f );

	DownLineCoord.fWidth = (float)TREE_LINE_THICK / DEFAULT_UI_SCREEN_WIDTH;

	DownLineCoord.fHeight = fDistance;
	if( bRenderArrow )
		DownLineCoord.fHeight -= (ARROW_DOWN_HEIGHT / DEFAULT_UI_SCREEN_HEIGHT);

	DownLineCoord.fX = FromUp.fX + FromUp.fWidth/2.0f - (DownLineCoord.fWidth/2.0f);
	DownLineCoord.fY = FromUp.fY + FromUp.fHeight;

	DrawSprite( m_hLineDownTexture, DownLineUVCoord, 0xffffffff, DownLineCoord );

	if( bRenderArrow )
	{
		// 화살표 끝 표시.
		SUICoord ArrowEndUVCoord( ARROW_DOWN_X_START / float(m_hArrowDownTexture->Width()), 
								  ARROW_DOWN_Y_START / float(m_hArrowDownTexture->Height()), 
								  ARROW_DOWN_WIDTH / float(m_hArrowDownTexture->Width()), 
								  ARROW_DOWN_HEIGHT / float(m_hArrowDownTexture->Height()));
		SUICoord ArrowEndCoord( 0.0f, 0.0f, 0.0f, 0.0f );
		ArrowEndCoord.fWidth = ARROW_DOWN_WIDTH / DEFAULT_UI_SCREEN_WIDTH;
		ArrowEndCoord.fHeight = ARROW_DOWN_HEIGHT / DEFAULT_UI_SCREEN_HEIGHT;
		ArrowEndCoord.fX = FromUp.fX + FromUp.fWidth/2.0f - (ArrowEndCoord.fWidth/2.0f);
		ArrowEndCoord.fY = FromUp.fY + FromUp.fHeight + (fDistance - ArrowEndCoord.fHeight);

		DrawSprite( m_hArrowDownTexture, ArrowEndUVCoord, 0xffffffff, ArrowEndCoord );
	}
}

void CDnSkillTreeContentDlg::_DrawAutoTreeLine( void )
{
	// 따로 스킬트리의 화살표를 렌더링. SkillTreeSystem의 렌더링 정보를 참고한다.
	// 좌표는 비율로!
	for( int iSlotIndex = 0; iSlotIndex < MAX_SKILL_SLOT_COUNT; ++iSlotIndex )
	{
		CDnSkillTask::S_SKILLTREE_SLOT_INFO SlotInfo;
		GetSkillTask().GetSkillTreeSlotInfo( m_pSkillTreeDlg->GetSelectedJobID(), iSlotIndex, &SlotInfo );

		if( SlotInfo.hSkill )
		{
			// 자식 스킬이 있는 경우 거기까지 라인을 그려준다.
			if( false == SlotInfo.NodeRenderInfo.vlChildSlotIndexInJob.empty() )
			{
				const S_SKILL_SLOT_UI& SlotUI = m_vlSkillSlot.at( iSlotIndex );
				const SUICoord& Coord = SlotUI.pSkillQuickSlot->GetUICoord();

				int iNumChild = (int)SlotInfo.NodeRenderInfo.vlChildSlotIndexInJob.size();
				for( int iChild = 0; iChild < iNumChild; ++iChild )
				{	
					int iChildSlotIndex = SlotInfo.NodeRenderInfo.vlChildSlotIndexInJob.at( iChild );
					const S_SKILL_SLOT_UI& ChildSlotUI = m_vlSkillSlot.at( iChildSlotIndex ); 
					const SUICoord& ChildCoord = ChildSlotUI.pSkillQuickSlot->GetUICoord();

					// 인덱스로 초기 방향 및 꺽이는 곳 판단.
					int iSlotGap = iChildSlotIndex - iSlotIndex;
					int iSlotHorizonGap = ((iChildSlotIndex % MAX_SKILL_PER_LINE) - (iSlotIndex % MAX_SKILL_PER_LINE));
					bool bSameDeep = ((iChildSlotIndex / MAX_SKILL_PER_LINE) == (iSlotIndex / MAX_SKILL_PER_LINE));

					// 왼쪽에 자식
					if( bSameDeep && (-4 < iSlotGap && iSlotGap < 0) )
					{
						_DrawTreeLeftLine( Coord, ChildCoord );
					}
					else
					// 오른쪽에 자식
					if( bSameDeep && ( 0 < iSlotGap && iSlotGap < 4) )
					{
						_DrawTreeRightLine( Coord, ChildCoord );
					}
					else
					// 직선 아래 방향.
					if( (iSlotGap % MAX_SKILL_PER_LINE) == 0 )
					{
						_DrawTreeDownLine( Coord, ChildCoord );
					}
					else
					// 왼쪽 아래 방향. 전제는 무조건 아래로 한 칸임.
					if( -4 < iSlotHorizonGap && iSlotHorizonGap < 0 )
					{
						float fVerticalHalfDistance = (ChildCoord.fY - (Coord.fY+Coord.fHeight)) / 2.0f;
						float fHalfLineThickU = (TREE_LINE_THICK/DEFAULT_UI_SCREEN_WIDTH)/2.0f;

						// 아래로 절반 진행
						SUICoord DownHalf = Coord;
						DownHalf.fY += DownHalf.fHeight + fVerticalHalfDistance;
						_DrawTreeDownLine( Coord, DownHalf, false );

						// 아래로 절반 진행.
						SUICoord EndDownHalf = ChildCoord;
						SUICoord FromUp = ChildCoord;
						FromUp.fY -= FromUp.fHeight + fVerticalHalfDistance;
						_DrawTreeDownLine( FromUp, EndDownHalf );

						// 왼쪽으로 왼쪽 거리 끝까지 진행
						SUICoord FromRight = Coord;
						FromRight.fX += FromRight.fWidth/2.0f + fHalfLineThickU;		// 라인 두께를 감안해야함.
						FromRight.fY += FromRight.fHeight / 2.0f + fVerticalHalfDistance;
						SUICoord ToLeft = FromRight;
						ToLeft.fX = ChildCoord.fX - ChildCoord.fWidth / 2.0f - fHalfLineThickU;
						_DrawTreeLeftLine( FromRight, ToLeft, false );
					}
					else
					// 오른쪽 아래 방향.
					if( 0 < iSlotHorizonGap && iSlotHorizonGap < 4 )
					{
						float fVerticalHalfDistance = (ChildCoord.fY - (Coord.fY+Coord.fHeight)) / 2.0f;
						float fHalfLineThickU = (TREE_LINE_THICK/DEFAULT_UI_SCREEN_WIDTH)/2.0f;

						// 아래로 절반 진행
						SUICoord DownHalf = Coord;
						DownHalf.fY += DownHalf.fHeight + fVerticalHalfDistance;
						_DrawTreeDownLine( Coord, DownHalf, false );

						// 아래로 절반 진행.
						SUICoord EndDownHalf = ChildCoord;
						SUICoord FromUp = ChildCoord;
						FromUp.fY -= FromUp.fHeight + fVerticalHalfDistance;
						_DrawTreeDownLine( FromUp, EndDownHalf );

						// 오른쪽으로 오른쪽 거리 끝까지 진행
						SUICoord FromLeft = Coord;
						FromLeft.fX -= FromLeft.fWidth/2.0f + fHalfLineThickU;		// 라인 두께를 감안해야함.
						FromLeft.fY += FromLeft.fHeight / 2.0f + fVerticalHalfDistance;
						SUICoord ToRight = FromLeft;
						ToRight.fX = ChildCoord.fX + ChildCoord.fWidth / 2.0f + fHalfLineThickU;
						_DrawTreeRightLine( FromLeft, ToRight, false );
					}
				}
			}
		}
	}
}
#endif

void CDnSkillTreeContentDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render( fElapsedTime );

	_ASSERT( MAX_SKILL_SLOT_COUNT == (int)m_vlSkillSlot.size() );
	
	if( m_bShow )
	{
#ifdef DRAW_AUTO_TREE_LINE
		_DrawAutoTreeLine();
#endif
		CDnMessageBox * pMessageBox = GetInterface().GetMessageBox();

		if( NULL == pMessageBox )
			return;

		if( true == pMessageBox->IsShow() )
			HideSlotInformation();
	}
}

void CDnSkillTreeContentDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( EVENT_BUTTON_CLICKED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_BUTTON_LOCK" ) ||
			strstr( pControl->GetControlName(), "ID_BUTTON_SPECIAL" ) )
		{
			// 배타적인 스킬의 버튼도 같은 SkillAcquire 버튼을 쓰므로 아래코드는 동이랗게 가져간다.
			CEtUIButton *pSkillUnlockButton = (CEtUIButton*)pControl;
			m_iSelectedSlotIndex = pSkillUnlockButton->GetButtonID();

			if( !pSkillUnlockButton->IsMouseEnter() )
				return;

			// 아직 획득하지 않은 스킬일 때는 새로 얻도록 해준다.
			CDnSkill *pSkill = static_cast<CDnSkill*>(m_vlSkillSlot.at( m_iSelectedSlotIndex ).pSkillQuickSlot->GetItem());

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			const int nSkillID = pSkill->GetClassID();
			if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 )
				GetSkillTask().Change_SkillLevelUpMax( nSkillID );
			else
				GetSkillTask().Change_SkillLevelUp( nSkillID, static_cast<CDnSkill*>(pSkill)->GetLevel(), ReservationSKillList::Type::UnLock );

#if defined( PRE_ADD_PRESET_SKILLTREE )
			m_pSkillTreeDlg->SetPresetModify( true );
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

			GetSkillTask().SetReservationSkill_Item( nSkillID );
			UpdateSkillTree();
			m_pSkillTreeDlg->ShowGuildSkillPointInfo();
			m_pSkillTreeDlg->ButtonStateSkillReservation();
#else
			UnLockSkill( pSkill );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

			if( drag::IsValid() )
			{
				CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
				if( pDragButton->GetSlotType() == ST_SKILL )
					drag::ReleaseControl();
			}
		}
		else
		if( strstr( pControl->GetControlName(), "ID_SKILLUP" ) )
		{
			CDnSkillUpButton *pSkillUpButton = (CDnSkillUpButton*)pControl;
			m_iSelectedSlotIndex = pSkillUpButton->GetButtonID();

			if( !pSkillUpButton->IsMouseEnter() )
				return;

			// 아직 획득하지 않은 스킬일 때는 새로 얻도록 해준다.
			MIInventoryItem *pSkill = m_vlSkillSlot.at( m_iSelectedSlotIndex ).pSkillQuickSlot->GetItem();
			_ASSERT( pSkill );

			if( pSkill && !GetSkillTask().IsRequestWait() && !GetSkillTask().IsTempSkillAdded() ) // 스킬요청중이거나 임시스킬이 추가된 상태에서 스킬레벨을 올릴 수 없다.
			{
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
				if( false == m_bGuildWarSkillMode )
				{
					const int eReservation = static_cast<CDnSkill*>(pSkill)->IsAcquired() ? ReservationSKillList::Type::LevelUp : ReservationSKillList::Type::Acquire;
					const int nSkillID = pSkill->GetClassID();

					if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 )
						GetSkillTask().Change_SkillLevelUpMax( nSkillID );
					else
					{
						if( ReservationSKillList::Type::Acquire == eReservation )
							GetSkillTask().Change_SkillLevelUp( nSkillID, static_cast<CDnSkill*>(pSkill)->GetLevel(), eReservation );
						else
						{
							DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
							if( !hLocalActor )
								return;

							const int nLevelUp = hLocalActor->GetSkillLevelUpValue( nSkillID );

							GetSkillTask().Change_SkillLevelUp( nSkillID, static_cast<CDnSkill*>(pSkill)->GetLevel() + 1 - nLevelUp, eReservation );
						}
					}

#if defined( PRE_ADD_PRESET_SKILLTREE )
					m_pSkillTreeDlg->SetPresetModify( true );
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

					GetSkillTask().SetReservationSkill_Item( nSkillID );
					UpdateSkillTree();
					m_pSkillTreeDlg->ShowGuildSkillPointInfo();
					m_pSkillTreeDlg->ButtonStateSkillReservation();
				}
				else
				{
					GetSkillTask().Change_GuildSkillLevelUp( pSkill->GetClassID() );
					m_pSkillTreeDlg->UpdateGuildWarSkillTreeContent();
					m_pSkillTreeDlg->ShowGuildSkillPointInfo();
					m_pSkillTreeDlg->ButtonStateSkillReservation();
				}
#else	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
				if( static_cast<CDnSkill*>(pSkill)->IsAcquired() )
				{
					m_pSkillLevelUpDlg->SetSkill( pSkill );
					m_pSkillLevelUpDlg->Show( true );

					HideSlotInformation();
				}
				else
				{
					m_pSkillAcquireDlg->SetSkill( pSkill );
					m_pSkillAcquireDlg->Show( true );
					HideSlotInformation();
				}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			}

			if( drag::IsValid() )
			{
				CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
				if( pDragButton->GetSlotType() == ST_SKILL )
					drag::ReleaseControl();
			}
			return;
		}
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		else if( strstr( pControl->GetControlName(), "ID_SKILLDOWN" ) )
		{
			CDnSkillUpButton *pSkillUpButton = (CDnSkillUpButton*)pControl;
			m_iSelectedSlotIndex = pSkillUpButton->GetButtonID();

			if( !pSkillUpButton->IsMouseEnter() )
				return;

			// 아직 획득하지 않은 스킬일 때는 새로 얻도록 해준다.
			MIInventoryItem *pSkill = m_vlSkillSlot.at( m_iSelectedSlotIndex ).pSkillQuickSlot->GetItem();
			_ASSERT( pSkill );

			if( pSkill && !GetSkillTask().IsRequestWait() && !GetSkillTask().IsTempSkillAdded() ) // 스킬요청중이거나 임시스킬이 추가된 상태에서 스킬레벨을 올릴 수 없다.
			{
				if( false == m_bGuildWarSkillMode )
				{
					const int nSkillID = pSkill->GetClassID();
					if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 )
						GetSkillTask().Change_SkillLevelDownMax( nSkillID );
					else
						GetSkillTask().Change_SkillLevelDown( nSkillID, static_cast<CDnSkill*>(pSkill)->GetLevel() );

#if defined( PRE_ADD_PRESET_SKILLTREE )
					m_pSkillTreeDlg->SetPresetModify( true );
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

					GetSkillTask().SetReservationSkill_Item( nSkillID );
					UpdateSkillTree();
					m_pSkillTreeDlg->ShowGuildSkillPointInfo();
					m_pSkillTreeDlg->ButtonStateSkillReservation();
				}
				else
				{
					GetSkillTask().Change_GuildSkillLevelDown( pSkill->GetClassID() );
					m_pSkillTreeDlg->UpdateGuildWarSkillTreeContent();
					m_pSkillTreeDlg->ShowGuildSkillPointInfo();
					m_pSkillTreeDlg->ButtonStateSkillReservation();
				}
			}

			if( drag::IsValid() )
			{
				CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
				if( pDragButton->GetSlotType() == ST_SKILL )
					drag::ReleaseControl();
			}
			return;
		}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		// 스킬 퀵슬롯 클릭 시.
		if( strstr( pControl->GetControlName(), "ID_SKILL" ) )
		{
			CDnSlotButton *pDragButton;
			CDnQuickSlotButton *pPressedButton;

			pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			pPressedButton = static_cast<CDnQuickSlotButton*>(pControl);

			CDnSkill *pSkill = static_cast<CDnSkill*>(pPressedButton->GetItem());

			// 스킬북 아이템이 있는지 찾아서 있으면 락을 해제시켜 줌.
			if( WM_RBUTTONUP == uMsg )
			{
				if (m_pSkillTreeDlg && pSkill && CDnSkillTask::IsActive() && pSkill->IsEnableTooltip())
				{
					int skillId = pSkill->GetClassID();
					const char* pSkillFileName = CDnSkillTask::GetInstance().GetSkillMovieName(skillId);
					std::string skillFileName;
					if (pSkillFileName != NULL)
						skillFileName = pSkillFileName;
					m_pSkillTreeDlg->ShowSkillMovieDlg(true, pSkill->GetName(),skillFileName);
				}
			}

			if( pDragButton == NULL )
			{
				if( !pSkill ) 
					return;
				
				if( !pSkill->IsAcquired() ) 
					return;
				if( m_bGuildWarSkillMode )
					return;				
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
				if (m_bTotalLevelSkillMode == false)
				{
					if( pSkill->GetSkillType() == CDnSkill::Passive || 
						pSkill->GetSkillType() == CDnSkill::AutoPassive || 
						pSkill->GetSkillType() == CDnSkill::EnchantPassive ) 
						return;
				}
				else
				{
					//통합 레벨 스킬은 패시브
					if( pSkill->GetSkillType() != CDnSkill::Passive )
						return;

					//사용 가능 한 스킬만 
					if (GetSkillTask().IsAvailableTotalSkill(CDnActor::s_hLocalActor, pSkill) == false)
						return;
				}
#else
				if( pSkill->GetSkillType() == CDnSkill::Passive || 
					pSkill->GetSkillType() == CDnSkill::AutoPassive || 
					pSkill->GetSkillType() == CDnSkill::EnchantPassive ) 
					return;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

				if( uMsg == WM_RBUTTONUP )
				{
					//CDnActor::s_hLocalActor->UseSkill( pSkill->GetClassID() );

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
					//통합 레벨 스킬 오른쪽 클릭으로 등록..
					if (m_bTotalLevelSkillMode == true)
					{
						if (GetSkillTask().IsAvailableTotalSkill(CDnActor::s_hLocalActor, pSkill) == true)
						{
							//빈슬롯이 존재 하고, 추가 가능 하다면, 통합레벨 스킬에 등록 요청
							GetSkillTask().AddTotalLevelSkill(pSkill->GetMySmartPtr());
						}
					}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
				}
				else
				{
					pPressedButton->EnableSplitMode(0);
					drag::SetControl(pPressedButton);

					// 전투모드 미리 변경. 물약과 달리 스킬은 집는거 자체가 퀵슬롯에 등록시키려고 할때밖에 없으므로.
					CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					if( !pActor->IsBattleMode() )
					{
						if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
						{
							if( pActor->IsCanBattleMode() )
							{
								if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
								pActor->CmdToggleBattle( true );
							}
						}
					}

					if( m_iDragSoundIndex != -1 )
						CEtSoundEngine::GetInstance().PlaySound( "2D", m_iDragSoundIndex );
				}
			}
			else
			{
				pDragButton->DisableSplitMode(false);
				drag::ReleaseControl();
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSkillTreeContentDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	if( EVENT_BUTTON_CLICKED == nCommand )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			if( m_iSelectedSlotIndex >= 0 )
			{
				// 아직 획득하지 않은 스킬일 때는 새로 얻도록 해준다.
				CDnSkill *pSkill = static_cast<CDnSkill*>(m_vlSkillSlot.at( m_iSelectedSlotIndex ).pSkillQuickSlot->GetItem());
				if( pSkill )
				{
					if( !m_bGuildWarSkillMode )
					{
						if( pSkill->IsLocked() )
						{
							// 돈으로 스킬 언락 패킷 보냄. 결과는 스킬북으로 언락할 때와 동일한 패킷으로 온다. (CDnSkillTask::OnRecvUnlockSkill)
							GetSkillTask().SendUnlockSkillByMoneyReq( pSkill->GetClassID() );
							m_pSkillUnlockDlg->Show( false );
						}
						else
						{
							if( pSkill->IsAcquired() )
							{
								// 스킬 레벨 업.
								// Note: 현재는 UI 에서 스킬레벨업 시에 한번에 한 레벨씩만 올리게 되어있음.
								// CDnSkillTask::OnRecvSkillAcquire() 에서 실패 패킷이 온 경우와 연관이 있으므로 그쪽도 수정해 줌

								pSkill->CalcLevelUpUsePoint( 1 );			// 서버에서 응답받고 SP 계산 후 처리하도록 수정. (#22216)
								GetSkillTask().SendSkillLevelUpReq( pSkill->GetClassID(), pSkill->GetUsePoint() );
								pSkill->ResetLevelUpUsePoint( 1 );
								m_pSkillLevelUpDlg->Show( false );
							}
							else
							{
								// 스킬 획득 요청 패킷 보냄.
								GetSkillTask().SendAcquireSkillReq( pSkill->GetClassID() );
								m_pSkillAcquireDlg->Show( false );
							}
						}
					}
					else
					{
						GetSkillTask().SendAcquireGuildWarSkillReq( pSkill->GetClassID(), pSkill->GetLevel() );
						m_pSkillAcquireDlg->Show( false );
					}
				}
			}
			return;
		}

		if( IsCmdControl( "ID_CANCEL" ) ) 
		{
			m_iSelectedSlotIndex = -1;
			m_pSkillUnlockDlg->Show( false );
			m_pSkillAcquireDlg->Show( false );
			m_pSkillLevelUpDlg->Show( false );
			return;
		}
	}
}

#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
void CDnSkillTreeContentDlg::RefreshToolTip(CDnSkill* pSkill)
{
	CDnTooltipDlg* pToolTipDlg = GetInterface().GetTooltipDialog();

	if( NULL == pToolTipDlg || NULL == pSkill )
	{
		return;
	}
	pToolTipDlg->RefreshTooltip(pSkill);
}
#endif

bool CDnSkillTreeContentDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 감싸고 있는 스킬트리 다이얼로그 영역 안에 있을 때만 메시지를 처리한다.
	if( m_pSkillTreeDlg && false == m_pSkillTreeDlg->IsMouseInDlg() )
		return false;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (m_bTotalLevelSkillMode == true)
	{
		switch(uMsg)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
			{
				// 채팅탭 세로 사이즈 조절
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );

				float fMouseX, fMouseY;
				PointToFloat( MousePoint, fMouseX, fMouseY );

				CDnTotalLevelSkillActivateListDlg* pTotalLevelSkillDlg = m_pSkillTreeDlg->GetTotalLevelSkillActivateListDlg();
				if (pTotalLevelSkillDlg)
				{
					SUICoord dlgCoord;
					pTotalLevelSkillDlg->GetDlgCoord(dlgCoord);
					dlgCoord.SetPosition( pTotalLevelSkillDlg->GetXCoord(), pTotalLevelSkillDlg->GetYCoord() );

					if( dlgCoord.IsInside( s_fScreenMouseX, s_fScreenMouseY ) )
					{
						return false;
					}
				}
			}
			break;
		}
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	switch( uMsg )
	{
		case WM_MOUSEWHEEL:
			{
				if( IsMouseInDlg() )
				{
					UINT uLines;
					SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
					int iScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
					m_pSkillTreeDlg->OnMouseWheel( iScrollAmount );
				}
			}
			break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}


void CDnSkillTreeContentDlg::SetUnlockByMoneyMode( bool bUnlockByMoneyMode )
{
	if( m_bUnlockByMoneyMode != bUnlockByMoneyMode )
	{
		m_bUnlockByMoneyMode = bUnlockByMoneyMode;
		UpdateSkillTree();
	}
}

bool CDnSkillTreeContentDlg::IsUnlockByMoneyMode()
{
	return m_bUnlockByMoneyMode;
}

void CDnSkillTreeContentDlg::CloseChildDialog( void )
{
	if( m_pSkillUnlockDlg && m_pSkillUnlockDlg->IsShow() ) m_pSkillUnlockDlg->Show( false );
	if( m_pSkillLevelUpDlg && m_pSkillLevelUpDlg->IsShow() ) m_pSkillLevelUpDlg->Show( false );
	if( m_pSkillAcquireDlg && m_pSkillAcquireDlg->IsShow() ) m_pSkillAcquireDlg->Show( false );
}

void CDnSkillTreeContentDlg::UnLockSkill( CDnSkill * pSkill )
{
	if( NULL != pSkill && pSkill->IsLocked() )
	{
		m_pSkillUnlockDlg->SetSkill( pSkill );
		m_pSkillUnlockDlg->SetPrice( static_cast<CDnSkill*>(pSkill)->GetUnlockPrice() );

		m_pSkillUnlockDlg->Show( true );

		HideSlotInformation();
	}	
}

void CDnSkillTreeContentDlg::ShowSlotInformation( const float fX, const float fY )
{
	Hide_Highlight();

	CDnSimpleTooltipDlg * pSimpleTooltipDlg = dynamic_cast<CDnSimpleTooltipDlg *>(CEtUIDialog::s_pTooltipDlg);

	if( NULL == pSimpleTooltipDlg )
		return;

	bool bControlTooltip = false;

	for( DWORD itr = 0; itr < m_vlSkillSlot.size(); ++itr )
	{
		if( NULL == m_vlSkillSlot[itr].pSkillQuickSlot )
			continue;

		if( CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillLevel
			|| CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillUpButton
			|| CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillLockButton
			|| CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pExclusiveSkillLockButton 
			|| CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillQuickSlot 
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			|| CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillDownButton
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			)
		{
			if( CEtUIDialog::s_pMouseEnterControl != m_vlSkillSlot[itr].pSkillQuickSlot )
				Show_SlotTooltipDlg( fX, fY, m_vlSkillSlot[itr].pSkillQuickSlot );

			if( CEtUIDialog::s_pMouseEnterControl == m_vlSkillSlot[itr].pSkillLockButton && m_vlSkillSlot[itr].pSkillLockButton->GetResult() == CDnSkillTreeSystem::R_ONLY_SKILL_BOOK )
			{
				pSimpleTooltipDlg->SetDelayTime( SHOW_SKILL_BOOK_TOOLTIP_TIME );
			}
			else
			{
				pSimpleTooltipDlg->SetDelayTime( SHOW_TOOLTIP_TIME );
			}
	
			Show_Highlight( m_vlSkillSlot[itr].pSkillQuickSlot );
			bControlTooltip = true;
			break;
		}
	}

	if( false == bControlTooltip )
		HideSlotInformation();
}

void CDnSkillTreeContentDlg::Show_SlotTooltipDlg( const float fX, const float fY, CDnQuickSlotButton * pControl )
{
	CDnTooltipDlg * pTooltipDlg = GetInterface().GetTooltipDialog();

	if( NULL == pTooltipDlg || NULL == pControl )
		return;

	pControl->MouseEnter( true );
	pTooltipDlg->ShowTooltip( pControl, fX, fY );
}

void CDnSkillTreeContentDlg::Show_Highlight( CDnQuickSlotButton * pControl )
{
	if( NULL == pControl )
		return;

	MIInventoryItem * pInvenItem = pControl->GetItem();
	CDnSkill * pSkill = dynamic_cast<CDnSkill*>(pInvenItem);

	if( NULL == pSkill )
		return;

	Show_Highlight_ParentSkill( pSkill );
	Show_Highlight_SP_Point( pSkill );
}

void CDnSkillTreeContentDlg::Show_Highlight_ParentSkill( CDnSkill * pSkill )
{
	const CDnSkillTreeSystem::S_NODE_RENDER_INFO & NodeRenderInfo = pSkill->GetNodeRenderInfo();

	if( false == NodeRenderInfo.vlParentSkillIDs.empty() )
	{
		for( DWORD itr = 0; itr < NodeRenderInfo.vlParentSkillIDs.size(); ++itr )
		{
			const int nParentSkillID = NodeRenderInfo.vlParentSkillIDs.at(itr);
			const int nParentSkillLevel = NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(itr);
			const int nParentSkillJob = NodeRenderInfo.vlParentSkillJobIDs.at(itr);

			if( true == IsParentSkill_Complete( nParentSkillID, nParentSkillLevel ) )
				continue;

			bool bSkill_Highlight = false;
			for( DWORD iSlot = 0; iSlot < m_vlSkillSlot.size(); ++iSlot )
			{
				S_SKILL_SLOT_UI & SkillSlot = m_vlSkillSlot.at( iSlot );
				MIInventoryItem * pInvenItem = SkillSlot.pSkillQuickSlot->GetItem();
				CDnSkill * pSlotSkill = dynamic_cast<CDnSkill *>(pInvenItem);

				if( NULL == pSlotSkill )
					continue;
				
				if( nParentSkillID == pSlotSkill->GetClassID() )
				{
					SkillSlot.pHighlightStatic->Show( true );
					bSkill_Highlight = true;
					break;
				}
			}

			if( false == bSkill_Highlight )
				m_pSkillTreeDlg->_Show_Highlight_Job( nParentSkillJob );
		}
	}
}

void CDnSkillTreeContentDlg::Show_Highlight_SP_Point( CDnSkill * pSkill )
{
	std::vector<int> nNeedSPValues;

	GetSkillTask().GetNeedSPValuesByJob( pSkill->GetClassID(), nNeedSPValues );

	m_pSkillTreeDlg->_Show_Highlight_SP( nNeedSPValues );
}

bool CDnSkillTreeContentDlg::IsParentSkill_Complete( const int nParentSkillID, const int nParentSkillLevel )
{
	DnSkillHandle hParentSkill = GetSkillTask().FindSkill( nParentSkillID );

	if( NULL == hParentSkill )
	{
		hParentSkill = GetSkillTask().FindLockedSkill( nParentSkillID );

		if( NULL == hParentSkill )
			return false;
	}

	if( false == hParentSkill->IsAcquired() 
		|| true == hParentSkill->IsLocked() 
		|| hParentSkill->GetLevel() < nParentSkillLevel )
		return false;

	return true;
}

bool CDnSkillTreeContentDlg::IsSP_Complete( DnSkillHandle hParentSkill )
{
	return true;
}

void CDnSkillTreeContentDlg::HideSlotInformation()
{
	Hide_SlotTooltipDlg();
	Hide_Highlight();
}

void CDnSkillTreeContentDlg::Hide_SlotTooltipDlg()
{
	CDnTooltipDlg * pTooltipDlg = GetInterface().GetTooltipDialog();

	if( NULL == pTooltipDlg )
		return;
	
	pTooltipDlg->HideTooltip();
}

void CDnSkillTreeContentDlg::Hide_Highlight()
{
	for( DWORD iSlot = 0; iSlot < m_vlSkillSlot.size(); ++iSlot )
	{
		S_SKILL_SLOT_UI & SkillSlot = m_vlSkillSlot.at( iSlot );
		SkillSlot.pHighlightStatic->Show( false );
	}

	m_pSkillTreeDlg->_Hide_Highlight_Job();
	m_pSkillTreeDlg->_Hide_Highlight_SP();
}

void CDnSkillTreeContentDlg::Update_SkillSlotUI( CDnSkillTask::S_SKILLTREE_SLOT_INFO & SlotInfo, const int nSlotIndex )
{
	wchar_t szTemp[ 36 ] = {0,};

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	DnSkillHandle hReservationSkill = GetSkillTask().GetReservationSkill( SlotInfo.hSkill->GetClassID() );
	DnSkillHandle hBackupSkill( CDnSkill::Identity() );

	if( hReservationSkill )
	{
		hBackupSkill = SlotInfo.hSkill;
		SlotInfo.hSkill = hReservationSkill;
	}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

	// 락/언락 상태 구분하여 보여주고, 획득한 스킬이 아니면 레벨은 0으로.
	S_SKILL_SLOT_UI& SkillSlotUI = m_vlSkillSlot.at( nSlotIndex );
	SkillSlotUI.pSkillQuickSlot->SetQuickItem( SlotInfo.hSkill.GetPointer() );

	// 패시브/오토패시브 스킬과 액티브 스킬이 구분됨.
	if( CDnSkill::Active == SlotInfo.hSkill->GetSkillType() )
	{
		SkillSlotUI.pSlotBase->Show( true );
		SkillSlotUI.pPassiveSlotBase->Show( false );
	}
	else
	{
		SkillSlotUI.pSlotBase->Show( false );
		SkillSlotUI.pPassiveSlotBase->Show( true );
	}

	SkillSlotUI.pSkillQuickSlot->Show( true );
	SkillSlotUI.pSkillLevel->Show( true );
	SkillSlotUI.pSkillLockButton->Show( true );
	SkillSlotUI.pSkillLockButton->Enable( true );
	SkillSlotUI.pSkillUpButton->Show( true );
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	SkillSlotUI.pSkillDownButton->Show( false );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

	this->ShowTooltipDlg( SkillSlotUI.pSkillUpButton, false );

	// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
	bool bAlreadyGlobalSkillAcquired = GetSkillTask().HasSameGlobalIDSkill( SlotInfo.hSkill );


#ifdef PRE_FIX_CLIENT_SKILL_MAX
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int nMaxSkillLevel = pSkillTable->GetFieldFromLablePtr( SlotInfo.hSkill->GetClassID(), "_MaxLevel" )->GetInteger();
#else
	int nMaxSkillLevel = SlotInfo.hSkill->GetMaxLevel();
#endif

	// 최대 1렙밖에 없는 리셋된 상태의 스킬
	bool bSingleLevelResettedSkill = ((1 == nMaxSkillLevel) && GetSkillTask().FindZeroLevelSkill( SlotInfo.hSkill->GetClassID() ));

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	bSingleLevelResettedSkill = ( bSingleLevelResettedSkill && !GetSkillTask().GetReservationSkill( SlotInfo.hSkill->GetClassID() ) );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	if( SlotInfo.hSkill->IsLocked() )
	{
		SkillSlotUI.pSkillUpButton->Show( false );
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		SkillSlotUI.pSkillDownButton->Show( false );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		// 배울 수 없는 상황이면 아무것도 안보임.
		CDnSkillTreeSystem::RESULT_CODE eResult = GetSkillTask().CanUnlockThis( SlotInfo.hSkill );

		bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == eResult) ||
			(CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == eResult) ||
			(CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == eResult) ) &&
			true == bAlreadyGlobalSkillAcquired;

		if( CDnSkillTreeSystem::R_SUCCESS == eResult ||
			true == bIgnoreParentSkillCondition )
		{
			SkillSlotUI.pSkillLockButton->Show( true );

			SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1144 ) );
		}
		else 
		{
			switch( eResult ) 
			{
			case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
			case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
				// 배울 수 없는 상황에서 다른 건 만족하나 선행스킬이 없어서 못배우는 경우엔 회색 깃발 표시
				SkillSlotUI.pSkillLockButton->Enable( false );

				// 필요한 선행 스킬과 레벨은 하나만 표시
				SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1150 ) );	
				break;
			case CDnSkillTreeSystem::R_EXCLUSIVE_SKILL:
				SkillSlotUI.pSkillLockButton->Enable( false );
				SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1153 ) );
				break;
			case CDnSkillTreeSystem::R_ONLY_SKILL_BOOK:
				SkillSlotUI.pSkillLockButton->Enable( false );
#ifdef PRE_ADD_EXSKILL_TOOLTIP
				if( nSlotIndex == 13 )
					SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1127 ) );
				else
					SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1126 ) );
#else // PRE_ADD_EXSKILL_TOOLTIP
				SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1126 ) );
#endif // PRE_ADD_EXSKILL_TOOLTIP
				break;
			default:
				SkillSlotUI.pSkillLockButton->Show( false );
				break;
			}

			SkillSlotUI.pSkillLockButton->SetResult( (int)eResult );

		}
	}
	else
	{	
		SkillSlotUI.pSkillLockButton->Show( false );

		// 최고 레벨 직전까지 스킬 레벨업 버튼 보여준다.
		// 하지만 최대 레벨에 1 이고 언락 0 레벨 상태라면 보여준다.

#ifdef PRE_FIX_CLIENT_SKILL_MAX
		DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		int nMaxSkillLevel = pSkillTable->GetFieldFromLablePtr( SlotInfo.hSkill->GetClassID(), "_MaxLevel" )->GetInteger();
#else
		int nMaxSkillLevel = SlotInfo.hSkill->GetMaxLevel();
#endif

		if( bSingleLevelResettedSkill || ( SlotInfo.hSkill->GetLevel() < nMaxSkillLevel ) )
		{
			SkillSlotUI.pSkillUpButton->Show( true );
		}
		else
		{
			SkillSlotUI.pSkillUpButton->Show( false );
		}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		const int nSkillState = GetSkillTask().IsReservationSkillDown( SlotInfo.hSkill->GetClassID() );
		if( CDnSkillTask::eSkillMinus_Enable == nSkillState )
		{
			DnSkillHandle hLevelDownSkill = CDnSkill::CreateSkill( CDnActor::Identity(), SlotInfo.hSkill->GetClassID(), SlotInfo.hSkill->GetLevel() - 1 );

			if( hLevelDownSkill )
			{
				int nNextLevelUpSkillPoint = 0;
				if( 0 == SlotInfo.hSkill->GetLevel() - 1 )
					nNextLevelUpSkillPoint = hLevelDownSkill->GetNeedAcquireSkillPoint();
				else
					nNextLevelUpSkillPoint = hLevelDownSkill->GetNextLevelSkillPoint();

				WCHAR wszString[256] = {0,};
				swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5191 ), nNextLevelUpSkillPoint );

				SkillSlotUI.pSkillDownButton->Show( true );
				SkillSlotUI.pSkillDownButton->Enable( true );
				SkillSlotUI.pSkillDownButton->SetTooltipText( wszString );
			}
		}
		else if( CDnSkillTask::eSkillMinus_Disable == nSkillState )
		{
			SkillSlotUI.pSkillDownButton->Show( true );
			SkillSlotUI.pSkillDownButton->Enable( false );
			SkillSlotUI.pSkillDownButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1124 ) );	// UISTRING: 연계된 스킬이 있습니다.
		}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	}

	if( SlotInfo.hSkill->IsAcquired() )
	{
		//rlkt 2016
		//swprintf_s( szTemp, _countof(szTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 728 ), SlotInfo.hSkill->GetLevel());
		swprintf_s( szTemp, _countof(szTemp), L"%d/%d", SlotInfo.hSkill->GetLevel(), SlotInfo.hSkill->GetMaxLevel() );
		SkillSlotUI.pSkillQuickSlot->SetForceUsable( true );
		SkillSlotUI.pSkillLevel->SetText( szTemp );
	}
	else
	{
		//swprintf_s( szTemp, _countof(szTemp), L"%d", 0 );
		// 레벨 0이면 표시하지 않도록 수정됨.
		SkillSlotUI.pSkillQuickSlot->SetForceUnUsable( true );
		SkillSlotUI.pSkillLevel->Show( false );
	}

	// 스킬 레벨업이 가능한지 여부에 따라 enable/disable 시켜줌.
	// 장비 아이템은 스킬 레벨업 불가능.
	SkillSlotUI.pSkillUpButton->Enable( false );
	if( (bSingleLevelResettedSkill || (SlotInfo.hSkill->GetLevel() < SlotInfo.hSkill->GetMaxLevel())) && 
		!SlotInfo.hSkill->IsEquipItemSkill() )
	{
		int iSkillPoint = GetSkillTask().GetAvailSkillPointForThisJob( m_pSkillTreeDlg->GetSelectedJobID() );

		//if( iSkillPoint <= 0 ) 
		//	continue;

		// 궁극기 중복 안되게하는 아이디에 걸리는지 확인. 걸린다면
		// "다른 궁극스킬을 습득하여 스킬을 배울 수 없습니다." 1153 번 메시지 툴팁에 표시
		bool bExclusiveSkill = GetSkillTask().IsExclusiveSkill( SlotInfo.hSkill->GetClassID(), SlotInfo.hSkill->GetExclusiveID() );
		if( bExclusiveSkill )
		{
			SkillSlotUI.pSkillUpButton->Enable( false );
			SkillSlotUI.pSkillUpButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1153 ) );
			return;
		}

		// 토글이 켜져 있거나 오라가 발동된 상태에서는 스킬 레벨업 불가능.
		if( SlotInfo.hSkill->IsToggleOn() || SlotInfo.hSkill->IsAuraOn() )
		{
			SkillSlotUI.pSkillLockButton->Enable( false );
			SkillSlotUI.pSkillUpButton->Enable( false );
			return;
		}

		// 부모 스킬을 얻은 상태가 아니라면 스킬 획득/레벨업 버튼 비활성화.
		bool bHasParentSkills = true;
		int iNumParentSkill = (int)SlotInfo.NodeRenderInfo.vlParentSkillIDs.size();
		for( int iParentSkill = 0; iParentSkill < iNumParentSkill; ++iParentSkill )
		{
			int iParentSkillID = SlotInfo.NodeRenderInfo.vlParentSkillIDs.at( iParentSkill );
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			DnSkillHandle hParentSkill = GetSkillTask().GetReservationSkill( iParentSkillID );
			if( !hParentSkill )
				hParentSkill = GetSkillTask().FindSkill( iParentSkillID );
#else	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			DnSkillHandle hParentSkill = CDnActor::s_hLocalActor->FindSkill( iParentSkillID );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

			int iSkillLevel = 0;

			if (hParentSkill)
			{
				iSkillLevel = hParentSkill->GetLevel();
				iSkillLevel -= hParentSkill->GetLevelUpValue();
			}
			if( !hParentSkill ||
				iSkillLevel < SlotInfo.NodeRenderInfo.pAcquireCondition->vlParentSkillNeedLevel.at(iParentSkill) )
			{
				bHasParentSkills = false;
				break;
			}
		}

		if( false == bHasParentSkills && false == bAlreadyGlobalSkillAcquired )
		{
			SkillSlotUI.pSkillUpButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1817 ) );
			//SkillSlotUI.pSkillLockButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1817 ) );
			return;
		}

#if defined( PRE_MOD_68531_NEW )
		DnSkillHandle hLevelUpSkill = CDnSkill::Identity();
		if( CDnActor::s_hLocalActor )
		{
			CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if( NULL != pPlayer )
			{
				const int nSkillID = SlotInfo.hSkill->GetClassID();
				const int nSkillLevel = SlotInfo.hSkill->GetLevel();
				const int nLevelUp = pPlayer->GetSkillLevelUpValue( nSkillID );
				if( 0 != nLevelUp )
				{
					const int nRealLevel = nSkillLevel - nLevelUp;
					hLevelUpSkill = SlotInfo.hSkill;
					SlotInfo.hSkill = CDnSkill::CreateSkill( CDnActor::Identity(), nSkillID, nRealLevel );
				}
			}
		}
#endif	// #if defined( PRE_MOD_68531_NEW )

#if defined(PRE_FIX_NEXTSKILLINFO)
#else
		const CDnSkill::SkillInfo* pSkillInfo = SlotInfo.hSkill->GetInfo();
#endif // PRE_FIX_NEXTSKILLINFO
		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
		int nLevelLimit = 0;

#if defined(PRE_FIX_NEXTSKILLINFO)
		int iSkillLevelTableID = -1;
		int iMinSkillLevelTableID = -1;
		int iNextSkillLevelTableID = -1;

		SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(SlotInfo.hSkill->GetClassID(), SlotInfo.hSkill->GetSelectedLevelDataType());

		//현재 스킬 레벨의 테이블 ID
		SKILL_LEVEL_TABLE_IDS::iterator findIter = pTableInfo->_SkillLevelTableIDs.find(SlotInfo.hSkill->GetLevel());
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			iSkillLevelTableID = findIter->second;

		//Min레벨의 테이블 ID
		findIter = pTableInfo->_SkillLevelTableIDs.find(pTableInfo->_MinLevel);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			iMinSkillLevelTableID = findIter->second;

		//다음 레벨 테이블 ID
		findIter = pTableInfo->_SkillLevelTableIDs.find(SlotInfo.hSkill->GetLevel() + 1);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			iNextSkillLevelTableID = findIter->second;

		// 스킬 미획득 상태에서는 스킬레벨 테이블의 1렙에 해당되는 레벨 제한 값이 필요하다.
		if( SlotInfo.hSkill->IsAcquired() )
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_LevelLimit" )->GetInteger();
		else
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iMinSkillLevelTableID, "_LevelLimit" )->GetInteger();

#else
		// 스킬 미획득 상태에서는 스킬레벨 테이블의 1렙에 해당되는 레벨 제한 값이 필요하다.
		if( SlotInfo.hSkill->IsAcquired() )
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset+SlotInfo.hSkill->GetLevel(), "_LevelLimit" )->GetInteger();
		else
			nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( pSkillInfo->iSkillLevelIDOffset, "_LevelLimit" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

		bool bLevelLimit = (CDnActor::s_hLocalActor->GetLevel() >= nLevelLimit);
		bool bNeedSkillPoint = false;
		int nNeedSkillPoint = 0;
		if( SlotInfo.hSkill->IsAcquired() )
			nNeedSkillPoint = SlotInfo.hSkill->GetNextLevelSkillPoint();
		else
			nNeedSkillPoint = SlotInfo.hSkill->GetNeedAcquireSkillPoint();

		bNeedSkillPoint = (iSkillPoint >= nNeedSkillPoint);

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
		bool bAvailableSPByJob = GetSkillTask().IsAvailableSPByJob(CDnActor::s_hLocalActor, SlotInfo.NodeRenderInfo.vlNeedJobSP);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

		if( bLevelLimit && bNeedSkillPoint 
#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			&& bAvailableSPByJob
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
			)
		{
			// 스킬 언락 상태에서.. "스킬 레벨을 올립니다."
			SkillSlotUI.pSkillUpButton->Enable( true );

			WCHAR szTemp[512] = {0,};
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5190 ), nNeedSkillPoint );
#else	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			swprintf_s( szTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5117 ), nNeedSkillPoint );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

			SkillSlotUI.pSkillUpButton->SetTooltipText( szTemp );
		}
		else
		{
			// "레벨이 맞지 않음. 레벨이 낮습니다. or SP 가 부족합니다."
			if( !bLevelLimit )
			{
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
				if( 0 != GetSkillTask().GetReservationSkillNeedSP() )
				{
					//SkillSlotUI.pSkillUpButton->Show( true );
					//SkillSlotUI.pSkillUpButton->Enable( false );
				}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
				SkillSlotUI.pSkillUpButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1145 ) );
			}
			else
				if( !bNeedSkillPoint )
					SkillSlotUI.pSkillUpButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1151 ) );
		}

#if defined( PRE_MOD_68531_NEW )
		if( CDnSkill::Identity() != hLevelUpSkill )
		{
			SAFE_RELEASE_SPTR( SlotInfo.hSkill );
			SlotInfo.hSkill = hLevelUpSkill;
		}
#endif	// #if defined( PRE_MOD_68531_NEW )
	}
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	// 백업 스킬정보가 있다면 SlotInfo.hSkill에 다시 백업 스킬을 넣어준다.
	if( hBackupSkill )
		SlotInfo.hSkill = hBackupSkill;
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
}

bool CDnSkillTreeContentDlg::FindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord )
{
	// 스킬트리컨텐츠 다이얼로그의 경우엔 ScissorRect을 이용해서 렌더링하기때문에, 컨트롤을 찾은 후 스크롤상태를 확인해야한다.
	bool bRet = false;
	std::vector< CEtUIControl* > vecTempControl;
	bRet = CEtUIDialog::FindControl( vecTempControl, nTypeCount, pType, bCheckCoveredControl, vecDlgCoord );

	// 렌더링되는 영역 구해서
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	SUICoord DlgRenderCoord;
	pSkillTreeDlg->GetContentDialogCoord( DlgRenderCoord );
	DlgRenderCoord = pSkillTreeDlg->DlgCoordToScreenCoord( DlgRenderCoord );

	// 골라진 컨트롤 중에서도 렌더링 영역 안에 들어있는 컨트롤을 골라내 vecControl에 추가한다.
	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )vecTempControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = vecTempControl[i];

		SUICoord ControlCoord;
		pControl->GetUICoord( ControlCoord );
		ControlCoord.fX = ControlCoord.fX + ControlCoord.fWidth/2.0f;
		ControlCoord.fY = ControlCoord.fY + ControlCoord.fHeight/2.0f;
		ControlCoord = DlgCoordToScreenCoord( ControlCoord );

		if( ControlCoord.fX >= DlgRenderCoord.fX && ControlCoord.fY >= DlgRenderCoord.fY &&
			ControlCoord.fX <= DlgRenderCoord.fX+DlgRenderCoord.fWidth && ControlCoord.fY <= DlgRenderCoord.fY+DlgRenderCoord.fHeight )
		{
			vecControl.push_back( pControl );
		}
	}
	return bRet;
}

#ifdef PRE_ADD_TOTAL_LEVEL_SKILL
void CDnSkillTreeContentDlg::UpdateTotalLevelSkillTree( bool bSetTotalLevelSkill )
{
	_ASSERT( MAX_SKILL_SLOT_COUNT == (int)m_vlSkillSlot.size() );

	// 슬롯의 값들을 리셋함. 나중엔 스킬의 자리가 옮겨지지 않기 때문에 락 언락만 업데이트 해주면 될 듯.
	int iNumSkillSlot = (int)m_vlSkillSlot.size();
	for( int iSlot = 0; iSlot < iNumSkillSlot; ++iSlot )
	{
		S_SKILL_SLOT_UI& SkillSlot = m_vlSkillSlot.at( iSlot );
		SkillSlot.Reset();
	}
	SAFE_RELEASE_SPTR( m_hTreeTexture );	// 스킬 트리 라인 삭제

	vector<DnSkillHandle> vlhTotalLevelSkillList;
	CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = NULL;
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pTotalLevelSkillSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;

		if (pTotalLevelSkillSystem)
			vlhTotalLevelSkillList = pTotalLevelSkillSystem->GetTotalLevelSkillList();
	}
	vector<DnSkillHandle>::iterator iter = vlhTotalLevelSkillList.begin();

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TTOTALLEVELSKILL );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid BattleGroundSkillTree" );
		return;
	}

	for( ; iter != vlhTotalLevelSkillList.end(); iter++ )
	{
		DnSkillHandle hSkill = (*iter);
		if( hSkill == NULL ) continue;

		int nItemID = pSox->GetItemIDFromField( "_SkillTableID", hSkill->GetClassID() );
		int iSlot = pSox->GetFieldFromLablePtr( nItemID, "_TreeSlotIndex" )->GetInteger();

		S_SKILL_SLOT_UI& SkillSlotUI = m_vlSkillSlot.at( iSlot );
		SkillSlotUI.pSkillQuickSlot->SetQuickItem( (*iter).GetPointer() );

		SkillSlotUI.pSkillUpButton->Show( false );
		SkillSlotUI.pSkillUpButton->Enable( false );
		
		SkillSlotUI.pSlotBase->Show( false );
		SkillSlotUI.pPassiveSlotBase->Show( true );
		SkillSlotUI.pSkillQuickSlot->Show( true );
		SkillSlotUI.pSkillLevel->Show( false );
		SkillSlotUI.pSkillLockButton->Show( false );
		
		SkillSlotUI.pSkillLevel->Show( false );
		SkillSlotUI.pSkillLockButton->Show( false );
		SkillSlotUI.pExclusiveSkillLockButton->Show( false );
		SkillSlotUI.pSkillUpButton->Show( false );
		SkillSlotUI.pHighlightStatic->Show( false );

		bool isUsable = false;
		if (pTotalLevelSkillSystem)
		{
			isUsable = pTotalLevelSkillSystem->IsUsableSkill(hSkill);
		}

		if (isUsable)
			SkillSlotUI.pSkillQuickSlot->SetForceUsable( isUsable );
		else
			SkillSlotUI.pSkillQuickSlot->SetForceUnUsable( true );

		this->ShowTooltipDlg( SkillSlotUI.pSkillUpButton, false );
	}

	m_bGuildWarSkillMode = false;

	m_bTotalLevelSkillMode = true;

}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL