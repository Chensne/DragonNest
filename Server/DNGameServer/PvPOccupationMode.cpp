
#include "Stdafx.h"
#include "PvPOccupationMode.h"
#include "PvPOccupationSystem.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "DNMasterConnectionManager.h"
#include "DNGameRoom.h"
#include "DNGuildGame.h"
#include "DNDBConnectionManager.h"
#include "DNGameDataManager.h"
#include "PvPOccupationScoreSystem.h"
#include "PvPGuildWarScoreSystem.h"

CPvPOccupationMode::CPvPOccupationMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket ) : \
CPvPGameMode( pGameRoom, pPvPGameModeTable, pPacket ), m_pOccupationSystem(new CPvPOccupactionSystem(m_pGameRoom, this)), m_bGuildWar(pPacket->bIsGuildWar)
{
	memset(&m_ModeInfo, 0, sizeof(TOccupationModeInfo));
}

CPvPOccupationMode::~CPvPOccupationMode()
{
}

bool CPvPOccupationMode::GetOccupationTeamScore(int nTeam, int &nScore)
{
	return static_cast<CPvPGuildWarScoreSystem*>(m_pScoreSystem)->GetOccupationTeamScore(nTeam, nScore);
}

bool CPvPOccupationMode::SetForceWinGuild(UINT nWinGuildDBID)
{
	if (m_pOccupationSystem)
		return m_pOccupationSystem->SetForceWinGuild(nWinGuildDBID);
	return false;
}

void CPvPOccupationMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if (m_pOccupationSystem)
		m_pOccupationSystem->Process(LocalTime, fDelta);

	CPvPGameMode::Process(LocalTime, fDelta);
}

bool CPvPOccupationMode::InitializeOccupationSystem(TBattleGourndModeInfo * pModeInfo)
{
	if (m_pGameRoom == NULL || pModeInfo == NULL)
	{
		_DANGER_POINT();
		return false;
	}

	if (m_pOccupationSystem == NULL)
	{
		_DANGER_POINT();
		return false;
	}

	if (m_pOccupationSystem->InitializeOccupationSystem(pModeInfo) == false)
	{
		_DANGER_POINT();
		return false;
	}
	return m_pScoreSystem->InitializeOccupationSystem(pModeInfo);
}

bool CPvPOccupationMode::InitializeOccupationMode(TOccupationModeInfo * pInfo)
{
	m_ModeInfo = *pInfo;
	return true;
}

int CPvPOccupationMode::OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	if (pSession == NULL)
		return ERROR_GENERIC_INVALIDREQUEST;

	switch (nSubCmd)
	{
	case ePvP::CS_TRYCAPTURE:
		{
			if (sizeof(CSPvPTryAcquirePoint) != nLen)
				return ERROR_INVALIDPACKET;

			CSPvPTryAcquirePoint * pPacket = (CSPvPTryAcquirePoint*)pData;
			
			bool bRet = OnTryAcquirePoint(pSession->GetActorHandle(), pPacket->nAreaID, LocalTime);
			pSession->SendPvPTryAcquirePoint(pPacket->nAreaID, bRet == true ? ERROR_NONE : ERROR_PVP_TRYACQUIREPOINT_FAIL);
			return ERROR_NONE;
		}

	case ePvP::CS_CANCELCAPTURE:
		{
			if (0 != nLen)
				return ERROR_INVALIDPACKET;
			
			OnCancelAcquirePoint(pSession->GetActorHandle());
			return ERROR_NONE;
		}

	case ePvP::CS_TRYACQUIRE_SKILL:
		{
			if (sizeof(CSPvPTryAcquireSKill) != nLen)
				return ERROR_INVALIDPACKET;

			CSPvPTryAcquireSKill * pPacket = (CSPvPTryAcquireSKill*)pData;

			if (!(pSession->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain))
				return ERROR_PVP_TRYACQUIRESKILL_FAIL_AUTH;

			if (m_pOccupationSystem == NULL)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (pPacket->cSlotIndex >= QUICKSLOTMAX || pPacket->cSlotIndex < 0)
				return ERROR_PVP_TRYACQUIRESKILL_FAIL;

			if (m_pOccupationSystem->OnTryAcquireSkill(pSession, pPacket->nSkillID, pPacket->nLevel, pPacket->cSlotIndex) == false)
				pSession->SendPvPTryAcquireSkill(pPacket->nSkillID, pPacket->nLevel, ERROR_PVP_TRYACQUIRESKILL_FAIL);
			return ERROR_NONE;
		}

	case ePvP::CS_INITSKILL:
		{
			if (0 != nLen)
				return ERROR_INVALIDPACKET;

			if (!(pSession->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain))
				return ERROR_PVP_INITSKILL_FAIL;

			if (m_pOccupationSystem == NULL)
				return ERROR_GENERIC_UNKNOWNERROR;
			
			if (m_pOccupationSystem->OnInitSkill(pSession) == false)
				pSession->SendPvPInitSkill(ERROR_PVP_INITSKILL_FAIL);
			return ERROR_NONE;
		}

	case ePvP::CS_USESKILL:
		{
			if (sizeof(CSPvPUseSkill) != nLen)
				return ERROR_INVALIDPACKET;

			CSPvPUseSkill * pPacket = (CSPvPUseSkill*)pData;

			if (!(pSession->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain) && !(pSession->GetPvPUserState()&PvPCommon::UserState::GuildWarSedcondCaptain))
				return ERROR_PVP_USESKILL_FAIL;

			if (m_pOccupationSystem == NULL)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (m_pOccupationSystem->OnUseSkill(pSession, pPacket->nSkillID) == false)
				pSession->SendPvPUseSkill(pSession->GetSessionID(), pPacket->nSkillID, ERROR_PVP_USESKILL_FAIL);
			
			return ERROR_NONE;
		}

	case ePvP::CS_SWAPSKILLINDEX:
		{
			if (sizeof(CSPvPSwapSkillIndex) != nLen)
				return ERROR_INVALIDPACKET;

			CSPvPSwapSkillIndex * pPacket = (CSPvPSwapSkillIndex*)pData;

			if (!(pSession->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain))
				return ERROR_PVP_USESKILL_FAIL;

			if (m_pOccupationSystem == NULL)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (m_pOccupationSystem->SwapSkillIndex(pSession, pPacket->cFromIndex, pPacket->cToIndex) == false)
				pSession->SendPvPSwapSkillIndex(pPacket->cFromIndex, pPacket->cToIndex, ERROR_PVP_SWAPSKILLINDEX_FAIL);
			return ERROR_NONE;
		}
	}
	return ERROR_NONE;
}

void CPvPOccupationMode::OnFinishProcessDie( DnActorHandle hActor )
{
	_RebirthProcess(hActor);
}

void CPvPOccupationMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	if (PvPCommon::FinishReason::OpponentTeamQualifierScore == Reason && m_bGuildWar == false)
	{
		_DANGER_POINT();
		return;		//시스템에의한 길드전이 아닌경우에는 예전선 스코어 같은게 없다
	}

	if (bIsFinishFlag())
		return;

	bool bFinish = false;
	USHORT nTeam = PvPCommon::Team::Max;
	UINT nWinGuildDBID = 0;
	UINT nLoseGuildDBID = 0;
	if (m_pOccupationSystem)
		bFinish = m_pOccupationSystem->OnCheckFinishRound(Reason, nTeam, nWinGuildDBID, nLoseGuildDBID);

	if (bFinish)
	{
		m_pOccupationSystem->OccupationReportLog(Reason, nWinGuildDBID, nLoseGuildDBID);

		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
			// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
			if( pPartyStruct->pSession && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
			{
				DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
				if( hActor )
					m_pScoreSystem->OnFinishRound( hActor, (nTeam == hActor->GetTeam() ? true : false) );
			}		
		}	

		FinishGameMode(nTeam, Reason);
		if (g_pMasterConnectionManager && bIsGuildWarSystem())
		{
			g_pMasterConnectionManager->SendPvPGuildWarResult(m_pGameRoom->GetWorldSetID(), nWinGuildDBID, nLoseGuildDBID, nTeam == PvPCommon::Team::Max ? true : false);

			GetGameRoom()->QueryAddGuildWarFinalResults(nWinGuildDBID, 1);
			GetGameRoom()->QueryAddGuildWarFinalResults(nLoseGuildDBID, 2);
		}
	}
}

void CPvPOccupationMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CPvPGameMode::OnSuccessBreakInto(pGameSession);

	if (m_pOccupationSystem)
		m_pOccupationSystem->OnSuccessBreakInto(pGameSession);
}

bool CPvPOccupationMode::OnTryAcquirePoint(DnActorHandle hActor, int nAreaID, LOCAL_TIME LocalTime)
{
	if (m_pOccupationSystem)
		return m_pOccupationSystem->OnTryAcquirePoint(hActor, nAreaID, LocalTime);
	return false;
}

bool CPvPOccupationMode::OnCancelAcquirePoint(DnActorHandle hActor)
{
	if (m_pOccupationSystem)
		return m_pOccupationSystem->OnCancelAcquirePoint(hActor);
	return false;
}

void CPvPOccupationMode::OnRebirth( DnActorHandle hActor, bool bForce/*=false*/ )
{
	if (m_bFinishGameModeFlag)
		CPvPGameMode::OnRebirth( hActor, true );	
	else
		_RebirthProcess(hActor, true);
}

void CPvPOccupationMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie(hActor, hHitter);
	if (m_pOccupationSystem)
		m_pOccupationSystem->OnDie(hActor, hHitter);
}

void CPvPOccupationMode::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage )
{
	if (m_pOccupationSystem)
		m_pOccupationSystem->OnDamage(hActor, hHitter, iDamage);
}

bool CPvPOccupationMode::bIsBoss(DnActorHandle hActor)
{
	if (m_pOccupationSystem)
		return m_pOccupationSystem->IsTeamBoss(hActor);
	return false;
}

void CPvPOccupationMode::_RebirthProcess(DnActorHandle hActor, bool bUseCurPos)
{
	if (hActor && hActor->IsDie())
	{
		// 리스폰위치 설정
		if (bUseCurPos == false)
			_SetRespawnPosition( hActor );

		// 부활시킴
		UINT uiRespawnHPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnHPPercent			: 100;
		UINT uiRespawnMPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnMPPercent			: 100;
		UINT uiImmortalTime		= GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnNoDamageTimeSec	: 5;

		char szParam[32];
		sprintf_s( szParam, "Coin/%d/%d/%d", uiRespawnHPPercent, uiRespawnMPPercent, uiImmortalTime );
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_057, uiImmortalTime*1000, szParam );
	}
}
