
#pragma once

#include "EventControlHeader.h"
#include "EtWorldEventArea.h"
#include "DnDropItem.h"

class CEtWorldEventArea;
class CDNGameRoom;
class CPvPItemRespawnLogic;

class CPvPRespawnLogic:public TBoostMemoryPool<CPvPRespawnLogic>
{
public:

	CPvPRespawnLogic( CDNGameRoom* pGameRoom );
	~CPvPRespawnLogic();

	// Process
	void				Process( LOCAL_TIME LocalTime, float fDelta );
	//
	void				FinishInitialize(){ m_bInitialize = true; }
	void				AddRespawnArea( CEtWorldEventArea* pArea );
	void				AddItemRespawnArea( CEtWorldEventArea* pArea );
	void				ResetPoint();
	void				ToggleCheat();
	void				FinishRound();
	//
	void				OnDie( DnActorHandle hActor );
	CEtWorldEventArea*	OnRespawn( DnActorHandle hActor );

	typedef struct tagRespawnPoint
	{
		CEtWorldEventArea*	pArea;
		int					iPoint;
		//
		tagRespawnPoint( CEtWorldEventArea* _pArea ):pArea(_pArea),iPoint(0){}

		// 최종 선호 점수
		int GetTotalPoint() const
		{
			PvPRespawnAreaStruct* pStruct = reinterpret_cast<PvPRespawnAreaStruct*>(pArea->GetData());
			return pStruct->nBasePreferScore + iPoint;
		}

		// 최종 선호 점수 Range 값 검사
		void CheckRange()
		{
			PvPRespawnAreaStruct* pStruct = reinterpret_cast<PvPRespawnAreaStruct*>(pArea->GetData());
			if( pStruct->nBasePreferScore + iPoint > RespawnPoint::eMaxRange )
				iPoint = RespawnPoint::eMaxRange-pStruct->nBasePreferScore;
			else if( pStruct->nBasePreferScore + iPoint < -RespawnPoint::eMaxRange )
				iPoint = -RespawnPoint::eMaxRange-pStruct->nBasePreferScore;
		}
	}SRespawnPoint;

private:

	struct RespawnPoint
	{
		enum
		{
			eDiePoint			= -10,	// 우리팀 죽었을 때 리스폰 포인트
			eOpponentDiePoint	= 20,	// 상대팀 죽었을 때 리스폰 포인트
			eRespawnPoint		= -10,	// 리스폰 할때 마다 주어지는 포인트
			eNotUseRespawnPoint	= 5,	// 사용하지 않은 리스폰 위치 포인트
			eMaxRange			= 100,	// 최종 선호 점수 -,+ MAX
		};
	};

	struct Common
	{
		enum
		{
			eCheatSec			= 1,
		};
	};

	void						_AddRespawnPoint( SRespawnPoint* pRespawnPoint, int iAddPoint );
	SRespawnPoint*				_GetNearestRespawnArea( EtVector3* pVec, const UINT uiTeam );
	void						_SendCheat();

	CDNGameRoom*				m_pGameRoom;
	bool						m_bInitialize;
	bool						m_bCheat;
	float						m_fCheat;
	DNVector(SRespawnPoint)		m_vTeamARespawnPoint;
	DNVector(SRespawnPoint)		m_vTeamBRespawnPoint;

	CPvPItemRespawnLogic*		m_pItemRespawnLogic;

	void						_ProcessItemRespawn( const float fDelta );
};
