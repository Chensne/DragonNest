#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"


class CDnWorldBrokenBuffProp : public CDnWorldBrokenProp, public MAActorProp
{
private:

protected:
	void ReleasePostCustomParam( void )
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BuffBrokenStruct* pBuffBrokenStruct = static_cast<BuffBrokenStruct*>( m_pData );
			SAFE_DELETE( pBuffBrokenStruct ); 
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

public:
	CDnWorldBrokenBuffProp(void);
	virtual ~CDnWorldBrokenBuffProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	bool InitializeTable( int nTableID );
};
