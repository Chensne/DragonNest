#pragma once
#include "DnCustomDlg.h"
#include "DNPacket.h"

#ifdef PRE_SPECIALBOX

class CDnSpecialBoxConfirmDlg;

class CDnSpecialBoxListDlg : public CDnCustomDlg , public CEtUICallback
{
protected:
	struct sUIGoldControl
	{
		enum eType
		{
			Text = 0,
			Cover = 1,
			Gold = 2,
			Silver = 3,
			Bronze = 4,
			Max,
		};

		CEtUIStatic *pStatic[sUIGoldControl::Max];

		sUIGoldControl()
		{
			for( int i=0; i<eType::Max; i++ )
				pStatic[i] = NULL;
		}

		void Show( bool bShow )
		{
			for( int i=0; i<eType::Max; i++ )
				pStatic[i]->Show( bShow );
		}
		void Enable( bool bEnable )
		{
			for( int i=0; i<eType::Max; i++ )
				pStatic[i]->Enable( bEnable );
		}
	};

protected:
	int m_nReceiveType;
	int m_nRewardID;

	sUIGoldControl m_sUIGoldSet;
	CEtUIStatic *m_pStaticInfo;
	CEtUIStatic *m_pStaticType;
	CEtUIListBoxEx *m_pSpecialBoxList;

	CDnSpecialBoxConfirmDlg *m_pSpecialBoxConfirmDlg;

public:
	CDnSpecialBoxListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSpecialBoxListDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

public:

	void SetSpecialBoxListItem( SCSpecialBoxItemList *pData );
	void SetParentsInfo( TSpecialBoxInfo m_SpecialBoxInfo );
	int RequestItemErrorHandler();
	void OpenItemConfirmDlg();
	void OnCompleteReceiveItem();

};

#endif // PRE_SPECIALBOX