#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"


// 터지면 데미지를 주는 프랍. 클라에서 딱히 하는 일 없음.
class CDnWorldBrokenDamageProp : public CDnWorldBrokenProp,
								 public CDnDamageBase,
								 public MAActorProp
{
private:
	


protected:
	virtual bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BrokenDamageStruct* pBrokenDamageStruct = static_cast<BrokenDamageStruct*>( m_pData );
			SAFE_DELETE( pBrokenDamageStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

public:
	CDnWorldBrokenDamageProp(void);
	virtual ~CDnWorldBrokenDamageProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
};
