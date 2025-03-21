#pragma once
#include "DnObservable.h"

namespace BubbleSystem
{

//------------------------------------------------------------------------------
/**
	특정 스킬 사용 메시지
*/
//------------------------------------------------------------------------------
class CDnUseSkillMessage : public ::IDnObserverNotifyEvent
{
private:
	// 이 블록 상태효과를 가진 스킬 ID
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
	블록 상태효과에서 블록 성공 메시지
*/
//------------------------------------------------------------------------------
class CDnBlockSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// 이 블록 상태효과를 가진 스킬 ID
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
	특정 버블 타입의 갯수가 업데이트됨.
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
	패링 상태효과에서 패링 성공 메시지
*/
//------------------------------------------------------------------------------
class CDnParringSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// 이 패링 상태효과를 가진 스킬 ID
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
	쿨타임 패링 상태효과에서 쿨타임 패링 성공 메시지
*/
//------------------------------------------------------------------------------
class CDnCooltimeParringSuccessMessage : public ::IDnObserverNotifyEvent
{
private:
	// 이 쿨타임 패링 상태효과를 가진 스킬 ID
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
	일반 공격을 했을 때 메시지.
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
	플레이어가 다른 대상을 죽였을 때.
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