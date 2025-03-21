#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "DnCustomDlg.h"

class CDnGameQuitRewardDlg : public CDnCustomDlg
{
private:

	// Controls //
	CEtUIStatic * m_pStatic;
	std::vector< CDnItemSlotButton * > m_vecItemSlot;

	std::vector< CDnItem * > m_vecItem[2]; // 0(신규유저),  1(귀환유저)

	int m_nRewardType; // GameQuitReward::RewardType::eType

	bool m_bRecvReward; // 신규유저가 즉시받기 선물을 받은경우 true.

public:


	CDnGameQuitRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnGameQuitRewardDlg();

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	virtual void Show( bool bShow );

private:

	// 보상대상에 따른 보상아이템 설정.
	void SetRewardItem();

	// 선물받기요청.
	void GiftReceiving();

	// 다음접속시 보상출력.
	void ShowNextTimeReward();

public:

	// 보상대상 타입( 신규유저, 귀환유저 )
	void SetRewardType( int type );

	void RecvReward();
	bool GetRecvReward(){
		return m_bRecvReward;
	}
	bool ShowNewbieNextTimeReward();

};
#endif // PRE_ADD_GAMEQUIT_REWARD