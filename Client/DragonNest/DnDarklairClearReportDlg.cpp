#include "StdAfx.h"
#include "DnDarklairClearReportDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnStageClearCounterMng.h"
#include "DnDLGameTask.h"
#include "DnDarklairClearImp.h"
#include "TaskManager.h"
#include "DnTableDB.h"
#include "DnStageClearRewardGoldDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDarklairClearReportDlg::CDnDarklairClearReportDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_RoundShowTime(0.1f)
, m_CheckShowTime(0.3f)
, m_RoundInitScale(3.f)
, m_CheckInitScale(3.f)
, m_RoundAfterShowTime(1.0f)
, m_RoundAfterLastScale(2.f)
, m_RoundAfterInitAlpha(0.3f)
{
	m_CurPhase = PHASE_NONE;
	m_PhaseTimer = 0.f;
	m_fPhaseTime = 0.f;
	m_pExpMng = NULL;
	m_pRewardGoldDlg = NULL;
	m_RoundAccel = 1.f;
	m_CheckAccel = 1.f;
	m_RoundCheckTimer = 0.f;
	m_RoundAfterImangeTimer	= 0.f;
	m_bRoundNextStart = false;

	m_pStaticAllRoundClear = NULL;
	m_pStaticTime = NULL;
	m_pStaticExp = NULL;
	m_pStaticExpValue = NULL;
	m_pStaticRoundCheck = NULL;
	memset( m_pTextureRound, 0, sizeof(m_pTextureRound) );
	memset( m_pTextureRoundAfterImage, 0, sizeof(m_pTextureRoundAfterImage) );
	m_pStaticRound = NULL;
	m_pStaticChannelBonus = NULL;
	m_pStatic2ndPwdBonus = NULL;
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	m_pStaticEKeyBonus = NULL;
#endif

	m_pPlayerDungeonClearInfo = NULL;
	m_pDungeonClearValue = NULL;
	m_nSoundIndex = -1;
	m_pStaticMaxLevelCharBonus = NULL;
	m_pStaticMaxLevelCharCount = NULL;
}

CDnDarklairClearReportDlg::~CDnDarklairClearReportDlg()
{
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
	SAFE_RELEASE_SPTR( m_hRoundTexture );
	SAFE_DELETE( m_pExpMng );
	SAFE_DELETE( m_pRewardGoldDlg );
}


void CDnDarklairClearReportDlg::InitialUpdate()
{
	m_pStaticAllRoundClear = GetControl<CEtUIStatic>("ID_TEXT_ARC");
	m_pStaticTime = GetControl<CEtUIStatic>("ID_TIME");
	m_pStaticExp = GetControl<CEtUIStatic>("ID_STATIC_EXP");
	m_pStaticExpValue = GetControl<CEtUIStatic>("ID_EXP");
	m_pStaticRoundCheck = GetControl<CEtUIStatic>("ID_STATIC_CHECK");
	m_pStaticRound = GetControl<CEtUIStatic>("ID_STATIC_ROUND");
	m_pStaticChannelBonus = GetControl<CEtUIStatic>("ID_CHANNEL_BONUS");
	m_pStatic2ndPwdBonus = GetControl<CEtUIStatic>("ID_2NDPASS_BONUS");
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	m_pStaticEKeyBonus = GetControl<CEtUIStatic>("ID_EKEY");
	if (m_pStaticEKeyBonus)
		m_pStaticEKeyBonus->Show(false);
#endif

	m_pStaticMaxLevelCharBonus = GetControl<CEtUIStatic>("ID_STATIC_ML1");
	m_pStaticMaxLevelCharCount = GetControl<CEtUIStatic>("ID_STATIC_MLCOUNT");

	m_pStaticAllRoundClear->Show( false );
	m_pStaticTime->Show( false );
	m_pStaticExp->Show( false );
	m_pStaticExpValue->Show( false );
	m_pStaticRoundCheck->Show( false );
	m_pStaticRound->Show( false );
	m_pStaticChannelBonus->Show( false );

	char szString[256] = {0,};
	for( int itr = 0; itr < eRoundNumber_Cipher; ++itr )
	{
		sprintf_s( szString, _countof(szString), "ID_TEXTURE_ROUND%d", itr );
		m_pTextureRound[itr] = GetControl<CEtUITextureControl>( szString );

		sprintf_s( szString, _countof(szString), "ID_TEXTURE_ROUND%d_AFTER", itr );
		m_pTextureRoundAfterImage[itr] = GetControl<CEtUITextureControl>( szString );

		m_pTextureRound[itr]->Show( false );
		m_pTextureRoundAfterImage[itr]->Show( false );
	}

	m_hRoundTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DarkRareRankIcon.dds" ).c_str(), RT_TEXTURE );
	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10021 ), false, false );

	if( m_pExpMng == NULL )
	{
		m_pExpMng = new CDnStageClearCounterMng;
#ifdef _COUNT_RENEW
		m_pExpMng->Initialize();
#endif
	}
	if( m_pRewardGoldDlg == NULL ) {
		m_pRewardGoldDlg = new CDnStageClearRewardGoldDlg;
		m_pRewardGoldDlg->Initialize( false );
	}
}

void CDnDarklairClearReportDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DarkRareClearReportDlg.ui" ).c_str(), bShow );
}

void CDnDarklairClearReportDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDarklairClearReportDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;
	if( bShow )
		m_CurPhase = PHASE_START;
	else
		OnClose();

	CEtUIDialog::Show( bShow );
}

void CDnDarklairClearReportDlg::Process( float fElapsedTime )
{
	if( IsShow() )
	{
		if( m_fPhaseTime > 0.0f )
		{
			m_fPhaseTime -= fElapsedTime;
		}
		else
		{
			switch(m_CurPhase)
			{
			case PHASE_NONE:	
				break;

			case PHASE_START:
				{
					HideAllControl();

					CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
					SetStageClearInfo();

					if (m_pDungeonClearValue != NULL)
					{
						if (CDnLocalPlayerActor::s_hLocalActor && m_pDungeonClearValue->bClear == false)
						{
							CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
							if (pLocalActor)
								pLocalActor->SetWorldColorFilterOnDie();
						}
					}

					m_CurPhase = PHASE_ROUND_START;
				}
				break;
			case PHASE_ROUND_START:
				{
					int nEventBonus = 0;
					int nChannelBonus = m_pPlayerDungeonClearInfo->sBaseInfo.nMeritBonusExperience;
					int nPromitionBonus = 0;
					int nExp = m_pPlayerDungeonClearInfo->sBaseInfo.nExperience;

					int eventBonusCnt = m_pPlayerDungeonClearInfo->sBaseInfo.cBonusCount;
					if (eventBonusCnt > 0)
					{
						for (int i = 0; i < eventBonusCnt; ++i)
						{
							const sEventStageClearBonus& bonus = m_pPlayerDungeonClearInfo->sBaseInfo.EventClearBonus[i];
#if defined(PRE_ADD_WORLD_EVENT)
							if (bonus.nType != WorldEvent::EVENT_NONE)
								InitPhase_EventBouns((WorldEvent::eDetailEventType)bonus.nType);
#else
							if (bonus.nType != _EVENT_3_NONE)
								InitPhase_EventBouns((eEventType3)bonus.nType);
#endif // #if defined(PRE_ADD_WORLD_EVENT)

#if defined _DEBUG || defined _RDEBUG
							if (bonus.nClearEventBonusExperience < 0)
								OutputDebug("[EXP] ERROR : Stage Clear Bonus Exp - (type:%d exp:%d)\n", bonus.nType, bonus.nClearEventBonusExperience);
#endif

							nEventBonus += bonus.nClearEventBonusExperience;
						}
					}

					nPromitionBonus = m_pPlayerDungeonClearInfo->sBaseInfo.nPromotionExperience;

					InitPhase_RoundChecker();
					InitPhase_Round();
					InitPhase_Exp(false, true, nExp, nExp + nChannelBonus + nEventBonus + nPromitionBonus );
					if (nChannelBonus > 0) InitPhase_ChannelBonus();
					InitPhase_PromotionBonus();

					RefreshBonusAlarmCoord();
					m_CurPhase = PHASE_ROUND_PROCESS;
				}
				break;

			case PHASE_ROUND_PROCESS:
				{
					bool bChecker, bRound = false; // code analysis
					//bool bChecker, bRound; 
					bChecker = ShowPhase_RoundChecker(fElapsedTime);

					if (m_bRoundNextStart)
						bRound = ShowPhase_Round(fElapsedTime);

					if (bChecker && bRound)			 
					{
						m_fPhaseTime = 0.05f;
						m_CurPhase = PHASE_ROUND_BONUS_COUNT_PROCESS;
					}
				}
				break;
			case PHASE_ROUND_BONUS_COUNT_PROCESS:
				{
					ShowPhase_Exp(fElapsedTime);
					if (m_pStaticChannelBonus->IsShow())
						ShowPhase_ChannelBonus(fElapsedTime);

					if( m_pStaticMaxLevelCharBonus->IsShow() )
						ShowPhase_PromotionBonus( fElapsedTime );
				}
				break;

			case PHASE_FINISH:			OnClose();					break;
			default:
				ASSERT(0);
			}
		}
	}
	CEtUIDialog::Process( fElapsedTime );
}

void CDnDarklairClearReportDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );
}

void CDnDarklairClearReportDlg::OnBlindOpen()
{
	CDnLocalPlayerActor::LockInput(true);
}

void CDnDarklairClearReportDlg::OnBlindOpened()
{
	Show(true);
	GetInterface().ShowChatDialog();
}

void CDnDarklairClearReportDlg::HideAllControl()
{
	m_pStaticAllRoundClear->Show( false );
	m_pStaticTime->Show( false );
	m_pStaticExp->Show( false );
	m_pStaticExpValue->Show( false );
	m_pStaticRoundCheck->Show( false );
	m_pStaticRound->Show( false );
	m_pStaticChannelBonus->Show( false );
	m_pStatic2ndPwdBonus->Show(false);
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	if (m_pStaticEKeyBonus)
		m_pStaticEKeyBonus->Show(false);
#endif

	for( int itr = 0; itr < eRoundNumber_Cipher; ++itr )
	{
		m_pTextureRound[itr]->Show( false );
		m_pTextureRoundAfterImage[itr]->Show( false );
	}

#ifdef _COUNT_RENEW
	m_pExpMng->Reset();
#else
	m_pExpMng->Clear();
#endif
	m_pRewardGoldDlg->Show( false );
	m_pStaticMaxLevelCharBonus->Show( false );
	m_pStaticMaxLevelCharCount->Show( false );
}

void CDnDarklairClearReportDlg::SetStageClearInfo()
{
	if( !CDnActor::s_hLocalActor ) return;
	DnActorHandle hActor = CDnActor::s_hLocalActor;

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( !pGameTask )
	{
		CDebugSet::ToLogFile( "CDnDarklairClearReportDlg::SetPlayerInfo, pPartyTask is NULL!" );
		return;
	}

	int nSessionID = CDnBridgeTask::GetInstance().GetSessionID();
	CDnDarklairClearImp *pClearImp = (CDnDarklairClearImp *)pGameTask->GetStageClearImp();
	if( !pClearImp ) return;

	m_pPlayerDungeonClearInfo = pClearImp->GetDungeonClearInfo( nSessionID );
	if( !m_pPlayerDungeonClearInfo )
	{
		CDebugSet::ToLogFile( "CDnDarklairClearReportDlg::SetPlayerInfo, m_pPlayerDungeonClearInfo is NULL!" );
		return;
	}

	m_pDungeonClearValue = &(pClearImp->GetDungeonClearValue());
	if( !m_pDungeonClearValue )
	{
		CDebugSet::ToLogFile( "CDnDarklairClearReportDlg::SetPlayerInfo, m_pDungeonClearValue is NULL!" );
		return;
	}
	m_pStaticTime->SetText( m_pDungeonClearValue->strTime.c_str() );
	m_pStaticTime->Show( true );

	if( m_pDungeonClearValue->nClearRound == ((CDnDLGameTask*)pGameTask)->GetTotalRound() && m_pDungeonClearValue->bClear ) {
		m_pStaticAllRoundClear->Show( true );
	}
}

void CDnDarklairClearReportDlg::InitPhase_Exp(bool bWithSign, bool bCounting, int initExp, int targetExp)
{
	if( !m_pPlayerDungeonClearInfo )
	{
		_ASSERT(0);
		return;
	}

	switch( m_pPlayerDungeonClearInfo->sBaseInfo.cClearRewardType ) {
		case 0:	// 경험치 보너스 ( 일반 )
			{
				SUICoord pointCoord;
				m_pStaticExpValue->GetUICoord(pointCoord);
				m_pExpMng->Set(targetExp, COLOR_YELLOW, bWithSign, bCounting, initExp);

				EtColor color(1.f, 0.75f, 0.f, 1.f); // Green
				m_pStaticExp->SetTextureColor(color);
				m_pStaticExp->SetManualControlColor(true);
				m_pStaticExp->Show( true );
			}
			break;
		case 1: // 만랩시 골드보상
			m_pRewardGoldDlg->Set( m_pPlayerDungeonClearInfo->sBaseInfo.nRewardGold );
			m_pRewardGoldDlg->Show( true );
			break;
	}
}

void CDnDarklairClearReportDlg::InitPhase_RoundChecker()
{
	if( !m_pPlayerDungeonClearInfo )
	{
		_ASSERT(0);
		return;
	}

	////////////////////
	m_pStaticRoundCheck->Show(true);
	m_pStaticRoundCheck->GetUICoord(m_RoundCheckOrgCoord);
	OutputDebug("[CHECK_START] coord(%f %f %f %f)\n", m_RoundCheckOrgCoord.fX, m_RoundCheckOrgCoord.fY, m_RoundCheckOrgCoord.fWidth, m_RoundCheckOrgCoord.fHeight);

	DWORD curCheckTextureColor = m_pStaticRoundCheck->GetTextureColor();
	EtColor color(curCheckTextureColor);
	color.a = 0.f;
	m_pStaticRoundCheck->SetTextureColor(color);
	m_pStaticRoundCheck->SetManualControlColor(true);

	/////////////////////
	m_pStaticRound->Show(true);
	m_pStaticRound->GetUICoord(m_RoundOrgCoord);
	OutputDebug("[CHECK_START] coord(%f %f %f %f)\n", m_RoundCheckOrgCoord.fX, m_RoundCheckOrgCoord.fY, m_RoundCheckOrgCoord.fWidth, m_RoundCheckOrgCoord.fHeight);

	curCheckTextureColor = m_pStaticRound->GetTextureColor();
	color = curCheckTextureColor;
	color.a = 0.f;
	m_pStaticRound->SetTextureColor(color);
	m_pStaticRound->SetManualControlColor(true);


	///////////////////////
	m_RoundCheckTimer	= 0.f;
	m_CheckAccel		= 1.f;
	m_bRoundNextStart	= false;
}

bool CDnDarklairClearReportDlg::ShowPhase_RoundChecker(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo )
		return false;

	m_RoundCheckTimer += fElapsedTime;

	if (m_RoundCheckTimer < m_CheckShowTime)
	{
		float ratio = 1.f - ((m_CheckShowTime - m_RoundCheckTimer) / m_CheckShowTime);

		///////////////////
		EtColor color(m_pStaticRoundCheck->GetTextureColor());
		color.a = ratio;
		m_pStaticRoundCheck->SetTextureColor(color);

		EtVector2 prev, cur, result;
		float scale = m_CheckInitScale - ((m_CheckInitScale - 1.f) * ratio) * m_CheckAccel;
		m_CheckAccel += 0.1f;
		if (scale < 1.f)
			scale = 1.f;
		m_pStaticRoundCheck->SetTextureScale(scale);
		GetUICtrlCenter(prev, m_pStaticRoundCheck, 1.f);
		GetUICtrlCenter(cur, m_pStaticRoundCheck, scale);
		result = prev - cur;

		m_pStaticRoundCheck->SetPosition(m_RoundCheckOrgCoord.fX + result.x, m_RoundCheckOrgCoord.fY + result.y);

		////////////////////////
		color = m_pStaticRound->GetTextureColor();
		color.a = ratio;
		m_pStaticRound->SetTextureColor(color);

		m_pStaticRound->SetTextureScale(scale);
		GetUICtrlCenter(prev, m_pStaticRound, 1.f);
		GetUICtrlCenter(cur, m_pStaticRound, scale);
		result = prev - cur;

		m_pStaticRound->SetPosition(m_RoundOrgCoord.fX + result.x, m_RoundOrgCoord.fY + result.y);


		if (m_RoundCheckTimer > m_CheckShowTime * 0.5f)
			m_bRoundNextStart = true;

		return false;
	}
	else
	{
		EtColor color(m_pStaticRoundCheck->GetTextureColor());
		color.a = 1.f;
		m_pStaticRoundCheck->SetTextureColor(color);
		m_pStaticRoundCheck->SetTextureScale(1.f);
	}

	return true;
}

bool CDnDarklairClearReportDlg::ShowPhase_Round(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo )
		return false;

	m_PhaseTimer += fElapsedTime;

	if (m_PhaseTimer < m_RoundShowTime)
	{
		float ratio = 1.f - ((m_RoundShowTime - m_PhaseTimer) / m_RoundShowTime);

		float scale = m_RoundInitScale - ((m_RoundInitScale - 1.f) * ratio) * m_RoundAccel;
		m_RoundAccel += 0.1f;
		if (scale < 1.f) scale = 1.f;

		for( int itr = 0; itr < eRoundNumber_Cipher; ++itr )
		{
			if( false == m_pTextureRound[itr]->IsShow() )
				continue;

			m_pTextureRound[itr]->SetScale(scale);

			EtVector2 prev, cur, result;
			GetUICtrlCenter(prev, m_pTextureRound[itr], 1.f);
			GetUICtrlCenter(cur, m_pTextureRound[itr], scale);
			result = prev - cur;

			SUICoord curCoord;
			m_pTextureRound[itr]->GetUICoord(curCoord);
			m_pTextureRound[itr]->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
		}

		return false;
	}
	else
	{
		if (m_RoundAfterImangeTimer < m_RoundAfterShowTime)
		{
			if (m_RoundAfterImangeTimer == 0.f)
			{
				int nRound = m_pDungeonClearValue->nClearRound;

				int Cipher_4 = nRound / 1000;
				int Cipher_3 = (nRound % 1000) / 100;
				int Cipher_2 = (nRound % 100) / 10;
				int Cipher_1 = nRound%10;

				if( nRound >= 1000 )
				{
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_4_Cipher] , Cipher_4 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_3_Cipher] , Cipher_3 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_2_Cipher] , Cipher_2 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_1_Cipher] , Cipher_1 );
				}
				else if( nRound >= 100 )
				{
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_3_Cipher] , Cipher_3 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_2_Cipher] , Cipher_2 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_1_Cipher] , Cipher_1 );
				}
				else if( nRound >= 10 )
				{
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_2_Cipher] , Cipher_2 );
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_1_Cipher] , Cipher_1 );
				}
				else
					TextureRoundSet( m_pTextureRoundAfterImage[eRounNumber_1_Cipher] , Cipher_1 );

				m_RoundAccel = 1.f;
			}

			m_RoundAfterImangeTimer += fElapsedTime;

			for( int i=0; i<eRoundNumber_Cipher; i++ )
			{
				if( false == m_pTextureRoundAfterImage[i]->IsShow() )
					continue;

				float ratio = (m_RoundAfterShowTime - m_RoundAfterImangeTimer) / m_RoundAfterShowTime;
				EtColor color(m_pTextureRoundAfterImage[i]->GetTextureColor());
				color.a = 0.7f - float(pow(3 * m_RoundAfterImangeTimer, 2));
				if (color.a < 0.f)
					color.a = 0.f;
				m_pTextureRoundAfterImage[i]->SetTextureColor(color);
				float scale = 1.f + log(10.f * m_RoundAfterImangeTimer + 1);
				m_pTextureRoundAfterImage[i]->SetScale(scale);

				EtVector2 prev, cur, result;
				GetUICtrlCenter(prev, m_pTextureRoundAfterImage[i], 1.f);
				GetUICtrlCenter(cur, m_pTextureRoundAfterImage[i], scale);
				result = prev - cur;

				SUICoord curCoord;
				m_pTextureRoundAfterImage[i]->GetUICoord(curCoord);
				m_pTextureRoundAfterImage[i]->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
			}

			return false;
		}
		else
		{
			for( int itr = 0; itr < eRoundNumber_Cipher; ++itr )
				m_pTextureRoundAfterImage[itr]->Show(false);
		}
	}

	return true;
}

void CDnDarklairClearReportDlg::OnClose()
{
	HideAllControl();

	m_pPlayerDungeonClearInfo	= NULL;
	m_pDungeonClearValue		= NULL;
	m_fPhaseTime	= 0.0f;
	m_CurPhase		= PHASE_NONE;
}

void CDnDarklairClearReportDlg::TextureRoundSet( CEtUITextureControl * pTextureRound, int nNumber )
{
	pTextureRound->SetTexture( m_hRoundTexture, nNumber * 115, 0, 115, 150 );
	pTextureRound->SetTextureColor( pTextureRound->GetTextureColor() );
	pTextureRound->SetManualControlColor(true);
	pTextureRound->Show( true );
}

void CDnDarklairClearReportDlg::InitPhase_Round()
{
	int nRound = m_pDungeonClearValue->nClearRound;

	int Cipher_4 = nRound / 1000;
	int Cipher_3 = (nRound % 1000) / 100;
	int Cipher_2 = (nRound % 100) / 10;
	int Cipher_1 = nRound%10;

	if( nRound >= 1000 )
	{
		TextureRoundSet( m_pTextureRound[eRounNumber_4_Cipher] , Cipher_4 );
		TextureRoundSet( m_pTextureRound[eRounNumber_3_Cipher] , Cipher_3 );
		TextureRoundSet( m_pTextureRound[eRounNumber_2_Cipher] , Cipher_2 );
		TextureRoundSet( m_pTextureRound[eRounNumber_1_Cipher] , Cipher_1 );
	}
	else if( nRound >= 100 )
	{
		TextureRoundSet( m_pTextureRound[eRounNumber_3_Cipher] , Cipher_3 );
		TextureRoundSet( m_pTextureRound[eRounNumber_2_Cipher] , Cipher_2 );
		TextureRoundSet( m_pTextureRound[eRounNumber_1_Cipher] , Cipher_1 );
	}
	else if( nRound >= 10 )
	{
		TextureRoundSet( m_pTextureRound[eRounNumber_2_Cipher] , Cipher_2 );
		TextureRoundSet( m_pTextureRound[eRounNumber_1_Cipher] , Cipher_1 );
	}
	else
		TextureRoundSet( m_pTextureRound[eRounNumber_1_Cipher] , Cipher_1 );

	m_PhaseTimer			= 0.f;
	m_RoundAfterImangeTimer	= 0.f;
	m_RoundAccel			= 1.f;
	m_RoundCheckTimer		= 0.f;

}

void CDnDarklairClearReportDlg::InitPhase_ChannelBonus()
{
	m_ChannelBonusTimer = 0.f;
	//m_pStaticChannelBonus->SetBlendRate(0.f);
	m_pStaticChannelBonus->SetBlink(true);
	m_pStaticChannelBonus->Show(true);
}

void CDnDarklairClearReportDlg::GetUICtrlCenter(EtVector2& result, CEtUIControl* ctrl, float scale)
{
	if (ctrl == NULL)
	{
		_ASSERT(0);
		return;
	}
	SUICoord curCoord;
	ctrl->GetUICoord(curCoord);
	curCoord.fWidth *= scale;
	curCoord.fHeight *= scale;
	result.x = (curCoord.fX + curCoord.fWidth) * 0.5f;
	result.y = (curCoord.fY + curCoord.fHeight) * 0.5f;
}


void CDnDarklairClearReportDlg::ShowPhase_ChannelBonus(float fElapsedTime)
{
	m_ChannelBonusTimer += fElapsedTime;

	if (m_ChannelBonusTimer >= 1.f) {
		m_pStaticChannelBonus->SetBlink(false);
		m_pStatic2ndPwdBonus->SetBlink(false);

#ifdef PRE_ADD_CH_EKEYCLEARBONUS
		if (m_pStaticEKeyBonus)
			m_pStaticEKeyBonus->SetBlink(false);
#endif
	}
}

bool CDnDarklairClearReportDlg::ShowPhase_Exp(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo ) return false;

	switch( m_pPlayerDungeonClearInfo->sBaseInfo.cClearRewardType ) {
		case 0:
			m_pExpMng->Process(fElapsedTime);
			return (m_pExpMng->IsProcessing() == false);
		case 1:
			return true;
			break;
	}
	return true;
}

#if defined(PRE_ADD_WORLD_EVENT)
void CDnDarklairClearReportDlg::InitPhase_EventBouns(WorldEvent::eDetailEventType type)
#else
void CDnDarklairClearReportDlg::InitPhase_EventBouns(eEventType3 type)
#endif
{
	m_ChannelBonusTimer = 0.f;
#if defined(PRE_ADD_WORLD_EVENT)
	if (type == WorldEvent::EVENT8)
	{
		m_pStatic2ndPwdBonus->SetBlink(true);
		m_pStatic2ndPwdBonus->Show(true);
	}
#else
	if (type == _EVENT_3_SECONDAUTH)
	{
		m_pStatic2ndPwdBonus->SetBlink(true);
		m_pStatic2ndPwdBonus->Show(true);
	}
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	else if (type == _EVENT_3_EKEYANDECARD)
	{
		if (m_pStaticEKeyBonus)
		{
			m_pStaticEKeyBonus->SetBlink(true);
			m_pStaticEKeyBonus->Show(true);
		}
	}
#endif //#ifdef PRE_ADD_CH_EKEYCLEARBONUS
#endif //#if defined(PRE_ADD_WORLD_EVENT)
}

void CDnDarklairClearReportDlg::InitPhase_PromotionBonus()
{
	if( !m_pPlayerDungeonClearInfo ) return;

	m_MaxLevelCharCountTimer = 0.f;
	WCHAR wszTemp[32];
	swprintf_s( wszTemp, L"%d", m_pPlayerDungeonClearInfo->sBaseInfo.cMaxLevelCharCount );
	m_pStaticMaxLevelCharBonus->SetBlink( true );
	m_pStaticMaxLevelCharBonus->Show( true );

	m_pStaticMaxLevelCharCount->SetText( wszTemp );
	m_pStaticMaxLevelCharCount->SetBlink( true );
	m_pStaticMaxLevelCharCount->Show( true );
}

void CDnDarklairClearReportDlg::ShowPhase_PromotionBonus(float fElapsedTime)
{
	m_MaxLevelCharCountTimer += fElapsedTime;

	if( m_MaxLevelCharCountTimer >= 1.f ) {
		m_pStaticMaxLevelCharBonus->SetBlink( false );
		m_pStaticMaxLevelCharCount->SetBlink( false );
	}
}

void CDnDarklairClearReportDlg::RefreshBonusAlarmCoord()
{
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	if (m_pStaticChannelBonus->IsShow() && (m_pStaticEKeyBonus && m_pStaticEKeyBonus->IsShow()))
	{
		SUICoord secondPwdBonusCoord, channelBonusCoord;
		m_pStaticEKeyBonus->GetUICoord(secondPwdBonusCoord);
		m_pStaticChannelBonus->GetUICoord(channelBonusCoord);

		secondPwdBonusCoord.fX = channelBonusCoord.fX - secondPwdBonusCoord.fWidth;
		m_pStaticEKeyBonus->SetUICoord(secondPwdBonusCoord);
	}
	else if (m_pStaticChannelBonus->IsShow() && m_pStatic2ndPwdBonus->IsShow())
	{
		SUICoord secondPwdBonusCoord, channelBonusCoord;
		m_pStatic2ndPwdBonus->GetUICoord(secondPwdBonusCoord);
		m_pStaticChannelBonus->GetUICoord(channelBonusCoord);

		secondPwdBonusCoord.fX = channelBonusCoord.fX - secondPwdBonusCoord.fWidth;
		m_pStatic2ndPwdBonus->SetUICoord(secondPwdBonusCoord);
	}
#else
	if (m_pStaticChannelBonus->IsShow() && m_pStatic2ndPwdBonus->IsShow())
	{
		SUICoord secondPwdBonusCoord, channelBonusCoord;
		m_pStatic2ndPwdBonus->GetUICoord(secondPwdBonusCoord);
		m_pStaticChannelBonus->GetUICoord(channelBonusCoord);

		secondPwdBonusCoord.fX = channelBonusCoord.fX - secondPwdBonusCoord.fWidth;
		m_pStatic2ndPwdBonus->SetUICoord(secondPwdBonusCoord);
	}
#endif // PRE_ADD_CH_EKEYCLEARBONUS
}
