#pragma once

#include "DnCustomDlg.h"
#include "DnCustomControlCommon.h"

class CDnSlotButton;
class CDnRadioMsgButton;
class MIInventoryItem;

// �����޼��� ������ ���̾�α�
class CDnRadioMsgQuickSlotDlg : public CDnCustomDlg
{
public:
	CDnRadioMsgQuickSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRadioMsgQuickSlotDlg(void);

protected:
	std::vector< CDnRadioMsgButton* > m_vecSlotButton;

protected:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	void ResetSlot( int nIndex );
	int FindRadioMsgSlotItem( int nRadioMsgID );

	// �⺻ �����԰� �޸�, ���̾�αװ� �ϳ����̶�, MainDlg�� �ȸ���� ���� ó���� �� �ִ�.
	void ChangeQuickSlotButton( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	bool SetSlot( int nIndex, int nRadioMsgID );

	// ��ų�� ���Կ� ������ �Ҹ�, ������ �Ҹ�
	int m_nSkillLaydownSoundIndex;
	int m_nSkillDragSoundIndex;

public:
	bool InitSlot( int nIndex, int nRadioMsgID );

public:
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};