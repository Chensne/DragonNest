#pragma once
#include "DnCustomDlg.h"

class CDnItem;
class CDnItemUnSealDlg : public CDnCustomDlg
{
public:
	CDnItemUnSealDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnItemUnSealDlg(void);

	virtual void	Initialize(bool bShow);
	bool			SetInfo(const CDnItem *pItem, CDnSlotButton *pSlotButton);
	virtual void	Show( bool bShow );
	void			OnRecvUnSealItem(int nResult);

protected:
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

private:
	CDnItemSlotButton*	m_pItemSlotBtn;
	CEtUIStatic*		m_pItemName;
	CEtUIStatic*		m_pCountStatic;

	CEtUIButton*		m_pOkBtn;
	CEtUIButton*		m_pCancelBtn;

	CDnItem*			m_pItem;
	int					m_SlotIndex;
	INT64				m_biSerial;

	bool				m_bRequestingUnseal;
	int					m_nUnSealSoundIdx;

	CDnSlotButton *m_pSlotButton;
};