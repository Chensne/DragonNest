#pragma once
#include "DnWorldActProp.h"
#include "MAActorProp.h"


class CDnBuffProp : public CDnWorldActProp,
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
			BuffStruct* pBuffStruct = static_cast<BuffStruct*>( m_pData );
			SAFE_DELETE( pBuffStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	// 버프 프랍은 따로 시그널 돌 필요 없음... 서버에서 다 돌아감.

public:
	CDnBuffProp(void);
	virtual ~CDnBuffProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
};
