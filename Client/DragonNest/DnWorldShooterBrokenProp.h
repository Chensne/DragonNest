#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"
#include "DnDamageBase.h"


// ���� ��鼭 hit �ǰ� ������ �� �Ǹ� Broken �Ǵ� ����.
class CDnWorldShooterBrokenProp : public CDnWorldBrokenProp,
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
			ShooterBrokenStruct* pShooterBrokenStruct = static_cast<ShooterBrokenStruct*>( m_pData );
			SAFE_DELETE( pShooterBrokenStruct );
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
		}
	};

public:
	CDnWorldShooterBrokenProp(void);
	virtual ~CDnWorldShooterBrokenProp(void);

	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
};
