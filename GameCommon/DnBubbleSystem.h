#pragma once
#include "DnObserver.h"
#include "SmartPtrDef.h"
#include "DNTableFile.h"

class CPacketCompressStream;

namespace BubbleSystem
{
class CDnBubble;
class IDnConditionChecker;
class IDnBubbleEventHandler;

//------------------------------------------------------------------------------
/**
    버블 시스템 구현한 클래스.

	이 클래스는 클라이언트와 게임서버에서 CDnPlayerActor 에서 하나씩 갖고 있으며
	데이터에 정의된 대로 플레이어의 버블 획득, 유지, 소모를 처리한다.
	캐릭터의 직업별로 내용이 다를 수 있다.

	추후에 몬스터 스스로도 버블을 쌓게 된다면 CDnMonsterActor 쪽에서도
	멤버로 갖고 처리해줘야 한다.
*/
//------------------------------------------------------------------------------
class CDnBubbleSystem : public CDnObserver
{
public:
	struct S_CREATE_BUBBLE 
	{
		int iBubbleTypeID;
		int iIconIndex;
		float fDurationTime;
		int iServerBubbleCount;		// 클라이언트에서만 사용함. 서버와 갯수가 다를 경우 보정해주기 위해서 사용함.

		S_CREATE_BUBBLE( void ) : iBubbleTypeID( 0 ), iIconIndex( 0 ), fDurationTime( 0.0f ), iServerBubbleCount( 0 ) {};
	};

	// 외부에서 조회할 수 있는 버블 정보.
	struct S_BUBBLE_INFO
	{
		int iBubbleTypeID;
		int iCount;
		int iIconIndex;
		float fDurationTime;
		float fRemainTime;
		S_BUBBLE_INFO( void ) : iBubbleTypeID( 0 ), iCount( 0 ), iIconIndex( 0 ), fDurationTime( 0.0f ), fRemainTime( 0.0f ) {};
	};

private:
	// 이 버블 시스템 객체를 갖고 있는 액터
	DnActorHandle m_hActor;

	// 우선 먼저 추가된 버블 순대로 나중에 제거 된다.
	//vector<CDnBubble*> m_vlpBubbles;
	typedef map<int, deque<CDnBubble*> > BubblesByTypeID;
	typedef BubblesByTypeID::iterator BubblesByTypeID_iter;
	BubblesByTypeID m_mapBubblesByTypeID;

	// 버블 테이블에 정의된 이벤트 정보를 모아놓은 구조체
	struct S_DEFINED_BUBBLE_EVENT
	{
		int iTableID;		// 버블 테이블 ID
		vector<IDnConditionChecker*> vlpConditions;
		vector<IDnBubbleEventHandler*> vlpEventHandlers;

		S_DEFINED_BUBBLE_EVENT( void ) : iTableID( 0 ) {};
		~S_DEFINED_BUBBLE_EVENT( void );
	};

	// 테이블 ID 기준으로 정리된 이벤트 정보
	//map<int, S_DEFINED_BUBBLE_EVENT*> m_mapDefinedBubbleEvent;
	vector<S_DEFINED_BUBBLE_EVENT*> m_vlpDefinedBubbleEvent;

	// 각 이벤트 타입별로 모아 놓은 이벤트 정의 정보
	// 이벤트에서 사용되는 조건 정보들은 외부 데이터에서 정의되는 정보가 아니므로 
	// 이렇게 연관성에 대해서 코드에 박아놓는다.
	typedef multimap<int, S_DEFINED_BUBBLE_EVENT*> DefinedByEventMMap;
	typedef DefinedByEventMMap::iterator DefinedByEventMMap_iter;
	DefinedByEventMMap m_mmapDefinedByEvent;

	// 버블 타입별로 소모시 처리되는 이벤트 핸들러 모음.
	// 버블 소모시 이벤트는 따로 테이블에서 정의하지 않고 조건에 대해서 코드에서 직접 배치한다.
	//map<int, IDnBubbleEventHandler*> m_mapBubbleRemoveEventHandlers;

	// 버블로 추가된 상태효과들.
	map<int, vector<int> > m_mapBubbleStateBlowsByBubbleType;

	// 버블 타입별로 소모되는 방식.

	// 타입별로 버블을 유지할 경우 생성되는 효과.


protected:
	void _CreateBubble( const S_CREATE_BUBBLE& Info );

	CDnBubble* _CreateNewBubble( const S_CREATE_BUBBLE &Info );

	void _OnCreatedBubble( CDnBubble* pCreatedBubble );
	//void _OnRemoveBubble( CDnBubble* pBubbleToRemove );
	void _OnRemovedBubbles( int iBubbleTypeID, int iCount );

	int _GetRelatedEventMessageType( int iConditionType );

	void _OnCreateEventHandler( DNTableFileFormat* pBubbleTable, int iBubbleTableID, IDnBubbleEventHandler* pEventHandler, const char* pArgument );

public:
	CDnBubbleSystem( void );
	virtual ~CDnBubbleSystem( void );

	// TODO: 구조체로 필요한 정보를 외부에서 받는다. 직업 히스토리, 스킬 리스트 등등..
	void Initialize( DnActorHandle hActor );

	// TODO: 캐릭터가 전직을 하거나 하는 경우 데이터를 모두 리셋하고 다시 초기화 해야 함.
	void Clear( void );
	
	// 인자로 받은 버블 타입 ID 에 해당되는 갯수를 리턴한다.
	int GetBubbleCountByTypeID( int iBubbleTypeID );

	// 현재 추가되어있는 버블 타입의 갯수
	void GetAllAppliedBubbles( /*OUT*/ vector<S_BUBBLE_INFO>& vlBubbleInfos );
	
	// 버블 추가
	void AddBubble( const S_CREATE_BUBBLE& Info );

	// 클라이언트에서만 사용
	void AddBubbleAndCountRevision( const S_CREATE_BUBBLE& Info );

	void RevisionBubbleCount( const S_CREATE_BUBBLE& Info );

	// 인자로 받은 버블 타입 ID 에 해당되는 버블 갯수만큼 제거.
	void RemoveBubbleByTypeID( int iBubbleTypeID, int iRemoveCount );

	// 인자로 받은 버블 타입 ID 에 해당되는 버블 모두 제거.
	void RemoveAllBubbleByTypeID( int iBubbleTypeID );

	// 버블 모두 삭제.
	void RemoveAllBubbles( bool bHandleRemoveEvent = true );

	// 버블 타입에 해당되는 버블 객체 얻어오기.
	CDnBubble* GetBubble( int iBubbleTypeID );

	// 특정 타입의 버블의 지속시간을 인자로 주어진 시간으로 셋팅함.
	void SetDurationTime( int iBubbleTypeID, float fDurationTime );
	
	// 프로세스~
	void Process( LOCAL_TIME LocalTime, float fDelta );

	// from CDnObserver
	virtual void OnEvent( boost::shared_ptr<::IDnObserverNotifyEvent>& pEvent );

	// 이 버블 시스템을 갖고 있는 액터의 핸들을 얻음
	DnActorHandle GetHasActor( void ) { return m_hActor; };

#ifdef _GAMESERVER
	// CDnAddStateEffectHandler 에서만 호출됨.
	void AddBubbleStateBlow( int iBubbleTypeID, int iBlowID );

	// CDnRemoveStateEffectHandler 에서만 호출됨.
	void RemoveBubbleStateBlow( int iBubbleTypeID );
#else
	void CreateBubbleFromPacketStream( ::CPacketCompressStream* pStream );
	void RefreshBubbleDurationTimeFromPacketStream( ::CPacketCompressStream* pStream );
#endif // #ifdef _GAMESERVER
};

} // namespace BubbleSystem