#pragma once
#include "EtUIDialog.h"
#include "DNPacket.h"

#ifdef PRE_SPECIALBOX

class CDnSpecialBoxTabItemDlg : public CEtUIDialog
{
protected:

	enum eStaticMember
	{
		Title = 0,
		Condition = 1,
		ReceiveCondition = 2, 
		Date = 3,
		Level = 4,
		Job = 5,
		From = 6,
		Arrow = 7,
		Cover = 8,
		CoverBar = 9,
		Max = 10,
	};

	CEtUIStatic *m_pStatic[eStaticMember::Max];
	CEtUIButton *m_pButtonAll;
	CEtUIButton *m_pButtonChoice;

	bool m_bConditionCheck;
	TSpecialBoxInfo m_SpecialBoxInfo;

public:
	CDnSpecialBoxTabItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSpecialBoxTabItemDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:

	void SetSpecialBoxInfo( TSpecialBoxInfo *pData );
	void SetMessageText( TSpecialBoxInfo *pData );
	void SetConditionText( int nTargetType , int nReceiveType );
	void SetExpireTimeText( __time64_t time );
	void SetLevelText( int nMinLevel , int nMaxLevel );
	void SetJobText( int nClassID );
	void SetButtonState( TSpecialBoxInfo *pData );
	void RequestSpecialBoxItemList();

};

#endif // PRE_SPECIALBOX