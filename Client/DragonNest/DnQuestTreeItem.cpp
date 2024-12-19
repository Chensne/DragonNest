#include "StdAfx.h"
#include "DnQuestTreeItem.h"
#include "DnQuestTree.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnQuestTreeItem::CDnQuestTreeItem()
: m_pCheckBox( NULL )
#ifdef PRE_ADD_REMOTE_QUEST
, m_pRemoteButton ( NULL )
#endif // PRE_ADD_REMOTE_QUEST
{
}

CDnQuestTreeItem::~CDnQuestTreeItem(void)
{
	DeleteCheckBox();
#ifdef PRE_ADD_REMOTE_QUEST
	DeleteRemoteButton();
#endif // PRE_ADD_REMOTE_QUEST
}

void CDnQuestTreeItem::DeleteCheckBox()
{	
	if( m_pCheckBox ) {
		ASSERT( m_pParentControl != NULL );
		if( m_pParentControl ) {
			((CDnQuestTree*)m_pParentControl)->DeleteCheckBox( m_pCheckBox );
			m_pCheckBox = NULL;
		}
	}
	for ( TREEITEM_LIST_ITER it = m_listItem.begin(); it != m_listItem.end(); ++it) {
		(*it)->DeleteCheckBox();
	}
}

#ifdef PRE_ADD_REMOTE_QUEST
void CDnQuestTreeItem::DeleteRemoteButton()
{	
	if( m_pRemoteButton )
	{
		ASSERT( m_pParentControl != NULL );
		if( m_pParentControl ) 
		{
			((CDnQuestTree*)m_pParentControl)->DeleteRemoteButton( m_pRemoteButton );
			m_pRemoteButton = NULL;
		}
	}
}
#endif // PRE_ADD_REMOTE_QUEST