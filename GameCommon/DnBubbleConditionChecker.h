#pragma once


// 이벤트 클래스 전방 선언.
class IDnObserverNotifyEvent;

namespace BubbleSystem
{

class CDnBubbleSystem;

//------------------------------------------------------------------------------
/**
	데이터로 빠져 있는 이벤트 처리 세부 조건
	버블 테이블에 정의되어있는 "조건타입" 과 반드시 순서가 일치해야 하며 
	그에 해당되는 클래스가 정의가 되어 있어야 한다.
*/
//------------------------------------------------------------------------------
enum BUBBLE_CONDITION
{
	BUBBLE_CONDITION_NONE = 0,
	ON_USE_SKILL,					// 특정 스킬을 사용했을 때.
	BLOCK_SUCCESS,					// 블록 상태효과가 성공했을 경우.
	BUBBLE_COUNT_GREATER,			// 특정 버블의 갯수가 일정 이상인 경우.
	BUBBLE_COUNT_UPDATED,			// 특정 버블의 갯수가 업데이트 된 경우.
	OWN_THIS_SKILL,					// 특정 스킬을 보유한 상태인가.
	PARRING_SUCCESS,				// 패링 성공.
	COOLTIME_PARRING_SUCCESS,		// 쿨타임 패링 성공.
	DO_NORMAL_ATTACK,				// 평타 실행. (안맞더라도)
	PLAYER_KILL_TARGET_ON_GHOUL_MODE, // 플레이어가 다른 대상(몬스터/플레이어)를 죽였을 때.
	ON_USE_SKILL_WITH_SPECIFIC_SKILLLEVEL,		// 어떤 스킬을 특정 레벨로 사용했을 때.
	PROBABILITY,					// 일정 확률로 버블 획득.
	ONCRITICALHIT = 17,				//Mastery 1 Dark Avenger incrase bubble on critical damage !
};


//------------------------------------------------------------------------------
/** 
	버블 테이블에 정의된, 외부에 데이터로 노출된 조건 체크 객체의 인터페이스.
*/
//------------------------------------------------------------------------------
class IDnConditionChecker
{
protected:
	int m_iType;

	IDnConditionChecker( void ) : m_iType( 0 ) {};

public:
	virtual ~IDnConditionChecker( void ) {};

	static IDnConditionChecker* Create( int iConditionType, const char* pArgument );

	void SetType( int iType ) { m_iType = iType; };

	virtual void Initialize( const char* pArgument ) = 0;
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent ) = 0;
};


//------------------------------------------------------------------------------
/**
	특정 스킬을 사용했는지 체크하는 객체.
*/
//------------------------------------------------------------------------------
class CDnUseSkill : public IDnConditionChecker
{
private:
	int m_iSkillID;

public:
	CDnUseSkill( void ) : m_iSkillID( 0 )
	{
		SetType( ON_USE_SKILL );
	}
	virtual ~CDnUseSkill( void ) {};
	
	void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};


//------------------------------------------------------------------------------
/**
	특정 스킬에서 성공한 블록을 체크하는 객체.
*/
//------------------------------------------------------------------------------
class CDnBlockSuccess : public IDnConditionChecker
{
private:
	// 이 블록 상태효과를 가진 스킬 ID
	int m_iSkillID;

public:
	CDnBlockSuccess( void ) : m_iSkillID( 0 )
	{ 
		SetType( BLOCK_SUCCESS );
	};

	virtual ~CDnBlockSuccess( void ) {};

	void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 타입의 버블이 일정 갯수 이상인 경우.
*/
//------------------------------------------------------------------------------
class CDnBubbleCountGreater : public IDnConditionChecker
{
private:
	// 카운트를 체크할 버블 타입ID
	int m_iBubbleTypeID;
	
	// 이 갯수 이상 되어야 함.
	int m_iBasisCount;

public:
	CDnBubbleCountGreater( void ) : m_iBubbleTypeID( 0 ), m_iBasisCount( 0 )
	{ 
		SetType( BUBBLE_COUNT_GREATER );
	};

	virtual ~CDnBubbleCountGreater( void ) {};
	
	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 타입의 버블의 갯수가 업데이트 된 경우.
	이 조건은 직접 버블 시스템에서 호출되므로 아무 체크도 하지 않도록 하고 
	버블 타입만 맞으면 무조건 참으로 처리한다.
*/
//------------------------------------------------------------------------------
class CDnBubbleCountUpdated : public IDnConditionChecker
{
private:
	int m_iBubbleTypeID;

public:
	CDnBubbleCountUpdated( void ) : m_iBubbleTypeID( 0 )
	{ 
		SetType( BUBBLE_COUNT_UPDATED );
	};

	virtual ~CDnBubbleCountUpdated( void ) {};
	
	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 스킬을 갖고 있는가.
*/
//------------------------------------------------------------------------------
class CDnOwnThisSkill : public IDnConditionChecker
{
private:
	int m_iSkillID;

public:
	CDnOwnThisSkill( void ) : m_iSkillID( 0 )
	{ 
		SetType( OWN_THIS_SKILL );
	};

	virtual ~CDnOwnThisSkill( void ) {};
	
	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 스킬에서 성공한 패링을 체크하는 객체.
*/
//------------------------------------------------------------------------------
class CDnParringSuccess : public IDnConditionChecker
{
private:
	// 이 블록 상태효과를 가진 스킬 ID
	int m_iSkillID;

public:
	CDnParringSuccess( void ) : m_iSkillID( 0 )
	{ 
		SetType( PARRING_SUCCESS );
	};

	virtual ~CDnParringSuccess( void ) {};

	void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 스킬에서 성공한 쿨타임 패링을 체크하는 객체.
*/
//------------------------------------------------------------------------------
class CDnCooltimeParringSuccess : public IDnConditionChecker
{
private:
	// 이 블록 상태효과를 가진 스킬 ID
	int m_iSkillID;

public:
	CDnCooltimeParringSuccess( void ) : m_iSkillID( 0 )
	{ 
		SetType( COOLTIME_PARRING_SUCCESS );
	};

	virtual ~CDnCooltimeParringSuccess( void ) {};

	void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	평타를 실행했는가.
*/
//------------------------------------------------------------------------------
class CDnDoNormalAttack : public IDnConditionChecker
{
private:

public:
	CDnDoNormalAttack( void )
	{
		SetType( DO_NORMAL_ATTACK );
	}

	virtual ~CDnDoNormalAttack( void ) {};

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	플레이어가 다른 대상을 죽였을 때.
*/
//------------------------------------------------------------------------------
class CDnPlayerKillTargetOnGhoulMode : public IDnConditionChecker
{
private:

public:
	CDnPlayerKillTargetOnGhoulMode( void )
	{
		SetType( PLAYER_KILL_TARGET_ON_GHOUL_MODE );
	}

	virtual ~CDnPlayerKillTargetOnGhoulMode( void ) {};

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	특정 스킬을 특정 레벨로 사용했는지 체크하는 객체.
*/
//------------------------------------------------------------------------------
class CDnUseSkillWithSpecificSkillLevel : public IDnConditionChecker
{
private:
	int m_iSkillID;
	int m_iSkillLevel;

public:
	CDnUseSkillWithSpecificSkillLevel( void ) : m_iSkillID( 0 ), m_iSkillLevel( 0 )
	{
		SetType( ON_USE_SKILL_WITH_SPECIFIC_SKILLLEVEL );
	}
	virtual ~CDnUseSkillWithSpecificSkillLevel( void ) {};
	
	void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };
	void SetSkillLevel( int iSkillLevel ) { m_iSkillLevel = iSkillLevel; };

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//------------------------------------------------------------------------------
/**
	일정 확률을 체크함.
*/
//------------------------------------------------------------------------------
class CDnProbability : public IDnConditionChecker
{
private:
	float m_fProbability;

public:
	CDnProbability( void ) : m_fProbability( 0.0f )
	{
		SetType( PROBABILITY );
	}
	virtual ~CDnProbability( void ) {};

	virtual void Initialize( const char* pArgument );
	virtual bool IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );
};

//mastery 1 dark avenger on critical hit!
class CDnOnCriticalHit : public IDnConditionChecker
{
private:
	std::vector<int> m_vecCheckSkills;

public:
	CDnOnCriticalHit(void)
	{
		SetType(ONCRITICALHIT);
	}
	virtual ~CDnOnCriticalHit(void) {};
	void SetVecList(std::vector<int> lista) { m_vecCheckSkills = lista; }
	std::vector<int> GetVecList(void) { return m_vecCheckSkills; }
	virtual void Initialize(const char* pArgument);
	virtual bool IsSatisfy(CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent);
};

}