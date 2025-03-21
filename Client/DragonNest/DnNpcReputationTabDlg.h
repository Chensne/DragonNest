#pragma once
#include "EtUITabDialog.h"


class CDnNpcReputationDlg;
class CDnNpcReputationAlbumDlg;
class CDnNpcReputationBromideDlg;
class CDnRepUnionDlg;

// 호감도 탭 다이얼로그.
// 호감도 리스트 다이얼로그와 호감도 앨범 다이얼로그를 가지고 있다.
class CDnNpcReputationTabDlg : public CEtUITabDialog, public CEtUICallback
{
private:
	CEtUIRadioButton* m_pBtnListTab;
	CEtUIRadioButton* m_pBtnAlbumTab;
	CEtUIRadioButton* m_pBtnUnionTab;

	CDnNpcReputationDlg* m_pNpcListDlg;
	CDnNpcReputationAlbumDlg* m_pNpcAlbumDlg;
	CDnNpcReputationBromideDlg* m_pBromideDlg;
	CDnRepUnionDlg* m_pUnionDlg;

public:
	CDnNpcReputationTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, 
							int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNpcReputationTabDlg( void );

	void Initialize( bool bShow );
	void InitialUpdate(	void );
	void Show( bool bShow );
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ );

	void OnSelectNpcInUnionDlg(int npcId);
};