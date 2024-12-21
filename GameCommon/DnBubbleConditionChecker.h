#pragma once


// �̺�Ʈ Ŭ���� ���� ����.
class IDnObserverNotifyEvent;

namespace BubbleSystem
{

class CDnBubbleSystem;

//------------------------------------------------------------------------------
/**
	�����ͷ� ���� �ִ� �̺�Ʈ ó�� ���� ����
	���� ���̺� ���ǵǾ��ִ� "����Ÿ��" �� �ݵ�� ������ ��ġ�ؾ� �ϸ� 
	�׿� �ش�Ǵ� Ŭ������ ���ǰ� �Ǿ� �־�� �Ѵ�.
*/
//------------------------------------------------------------------------------
enum BUBBLE_CONDITION
{
	BUBBLE_CONDITION_NONE = 0,
	ON_USE_SKILL,					// Ư�� ��ų�� ������� ��.
	BLOCK_SUCCESS,					// ��� ����ȿ���� �������� ���.
	BUBBLE_COUNT_GREATER,			// Ư�� ������ ������ ���� �̻��� ���.
	BUBBLE_COUNT_UPDATED,			// Ư�� ������ ������ ������Ʈ �� ���.
	OWN_THIS_SKILL,					// Ư�� ��ų�� ������ �����ΰ�.
	PARRING_SUCCESS,				// �и� ����.
	COOLTIME_PARRING_SUCCESS,		// ��Ÿ�� �и� ����.
	DO_NORMAL_ATTACK,				// ��Ÿ ����. (�ȸ´���)
	PLAYER_KILL_TARGET_ON_GHOUL_MODE, // �÷��̾ �ٸ� ���(����/�÷��̾�)�� �׿��� ��.
	ON_USE_SKILL_WITH_SPECIFIC_SKILLLEVEL,		// � ��ų�� Ư�� ������ ������� ��.
	PROBABILITY,					// ���� Ȯ���� ���� ȹ��.
	ONCRITICALHIT = 17,				//Mastery 1 Dark Avenger incrase bubble on critical damage !
};


//------------------------------------------------------------------------------
/** 
	���� ���̺� ���ǵ�, �ܺο� �����ͷ� ����� ���� üũ ��ü�� �������̽�.
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
	Ư�� ��ų�� ����ߴ��� üũ�ϴ� ��ü.
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
	Ư�� ��ų���� ������ ����� üũ�ϴ� ��ü.
*/
//------------------------------------------------------------------------------
class CDnBlockSuccess : public IDnConditionChecker
{
private:
	// �� ��� ����ȿ���� ���� ��ų ID
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
	Ư�� Ÿ���� ������ ���� ���� �̻��� ���.
*/
//------------------------------------------------------------------------------
class CDnBubbleCountGreater : public IDnConditionChecker
{
private:
	// ī��Ʈ�� üũ�� ���� Ÿ��ID
	int m_iBubbleTypeID;
	
	// �� ���� �̻� �Ǿ�� ��.
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
	Ư�� Ÿ���� ������ ������ ������Ʈ �� ���.
	�� ������ ���� ���� �ý��ۿ��� ȣ��ǹǷ� �ƹ� üũ�� ���� �ʵ��� �ϰ� 
	���� Ÿ�Ը� ������ ������ ������ ó���Ѵ�.
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
	Ư�� ��ų�� ���� �ִ°�.
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
	Ư�� ��ų���� ������ �и��� üũ�ϴ� ��ü.
*/
//------------------------------------------------------------------------------
class CDnParringSuccess : public IDnConditionChecker
{
private:
	// �� ��� ����ȿ���� ���� ��ų ID
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
	Ư�� ��ų���� ������ ��Ÿ�� �и��� üũ�ϴ� ��ü.
*/
//------------------------------------------------------------------------------
class CDnCooltimeParringSuccess : public IDnConditionChecker
{
private:
	// �� ��� ����ȿ���� ���� ��ų ID
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
	��Ÿ�� �����ߴ°�.
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
	�÷��̾ �ٸ� ����� �׿��� ��.
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
	Ư�� ��ų�� Ư�� ������ ����ߴ��� üũ�ϴ� ��ü.
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
	���� Ȯ���� üũ��.
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