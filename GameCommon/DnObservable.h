#pragma once
#include "DnObserver.h"

//------------------------------------------------------------------------------
/**
	사용하는 이벤트별로 선언.
*/
//------------------------------------------------------------------------------
enum
{
	NONE_BUBBLE_EVENT_MESSAGE = 0,
	
	// 버블 시스템에 전달되는 메시지들. //////////////////////////////////////
	EVENT_BUBBLE_ON_USE_SKILL,					// 특정 스킬을 사용했을 때.
	EVENT_BUBBLE_BLOCK_SUCCESS,					// 특정 스킬에서 지정된 블록 상태효과가 성공했을 경우.
	EVENT_BUBBLE_COUNT_UPDATED,					// 특정 버블의 갯수가 갱신됨. 
	EVENT_BUBBLE_PARRING_SUCCESS,				// 특정 스킬에서 지정된 패링 상태효과가 성공했을 경우.
	EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS,		// 특정 스킬에서 시정된 쿨타임 패링 상태효과가 성공했을 경우.
	//EVENT_BUBBLE_HIT_BY_SKILL,					// 특정 스킬로 hit 됨.
	EVENT_ONCHANGEACTION,						// 액션 변경됨.
	EVENT_PLAYER_KILL_TARGET,					// 누군가를 죽였을 때
	//////////////////////////////////////////////////////////////////////////

	EVENT_ONCRITICALHIT = 50,
};



//------------------------------------------------------------------------------
/**
	이벤트 타입 인터페이스
*/
//------------------------------------------------------------------------------
class IDnObserverNotifyEvent
{
private:
	int m_iEventType;

protected:
	IDnObserverNotifyEvent( void ) 
	{ 
		m_iEventType = 0; 
	};
	
public:
	virtual ~IDnObserverNotifyEvent( void ) {};

	static IDnObserverNotifyEvent* Create( int iType );

	virtual void SetSkillID( int iSkillID ) {};
	virtual void SetBubbleTypeID( int iBubbleTypeID ) {};

	void SetEventType( int iEventType ) { m_iEventType = iEventType; };
	int GetEventType( void ) { return m_iEventType; }
};


//------------------------------------------------------------------------------
/**
	옵저버 등록/제거 및 이벤트 통지
*/
//------------------------------------------------------------------------------
class CDnObservable
{
private:
	std::vector<CDnObserver*> m_vlpObservers;

public:
	CDnObservable( void );
	virtual ~CDnObservable( void );

	bool RegisterObserver( CDnObserver* pObserver );
	bool RemoveObserver( CDnObserver* pObserver );
	void Notify( boost::shared_ptr<IDnObserverNotifyEvent>& pEvent );
};
