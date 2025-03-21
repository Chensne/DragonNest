
#pragma once

class CDNPvPGameRoom;
class CPvPOccupactionSystem;

class CPvPOccupationPoint : public TBoostMemoryPool<CPvPOccupationPoint>
{
public:
	CPvPOccupationPoint(CDNGameRoom * pGameRoom, CPvPOccupactionSystem * pSystem, SOBB * pOBB, TPositionAreaInfo * pAreaInfo);
	~CPvPOccupationPoint();

	void Process(LOCAL_TIME LocalTime, float fDelta);
	void OnDamage(DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage);

	int GetID() { return m_OccupationInfo.nAreaID; }
	bool CheckInside(EtVector3 &Position);
	void GetPointState(PvPCommon::OccupationStateInfo &Info);

	bool TryAcquirePoint(DnActorHandle hActor, LOCAL_TIME LocalTime);
	bool CancelAcquirePoint(DnActorHandle hActor);

	bool ChangeClimaxMode();

private:
	CDNGameRoom * m_pGameRoom;
	CPvPOccupactionSystem * m_pOccupationSystem;
	SOBB m_OBB;

	int m_nOccupantState;		//소유상태 PvPCommon::OccupationState	
	UINT m_nOccupantUniqueID;	//소유성공자
	short m_nOccupantTeamID;			//소유자(팀)
	
	UINT m_nTryUniqueID;		//시도중인 녀석
	short m_nTryTeamID;
	std::wstring m_wstrTryName;	//시도중인 녀석 이름

	UINT m_nWaitUniqueID;
	short m_nWaitTeamID;
	std::wstring m_wstrMoveWaitName;	//시도중인 녀석 이름

	LOCAL_TIME m_nTryOccupationTime;		//점령시도 시작시간
	LOCAL_TIME m_nMoveWaitOccupationTime;	//점령이동 시작시간
	LOCAL_TIME m_nLastOccupationCheckTime;	//점령시간

	TPositionAreaInfo m_OccupationInfo;
	volatile bool m_bClimaxMode;

	//Check
	void _CheckPoint(LOCAL_TIME LocalTime);
	void _ClearTry();
	void _ClearMoveWait();
	void _OnOccupyArea();

	//
	bool _AcquirePoint(int nUniqueID);
	bool _IsAcquireResourceTerm(LOCAL_TIME LocalTime);
	bool _IsFinishedTryTerm(LOCAL_TIME LocalTime);
	bool _IsFinishedMoveWaitTerm(LOCAL_TIME LocalTime);

	//send
	void _SendOccupationPointState();
};
