#pragma once
#include "DnPropState.h"

class CDnPropStateDoAction : public CDnPropState, public TBoostMemoryPool< CDnPropStateDoAction >
{
private:
	vector<string> m_vlAvailActionList;
	bool m_bUseCmdAction;

public:
	CDnPropStateDoAction( DnPropHandle hEntity );
	virtual ~CDnPropStateDoAction(void);

	// 1�� �̻��̸� �������� ����. �õ尪�� �������� �޴´�. �ݵ�� ��Ŷ���� ���� Ŭ��� ������ �õ带 �޾ƾ� �ϱ� ����..
	void AddActionName( const char* pActionName ) { m_vlAvailActionList.push_back( string(pActionName) ); };
	void UseCmdAction( bool bUseCmdAction ) { m_bUseCmdAction = bUseCmdAction; };

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
