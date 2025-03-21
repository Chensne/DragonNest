#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnPvPGameTask.h"

class CDnMutatorTeamGame:public CDnMutatorGame
{
public:
	CDnMutatorTeamGame( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorTeamGame();

protected:
	int		m_nTeam; //나의 팀
	float   m_fCountDownTime;
	
	bool	m_bIsTeamHairColorMode;	// 팀에 맞게 헤어 셋팅한다.
	bool    m_bIsHairColorMode;
	bool    m_bIsIndividualMode;

public:
	virtual void ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin = false, void * pData = NULL );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void RestartGame();

	void SetHairColorMode(bool bTrue){m_bIsHairColorMode = bTrue;}; // 개인전은 팀/적군 에따른 머리색 구분이 필요없을듯 합니다.
	void SetIndividualMode(bool bTrue){m_bIsIndividualMode = bTrue;}
	bool IsIndividualMode(){return m_bIsIndividualMode;} // 개인전이냐 아니냐? 

};