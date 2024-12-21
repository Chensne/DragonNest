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
	int		m_nTeam; //���� ��
	float   m_fCountDownTime;
	
	bool	m_bIsTeamHairColorMode;	// ���� �°� ��� �����Ѵ�.
	bool    m_bIsHairColorMode;
	bool    m_bIsIndividualMode;

public:
	virtual void ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin = false, void * pData = NULL );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void RestartGame();

	void SetHairColorMode(bool bTrue){m_bIsHairColorMode = bTrue;}; // �������� ��/���� ������ �Ӹ��� ������ �ʿ������ �մϴ�.
	void SetIndividualMode(bool bTrue){m_bIsIndividualMode = bTrue;}
	bool IsIndividualMode(){return m_bIsIndividualMode;} // �������̳� �ƴϳ�? 

};