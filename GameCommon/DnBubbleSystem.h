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
    ���� �ý��� ������ Ŭ����.

	�� Ŭ������ Ŭ���̾�Ʈ�� ���Ӽ������� CDnPlayerActor ���� �ϳ��� ���� ������
	�����Ϳ� ���ǵ� ��� �÷��̾��� ���� ȹ��, ����, �Ҹ� ó���Ѵ�.
	ĳ������ �������� ������ �ٸ� �� �ִ�.

	���Ŀ� ���� �����ε� ������ �װ� �ȴٸ� CDnMonsterActor �ʿ�����
	����� ���� ó������� �Ѵ�.
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
		int iServerBubbleCount;		// Ŭ���̾�Ʈ������ �����. ������ ������ �ٸ� ��� �������ֱ� ���ؼ� �����.

		S_CREATE_BUBBLE( void ) : iBubbleTypeID( 0 ), iIconIndex( 0 ), fDurationTime( 0.0f ), iServerBubbleCount( 0 ) {};
	};

	// �ܺο��� ��ȸ�� �� �ִ� ���� ����.
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
	// �� ���� �ý��� ��ü�� ���� �ִ� ����
	DnActorHandle m_hActor;

	// �켱 ���� �߰��� ���� ����� ���߿� ���� �ȴ�.
	//vector<CDnBubble*> m_vlpBubbles;
	typedef map<int, deque<CDnBubble*> > BubblesByTypeID;
	typedef BubblesByTypeID::iterator BubblesByTypeID_iter;
	BubblesByTypeID m_mapBubblesByTypeID;

	// ���� ���̺� ���ǵ� �̺�Ʈ ������ ��Ƴ��� ����ü
	struct S_DEFINED_BUBBLE_EVENT
	{
		int iTableID;		// ���� ���̺� ID
		vector<IDnConditionChecker*> vlpConditions;
		vector<IDnBubbleEventHandler*> vlpEventHandlers;

		S_DEFINED_BUBBLE_EVENT( void ) : iTableID( 0 ) {};
		~S_DEFINED_BUBBLE_EVENT( void );
	};

	// ���̺� ID �������� ������ �̺�Ʈ ����
	//map<int, S_DEFINED_BUBBLE_EVENT*> m_mapDefinedBubbleEvent;
	vector<S_DEFINED_BUBBLE_EVENT*> m_vlpDefinedBubbleEvent;

	// �� �̺�Ʈ Ÿ�Ժ��� ��� ���� �̺�Ʈ ���� ����
	// �̺�Ʈ���� ���Ǵ� ���� �������� �ܺ� �����Ϳ��� ���ǵǴ� ������ �ƴϹǷ� 
	// �̷��� �������� ���ؼ� �ڵ忡 �ھƳ��´�.
	typedef multimap<int, S_DEFINED_BUBBLE_EVENT*> DefinedByEventMMap;
	typedef DefinedByEventMMap::iterator DefinedByEventMMap_iter;
	DefinedByEventMMap m_mmapDefinedByEvent;

	// ���� Ÿ�Ժ��� �Ҹ�� ó���Ǵ� �̺�Ʈ �ڵ鷯 ����.
	// ���� �Ҹ�� �̺�Ʈ�� ���� ���̺��� �������� �ʰ� ���ǿ� ���ؼ� �ڵ忡�� ���� ��ġ�Ѵ�.
	//map<int, IDnBubbleEventHandler*> m_mapBubbleRemoveEventHandlers;

	// ����� �߰��� ����ȿ����.
	map<int, vector<int> > m_mapBubbleStateBlowsByBubbleType;

	// ���� Ÿ�Ժ��� �Ҹ�Ǵ� ���.

	// Ÿ�Ժ��� ������ ������ ��� �����Ǵ� ȿ��.


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

	// TODO: ����ü�� �ʿ��� ������ �ܺο��� �޴´�. ���� �����丮, ��ų ����Ʈ ���..
	void Initialize( DnActorHandle hActor );

	// TODO: ĳ���Ͱ� ������ �ϰų� �ϴ� ��� �����͸� ��� �����ϰ� �ٽ� �ʱ�ȭ �ؾ� ��.
	void Clear( void );
	
	// ���ڷ� ���� ���� Ÿ�� ID �� �ش�Ǵ� ������ �����Ѵ�.
	int GetBubbleCountByTypeID( int iBubbleTypeID );

	// ���� �߰��Ǿ��ִ� ���� Ÿ���� ����
	void GetAllAppliedBubbles( /*OUT*/ vector<S_BUBBLE_INFO>& vlBubbleInfos );
	
	// ���� �߰�
	void AddBubble( const S_CREATE_BUBBLE& Info );

	// Ŭ���̾�Ʈ������ ���
	void AddBubbleAndCountRevision( const S_CREATE_BUBBLE& Info );

	void RevisionBubbleCount( const S_CREATE_BUBBLE& Info );

	// ���ڷ� ���� ���� Ÿ�� ID �� �ش�Ǵ� ���� ������ŭ ����.
	void RemoveBubbleByTypeID( int iBubbleTypeID, int iRemoveCount );

	// ���ڷ� ���� ���� Ÿ�� ID �� �ش�Ǵ� ���� ��� ����.
	void RemoveAllBubbleByTypeID( int iBubbleTypeID );

	// ���� ��� ����.
	void RemoveAllBubbles( bool bHandleRemoveEvent = true );

	// ���� Ÿ�Կ� �ش�Ǵ� ���� ��ü ������.
	CDnBubble* GetBubble( int iBubbleTypeID );

	// Ư�� Ÿ���� ������ ���ӽð��� ���ڷ� �־��� �ð����� ������.
	void SetDurationTime( int iBubbleTypeID, float fDurationTime );
	
	// ���μ���~
	void Process( LOCAL_TIME LocalTime, float fDelta );

	// from CDnObserver
	virtual void OnEvent( boost::shared_ptr<::IDnObserverNotifyEvent>& pEvent );

	// �� ���� �ý����� ���� �ִ� ������ �ڵ��� ����
	DnActorHandle GetHasActor( void ) { return m_hActor; };

#ifdef _GAMESERVER
	// CDnAddStateEffectHandler ������ ȣ���.
	void AddBubbleStateBlow( int iBubbleTypeID, int iBlowID );

	// CDnRemoveStateEffectHandler ������ ȣ���.
	void RemoveBubbleStateBlow( int iBubbleTypeID );
#else
	void CreateBubbleFromPacketStream( ::CPacketCompressStream* pStream );
	void RefreshBubbleDurationTimeFromPacketStream( ::CPacketCompressStream* pStream );
#endif // #ifdef _GAMESERVER
};

} // namespace BubbleSystem