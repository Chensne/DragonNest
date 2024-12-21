
#pragma once

class CDNEffectItem;

class CDNEffectRepository:public TBoostMemoryPool<CDNEffectRepository>
{
public:
	CDNEffectRepository( CDNUserSession* pSession );
	~CDNEffectRepository();
	bool Add( INT64 biItemSerial, int iItemID, __time64_t tExpireDate = 0 );
	bool Remove( INT64 biItemSerial );

#if defined (PRE_ADD_VIP_FARM)
	void SendEffectItem();	
	bool bIsExpiredItem( int iItemType );
#endif // #if defined (PRE_ADD_VIP_FARM)		

	void DelGuildRewardItem();	// 길드보상 효과 아이템 삭제
#if defined( PRE_ADD_BESTFRIEND )
	void DelEffectItemType( char cType );	// 이미 디비에서는 삭제된 상태이므로 메모리에서만 삭제하는 함수
#endif

private:
	CDNUserSession* m_pSession;
	std::map<INT64,CDNEffectItem*> m_mRepository;
};


