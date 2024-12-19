#pragma once

#include "DnActorState.h"

class CDnPlayerActor;
class CDnItem;
class CDnPlayerState : virtual public CDnActorState
{
public:

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	struct sDamageLimitInfo
	{
		bool bPhysicalAttack;
		bool bMagicalAttack;
	};
#endif
	
protected:
	CDnState *m_pSetItemState;
	CDnState *m_pAppellationState;
	CDnState* m_pPetAbilityState;
#if defined(PRE_ADD_SKILLBUF_RENEW)
	CDnState m_BuffState;
#endif

	MSDT_DECL(float) m_fDeadDurabilityRatio;
	MSDT_DECL(char) m_cAccountLevel;
	MSDT_DECL(int) m_nExperience;

	char m_cPvPLevel;

	int m_nNextLevelExperience;
	int m_nCurrentLevelExperience;
	int m_nAppellationIndex;
	int m_nCoverAppellationIndex;

	std::vector<int> m_nVecJobHistoryList;
	vector<CDnState*> m_vecpPetAccessaryState;

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	sDamageLimitInfo m_sDamageLimitInfo;
#endif
	
protected:
	
	CDnPlayerState();
	virtual ~CDnPlayerState();

	virtual void CalcBaseState( StateTypeEnum Type = ST_All );
	virtual void CalcState( RefreshStateExtentEnum Extent, StateTypeEnum Type );
	virtual void GetStateList( int ListType, CDnState::ValueType Type, std::vector<CDnState *> &VecList );

	void CalcSetItemState();
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	void CalcOverLapSetItemState(int nOverLapSetItemID, std::map<int, int>& nMapSetList);
#endif
	void CalcAppellationState();
	void CalcPetAbilityState();
	void CalcPetAccessaryAbilityState();
	virtual void RefreshAdditionalState( RefreshStateExtentEnum &Extent, StateTypeEnum &Type );

	char GetUseRegulation();

#ifdef PRE_ADD_STAGE_DAMAGE_LIMIT
	float GetUseStageStateLimit();
#endif

#if defined (PRE_ADD_BESTFRIEND)
	bool IsApplyPartsState(CDnActor *pActor, CDnParts* pParts, bool Durability = true);
	bool IsApplyWeaponState(CDnActor *pActor, CDnWeapon* pWeapon, bool Durability = true);
	bool IsBestFriendItem(CDnActor* pActor, eItemTypeEnum cType, int nParam1, int nParam2, INT64 nSerial);
#endif

public:
	virtual void OnChangeJob( int nJobID );
	virtual void OnCalcPlayerState(){}

	__forceinline float GetDeadDurabilityRatio() { return m_fDeadDurabilityRatio; }

	virtual void SetAppellationIndex( int nCoverIndex, int nIndex );
	void ChangeAppellationState( int nCoverArrayIndex, int nArrayIndex );
	bool IsSetAppellationIndex();

	int GetAppellationIndex() { return m_nAppellationIndex; }

	void SetAccountLevel( char cLevel ) { m_cAccountLevel = cLevel; }
	char GetAccountLevel() { return m_cAccountLevel; }
	bool IsDeveloperAccountLevel();
	bool IsDeveloperAccountLevel(char cAccountLevel);

	virtual void SetPvPLevel( char cLevel ) { m_cPvPLevel = cLevel; }
	char GetPvPLevel() { return m_cPvPLevel; }

	void CalcExperience();
	__forceinline int GetExperience() { return m_nExperience; }
	__forceinline int GetNextLevelExperience() { return m_nNextLevelExperience; }
	__forceinline int GetCurrentLevelExperience() { return m_nCurrentLevelExperience; }
	void SetExperience( int nValue );
	void AddExperience( int nValue, int nLogCode, INT64 biFKey );	// -> nLogCode에 따라 biFKey값도 넣어준다 (eCharacterExpChangeCode참조)

	int GetJobClassID();

	int GetJobHistory( std::vector<int> &nVecResult );
	virtual void SetJobHistory( const std::vector<int> &nVecList );
	virtual void SetJobHistory( int nValue );
	virtual void PopJobHistory( int nValue );		// #26902 마지막으로 전직한 것을 롤백. 현재 서버에서 이런 거 하는 경우는 없다. 임시 전직에 대해서 클라에서 사용됨.
	bool IsPassJob( int nJobClassID );

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	const sDamageLimitInfo* GetDamageLimitInfo(){ return &m_sDamageLimitInfo; }
#endif

#if defined(PRE_ADD_SKILLBUF_RENEW)
	const CDnState* GetBuffState(){ return &m_BuffState; }
#endif

	// CDnActorState
	void CalcStrength( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcAgility( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcIntelligence( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcStamina( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcAttackP( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcAttackM( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcDefenseP( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcDefenseM( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcHP( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList, bool bRevisionCurrentHP = false );
	void CalcSP( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList, bool bRevisionCurrentSP = false );
	void CalcMoveSpeed( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcStiff( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcStiffResistance( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcCritical( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcCriticalResistance( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcStun( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcStunResistance( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcElementAttack( ElementEnum Type, RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcElementDefense( ElementEnum Type, RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcDownDelay( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcRecoverySP( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcSuperAmmor( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcFinalDamage( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcSpirit( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcSafeZoneMoveSpeed( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void CalcAddExp( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );

	virtual void OnCalcEquipStep( StateTypeEnum Type, CDnState *pState );
	virtual void OnCalcEquipStep_ElementAttack( ElementEnum eElementType , CDnState *pState ); 
	virtual void OnCalcEquipStep_ElementDefense( ElementEnum eElementType , CDnState *pState );
	// 속성은 모든 속성을 루프돌리기때문에 내부에서 값변경이 있을대 문제가 생긴다. 각 속성마다 값셋팅을 해주어야한다.

	static int GetJobToBaseClassID( int nJobID );
	CDnState MakeEquipState();
	CDnState MakeComparePotentialState(CDnItem* pCurrItemState, CDnItem* pNewItemState);
};