#pragma once
#include "DnObservable.h"

namespace BubbleSystem
{

//------------------------------------------------------------------------------
/**
	Ư�� ��ų ��� �޽���
*/
//------------------------------------------------------------------------------
class CDnUseSkillMessage : public ::IDnObserverNotifyEvent
{
private:
	// �� ��� ����ȿ���� ���� ��ų ID
	int m_iSkillID;
	int m_iSkillLevel;

public:
	CDnUseSkillMessage( void ) : m_iSkillID( 0 ), m_iSkillLevel( 0 )
	{ 
		SetEventType( EVENT_BUBBLE_ON_USE_SKILL );
	};

	virtual ~CDnUseSkillMessage( void ) {};

	virtual void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };
	int GetSkillID( void ) { return m_iSkillID; };
	void SetSkillLevel( int iSkillLevel );
	int GetSkillLevel( void ) { return m_iSkillID; };
};

//------------------------------------------------------------------------------
/**
	��� ����ȿ������ ��� ���� �޽���
*/
//------------------------------------------------------------------------------
class CDnBlockSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// �� ��� ����ȿ���� ���� ��ų ID
	int m_iSkillID;

public:
	CDnBlockSuccessMessage( void ) : m_iSkillID( 0 )
	{ 
		SetEventType( EVENT_BUBBLE_BLOCK_SUCCESS );
	};

	virtual ~CDnBlockSuccessMessage( void ) {};

	virtual void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };
	int GetSkillID( void ) { return m_iSkillID; };
};

//------------------------------------------------------------------------------
/**
	Ư�� ���� Ÿ���� ������ ������Ʈ��.
*/
//------------------------------------------------------------------------------
class CDnBubbleCountUpdatedMessage : public ::IDnObserverNotifyEvent
{
private:
	int m_iBubbleTypeID;

public:
	CDnBubbleCountUpdatedMessage( void ) : m_iBubbleTypeID( 0 )
	{ 
		SetEventType( EVENT_BUBBLE_COUNT_UPDATED );
	};

	virtual ~CDnBubbleCountUpdatedMessage( void ) {};

	virtual void SetBubbleTypeID( int iBubbleTypeID ) { m_iBubbleTypeID = iBubbleTypeID; };
	int GetBubbleTypeID( void ) { return m_iBubbleTypeID; };
};

//------------------------------------------------------------------------------
/**
	�и� ����ȿ������ �и� ���� �޽���
*/
//------------------------------------------------------------------------------
class CDnParringSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// �� �и� ����ȿ���� ���� ��ų ID
	int m_iSkillID;

public:
	CDnParringSuccessMessage( void ) : m_iSkillID( 0 )
	{ 
		SetEventType( EVENT_BUBBLE_PARRING_SUCCESS );
	};

	virtual ~CDnParringSuccessMessage( void ) {};

	virtual void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };
	int GetSkillID( void ) { return m_iSkillID; };
};

//------------------------------------------------------------------------------
/**
	��Ÿ�� �и� ����ȿ������ ��Ÿ�� �и� ���� �޽���
*/
//------------------------------------------------------------------------------
class CDnCooltimeParringSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// �� ��Ÿ�� �и� ����ȿ���� ���� ��ų ID
	int m_iSkillID;

public:
	CDnCooltimeParringSuccessMessage( void ) : m_iSkillID( 0 )
	{ 
		SetEventType( EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS );
	};

	virtual ~CDnCooltimeParringSuccessMessage( void ) {};

	virtual void SetSkillID( int iSkillID ) { m_iSkillID = iSkillID; };
	int GetSkillID( void ) { return m_iSkillID; };
};

//------------------------------------------------------------------------------
/**
	�Ϲ� ������ ���� �� �޽���.
*/
//------------------------------------------------------------------------------
class CDnOnChangeActionMessage : public ::IDnObserverNotifyEvent
{
private:
	//const char* m_pActionNameToChange;

public:
	CDnOnChangeActionMessage( void )
	{ 
		SetEventType( EVENT_ONCHANGEACTION );
	};

	virtual ~CDnOnChangeActionMessage( void ) {};
};

//------------------------------------------------------------------------------
/**
	�÷��̾ �ٸ� ����� �׿��� ��.
*/
//------------------------------------------------------------------------------
class CDnPlayerKillTargetMessage : public ::IDnObserverNotifyEvent
{
private:

public:
	CDnPlayerKillTargetMessage( void )
	{ 
		SetEventType( EVENT_PLAYER_KILL_TARGET );
	};

	virtual ~CDnPlayerKillTargetMessage( void ) {};
};

class CDnOnCriticalHitMessage : public ::IDnObserverNotifyEvent
{
private:
	int m_iSkillID;

public:
	CDnOnCriticalHitMessage(void)
	{
		SetEventType(EVENT_ONCRITICALHIT);
	};

	virtual ~CDnOnCriticalHitMessage(void) {};

	virtual void SetSkillID(int iSkillID) { m_iSkillID = iSkillID; };
	int GetSkillID(void) { return m_iSkillID; };
};

}