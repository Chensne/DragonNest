#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "DnCustomDlg.h"

class CDnGameQuitNextTimeRewardComboDlg : public CDnCustomDlg, public CEtUICallback
{
private:

	// Controls //	
	CEtUIComboBox * m_pComboCharacter;
	std::vector< CDnItemSlotButton * > m_vecItemSlot;
	std::vector< CDnItem * > m_vecItem;

	std::vector< int > m_vecJob;	

public:


	CDnGameQuitNextTimeRewardComboDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnGameQuitNextTimeRewardComboDlg();

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	virtual void Show( bool bShow );


	// Override - CetUICallback / 
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );


	// 보상대상에 따른 보상아이템 설정.
	void SetNextTimeRewardComboItem( int nLevel, int nClass );


private:

	void BuildComboBox();

public:

	// 캐릭터선택창에서 선택한 캐릭터Index.
	void SelectedGameQuitRewardCharIndex( int charIndex );

	void ResetComboBox();

};
#endif // PRE_ADD_GAMEQUIT_REWARD