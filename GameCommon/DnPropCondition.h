#pragma once
#include "TDnFSMTransitCondition.h"

//#define ENABLE_PROP_CONDITION_LOG


class CDnPropCondition : public TDnFSMTransitCondition<DnPropHandle>
{
public:
	enum
	{
		NULL_CONDITION, // 아무 조건 없음.
		COMPARE_DURABILITY,
		IS_HIT,
		OPERATED,
		NEED_ITEM,		// 유저가 특정 아이템을 갖고 있는가..
		ACTION_CHECK,	// 특정 액션을 취하고 있는지.
		COMPARE_SUB_DURABILITY,		// 서브 내구도 체크.
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
