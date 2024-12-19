#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "DnCustomDlg.h"

class CDnGameQuitRewardDlg : public CDnCustomDlg
{
private:

	// Controls //
	CEtUIStatic * m_pStatic;
	std::vector< CDnItemSlotButton * > m_vecItemSlot;

	std::vector< CDnItem * > m_vecItem[2]; // 0(�ű�����),  1(��ȯ����)

	int m_nRewardType; // GameQuitReward::RewardType::eType

	bool m_bRecvReward; // �ű������� ��ùޱ� ������ ������� true.

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

	// ������ ���� ��������� ����.
	void SetRewardItem();

	// �����ޱ��û.
	void GiftReceiving();

	// �������ӽ� �������.
	void ShowNextTimeReward();

public:

	// ������ Ÿ��( �ű�����, ��ȯ���� )
	void SetRewardType( int type );

	void RecvReward();
	bool GetRecvReward(){
		return m_bRecvReward;
	}
	bool ShowNewbieNextTimeReward();

};
#endif // PRE_ADD_GAMEQUIT_REWARD