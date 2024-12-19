#pragma once
#include "TDnFSMTransitCondition.h"

//#define ENABLE_PROP_CONDITION_LOG


class CDnPropCondition : public TDnFSMTransitCondition<DnPropHandle>
{
public:
	enum
	{
		NULL_CONDITION, // �ƹ� ���� ����.
		COMPARE_DURABILITY,
		IS_HIT,
		OPERATED,
		NEED_ITEM,		// ������ Ư�� �������� ���� �ִ°�..
		ACTION_CHECK,	// Ư�� �׼��� ���ϰ� �ִ���.
		COMPARE_SUB_DURABILITY,		// ���� ������ üũ.
		COUNT,
	};

protected:
	int m_iType;


public:
	CDnPropCondition( DnPropHandle hEntity );
	virtual ~CDnPropCondition(void);

	static CDnPropCondition* Create( DnPropHandle hEntity, int iType );

	int GetType( void ) { return m_iType; };
};
