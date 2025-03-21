#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"
#include "DnDamageBase.h"


// #24282 복수의 내구도를 가지는 broken prop
// 1차적인 모든 조작은 서버에서 행해지고 클라이언트는 액션 패킷 받고 액션만 취함..
class CDnWorldMultiDurabilityProp : public CDnWorldBrokenProp,
								  public MAActorProp,
								  public CDnDamageBase
{
private:


protected:
	bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			MultiDurabilityBrokenPropStruct* pMultiDurabilityStruct = static_cast<MultiDurabilityBrokenPropStruct*>( m_pData );
			SAFE_DELETE( pMultiDurabilityStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};
	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

public:
	CDnWorldMultiDurabilityProp(void);
	virtual ~CDnWorldMultiDurabilityProp(void);

	DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	//void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
};
