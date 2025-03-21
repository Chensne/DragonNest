#pragma once

class CDnWorldOperationProp;
class CEtWorldEventArea;

class CDnOccupationZone
{
public :

	enum{
		ZONE_GUILD = 0,
		ZONE_A,
		ZONE_B,
		ZONE_C,
	};

	enum{
		COUNT_INDEX = 3,	// dds 에 3가지 종류(A,B,C)
	};

	CDnWorldOperationProp * m_pProp;
	int m_eState;

	UINT m_nOwnedUniqueID;			// 소유시점 성공유저
	short m_nOwnedTemID;			// 현재소유팀

	UINT m_nTryUniqueID;			// 시도자 PvPCommon::OccupationState이 try상태일경우
	short m_nTryTeamID;				// 시도 하는 팀

	UINT m_nMoveWaitUniqueID;		// 이동대기상태 성공시킨 유저
	short m_nMoveWaitTeamID;		// 이동대기상태팀

	int m_nAreaID;
	SOBB m_OBB;

	int m_nRedClickStringID;
	int m_nBlueClickStringID;

	int m_nRedOwnStringID;
	int m_nBlueOwnStringID;

	int m_nRedDefendStringID;
	int m_nBlueDefendStringID;

	int m_nRedTeamStringID;
	int m_nBlueTeamStringID;

#ifdef PRE_MOD_PVPOBSERVER
	int m_nObserverRedTeamStringID;
	int m_nObserverBlueTeamStringID;
#endif // #ifdef PRE_MOD_PVPOBSERVER

	int m_nTrySound;
	int m_nOwnSound;

	int m_nTableModeID;

	std::wstring m_wszName;

	int m_nZoneIndex;	// 점령전 이미지 인덱스 표시 하기위한 값
	float m_fElapsedTime;

	CDnOccupationZone();
	virtual ~CDnOccupationZone();

	bool Initialize( CEtWorldEventArea * pArea, int nTableModeID );
	void Process( float fElapsedTime );
	
	void Clear();
	bool IsFlag( DnPropHandle hProp );
	bool IsFlag( int nAreaID );

	bool IsFlagOperation( DnPropHandle hProp );
	EtVector3 GetPropPosition();
	int GetImageIndex();
	DWORD GetImageColor();
};
