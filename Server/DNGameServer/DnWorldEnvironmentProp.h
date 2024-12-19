#pragma once
#include "dnworldactprop.h"

// �� ģ���� �������� �ϴ� ���� ����. 
// Ŭ�󿡼� ������ �ٹ̱� ��.
class CDnWorldEnvironmentProp : public CDnWorldActProp, public TBoostMemoryPool< CDnWorldEnvironmentProp >
{
public:
	CDnWorldEnvironmentProp( CMultiRoom* pRoom );
	virtual ~CDnWorldEnvironmentProp(void);

	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			EnvironmentPropStruct* pEnvironmentPropStruct = static_cast<EnvironmentPropStruct*>( m_pData );
			SAFE_DELETE( pEnvironmentPropStruct );
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
		}
	};
};


