#pragma once
#include "DnQuestTreeItem.h"

class CDnQuestTree : public CEtUITreeCtl
{
public:
	enum { SUB_SUB_IMAGE_COUNT = 7, };

#ifdef PRE_ADD_REMOTE_QUEST
	enum eRemoteQuestState
	{
		REMOTEQUEST_NONE = 0,
		REMOTEQUEST_ASK,
		REMOTEQUEST_PROGRESSING,
		REMOTEQUEST_COMPLETE,
	};

	struct stRemoteQuestStateButton
	{
		eRemoteQuestState	m_eRemoteQuestState;
		CEtUIButton*		m_pRemoteQuestButton;
		stRemoteQuestStateButton() : m_eRemoteQuestState( REMOTEQUEST_NONE ), m_pRemoteQuestButton( NULL ) {}
	};
#endif // PRE_ADD_REMOTE_QUEST

public:
	CDnQuestTree( CEtUIDialog *pParent );
	virtual ~CDnQuestTree(void);

protected:
	virtual void RenderTree( TREEITEM_LIST &VecTree );
	virtual void RenderButton( CTreeItem *pItem );
	void RenderState( CTreeItem *pItem );
	void RenderText( CTreeItem *pItem );
	void RenderBackImg( CTreeItem *pItem );
	void RenderFrame( CTreeItem *pItem );
	void RenderSelFrame( CTreeItem *pItem );

protected:
	SUIControlProperty	m_CheckBoxProperty;
#ifdef PRE_ADD_REMOTE_QUEST
	SUIControlProperty	m_RemoteButtonProperty;
	std::vector< stRemoteQuestStateButton >	m_vecRemoteButtonList;
#endif // PRE_ADD_REMOTE_QUEST
	std::vector< CEtUICheckBox* > m_vecCheckBoxList;

	EtTextureHandle	m_SubSubTextureList[ SUB_SUB_IMAGE_COUNT ];
	int				m_nSubImageIndex;
	bool			m_bIsMainQuest;

	float			m_fMouseX;
	float			m_fMouseY;

	typedef CEtUITreeCtl BaseClass;

public:
	CDnQuestTreeItem* AddQuestItem( DWORD dwType, LPCWSTR szText, DWORD dwColor = textcolor::WHITE );

	CEtUICheckBox* CreateCheckBox( int nQuestIndex, CDnQuestTreeItem *pItem);
	CDnQuestTreeItem* AddQuestChildItem( CEtUIQuestTreeItem *pTreeItem, DWORD dwType, LPCWSTR szText, DWORD dwColor = textcolor::WHITE );
	void SetCheckBoxProperty( SUIControlProperty *pProperty ) { m_CheckBoxProperty = *pProperty; }
#ifdef PRE_ADD_REMOTE_QUEST
	void SetRemoteQuestState( int nQuestIndex, eRemoteQuestState remoteQuestState );
	CDnQuestTree::eRemoteQuestState GetRemoteQuestState( int nQuestIndex );
	bool IsRemoteQuest( int nQuestIndex );
	CEtUIButton* CreateRemoteButton( int nQuestIndex );
	void SetRemoteButtonProperty( SUIControlProperty *pProperty ) { m_RemoteButtonProperty = *pProperty; }
	void DeleteRemoteButton( CEtUIButton* pRemoteButton );
	void SetSelectRemoteQuest( int nQuestIndex );
#endif // PRE_ADD_REMOTE_QUEST
	void DeleteAllItems() override;
	void DeleteCheckBox( CEtUICheckBox *pCheckBox );	
	bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam ) override;

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual int GetScrollBarTemplate() { return m_Property.QuestTreeControlProperty.nScrollBarTemplate; }
	virtual void SetScrollBarTemplate( int nIndex ) { m_Property.QuestTreeControlProperty.nScrollBarTemplate = nIndex; }

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );

	// 마지막 트리 노드를 선택한다.
	void SelectLastTree();

	std::vector< CEtUICheckBox* > & GetCheckBoxList() { return m_vecCheckBoxList; }
	void SetMainQuest( bool bMainQuest ) { m_bIsMainQuest = bMainQuest; }

	void SetExceptLastSort( int nGoalID );
};
