
#pragma once

#include "ReputationSystem.h"
#ifdef _CLIENT
#include "DnInterface.h"
#endif	// #ifdef _CLIENT

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

class CReputationSystemRepository:public TBoostMemoryPool<CReputationSystemRepository>
{
public:

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST( NpcReputation_unittest, PROCESSOR_TEST );
	FRIEND_TEST( NpcReputation_unittest, PRESENT_TEST );
	FRIEND_TEST( NpcReputation_unittest, MALICEBOMB_TEST );
#endif // #if !defined( _FINAL_BUILD )

#if defined( _SERVER )
	CReputationSystemRepository( CDNUserSession* pSession );
#else
	CReputationSystemRepository();
#endif // #if defined( _SERVER )
	virtual ~CReputationSystemRepository();

	void Clear();
	void Transaction();
	void Commit( bool bClientSend=true );
	
	IReputationSystemEventHandler* GetEventHandler(){ return m_pEventHandler; }
	void GetCommitData( std::vector<TNpcReputation>& Data );
	void GetAllNpcData( std::vector<int>& Data );

	int	GetNpcReputationPercent( const int iNpcID, IReputationSystem::eType Type );
	float GetNpcReputationPercentF( const int iNpcID, IReputationSystem::eType Type );
	REPUTATION_TYPE GetNpcReputation( const int iNpcID, IReputationSystem::eType Type );
	REPUTATION_TYPE GetNpcReputationMax( const int iNpcID, IReputationSystem::eType Type );
	void SetNpcReputation( const int iNpcID, IReputationSystem::eType Type, REPUTATION_TYPE value );
	void AddNpcReputation( const int iNpcID, IReputationSystem::eType Type, REPUTATION_TYPE value, bool bIsReachMax=true ); // bIsOverMax : Max �� ������ �� �ִ��� Flag
	bool IsFull( const int iNpcID, IReputationSystem::eType Type );
	bool IsExistNpcReputation( int iNpcID );
#ifdef _CLIENT
	UNIONPT_TYPE GetUnionPoint(const int& unionType) const;
	void SetUnionPoint(const int& unionType, const UNIONPT_TYPE& value);
	void SetUnionMembershipData(const int& unionType, const int& itemId, const __time64_t& receivedDate, const int& leftUseTime);
	const std::vector<CDnInterface::SUnionMembershipData>& GetUnionMembershipData() const { return m_UnionReputationData; }
	void SetLastUpdateNpcID(int npcId) { m_LastUpdateNpcID = npcId; }
	int	GetLastUpdateNpcID() const { return m_LastUpdateNpcID; }
#ifdef PRE_FIX_71455_REPUTE_RENEW
	void DeleteUnionMembershipData( int nUnionType, int nItemID );
#endif // PRE_FIX_71455_REPUTE_RENEW
#endif // _CLIENT

private:

	std::map<int,IReputationSystem*> m_NpcReputation;
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	std::map<IReputationSystem*, size_t> m_CommitReputationSystem;
#else
	std::map<IReputationSystem*,int> m_CommitReputationSystem;
#endif // #if define( PRE_ADD_REPUTATION_EXPOSURE )
	IReputationSystemEventHandler* m_pEventHandler;
#ifdef _CLIENT
	UNIONPT_TYPE m_UnionPoint[NpcReputation::UnionType::Etc];
	std::vector<CDnInterface::SUnionMembershipData> m_UnionReputationData;
	int	m_LastUpdateNpcID;
#endif
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
