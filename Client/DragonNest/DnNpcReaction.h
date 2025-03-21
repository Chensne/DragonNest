#pragma once

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

const int NPC_REACTION_COUNT_MAX = 10;

class CReputationSystemRepository;

// 마을에서 플레이어 캐릭터와 npc 의 호감도에 따라서 리액션을 처리한다.
class CDnNpcReaction
{
private:
	struct S_NPC_REACTION_INFO
	{
		int iNpcID;
		int aiFavorThresholdPercent[ NPC_REACTION_COUNT_MAX ];
		int aiSpeechUIStringID[ NPC_REACTION_COUNT_MAX ];
		string astrEffectActionName[ NPC_REACTION_COUNT_MAX ];


		S_NPC_REACTION_INFO( void ) : iNpcID( 0 )
		{
			SecureZeroMemory( aiFavorThresholdPercent, sizeof(aiFavorThresholdPercent) );
			SecureZeroMemory( aiSpeechUIStringID, sizeof(aiSpeechUIStringID) );
		};
	};

	// 초기화 된 상태인지
	bool m_bInitialized;

	// 빌리지 태스크에서 물려준 npc 호감도 저장소 객체 포인터
	CReputationSystemRepository* m_pReputationRepos;
	map<int, S_NPC_REACTION_INFO> m_mapNpcReaction;
	set<DWORD> m_setNpcsInRange;

	// 서버 스크립트로부터 npc 이펙트 출력 요청이 왔을 때 인덱스에 해당되는 액션 이름을 저장해 둠.
	vector<string> m_vlSCNpcEffectActionName;

public:
	CDnNpcReaction( void );
	virtual ~CDnNpcReaction( void );

	void Initialize( CReputationSystemRepository* pReputationRepos );
	bool IsInitialized( void ) { return m_bInitialized; };
	void Clear( void );
	void AttachNpcEffect( UINT uiNpcID, int iEffectdIndex );
	
	void Process( LOCAL_TIME LocalTime, float fDelta );

};

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM