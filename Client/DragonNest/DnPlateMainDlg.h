#pragma once
#include "DnCustomDlg.h"
#include "EtUIDialogGroup.h"
#include "DnItemTask.h"

class CDnPlateListSelectDlg;

class CDnPlateMainDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPlateMainDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPlateMainDlg(void);

	enum{
		COMPOUND_CONFIRM_DIALOG,
		COMPOUND_MOVIE_DIALOG,
	};

	enum eStep{
		eStepClose,
		eStepPlate,
		eStepRune,
		eStepMake,
	};

private :

	CDnPlateListSelectDlg * m_pPlateListSelectDlg;
	CDnItem	*				m_pItem;
	CDnSlotButton *			m_pSlotButton;
	CEtUIButton *			m_pButtonMake;
	CEtUIButton *			m_pButtonCancel;

	eStep					m_eStep;

public :

	void ChangeStep(eStep nStep);

	bool SetPlateItem( CDnQuickSlotButton *pPressedButton );
	int GetPlateItemID();

	void OnRecvEmblemCompoundOpen();
	void OnRecvEmblemCompound();
	void EnableButton( bool bEnable );

public :
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};