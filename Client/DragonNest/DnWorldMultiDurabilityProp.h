#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"
#include "DnDamageBase.h"


// #24282 ������ �������� ������ broken prop
// 1������ ��� ������ �������� �������� Ŭ���̾�Ʈ�� �׼� ��Ŷ �ް� �׼Ǹ� ����..
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
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
		}
	};
	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

public:
	CDnWorldMultiDurabilityProp(void);
	virtual ~CDnWorldMultiDurabilityProp(void);

	DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	//void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
};
