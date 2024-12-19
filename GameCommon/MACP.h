#pragma once

class MACP {
public:
	MACP();
	virtual ~MACP();

	enum CPTypeEnum {
		MaxComboCount,
		KillBossCount,
		KillMonsterScore,
		CriticalHitScore,
		StunHitScore,
		SuperAmmorBreakScore,
		BrokenShieldScore,
		GenocideScore,
		AirComboScore,
		PartyComboScore,
		RebirthPlayerScore,
		UseSkillScore,
		ComboScore,
		AttackedCount,
		AttackedHitScore,
		AttackedCriticalHitScore,
		AttackedStunHitScore,
		DieCount,
		DeadScore,
		PropBreakScore,
		GetItemScore,
		AssistMonsterScore,
		CPType_Max
	};

	typedef struct tagMACP
	{
		int nAccumulationCP;

		// 테이블에서 미리 읽어논 값들 서버와 공용이기때문에 언제나 Get 하기엔 무리가 있다. GameTask 에서 가지구있어도 되지만.. 얻어오기 애매하다.
		int pScore[CPTypeEnum::CPType_Max];

		// 현재 cp에서는 직접 사용하지 않지만, 미션에서 사용해야해서 추가한 것들
		int nPartyMemberDieCount;
		int nStartPartyCount;

		// 만랩일시 경험치 획득 넣어놀데가 없어서 여기어 넣습니다.
		int nMaxLevelGainExperience;

		// 스테이지 클리어 경험치..
		int nCompleteExperience;
	}SMACP;

protected:
	
	CDnActor *m_pActor;
	SMACP m_Data;

	int m_nCPTableID;

protected:
	virtual void OnEventCP( CPTypeEnum Type, int nResult );
	bool SetCPTableID( const int nMapIndex, const int nClassID );

public:
	virtual void ResetCP();
	virtual bool InitializeCPScore( const int nMapIndex, const int nClassID, const int nDungeonClearID );
	bool Initialize( CDnActor *pActor );

	void UpdateMaxCombo( int nValue );
	void UpdateKillBoss();

	virtual bool UpdateKillMonster( int *pResult = NULL );
	bool UpdateCriticalHit( int *pResult = NULL );
	bool UpdateStunHit( int *pResult = NULL );
	bool UpdateSuperAmmorBreak( int *pResult = NULL );
	bool UpdateBrokenShield( int *pResult = NULL );
	virtual bool UpdateGenocide( int *pResult = NULL );
	bool UpdateAirCombo( int *pResult = NULL );
	virtual bool UpdatePartyCombo( int nCombo, int *pResult = NULL );
	virtual bool UpdateRebirthPlayer( int *pResult = NULL );
	virtual bool UpdateUseSkill( DnSkillHandle hSkill, int *pResult = NULL );
	virtual bool UpdateCombo( int nCombo, int *pResult = NULL );

	bool UpdateAttackedHit( int *pResult = NULL );
	bool UpdateAttackedCriticalHit( int *pResult = NULL );
	bool UpdateAttackedStunHit( int *pResult = NULL );
	bool UpdateDead( int *pResult = NULL );

	// 미션추가용
	bool UpdatePartyMemberDead( int *pResult = NULL );
	bool UpdateStartPartyCount( int nPartyCount );

	int GetMaxComboCount() { return m_Data.pScore[CPTypeEnum::MaxComboCount]; }
	int GetKillBossCount() { return m_Data.pScore[CPTypeEnum::KillBossCount]; }
	int GetAccumulationCP() { return m_Data.nAccumulationCP; }
	int GetDieCount() { return m_Data.pScore[CPTypeEnum::DieCount]; }
	int GetAttackedCount() { return m_Data.pScore[CPTypeEnum::AttackedCount]; }

	// 미션추가용
	int GetPartyMemberDieCount() { return m_Data.nPartyMemberDieCount; }
	int GetStartPartyCount() { return m_Data.nStartPartyCount; }

	virtual bool UpdatePropBreak( int* pResult = NULL );
	virtual bool UpdateGetItem( int* pResult = NULL );

	// 치트땜시 만든다..이걸루 셋팅하는 일은 없어야합니다.
	void SetAccumulationCP( int nValue ) { m_Data.nAccumulationCP = nValue; }

	bool UpdateAssistScore( int myAggro, int wholeAggro );

	bool UpdateMaxLevelGainExperience( int nExp );
	int GetMaxLevelGainExperience() { return m_Data.nMaxLevelGainExperience; }

	inline void AddCompleteExperience( int nValue ) { m_Data.nCompleteExperience += nValue; };
	inline int GetCompleteExperience() { return m_Data.nCompleteExperience; };

	const SMACP & GetCP_Data() { return m_Data; }
};