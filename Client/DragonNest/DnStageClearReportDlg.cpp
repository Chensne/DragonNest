#include "StdAfx.h"
#include "DnStageClearReportDlg.h"
#include "TaskManager.h"
#include "DnLocalPlayerActor.h"
#include "DnWorldDataManager.h"
#include "DnWorldData.h"
#include "DnTableDB.h"
#include "GlobalValue.h"
#include "DnStageClearCounterMng.h"
#include "DnStageClearRewardGoldDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#define _COUNT_RENEW

CDnStageClearReportDlg::CDnStageClearReportDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
	, m_pStaticMaxCombo(NULL)
	, m_pMaxCombo(NULL)
	, m_pStaticComboBonus(NULL)
	, m_pComboBonus(NULL)
	, m_pStaticKillNumber(NULL)
	, m_pKillNumber(NULL)
	, m_pStaticKillBonus(NULL)
	, m_pKillBonus(NULL)
	, m_pStaticClearTime(NULL)
	, m_pClearTime(NULL)
	, m_pStaticTimeBonus(NULL)
	, m_pTimeBonus(NULL)
	, m_pStaticPartyNumber(NULL)
	, m_pPartyNumber(NULL)
	, m_pStaticPartyBonus(NULL)
	, m_pPartyBonus(NULL)
	, m_pStaticPoint(NULL)
	, m_pPoint(NULL)
	, m_pStaticEXP(NULL)
	, m_pEXP(NULL)
	, m_pStaticRankCheck(NULL)
	, m_pStaticRank(NULL)
	, m_pTextureRank(NULL)
	, m_pPlayerDungeonClearInfo(NULL)
	, m_pDungeonClearValue(NULL)
	, m_fPhaseTime(0.0f)
	, m_CurPhase(PHASE_NONE)
	, m_RankShowTime(0.1f)
	, m_CheckShowTime(0.3f)
	, m_RankInitScale(3.f)
	, m_CheckInitScale(3.f)
	, m_RankAfterShowTime(1.0f)
	, m_RankAfterLastScale(2.f)
	, m_RankAfterInitAlpha(0.3f)
	, m_pStaticOpenDungeonName(NULL)
	, m_pStaticOpenText(NULL)
	, m_pTextureOpenDungeon(NULL)
	, m_pStaticOpenLevel(NULL)
	, m_pStaticOpenBase(NULL)
	, m_nMapIndex(-1)
	, m_nMapLevel(0)
	, m_pStaticBase1(NULL)
	, m_pStaticBase2(NULL)
	, m_BonusSlidingTime(1.f)
	, m_pTextureRankAfterImage(NULL)
//#ifdef PRE_ADD_ACTIVEMISSION
//	,m_pStaticActiveMissionNumber(NULL)
//	,m_pStaticActiveMissionBonus(NULL)
//	,m_pStaticActiveMissionResult(NULL)
//#endif // PRE_ADD_ACTIVEMISSION
{
	ZeroMemory(m_BonusStaticPos, sizeof(EtVector2)*BONUS_MAX);
	ZeroMemory(m_BonusPos, sizeof(EtVector2)*BONUS_MAX);
	ZeroMemory(m_bStartBonusSliding, sizeof(bool)*BONUS_MAX);
	m_PointMng				= NULL;
	m_ExpMng				= NULL;
	m_pRewardGoldDlg		= NULL;
	m_BonusSlidingIdx		= 0;
	m_RankAccel				= 1.f;
	m_CheckAccel			= 1.f;
	m_RankCheckTimer		= 0.f;
	m_RankAfterImangeTimer	= 0.f;
	m_bRankNextStart		= false;
	m_pStaticChannelBonus	= NULL;
	m_pStatic2ndPwdBonus	= NULL;
	m_pStaticEKeyBonus		= NULL;
	m_pStaticMaxLevelCharBonus = NULL;
	m_pStaticMaxLevelCharCount = NULL;
	m_pStaticExpGauge = NULL;
	m_pStaticExpGaugeValue = NULL;
	memset( m_pProgressExpGauge, 0, sizeof(m_pProgressExpGauge) );

	m_dwStartExp = m_dwGainExp = 0;
	m_bIgnoreRewardItem = false;
}

CDnStageClearReportDlg::~CDnStageClearReportDlg(void)
{
	SAFE_RELEASE_SPTR( m_hRankTexture );
	SAFE_RELEASE_SPTR( m_hOpenDungeon );
	SAFE_DELETE( m_PointMng );
	SAFE_DELETE( m_ExpMng );
	SAFE_DELETE( m_pRewardGoldDlg );
	
	for ( int i = 0 ; i < SoundType::MAX ; i++ )
	{
		if( CEtSoundEngine::GetInstancePtr() )
			CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndexArray[i] );
	}
}

void CDnStageClearReportDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StageClearReportDlg.ui" ).c_str(), bShow );
}

void CDnStageClearReportDlg::InitialUpdate()
{
	m_pStaticBase1 = GetControl<CEtUIStatic>("ID_STATIC_BASE1");
	m_pStaticBase2 = GetControl<CEtUIStatic>("ID_STATIC_BASE2");

	m_pStaticMaxCombo = GetControl<CEtUIStatic>("ID_STATIC_MAX_COMBO");
	m_pStaticComboBonus = GetControl<CEtUIStatic>("ID_STATIC_COMBO_BONUS");
	m_pMaxCombo = GetControl<CEtUIStatic>("ID_MAX_COMBO");
	m_pComboBonus = GetControl<CEtUIStatic>("ID_COMBO_BONUS");

	m_pStaticKillNumber = GetControl<CEtUIStatic>("ID_STATIC_KILL_NUMBER");
	m_pStaticKillBonus = GetControl<CEtUIStatic>("ID_STATIC_KILL_BONUS");
	m_pKillNumber = GetControl<CEtUIStatic>("ID_KILL_NUMBER");
	m_pKillBonus = GetControl<CEtUIStatic>("ID_KILL_BONUS");

	m_pStaticClearTime = GetControl<CEtUIStatic>("ID_STATIC_CLEAR_TIME");
	m_pStaticTimeBonus = GetControl<CEtUIStatic>("ID_STATIC_TIME_BONUS");
	m_pClearTime = GetControl<CEtUIStatic>("ID_CLEAR_TIME");
	m_pTimeBonus = GetControl<CEtUIStatic>("ID_TIME_BONUS");

	m_pStaticPartyNumber = GetControl<CEtUIStatic>("ID_STATIC_PARTY_NUMBER");
	m_pStaticPartyBonus = GetControl<CEtUIStatic>("ID_STATIC_PARTY_BONUS");
	m_pPartyNumber = GetControl<CEtUIStatic>("ID_PARTY_NUMBER");
	m_pPartyBonus = GetControl<CEtUIStatic>("ID_PARTY_BONUS");

	m_pStaticPoint = GetControl<CEtUIStatic>("ID_STATIC_POINT");
	m_pStaticEXP = GetControl<CEtUIStatic>("ID_STATIC_EXP");
	m_pPoint = GetControl<CEtUIStatic>("ID_POINT");
	m_pEXP = GetControl<CEtUIStatic>("ID_EXP");

	m_pStaticRankCheck = GetControl<CEtUIStatic>("ID_STATIC_CHECK");
	m_pStaticRank = GetControl<CEtUIStatic>("ID_STATIC_RANK");
	m_pTextureRank = GetControl<CEtUITextureControl>("ID_TEXTURE_RANK");
	m_pTextureRankAfterImage = GetControl<CEtUITextureControl>("ID_TEXTURE_RANK_AFTER");
	m_pStaticOpenDungeonName = GetControl<CEtUIStatic>("ID_OPEN_NAME");
	m_pStaticOpenText = GetControl<CEtUIStatic>("ID_OPEN_TEXT");
	m_pStaticOpenLevel = GetControl<CEtUIStatic>("ID_OPEN_LEVEL");
	m_pStaticOpenBase = GetControl<CEtUIStatic>("ID_OPEN_BASE");
	m_pTextureOpenDungeon = GetControl<CEtUITextureControl>("ID_TEXTUREL_OPEN");


//#ifdef PRE_ADD_ACTIVEMISSION
//	m_pStaticActiveMissionNumber = GetControl<CEtUIStatic>("ID_STATIC_ACTIVEMISSION_NUMBER");
//	m_pStaticActiveMissionBonus = GetControl<CEtUIStatic>("ID_STATIC_ACTIVEMISSION_BONUS");
//	m_pStaticActiveMissionResult = GetControl<CEtUIStatic>("ID_STATIC_ACTIVEMISSION");
//#endif // PRE_ADD_ACTIVEMISSION


	m_hRankTexture				= LoadResource( CEtResourceMng::GetInstance().GetFullName( "DungeonRankIcon.dds" ).c_str(), RT_TEXTURE );
	m_nSoundIndexArray[SoundType::RESULT_SHOW]			= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10021 ), false, false );
	m_nSoundIndexArray[SoundType::GRADE_RESULT]			= CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10025 ), false, false );
	
	m_pStaticChannelBonus = GetControl<CEtUIStatic>("ID_CHANNEL_BONUS");
	m_pStatic2ndPwdBonus = GetControl<CEtUIStatic>("ID_2NDPASS_BONUS");
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	m_pStaticEKeyBonus = GetControl<CEtUIStatic>("ID_EKEY");
	if (m_pStaticEKeyBonus)
		m_pStaticEKeyBonus->Show(false);
#endif

	m_pStaticComboBonus->GetUICoord(m_BonusStaticPos[BONUS_COMBO]);
	m_pStaticKillBonus->GetUICoord(m_BonusStaticPos[BONUS_KILL]);
	m_pStaticPartyBonus->GetUICoord(m_BonusStaticPos[BONUS_PARTY]);
	m_pStaticTimeBonus->GetUICoord(m_BonusStaticPos[BONUS_TIME]);

//#ifdef PRE_ADD_ACTIVEMISSION
//	m_pStaticPartyBonus->GetUICoord(m_BonusStaticPos[BONUS_ACTIVEMISSION]);
//#endif // PRE_ADD_ACTIVEMISSION

	m_pStaticMaxLevelCharBonus = GetControl<CEtUIStatic>("ID_STATIC_ML1");
	m_pStaticMaxLevelCharCount = GetControl<CEtUIStatic>("ID_STATIC_MLCOUNT");

	m_pComboBonus->GetUICoord(m_BonusPos[BONUS_COMBO]);
	m_pKillBonus->GetUICoord(m_BonusPos[BONUS_KILL]);
	m_pPartyBonus->GetUICoord(m_BonusPos[BONUS_PARTY]);
	m_pTimeBonus->GetUICoord(m_BonusPos[BONUS_TIME]);
//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionResult )
//		m_pStaticActiveMissionResult->GetUICoord(m_BonusPos[BONUS_ACTIVEMISSION]);
//#endif // PRE_ADD_ACTIVEMISSION

	m_pStaticExpGauge = GetControl<CEtUIStatic>("ID_STATIC_BAR0");
	m_pStaticExpGaugeValue = GetControl<CEtUIStatic>("ID_STATIC_GAUGE_EXP");

	char szTemp[64];
	for( int i=0; i<5; i++ ) {
		sprintf_s( szTemp, "ID_EXP_GAUGE%d", i );
		m_pProgressExpGauge[i] = GetControl<CDnExpGauge>(szTemp);
		m_pProgressExpGauge[i]->UseGaugeEndElement( 1 );
		m_pProgressExpGauge[i]->Show( false );
	}

	if (m_PointMng == NULL)
	{
		m_PointMng = new CDnStageClearCounterMng;
#ifdef _COUNT_RENEW
		m_PointMng->Initialize();
#endif
	}
	if (m_ExpMng == NULL)
	{
		m_ExpMng = new CDnStageClearCounterMng;
#ifdef _COUNT_RENEW
		m_ExpMng->Initialize();
#endif
	}
	if( m_pRewardGoldDlg == NULL ) {
		m_pRewardGoldDlg = new CDnStageClearRewardGoldDlg;
		m_pRewardGoldDlg->Initialize( false );
	}

}

void CDnStageClearReportDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		m_CurPhase = PHASE_START;
	else
		OnClose();

	CEtUIDialog::Show( bShow );
}

void CDnStageClearReportDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );	
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

					CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndexArray[SoundType::RESULT_SHOW] );
					SetStageClearInfo();
					m_CurPhase = PHASE01_GAMERESULT;
				}
				break;

			case PHASE01_GAMERESULT:
				{
					if( !m_pPlayerDungeonClearInfo )
						return;

					m_pStaticBase1->Show( true );

					ShowPhase_GameResultStatic();
					ShowPhase_GameResult();
					m_fPhaseTime = 0.1f;
					m_CurPhase = PHASE01_VALUE_START;
				}
				break;

			case PHASE01_VALUE_START:
				{
					ShowPhase_ValueStatic();
					ShowPhase_Value();

					int i = 0;
					for (; i < BONUS_MAX; ++i)
						m_bStartBonusSliding[i] = false;
					m_fPhaseTime = m_PhaseTimer = m_Phase01PreTimer = 0.f;
					m_BonusSlidingIdx = 0;
					m_CurPhase = PHASE01_VALUE_PROCESS;
					m_bStartBonusSliding[BONUS_COMBO] = true;
					m_BonusSlidingSpeed = fabs(GetScreenWidthRatio() - m_BonusStaticPos[BONUS_COMBO].fX) / m_BonusSlidingTime;
				}
				break;

			case PHASE01_VALUE_PROCESS:
				{
					m_PhaseTimer += fElapsedTime;
					const float offset = 0.05f;
					if (m_BonusSlidingIdx < BONUS_MAX && m_PhaseTimer >= (m_Phase01PreTimer + offset))
					{
						m_bStartBonusSliding[m_BonusSlidingIdx++] = true;
						m_Phase01PreTimer = m_PhaseTimer;
					}

					if (ProcessPhase01(fElapsedTime))
					{
						m_fPhaseTime = 0.1f;
						m_CurPhase = PHASE02_POINT_START;
					}
				}
				break;

			case PHASE02_POINT_START:
				{
					InitPhase_Point();

					m_CurPhase = PHASE02_POINT_PROCESS;
				}
				break;

			case PHASE02_POINT_PROCESS:
				{
					bool bPointRet = false;
					bPointRet = ShowPhase_Point(fElapsedTime);
					if(bPointRet)
					{
						m_fPhaseTime = 1.f;
						m_CurPhase = PHASE03_START;
					}
				}
				break;

			case PHASE03_START:
				{
					HideAllControl();

					if( !m_pPlayerDungeonClearInfo )
					{
						_ASSERT(0);
						break;
					}

					m_pStaticBase2->Show( true );
					if( m_nMapIndex != -1 )
						SetOpenDungeon( m_nMapIndex, m_nMapLevel );

					m_PhaseTimer = 0.f;
					m_CurPhase = PHASE03_RANK_START;
				}
				break;

			case PHASE03_RANK_START:
				{
					int nEventBonus = 0;
					int nChannelBonus = m_pPlayerDungeonClearInfo->sBaseInfo.nMeritBonusExperience;
					int nPromitionBonus = 0;
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
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

							nEventBonus += bonus.nClearEventBonusExperience;
						}
					}
#else
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
#endif // PRE_ADD_CH_EKEYCLEARBONUS
					nPromitionBonus = m_pPlayerDungeonClearInfo->sBaseInfo.nPromotionExperience;

					InitPhase_RankChecker();
					InitPhase_Rank();

					DNTableFileFormat*  pSoxPlayerCommonLevel = GetDNTable( CDnTableDB::TPLAYERCOMMONLEVEL );
					if(pSoxPlayerCommonLevel)
					{
						int nMyLevel = CDnActor::s_hLocalActor->GetLevel();
						float fValue = pSoxPlayerCommonLevel->GetFieldFromLablePtr( nMyLevel, "_AddGainExp" )->GetFloat();
						if( fValue < 1 )
							fValue = 1;	//혹시 모르니 1보다 작으면 1로 적용
						int nTotalExp = (int)((m_pPlayerDungeonClearInfo->sBaseInfo.nRewardExperience + nChannelBonus + nEventBonus + nPromitionBonus) * fValue);
						InitPhase_Exp(false, true, m_pPlayerDungeonClearInfo->sBaseInfo.nCompleteExperience, nTotalExp);
					}
					else
					{
						InitPhase_Exp(false, true, m_pPlayerDungeonClearInfo->sBaseInfo.nCompleteExperience, 
							m_pPlayerDungeonClearInfo->sBaseInfo.nRewardExperience + nChannelBonus + nEventBonus + nPromitionBonus );
					}

					if( nChannelBonus > 0 ) InitPhase_ChannelBonus();
					InitPhase_PromotionBonus();

					RefreshBonusAlarmCoord();

					m_CurPhase = PHASE03_RANK_PROCESS;
				}
				break;

			case PHASE03_RANK_PROCESS:
				{
					bool bChecker = false, bRank = false;
					bChecker = ShowPhase_RankChecker(fElapsedTime);

					if (m_bRankNextStart)
						bRank = ShowPhase_Rank(fElapsedTime);

					if (bChecker && bRank)
					{
						m_fPhaseTime = 0.05f;
						m_CurPhase = PHASE03_RANK_BONUS_COUNT_PROCESS;
					}
				}
				break;
				
			case PHASE03_RANK_BONUS_COUNT_PROCESS:
				{
					ShowPhase_Exp(fElapsedTime);
					if (m_pStaticChannelBonus->IsShow() || m_pStatic2ndPwdBonus->IsShow())
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

}

void CDnStageClearReportDlg::RefreshBonusAlarmCoord()
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
#endif
}

void CDnStageClearReportDlg::SetStageClearInfo()
{
	if( !CDnActor::s_hLocalActor ) return;
	DnActorHandle hActor = CDnActor::s_hLocalActor;

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( !pGameTask )
	{
		CDebugSet::ToLogFile( "CDnStageClearReportDlg::SetPlayerInfo, pPartyTask is NULL!" );
		return;
	}

	int nSessionID = CDnBridgeTask::GetInstance().GetSessionID();
	CDnDungeonClearImp *pClearImp = (CDnDungeonClearImp *)pGameTask->GetStageClearImp();
	if( !pClearImp ) return;

	m_pPlayerDungeonClearInfo = pClearImp->GetDungeonClearInfo( nSessionID );
	if( !m_pPlayerDungeonClearInfo )
	{
		CDebugSet::ToLogFile( "CDnStageClearReportDlg::SetPlayerInfo, m_pPlayerDungeonClearInfo is NULL!" );
		return;
	}

	m_pDungeonClearValue = &(pClearImp->GetDungeonClearValue());
	if( !m_pDungeonClearValue )
	{
		CDebugSet::ToLogFile( "CDnStageClearReportDlg::SetPlayerInfo, m_pDungeonClearValue is NULL!" );
		return;
	}
	m_bIgnoreRewardItem = m_pDungeonClearValue->bIgnoreRewardItem;
}

void CDnStageClearReportDlg::HideAllControl()
{
	m_pStaticBase1->Show( false );
	m_pStaticBase2->Show( false );

	m_pStaticMaxCombo->Show( false );
	m_pStaticComboBonus->Show( false );
	m_pMaxCombo->Show( false );
	m_pComboBonus->Show( false );

	m_pStaticKillNumber->Show( false );
	m_pStaticKillBonus->Show( false );
	m_pKillNumber->Show( false );
	m_pKillBonus->Show( false );

	m_pStaticClearTime->Show( false );
	m_pStaticTimeBonus->Show( false );
	m_pClearTime->Show( false );
	m_pTimeBonus->Show( false );

	m_pStaticPartyNumber->Show( false );
	m_pStaticPartyBonus->Show( false );
	m_pPartyNumber->Show( false );
	m_pPartyBonus->Show( false );

	m_pStaticPoint->Show( false );
	m_pStaticEXP->Show( false );
	m_pPoint->Show( false );
	m_pEXP->Show( false );
	m_pStaticChannelBonus->Show(false);
	m_pStatic2ndPwdBonus->Show(false);
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	if (m_pStaticEKeyBonus)
		m_pStaticEKeyBonus->Show(false);
#endif
#ifdef _COUNT_RENEW
	m_PointMng->Reset();
	m_ExpMng->Reset();
#else
	m_PointMng->Clear();
	m_ExpMng->Clear();
#endif

	m_pStaticRankCheck->Show( false );
	m_pStaticRank->Show( false );
	m_pTextureRank->Show( false );
	m_pTextureRankAfterImage->Show(false);

	m_pStaticOpenDungeonName->Show( false );
	m_pStaticOpenText->Show( false );
	m_pStaticOpenLevel->Show( false );
	m_pStaticOpenBase->Show( false );
	m_pTextureOpenDungeon->Show( false );

	m_pRewardGoldDlg->Show( false );

	m_pStaticMaxLevelCharBonus->Show( false );
	m_pStaticMaxLevelCharCount->Show( false );

	m_pStaticExpGauge->Show( false );
	m_pStaticExpGaugeValue->Show( false );
	for( int i=0; i<5; i++ ) m_pProgressExpGauge[i]->Show( false );

//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionNumber )
//		m_pStaticActiveMissionNumber->Show( false );
//	if( m_pStaticActiveMissionBonus )
//		m_pStaticActiveMissionBonus->Show( false );
//	if( m_pStaticActiveMissionResult )
//		m_pStaticActiveMissionResult->Show( false );
//#endif // PRE_ADD_ACTIVEMISSION

}

bool CDnStageClearReportDlg::ProcessPhase01(float fElapsedTime)
{
	bool bRet = true;
	if (m_bStartBonusSliding[BONUS_COMBO] == false || ProcessBonus(fElapsedTime, m_pStaticComboBonus, m_BonusStaticPos[BONUS_COMBO].fX) == false)	bRet = false;
	if (m_bStartBonusSliding[BONUS_KILL] == false || ProcessBonus(fElapsedTime, m_pStaticKillBonus, m_BonusStaticPos[BONUS_KILL].fX) == false)		bRet = false;
	if (m_bStartBonusSliding[BONUS_TIME] == false || ProcessBonus(fElapsedTime, m_pStaticTimeBonus, m_BonusStaticPos[BONUS_TIME].fX) == false)		bRet = false;
	if (m_bStartBonusSliding[BONUS_PARTY] == false || ProcessBonus(fElapsedTime, m_pStaticPartyBonus, m_BonusStaticPos[BONUS_PARTY].fX) == false)	bRet = false;

	if (m_bStartBonusSliding[BONUS_COMBO] == false || ProcessBonus(fElapsedTime, m_pComboBonus, m_BonusPos[BONUS_COMBO].fX) == false)		bRet = false;
	if (m_bStartBonusSliding[BONUS_KILL] == false || ProcessBonus(fElapsedTime, m_pKillBonus, m_BonusPos[BONUS_KILL].fX) == false)			bRet = false;
	if (m_bStartBonusSliding[BONUS_TIME] == false || ProcessBonus(fElapsedTime, m_pTimeBonus, m_BonusPos[BONUS_TIME].fX) == false)			bRet = false;
	if (m_bStartBonusSliding[BONUS_PARTY] == false || ProcessBonus(fElapsedTime, m_pPartyBonus, m_BonusPos[BONUS_PARTY].fX) == false)		bRet = false;

//#ifdef PRE_ADD_ACTIVEMISSION
//	if (m_bStartBonusSliding[BONUS_ACTIVEMISSION] == false || ProcessBonus(fElapsedTime, m_pStaticActiveMissionBonus, m_BonusStaticPos[BONUS_ACTIVEMISSION].fX) == false)	bRet = false;
//	if (m_bStartBonusSliding[BONUS_ACTIVEMISSION] == false || ProcessBonus(fElapsedTime, m_pStaticActiveMissionResult, m_BonusPos[BONUS_ACTIVEMISSION].fX) == false)		bRet = false;
//#endif // PRE_ADD_ACTIVEMISSION

	return bRet;
}

bool CDnStageClearReportDlg::ProcessBonus(float fElapsedTime, CEtUIStatic* bonusStatic, float targetX)
{
	if (bonusStatic == NULL)
	{
		_ASSERT(0);
		return false;
	}

	bool bRet = false;
	float offset = -fElapsedTime * m_BonusSlidingSpeed;//-0.015f;
	SetCoords(bonusStatic, offset, 0.f, true);
	SUICoord coord;
	bonusStatic->GetUICoord(coord);
	if (coord.fX <= targetX)
	{
		coord.fX = targetX;
		bonusStatic->SetUICoord(coord);
		bRet = true;
	}
	return bRet;
}

void CDnStageClearReportDlg::ShowPhase_GameResultStatic()
{
	m_pStaticMaxCombo->Show( true );
	m_pStaticKillNumber->Show( true );
	m_pStaticClearTime->Show( true );
	m_pStaticPartyNumber->Show( true );

//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionNumber )
//		m_pStaticActiveMissionNumber->Show( true );
//#endif // PRE_ADD_ACTIVEMISSION

}

void CDnStageClearReportDlg::ShowPhase_GameResult()
{
	wchar_t wszTemp[64] = {0};

#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	swprintf_s( wszTemp, _countof(wszTemp), L"%d %s", m_pPlayerDungeonClearInfo->sBaseInfo.nMaxComboCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2358 ) );
#else
	swprintf_s( wszTemp, _countof(wszTemp), L"%d Hits", m_pPlayerDungeonClearInfo->sBaseInfo.nMaxComboCount );
#endif 
	m_pMaxCombo->SetText( wszTemp );
	m_pMaxCombo->Show( true );
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	swprintf_s( wszTemp, _countof(wszTemp), L"%d %s", m_pPlayerDungeonClearInfo->sBaseInfo.nKillBossCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2353 ) );
#else
	swprintf_s( wszTemp, _countof(wszTemp), L"%d Boss Kills", m_pPlayerDungeonClearInfo->sBaseInfo.nKillBossCount );
#endif 
	m_pKillNumber->SetText( wszTemp );
	m_pKillNumber->Show( true );

	m_pClearTime->SetText( m_pPlayerDungeonClearInfo->strTime );
	m_pClearTime->Show( true );

	swprintf_s( wszTemp, _countof(wszTemp), L"%d", m_pDungeonClearValue->nPartyCount );
	m_pPartyNumber->SetText( wszTemp );
	m_pPartyNumber->Show( true );
}

void CDnStageClearReportDlg::SetCoords(CEtUIStatic* uiStatic, float fX, float fY, bool bAdd)
{
	if (uiStatic == NULL)
	{
		_ASSERT(0);
		return;
	}

	SUICoord coord;
	uiStatic->GetUICoord(coord);
	if (bAdd)
		coord.SetPosition(coord.fX + fX, coord.fY + fY);
	else
		coord.SetPosition(fX, fY);
	uiStatic->SetUICoord(coord);
}

void CDnStageClearReportDlg::ShowPhase_ValueStatic()
{
	m_pStaticComboBonus->Show( true );
	m_pStaticKillBonus->Show( true );
	m_pStaticTimeBonus->Show( true );
	m_pStaticPartyBonus->Show( true );
//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionBonus )
//		m_pStaticActiveMissionBonus->Show( true );
//#endif // PRE_ADD_ACTIVEMISSION

	float fX = GetScreenWidthRatio();
	SetCoords(m_pStaticComboBonus, fX, m_BonusStaticPos[BONUS_COMBO].fY, false);
	SetCoords(m_pStaticKillBonus, fX, m_BonusStaticPos[BONUS_KILL].fY, false);
	SetCoords(m_pStaticTimeBonus, fX, m_BonusStaticPos[BONUS_TIME].fY, false);
	SetCoords(m_pStaticPartyBonus, fX, m_BonusStaticPos[BONUS_PARTY].fY, false);
//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionBonus )
//		SetCoords(m_pStaticActiveMissionBonus, fX, m_BonusStaticPos[BONUS_ACTIVEMISSION].fY, false);
//#endif // PRE_ADD_ACTIVEMISSION
}

void CDnStageClearReportDlg::ShowPhase_Value()
{
	wchar_t wszTemp[64] = {0};

	swprintf_s( wszTemp, _countof(wszTemp), L"+ %d", m_pPlayerDungeonClearInfo->nComboBonus );
	m_pComboBonus->SetText( wszTemp );
	m_pComboBonus->Show(true);

	swprintf_s( wszTemp, _countof(wszTemp), L"+ %d", m_pPlayerDungeonClearInfo->nKillBonus );
	m_pKillBonus->SetText( wszTemp );
	m_pKillBonus->Show(true);

	swprintf_s( wszTemp, _countof(wszTemp), L"+ %d", m_pPlayerDungeonClearInfo->nTimeBonus );
	m_pTimeBonus->SetText( wszTemp );
	m_pTimeBonus->Show(true);

	swprintf_s( wszTemp, _countof(wszTemp), L"+ %d", m_pPlayerDungeonClearInfo->nPartyBonus );
	m_pPartyBonus->SetText( wszTemp );
	m_pPartyBonus->Show(true);

	float fX = GetScreenWidthRatio();
	SetCoords(m_pComboBonus, fX, m_BonusPos[BONUS_COMBO].fY, false);
	SetCoords(m_pKillBonus, fX, m_BonusPos[BONUS_KILL].fY, false);
	SetCoords(m_pTimeBonus, fX, m_BonusPos[BONUS_TIME].fY, false);
	SetCoords(m_pPartyBonus, fX, m_BonusPos[BONUS_PARTY].fY, false);

//#ifdef PRE_ADD_ACTIVEMISSION
//	if( m_pStaticActiveMissionResult )
//	{
//		int nStrIdx = 9187;  // "Nothing"
//		int nRes = m_pPlayerDungeonClearInfo->sBaseInfo.cActiveMissionState;
//		if( nRes == ActiveMission::ActiveMissionState::COMPLETE_MISSION )
//			nStrIdx = 9189;  // "Success"
//		else if( nRes == ActiveMission::ActiveMissionState::GAIN_MISSION )
//			nStrIdx = 9188;	 // "Fail"
//		m_pStaticActiveMissionResult->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStrIdx ) );
//		m_pStaticActiveMissionResult->Show( true );
//		SetCoords(m_pStaticActiveMissionResult, fX, m_BonusPos[BONUS_ACTIVEMISSION].fY, false);
//	}
//#endif // PRE_ADD_ACTIVEMISSION

}

void CDnStageClearReportDlg::InitPhase_Point()
{
	if( !m_pPlayerDungeonClearInfo )
	{
		_ASSERT(0);
		return;
	}

	int point = m_pPlayerDungeonClearInfo->sBaseInfo.nDefaultCP + m_pPlayerDungeonClearInfo->sBaseInfo.nBonusCP;

 	m_PointMng->Set(point, COLOR_BLUE1, false);

	EtColor color(0.53f, 0.63f, 1.f, 1.f); // Green
	m_pStaticPoint->SetTextureColor(color);
	m_pStaticPoint->SetManualControlColor(true);
	m_pStaticPoint->Show( true );
}

bool CDnStageClearReportDlg::ShowPhase_Point(float fElapsedTime)
{
	m_PointMng->Process(fElapsedTime); 

	return (m_PointMng->IsProcessing() == false);
}

void CDnStageClearReportDlg::InitPhase_Exp(bool bWithSign, bool bCounting, int initExp, int targetExp)
{
	if( !m_pPlayerDungeonClearInfo )
	{
		_ASSERT(0);
		return;
	}

	//int exp = m_pPlayerDungeonClearInfo->sBaseInfo.nCompleteExperience;

	switch( m_pPlayerDungeonClearInfo->sBaseInfo.cClearRewardType ) {
		case 0:	// 경험치 보너스 ( 일반 )
			{
				SUICoord pointCoord;
				m_pEXP->GetUICoord(pointCoord);
				m_ExpMng->Set(targetExp, COLOR_YELLOW, bWithSign, bCounting, initExp);

				EtColor color(1.f, 0.75f, 0.f, 1.f); // Green
				m_pStaticEXP->SetTextureColor(color);
				m_pStaticEXP->SetManualControlColor(true);
				m_pStaticEXP->Show( true );

				m_pStaticExpGaugeValue->Show( true );
				m_pStaticExpGauge->Show( true );
				for( int i=0; i<5; i++ ) m_pProgressExpGauge[i]->Show( true );

#ifndef _WORK
				if( CDnActor::s_hLocalActor ) {
					CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
					if( pActor->GetAccountLevel() < AccountLevel_Master ) {
						int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
						if( CDnActor::s_hLocalActor->GetLevel() >= nLevelLimit ) {
							targetExp = 0;
						}
					}
				}
#endif

				m_dwGainExp = targetExp;
				m_dwStartExp = m_pDungeonClearValue->nCurrentExp;
				m_fExpGaugeDelta = 0.f;

				UpdateExpGauge( 0.f );
			}
			break;	
		case 1: // 만랩시 골드보상
			m_pRewardGoldDlg->Set( m_pPlayerDungeonClearInfo->sBaseInfo.nRewardGold );
			m_pRewardGoldDlg->Show( true );
			break;
	}
}

void CDnStageClearReportDlg::CalcUpdateLevelRange( int nExp, int &nCurExp, int &nMaxExp )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
	int nExperience;
	int nTempLevel = 1;
	int nOffset = m_pDungeonClearValue->nCurrentLevel - 1;

	if( nOffset < 0 ) nOffset = 0;

	for( int i=nOffset; i<PLAYER_MAX_LEVEL; i++ ) {
		nExperience = CPlayerLevelTable::GetInstance().GetValue( pActor->GetJobClassID(), i + 1, CPlayerLevelTable::Experience );
		if( nExp >= nExperience ) {
			nCurExp = nExperience;
			nTempLevel = i + 1;
		}
		else {
			nMaxExp = nExperience;
			break;
		}
	}
#if defined(_GAMESERVER)
#ifndef _WORK
	if( pActor->GetAccountLevel() < AccountLevel_Master ) {
		int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
		if( nTempLevel >= nLevelLimit ) {
			nTempLevel = nLevelLimit;
			nExp = CPlayerLevelTable::GetInstance().GetValue( pActor->GetJobClassID(), nLevelLimit, CPlayerLevelTable::Experience );
			nCurExp = nExp;
			nMaxExp = nExp;
		}
	}
#endif
#endif

}

void CDnStageClearReportDlg::UpdateExpGauge( float fElapsedTime )
{
	if( !m_pStaticExpGaugeValue->IsShow() ) return;

	m_fExpGaugeDelta += fElapsedTime;
	if( m_fExpGaugeDelta > 1.f ) m_fExpGaugeDelta = 1.f;

	for( int i=0; i<5; i++ )
		m_pProgressExpGauge[i]->SetProgress(0.0f);

	int nCurExp = m_dwStartExp + (int)( m_dwGainExp * m_fExpGaugeDelta );
	int nCurLevExp = 0, nNextLevExp = 0;
	CalcUpdateLevelRange( nCurExp, nCurLevExp, nNextLevExp );
	nNextLevExp -= nCurLevExp;
	nCurExp -= nCurLevExp;

	bool bSetExpGauge = true;
	int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() == nLimitLevel ) {
		CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
		if( pActor->GetAccountLevel() < AccountLevel_Master ) {
			m_pStaticExpGaugeValue->SetText( L"Max" );
			bSetExpGauge = false;
		}
	}
	if( bSetExpGauge ) {
		wchar_t szTemp[32] = {0};
		swprintf_s( szTemp, 32, L"%.2f%%", nNextLevExp ? ((float)nCurExp/(float)nNextLevExp*100.0f) : 0.0f );
		m_pStaticExpGaugeValue->SetText( szTemp );
	}

	int nMaxExp = nNextLevExp / 5;
	if( nMaxExp == 0 ) return;
	int nIndex = nCurExp / nMaxExp;
	int nExp = nCurExp % nMaxExp;

	for( int i=0; i<nIndex; i++ )
	{
		m_pProgressExpGauge[i]->SetProgress(100.0f);
	}

	if( nIndex < 5 )
	{
		m_pProgressExpGauge[nIndex]->SetProgress((nExp*100.0f)/nMaxExp);
	}
}


void CDnStageClearReportDlg::InitPhase_ChannelBonus()
{
	m_ChannelBonusTimer = 0.f;

	m_pStaticChannelBonus->SetBlink(true);
	m_pStaticChannelBonus->Show(true);
}

#if defined(PRE_ADD_WORLD_EVENT)
void CDnStageClearReportDlg::InitPhase_EventBouns(WorldEvent::eDetailEventType type)
#else
void CDnStageClearReportDlg::InitPhase_EventBouns(eEventType3 type)
#endif // #if defined(PRE_ADD_WORLD_EVENT)
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

void CDnStageClearReportDlg::InitPhase_PromotionBonus()
{
	if( !m_pPlayerDungeonClearInfo ) return;

	if( m_pPlayerDungeonClearInfo->sBaseInfo.cMaxLevelCharCount <= 0 ) return;

	m_MaxLevelCharCountTimer = 0.f;
	WCHAR wszTemp[32];
	swprintf_s( wszTemp, L"%d", m_pPlayerDungeonClearInfo->sBaseInfo.cMaxLevelCharCount );
	m_pStaticMaxLevelCharBonus->SetBlink( true );
	m_pStaticMaxLevelCharBonus->Show( true );

	m_pStaticMaxLevelCharCount->SetText( wszTemp );
	m_pStaticMaxLevelCharCount->SetBlink( true );
	m_pStaticMaxLevelCharCount->Show( true );
}

void CDnStageClearReportDlg::ShowPhase_PromotionBonus(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo ) return;
	if( m_pPlayerDungeonClearInfo->sBaseInfo.cMaxLevelCharCount <= 0 ) return;

	m_MaxLevelCharCountTimer += fElapsedTime;

	if( m_MaxLevelCharCountTimer >= 1.f ) {
		m_pStaticMaxLevelCharBonus->SetBlink( false );
		m_pStaticMaxLevelCharCount->SetBlink( false );
	}
}

void CDnStageClearReportDlg::ShowPhase_ChannelBonus(float fElapsedTime)
{
	m_ChannelBonusTimer += fElapsedTime;

	if (m_ChannelBonusTimer >= 1.f)
	{
		m_pStaticChannelBonus->SetBlink(false);
		m_pStatic2ndPwdBonus->SetBlink(false);

#ifdef PRE_ADD_CH_EKEYCLEARBONUS
		if (m_pStaticEKeyBonus)
			m_pStaticEKeyBonus->SetBlink(false);
#endif
	}
}

bool CDnStageClearReportDlg::ShowPhase_Exp(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo ) return false;

	switch( m_pPlayerDungeonClearInfo->sBaseInfo.cClearRewardType ) {
		case 0:
			UpdateExpGauge( fElapsedTime );
			m_ExpMng->Process(fElapsedTime);
			return (m_ExpMng->IsProcessing() == false);
		case 1:
			return true;
			break;
	}
	return true;
}

void CDnStageClearReportDlg::GetUICtrlCenter(EtVector2& result, CEtUIControl* ctrl, float scale)
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

bool CDnStageClearReportDlg::ShowPhase_Rank(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo )
		return false;

	m_PhaseTimer += fElapsedTime;

	if (m_PhaseTimer < m_RankShowTime)
	{
		float ratio = 1.f - ((m_RankShowTime - m_PhaseTimer) / m_RankShowTime);

		float scale = m_RankInitScale - ((m_RankInitScale - 1.f) * ratio) * m_RankAccel;
		m_RankAccel += 0.1f;
		if (scale < 1.f)
			scale = 1.f;
		m_pTextureRank->SetScale(scale);

		EtVector2 prev, cur, result;
		GetUICtrlCenter(prev, m_pTextureRank, 1.f);
		GetUICtrlCenter(cur, m_pTextureRank, scale);
		result = prev - cur;

		SUICoord curCoord;
		m_pTextureRank->GetUICoord(curCoord);
		m_pTextureRank->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);

		return false;
	}
	else
	{
		if (m_RankAfterImangeTimer < m_RankAfterShowTime)
		{
			if (m_RankAfterImangeTimer == 0.f)
			{
				m_pTextureRank->SetScale(1.f);
				m_pTextureRankAfterImage->SetTexture(m_hRankTexture, m_pPlayerDungeonClearInfo->sBaseInfo.cRank*216, 0, 216, 256);
				m_pTextureRankAfterImage->SetTextureColor(m_pTextureRankAfterImage->GetTextureColor());
				m_pTextureRankAfterImage->SetManualControlColor(true);
				m_pTextureRankAfterImage->Show(true);
				m_RankAccel = 1.f;
			}

			m_RankAfterImangeTimer += fElapsedTime;
			float ratio = (m_RankAfterShowTime - m_RankAfterImangeTimer) / m_RankAfterShowTime;
			EtColor color(m_pTextureRankAfterImage->GetTextureColor());
			color.a = 0.7f - float(pow(3 * m_RankAfterImangeTimer, 2)); //ratio - m_RankAfterInitAlpha;
			if (color.a < 0.f)
				color.a = 0.f;
			m_pTextureRankAfterImage->SetTextureColor(color);
			//OutputDebug("[CLEARUI] alpha(%f)\n", color.a);
			float scale = 1.f + log(10.f * m_RankAfterImangeTimer + 1);
 			m_pTextureRankAfterImage->SetScale(scale);

			EtVector2 prev, cur, result;
			GetUICtrlCenter(prev, m_pTextureRankAfterImage, 1.f);
			GetUICtrlCenter(cur, m_pTextureRankAfterImage, scale);
			result = prev - cur;

			SUICoord curCoord;
			m_pTextureRankAfterImage->GetUICoord(curCoord);
			m_pTextureRankAfterImage->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);

			return false;
		}
		else
		{
			m_pTextureRankAfterImage->Show(false);
		}
	}

	return true;
}

void CDnStageClearReportDlg::InitPhase_Rank()
{
	CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndexArray[SoundType::GRADE_RESULT] );

	m_pTextureRank->SetTexture( m_hRankTexture, m_pPlayerDungeonClearInfo->sBaseInfo.cRank*216, 0, 216, 256 );
	m_pTextureRank->SetTextureColor(m_pTextureRank->GetTextureColor());
	m_pTextureRank->SetManualControlColor(true);
	m_pTextureRank->Show( true );

	m_PhaseTimer			= 0.f;
	m_RankAfterImangeTimer	= 0.f;
	m_RankAccel				= 1.f;
	m_RankCheckTimer		= 0.f;

	m_CurPhase = PHASE03_RANK_PROCESS;
}

void CDnStageClearReportDlg::InitPhase_RankChecker()
{
	if( !m_pPlayerDungeonClearInfo )
	{
		_ASSERT(0);
		return;
	}

	m_pStaticRankCheck->Show(true);
	m_pStaticRankCheck->GetUICoord(m_RankCheckOrgCoord);
	OutputDebug("[CHECK_START] coord(%f %f %f %f)\n", m_RankCheckOrgCoord.fX, m_RankCheckOrgCoord.fY, m_RankCheckOrgCoord.fWidth, m_RankCheckOrgCoord.fHeight);

	DWORD curCheckTextureColor = m_pStaticRankCheck->GetTextureColor();
	EtColor color(curCheckTextureColor);
	color.a = 0.f;
	m_pStaticRankCheck->SetTextureColor(color);
	m_pStaticRankCheck->SetManualControlColor(true);

	m_RankCheckTimer	= 0.f;
	m_CheckAccel		= 1.f;
	m_bRankNextStart	= false;
}

bool CDnStageClearReportDlg::ShowPhase_RankChecker(float fElapsedTime)
{
	if( !m_pPlayerDungeonClearInfo )
		return false;

	m_RankCheckTimer += fElapsedTime;

	if (m_RankCheckTimer < m_CheckShowTime)
	{
		float ratio = 1.f - ((m_CheckShowTime - m_RankCheckTimer) / m_CheckShowTime);

		EtColor color(m_pStaticRankCheck->GetTextureColor());
		color.a = ratio;
		m_pStaticRankCheck->SetTextureColor(color);

		EtVector2 prev, cur, result;
		float scale = m_CheckInitScale - ((m_CheckInitScale - 1.f) * ratio) * m_CheckAccel;
		m_CheckAccel += 0.1f;
		if (scale < 1.f)
			scale = 1.f;
		m_pStaticRankCheck->SetTextureScale(scale);
		GetUICtrlCenter(prev, m_pStaticRankCheck, 1.f);
		GetUICtrlCenter(cur, m_pStaticRankCheck, scale);
		result = prev - cur;

		m_pStaticRankCheck->SetPosition(m_RankCheckOrgCoord.fX + result.x, m_RankCheckOrgCoord.fY + result.y);

		if (m_RankCheckTimer > m_CheckShowTime * 0.5f)
			m_bRankNextStart = true;

		return false;
	}
	else
	{
		EtColor color(m_pStaticRankCheck->GetTextureColor());
		color.a = 1.f;
		m_pStaticRankCheck->SetTextureColor(color);
		m_pStaticRankCheck->SetTextureScale(1.f);
	}

	return true;
}

void CDnStageClearReportDlg::OnClose()
{
	HideAllControl();

	m_pPlayerDungeonClearInfo	= NULL;
	m_pDungeonClearValue		= NULL;
	m_fPhaseTime	= 0.0f;
	m_CurPhase		= PHASE_NONE;
	m_nMapIndex		=-1;
	m_nMapLevel		= 0;
}

void CDnStageClearReportDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render(fElapsedTime);
}

void CDnStageClearReportDlg::OnBlindOpen() 
{
	CDnLocalPlayerActor::LockInput(true);
//	CDnMouseCursor::GetInstance().ShowCursor( true );
}

void CDnStageClearReportDlg::OnBlindOpened()
{
	Show(true);
	GetInterface().ShowChatDialog();
}

void CDnStageClearReportDlg::OnBlindClosed()
{
	if( m_bIgnoreRewardItem ) {
		GetInterface().OpenBaseDialog();

		CDnLocalPlayerActor::LockInput(false);
	}
}

void CDnStageClearReportDlg::SetOpenDungeon( int nMapIndex, int nMapLevel )
{
	SAFE_RELEASE_SPTR( m_hOpenDungeon );

	char szImageName[128] = {0};
	sprintf_s( szImageName, _countof(szImageName), "DungeonImage_%d.dds", nMapIndex );

	m_hOpenDungeon = LoadResource( CEtResourceMng::GetInstance().GetFullName( szImageName ).c_str(), RT_TEXTURE );
	if( m_hOpenDungeon )
		m_pTextureOpenDungeon->SetTexture( m_hOpenDungeon, 0, 0, m_hOpenDungeon->Width(), m_hOpenDungeon->Height() );

	CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nMapIndex );
	if( pWorldData )
	{
		std::wstring strMapName;
		pWorldData->GetMapName( strMapName );

		m_pStaticOpenDungeonName->SetText( strMapName );
		m_pStaticOpenLevel->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2305+nMapLevel ) );
	}

	m_pStaticOpenDungeonName->Show( true );
//	m_pStaticOpenLevel->Show( true );	#57775 Dungeon Difficulty Level not match (오픈레벨 표시 삭제)
	m_pStaticOpenBase->Show( true );
//	m_pStaticOpenText->Show( true );	#39276 "위의 스테이지를 입장할 수 있습니다." 라는 문구 삭제
	m_pTextureOpenDungeon->Show( true );
}

void CDnStageClearReportDlg::SetOpenDungeonInfo( int nIndex, int nMapLevel )
{
	m_nMapIndex = nIndex;
	m_nMapLevel = nMapLevel;
}
