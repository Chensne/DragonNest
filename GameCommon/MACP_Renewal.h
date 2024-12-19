#pragma once

#if defined( PRE_ADD_CP_RENEWAL )

#include "MACP.h"

class MACP_Renewal : public MACP
{
public :
	enum{
		eBonusCP_Revision_BossKill = 1,
		eBonusCP_Revision_MaxCombo = 1,
		eBonusCP_Revision_ClearTime = 5,
	};

public:
	MACP_Renewal();
	virtual ~MACP_Renewal();

public:
	virtual void ResetCP();
	virtual bool InitializeCPScore( const int nMapIndex, const int nClassID, const int nDungeonClearID );

protected:
	bool VerifyStandardCP( const MACP::CPTypeEnum eCPType, int & nCP );
	bool UpdateCP( const int nScore, const MACP::CPTypeEnum eCPType, int * pResult = NULL );
	
	virtual void OnEventCP( CPTypeEnum Type, int nResult );

public:
	virtual bool UpdateKillMonster( int *pResult = NULL );
	virtual bool UpdateGetItem( int* pResult = NULL );
	virtual bool UpdateGenocide( int *pResult = NULL );
	virtual bool UpdateUseSkill( DnSkillHandle hSkill, int *pResult = NULL );
	virtual bool UpdatePartyCombo( int nCombo, int *pResult = NULL );
	virtual bool UpdateRebirthPlayer( int *pResult = NULL );
	virtual bool UpdateCombo( int nCombo, int *pResult = NULL );
	virtual bool UpdatePropBreak( int* pResult = NULL );

	int GetStandardCP();

	const int * GetCP_IndexData() { return m_AccumulationCP;}

	static int GetClearTime_Revision( const Dungeon::Difficulty::eCode eDifficulty );

protected:
	int m_nStandardCP;
	float m_StandardWeight[MACP::CPTypeEnum::CPType_Max];
	int m_AccumulationCP[MACP::CPTypeEnum::CPType_Max];
};

#endif	// #if defined( PRE_ADD_CP_RENEWAL )