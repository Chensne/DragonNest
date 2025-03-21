#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "DnCustomDlg.h"

class CDnGameQuitNextTimeRewardDlg : public CDnCustomDlg
{
private:

	// Controls //
	//CEtUITextBox * m_pStatic;
	CEtUIStatic * m_pStatic;
	std::vector< CDnItemSlotButton * > m_vecItemSlot;
	std::vector< CDnItem * > m_vecItem;

	int m_nRewardType; // GameQuitReward::RewardType::eType

	bool m_bLogin; // 로그인창(캐릭터선택) 과 게임 2곳에서 모두 사용하는 창이지만, 각각 처리가 다름.

public:


	CDnGameQuitNextTimeRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnGameQuitNextTimeRewardDlg();

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	virtual void Show( bool bShow );

private:

	// 보상대상에 따른 보상아이템 설정.
	void SetNextTimeRewardItem( bool bCharSelect=false, int nLevel=0, int nClass=-1 );

public:

	// 보상대상 타입( 신규유저, 귀환유저 )
	void SetNextTimeRewardType( int type, bool bCharSelect=false, int nLevel=0, int nClass=-1 );



};
#endif // PRE_ADD_GAMEQUIT_REWARD