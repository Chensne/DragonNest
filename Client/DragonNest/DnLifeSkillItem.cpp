#include "StdAfx.h"
#include "DnLifeSkillItem.h"
#include "DnInterface.h"
#include "DnSkillTask.h"
#include "TaskManager.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnLifeSkillItem::CDnLifeSkillItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pName( NULL )
, m_pDescription( NULL )
, m_pLevel( NULL )
, m_pButtonDoSkill( NULL )
, m_pProgress( NULL )
, m_pPercent( NULL )
, m_pCount( NULL )
, m_pSkillButton( NULL )
{
	memset( &m_stLifeSkillInfoData, 0, sizeof(stLifeSkillInfoData) );
}

CDnLifeSkillItem::~CDnLifeSkillItem()
{
	if( drag::IsValid() )
	{
		CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
		if( pDragButton->GetSlotType() == ST_SECONDARYSKILL )
		{
			drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}
	}
}

void CDnLifeSkillItem::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeLifeDlg.ui" ).c_str(), bShow );
	SetElementDialog( true );
}

void CDnLifeSkillItem::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pName = GetControl<CEtUIStatic>( "ID_TEXT_TITLE" );
	m_pDescription = GetControl<CEtUIStatic>( "ID_TEXT_DETAIL" );
	m_pLevel = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pButtonDoSkill = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pProgress = GetControl<CEtUIProgressBar>( "ID_PRB_GAUGE" );
	m_pCount = GetControl<CEtUIStatic>( "ID_TEXT_COUNT0" );
	m_pPercent = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );
	m_pSkillButton = GetControl<CDnLifeSkillButton>( "ID_BUTTON0" );
	m_pSkillButton->SetSlotType( ST_SECONDARYSKILL );
}

void CDnLifeSkillItem::SetInfo( stLifeSkillInfoData& stInfoData )
{
	m_stLifeSkillInfoData.m_nSkillID = stInfoData.m_nSkillID;
	m_stLifeSkillInfoData.m_nLevel = stInfoData.m_nLevel;
	m_stLifeSkillInfoData.m_nExp = stInfoData.m_nExp;
	m_stLifeSkillInfoData.m_eGrade = stInfoData.m_eGrade;
	m_stLifeSkillInfoData.m_strName = stInfoData.m_strName;
	m_stLifeSkillInfoData.m_strDescription = stInfoData.m_strDescription;
	m_stLifeSkillInfoData.m_strDoSkill = stInfoData.m_strDoSkill;
	m_stLifeSkillInfoData.m_fpDoSkill = stInfoData.m_fpDoSkill;

	SetUIInfo();
}

void CDnLifeSkillItem::SetUIInfo()
{
	// UI 셋팅
	m_pName->SetText( m_stLifeSkillInfoData.m_strName );
	m_pDescription->SetText( m_stLifeSkillInfoData.m_strDescription );

	WCHAR wszGrade[64] = {0,};
	int nGradeStringIndex = 0;
	switch( m_stLifeSkillInfoData.m_eGrade )
	{
		case SecondarySkill::Grade::Beginning:
			nGradeStringIndex = 7303;
			break;
		case SecondarySkill::Grade::Intermediate:
			nGradeStringIndex = 7304;
			break;
		case SecondarySkill::Grade::Advanced:
			nGradeStringIndex = 7305;
			break;
	}
	swprintf_s( wszGrade, _countof( wszGrade ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nGradeStringIndex ) );
	WCHAR wszLevel[64] = {0,};
	swprintf_s( wszLevel, _countof( wszLevel ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3762 ), m_stLifeSkillInfoData.m_nLevel );
	WCHAR wszText[128] = {0,};
	swprintf_s( wszText, _countof( wszText ), L"%s %s", wszGrade, wszLevel );
	m_pLevel->SetText( wszText );

	if( m_stLifeSkillInfoData.m_strDoSkill.size() > 0 )
		m_pButtonDoSkill->SetText( m_stLifeSkillInfoData.m_strDoSkill );
	else
		m_pButtonDoSkill->Show( false );

	float fAchievementRate = GetAchievementRate();
	WCHAR wszPercent[64] = {0,};
	WCHAR wszPercentText[64] = {0,};
	swprintf_s( wszPercent, _countof( wszPercent ), L"%.2f", fAchievementRate );
	swprintf_s( wszPercentText, _countof( wszPercentText ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7307 ), wszPercent );
	m_pPercent->SetText( wszPercentText );

	m_pProgress->SetProgress( fAchievementRate );

	m_pSkillButton->SetSecondarySkillInfo( m_stLifeSkillInfoData.m_nSkillID );
}

float CDnLifeSkillItem::GetAchievementRate()
{
	DNTableFileFormat* pSoxSkillLevel = GetDNTable( CDnTableDB::TSecondarySkillLevel );
	if( pSoxSkillLevel == NULL )
		return 0.0f;


	std::vector<int> vecID;
	for( int i=0; i<pSoxSkillLevel->GetItemCount(); i++ )
		vecID.push_back( pSoxSkillLevel->GetItemID( i ) );

	float fAchievementRate = 0.0f;

	int nTotalSkillExp = 0;	// 현제 보조스킬 레벨 이전까지의 경험치 합
	int nTableSkillExp = 0;	// 현제 보조스킬 레벨의 필요 경험치

	std::vector<int>::iterator iter = vecID.begin();
	for( ; iter!= vecID.end(); iter++ )
	{
		if( m_stLifeSkillInfoData.m_nSkillID == pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillID" )->GetInteger() )
		{
			SecondarySkill::Grade::eType eSkillGrade = (SecondarySkill::Grade::eType)pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillClass" )->GetInteger();
			int nSkillLevel = pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillLevel" )->GetInteger();

			nTableSkillExp = pSoxSkillLevel->GetFieldFromLablePtr( (*iter), "_SecondarySkillExp" )->GetInteger();
			if( eSkillGrade == m_stLifeSkillInfoData.m_eGrade && nSkillLevel == m_stLifeSkillInfoData.m_nLevel )
				break;
			else
				nTotalSkillExp += nTableSkillExp;
		}
	}

	WCHAR wszCount[64] = {0,};
	swprintf_s( wszCount, _countof( wszCount ), L"%d / %d", m_stLifeSkillInfoData.m_nExp - nTotalSkillExp, nTableSkillExp );
	m_pCount->SetText( wszCount );

	if( m_stLifeSkillInfoData.m_nExp > nTotalSkillExp && nTableSkillExp > 0 )
		fAchievementRate = (( static_cast<float>( m_stLifeSkillInfoData.m_nExp - nTotalSkillExp ) / static_cast<float>( nTableSkillExp ) ) * 100.0f);

	return fAchievementRate;
}

void CDnLifeSkillItem::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_OK" ) )
		{
			if( m_stLifeSkillInfoData.m_fpDoSkill )
				(GetSkillTask().*m_stLifeSkillInfoData.m_fpDoSkill)();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_DEL" ) )
		{
			if( GetSkillTask().CheckAvailableDeleteLifeSkill( m_stLifeSkillInfoData.m_nSkillID ) )
			{
				WCHAR wzStrTmp[1024]={0,};
				swprintf_s( wzStrTmp, _countof( wzStrTmp ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7315 ), m_stLifeSkillInfoData.m_strName.c_str() );
				GetInterface().MessageBox( wzStrTmp, MB_YESNO, 0, this );
			}
		}
		else if( IsCmdControl("ID_BUTTON0" ) )
		{
			CDnSlotButton *pDragButton;
			CDnLifeSkillButton *pPressedButton;

			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnLifeSkillButton * )pControl;

			if( pDragButton == NULL )
			{
				if ( !pPressedButton->GetItemID() )
					return;

				if( !pPressedButton->IsUsable() )
					return;

				if( (int)m_stLifeSkillInfoData.m_strDoSkill.size() == 0 )
					return;

				pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
				drag::SetControl( pPressedButton );

				// 라이프스킬을 집었을땐 비전투모드로 바꿔주자.
				CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if( pActor->IsBattleMode() )
				{
					if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
					{
						if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
						pActor->CmdToggleBattle(false);
					}
				}
			}
			else
			{
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();
			}
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnLifeSkillItem::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nID == 0 )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
				if( pSkillTask  )
					pSkillTask->SendLifeSkillDelete( m_stLifeSkillInfoData.m_nSkillID );
			}
		}
	}
}
