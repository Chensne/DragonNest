#pragma once
#include "DnCustomDlg.h"
#include "DNPacket.h"

#ifdef PRE_SPECIALBOX

class CDnSpecialBoxListDlg;

class CDnEventReceiverTabDlg : public CDnCustomDlg
{
protected:
	static float s_fRequestDelay;

	CEtUIStatic *m_pStaticItemCount;
	CEtUIListBoxEx *m_pListBoxExSpecialBox;
	CEtUIRadioButton *m_pRadioButtonSpecialBox;

	CDnSpecialBoxListDlg *m_pSpecialBoxListDlg;
	std::vector<TSpecialBoxInfo> m_vecSpecialBoxInfo;
	
public:
	CDnEventReceiverTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnEventReceiverTabDlg();

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void AddSpecialBoxInfo( TSpecialBoxInfo *pBoxInfo );
	void RemoveSpecialBoxInfoFromRewardID( int nEventRewardID );
	void ClearSpecialBoxInfo();
	void RefreshSpecialList();
	
	static void ResetDelayTime();
	static void RequestDealy();
	static bool CanRequestSpecialBoxInfo();

	CDnSpecialBoxListDlg *GetSpecialListDlg();
};

#endif // PRE_SPECIALBOX