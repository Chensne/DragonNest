#pragma once
#include "EtUITreeItem.h"

class CDnQuestTreeItem : public CEtUIQuestTreeItem
{
private:
	CEtUICheckBox*	m_pCheckBox;
#ifdef PRE_ADD_REMOTE_QUEST
	CEtUIButton*	m_pRemoteButton;
#endif // PRE_ADD_REMOTE_QUEST

public:
	CDnQuestTreeItem();
	virtual ~CDnQuestTreeItem();

	typedef CEtUIQuestTreeItem BaseClass;

public:
	void SetCheckBox( CEtUICheckBox* pCheckBox )  { m_pCheckBox = pCheckBox; }
	CEtUICheckBox* GetCheckBox() { return m_pCheckBox; }
	void DeleteCheckBox() override;
#ifdef PRE_ADD_REMOTE_QUEST
	void SetRemoteButton( CEtUIButton* pRemoteButton) {	m_pRemoteButton = pRemoteButton; }
	CEtUIButton* GetRemoteButton() { return m_pRemoteButton; }
	void DeleteRemoteButton();
#endif // PRE_ADD_REMOTE_QUEST
};
