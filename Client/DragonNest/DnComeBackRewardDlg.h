
#ifdef PRE_ADD_NEWCOMEBACK

#include "DnCustomDlg.h"


class CDnComeBackRewardDlg : public CDnCustomDlg, public CEtUICallback
{

private:

	// test.
	int m_level;

	// Controls //
	CEtUIComboBox * m_pComboCharacter;
	CEtUIStatic * m_pStaticGuild;
	std::vector< CEtUIStatic * > m_vecStaticName;
	std::vector< CDnItemSlotButton * > m_vecItemSlot;
	std::vector< CEtUIRadioButton * > m_vecRadioBtn;

	std::vector< int > m_vecJob;

	int m_crrEnableRadioIndex;

	int m_SelectedCharIndex; // ĳ���ͼ���â���� ������ ĳ�����ε���.

public:


	CDnComeBackRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnComeBackRewardDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	virtual void Show( bool bShow );

	// Override - CEtUICallback //
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

private:

	// ĳ���� �޺��ڽ�����.
	void BuildComboBox();

	// ��������۽��� �缳��. 
	void RefreshItemSlot( int nIndex, bool bForce=false );
	
	void Reset();

public:

	// ĳ���ͼ���â���� ������ ĳ���ͷ� ��������� ����.
	void SetSelectedChar( int nIndex );

};

#endif // PRE_ADD_NEWCOMEBACK
