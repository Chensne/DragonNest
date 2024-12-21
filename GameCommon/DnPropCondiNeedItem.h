#pragma once
#include "DnPropCondition.h"

class CDnPropCondiNeedItem : public CDnPropCondition, public TBoostMemoryPool< CDnPropCondiNeedItem >
{
private:
	DnActorHandle m_hAccessActor;
	int m_iNeedItemID;
	int m_iNumNeedItem;
	bool m_bRemoveNeedItem;

public:
	CDnPropCondiNeedItem( DnPropHandle hEntity );
	virtual ~CDnPropCondiNeedItem(void);

	virtual bool IsSatisfy( void );

	void SetAccessActor( DnActorHandle hActor ) { m_hAccessActor = hActor; };
	void SetNeedItemID( int iNeedItemID ) { m_iNeedItemID = iNeedItemID; };
	void SetNumNeedItem( int iNumNeedItem ) { m_iNumNeedItem = iNumNeedItem; };
	void SetRemoveNeedItem( bool bRemoveNeedItem ) { m_bRemoveNeedItem = bRemoveNeedItem; };
};
