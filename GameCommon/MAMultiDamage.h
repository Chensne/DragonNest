#pragma once

#include "MASingleDamage.h"


class MonsterParts
{
public:

#if defined( PRE_ADD_LOTUSGOLEM )
	enum ePartsState
	{
		eNormal		= 0,		// 일반 상태
		eIgnore		= 1,		// 파츠 무시(?)상태
		eNoDamage	= (1<<1),	// 이 상태에서는 클라이언트에서 헛방(?) 처리
	};
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

	struct _PartsInfo 
	{
		_PartsInfo( const char* pszMeshName, const char* pszBoneName, bool bApplyEffect ):szMeshName(pszMeshName),szBoneName(pszBoneName),bApplyDeadEffect(bApplyEffect){}

		std::string szBoneName;
		std::string szMeshName;
		bool bApplyDeadEffect;
	};

	struct _HitCondition
	{
		int iRequiredPartsID;
		int iRequiredPartsUnderHP;
	};

	struct  _Info
	{
		UINT	uiMonsterPartsTableID;
#if defined( PRE_ADD_LOTUSGOLEM )
		int	PartsState;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
		int		nHP;				// 파츠HP
		int		nHPRefreshTimems;	// HP회복시간(ms)
		float	fMainDamageRate;	// 본체 데미지 비율
		float	fDefenseRate;		// 방어력비율
		int		nPassiveSkillIndex;	// 파츠 스킬 효과
		int		nPassiveSkillLevel;	// 파츠 스킬 레벨
		bool	bIsRefreshUseSkill;	// 파츠 재생 되었을 때 nPassiveSkillIndex 자동 생성할지 말지 플래그
		int		nDestroySkillIndex;	// 파괴되었을 때 사용할 스킬 인덱스
		std::string szDeadSkinName;
		std::string szDeadAniName;
		std::string szDeadActName;
		std::string szAction;
		std::string szDeadActorActName;
		std::string szDeadActorActBoneName;
		std::string szDeadActorFixBoneName;

#ifndef _GAMESERVER
		std::wstring wszPartsName;
		bool m_bEnalbeUI;
#endif

#if defined( _GAMESERVER )
		std::vector<UINT>			vRebirthPartsTableID;
#endif
		DNVector(std::string)		vLimitAction;	// 액션제한
		DNVector(_PartsInfo)		vParts;
		DNVector(_HitCondition)		vHitCondition;

		_Info()
		{
			uiMonsterPartsTableID = 0;
			nHP = 0;
			nHPRefreshTimems = 0;
			fMainDamageRate = 0.f;
			fDefenseRate = 0.f;
			nPassiveSkillIndex = 0;	
			nPassiveSkillLevel = 0;
			bIsRefreshUseSkill = false;
			nDestroySkillIndex = 0;
#ifndef _GAMESERVER
			m_bEnalbeUI = false;
#endif


#if defined( PRE_ADD_LOTUSGOLEM )
			PartsState	= eNormal;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
		}
	};

	MonsterParts();
	~MonsterParts();

	bool			Process( const float fDelta );
	bool			Create( const _Info& info );
	bool			CreateSkill( DnActorHandle hActor );
	bool			HasBone( const char* pszBoneName );
	bool			IsLimitAction( const char* pszActionName );
	int             GetMaxHP(){ return m_OriginalInfo.nHP; }
	int				GetHP(){ return m_Info.nHP; }
	void			SetHP( const int nHP ){ m_Info.nHP = nHP; }
	void			SetRefresh( DnActorHandle hActor );
	void			SetDestroy( DnActorHandle hActor );
	void			ResetSetting() { m_Info = m_OriginalInfo; }
	void			ResetDestoryDelta(){ m_fDestroyDelta = 0.f; }
	//
	float			GetDefenseRate(){ return m_Info.fDefenseRate; }
	float			GetMainDamageRate(){ return m_Info.fMainDamageRate; }
	int				GetDestroySkillIndex(){ return m_Info.nDestroySkillIndex; }	// 파츠 파괴되었을 때 사용할 스킬 인덱스
	const _Info&	GetPartsInfo() const { return m_Info; }
	const _Info&	GetPartsOriginalInfo() const { return m_OriginalInfo; }
	UINT			GetPartsTableID() const { return m_Info.uiMonsterPartsTableID; }
	DnSkillHandle	GetSkillHandle(){ return m_hSkill; }

#ifndef _GAMESERVER
	std::wstring GetPartsName(){return m_Info.wszPartsName;}
	bool IsEnalbeUIParts(){return m_Info.m_bEnalbeUI;}
#endif

#if defined( PRE_ADD_LOTUSGOLEM )
	bool CheckPartsState( int State ){ return m_Info.PartsState&State ? true : false; }
	void SetIgnore( bool bFlag )
	{
		if( bFlag )
			m_Info.PartsState |= ePartsState::eIgnore;
		else
			m_Info.PartsState &= ~ePartsState::eIgnore;
	}
	void SetNoDamage( bool bFlag )
	{ 
		if( bFlag )
			m_Info.PartsState |= ePartsState::eNoDamage;
		else
			m_Info.PartsState &= ~ePartsState::eNoDamage;
	}
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

private:

	_Info			m_OriginalInfo;
	_Info			m_Info;
	float			m_fDestroyDelta;
	DnSkillHandle	m_hSkill;
};


class MAMultiDamage : virtual public MASingleDamage
{
public:
	MAMultiDamage() { }
	virtual ~MAMultiDamage() {}


	void LoadMultiDamageInfo();
	inline int GetPartsSize() { return (int)m_Parts.size(); }

	inline void ResetAllParts() 
	{
		for ( size_t i = 0 ; i < m_Parts.size() ; i++ )
		{
			m_Parts[i].ResetSetting();
		}
	}

	virtual void		OnSetPartsHP( DnActorHandle hActor, const int iPartsTableID, const int iCurHP , const int iHitterID);

	MonsterParts*		GetParts( const UINT uiTableID );
	MonsterParts*		GetPartsByIndex(int nIndex);
#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
	int					GetPartsIndexFromPartsID( const UINT uiTableID );
#endif
private:
	bool	_AddMonsterParts( const MonsterParts::_Info& info );
	
	void	_OnRefreshParts( DnActorHandle hActor, MonsterParts* pParts, int nPartsIndex );
	void	_OnDestroyParts( DnActorHandle hActor, MonsterParts* pParts, int nPartsIndex );


protected:
	DNVector(MonsterParts)	m_Parts;
};
