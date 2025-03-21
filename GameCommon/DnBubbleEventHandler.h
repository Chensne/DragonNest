#pragma once
#include "SmartPtrDef.h"

class IDnObserverNotifyEvent;
class CPacketCompressStream;

namespace BubbleSystem
{

class CDnBubbleSystem;


enum BUBBLE_HANDLER
{
	BUBBLE_HANDLER_NONE = 0,
	GETTING_BUBBLE,
	REMOVE_BUBBLE,
	UPDATE_BUBBLE_STATE_EFFECT,
	ENCHANT_SKILL_ONCE,
};

//------------------------------------------------------------------------------
/**
	버블 이벤트 핸들러의 인터페이스 선언.
*/
//------------------------------------------------------------------------------
class IDnBubbleEventHandler
{
public:
	static IDnBubbleEventHandler* Create( int iEventHandlerType, DnActorHandle hActor, const char* pArgument );

	virtual ~IDnBubbleEventHandler() {}
	virtual void Initialize( DnActorHandle hActor, const char* pArgument ) = 0;
	virtual void ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent ) = 0;
	
	virtual int GetType( void ) = 0;
};


//------------------------------------------------------------------------------
/**
	버블 획득 처리.
*/
//------------------------------------------------------------------------------
class CDnGettingBubbleHandler : public IDnBubbleEventHandler
{
private:
	DnActorHandle m_hActor;
	int m_iBubbleTypeID;
	int m_iAllowedMaxBubbleCount;
	int m_iIconIndex;
	float m_fDurationTime;

public: 
	CDnGettingBubbleHandler( void ) : m_iBubbleTypeID( 0 ), m_fDurationTime( 0.0f ), m_iIconIndex( 0 ), m_iAllowedMaxBubbleCount( 0 ) {};
	virtual ~CDnGettingBubbleHandler() {}

	virtual void Initialize( DnActorHandle hActor, const char* pArgument );
	virtual void ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );

	virtual int GetType( void ) { return BUBBLE_HANDLER::GETTING_BUBBLE; }

	void SetIconIndex( int iIconIndex ) { m_iIconIndex = iIconIndex; };
};


//------------------------------------------------------------------------------
/**
	특정 타입의 버블을 입력 받은 갯수만큼 처리.
*/
//------------------------------------------------------------------------------
class CDnRemoveBubbleHandler : public IDnBubbleEventHandler
{
private:
	DnActorHandle m_hActor;
	int m_iBubbleTypeID;
	int m_iRemoveCount;

	boost::shared_ptr<CPacketCompressStream> m_pPacketStream;
	char m_PacketBuffer[ 32 ];

public:
	CDnRemoveBubbleHandler( void ) : m_iBubbleTypeID( 0 ), m_iRemoveCount( 0 ) 
	{
		memset (m_PacketBuffer, 0x00, sizeof(m_PacketBuffer));
	};
	virtual ~CDnRemoveBubbleHandler() { m_pPacketStream.reset(); }

	virtual void Initialize( DnActorHandle hActor, const char* pArgument );
	virtual void ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );

	virtual int GetType( void ) { return BUBBLE_HANDLER::REMOVE_BUBBLE; }
};


//------------------------------------------------------------------------------
/**
	인자로 받은 스킬 ID 에 속해있는 상태효과들을 특정 버블 타입의 갯수에 
	따라 맞는 상태효과로 갱신.
*/
//------------------------------------------------------------------------------
class CDnUpdateStateEffectHandler : public IDnBubbleEventHandler
{
private:
	DnActorHandle m_hActor;
	int m_iBubbleTypeID;
	int m_iSkillID;
	DnSkillHandle m_hSkill;

public:
	CDnUpdateStateEffectHandler( void ) : m_iBubbleTypeID( 0 ), m_iSkillID( 0 ) {};

	virtual void Initialize( DnActorHandle hActor, const char* pArgument );
	virtual void ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );

	virtual int GetType( void ) { return BUBBLE_HANDLER::UPDATE_BUBBLE_STATE_EFFECT; }

	int GetBubbleTypeID( void ) { return m_iBubbleTypeID; };
};

//------------------------------------------------------------------------------
/**
	특정 스킬에 강화스킬을 적용해주는 핸들러.
	사용될 때 적용되고 스킬 종료될 때 해제된다.
*/
//------------------------------------------------------------------------------
class CDnEnchantSkillOnce : public IDnBubbleEventHandler
{
private:
	DnActorHandle m_hActor;
	int m_iTargetSkillID;
	int m_iEnchantSkillID;

public:
	CDnEnchantSkillOnce( void ) : m_iTargetSkillID( 0 ), m_iEnchantSkillID( 0 ) {};

	virtual void Initialize( DnActorHandle hActor, const char* pArgument );
	virtual void ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent );

	virtual int GetType( void ) { return BUBBLE_HANDLER::ENCHANT_SKILL_ONCE; }
};

}