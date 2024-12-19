#pragma once

#ifdef PRE_ADD_DONATION


#include "DnCustomDlg.h"


//------------------------------------------------
// 기부시스템 메인창.

class CDnContributionDlg : public CDnCustomDlg
{

private:

	// 기부관련 Dialog Group.
	enum EDlgGroup
	{
		EDlgGroup_GOLD = 0,  // 골드기부.
		EDlgGroup_REWARD = 1,// 보상보기.
		EDlgGroup_MAX
	};

	// Diglaog Group.
	class CEtUIDialogGroup * m_pDlgGroup;	

	// LocalPlayer 컨트롤들.
	CEtUIStatic * m_pStaticNameJobLv;
	CEtUIStatic * m_pStaticRank;
	CEtUIStatic * m_pStaticGold;
	/*CEtUIStatic * m_pStaticSivler;
	CEtUIStatic * m_pStaticCopper;*/

	//// 상위랭커 1,2,3 위 용 컨트롤들. UI변경으로 주석처리.
	//struct SHighRankerControls
	//{
	//	CDnJobIconStatic * pJobIcon;
	//	CEtUIStatic * pStaticGuild;
	//	CEtUIStatic * pStaticID;
	//	CEtUIStatic * pStaticGold;
	//	/*CEtUIStatic * pStaticSilver;
	//	CEtUIStatic * pStaticCopper;*/
	//	CDnItemSlotButton * pItemSlot;
	//};
	//vector< SHighRankerControls > m_vecHighRanker;

	// 7~10 위 ListBox.
	CEtUIListBoxEx * m_pListRank;

	
	// 컨트롤 - 기부하기.
	CEtUIButton * m_pBtnContribute;

	bool m_bFirstOpen; // Dlg 최초오픈. 최초:true, 이후:false


public:

	CDnContributionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnContributionDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();


	// Override - CEtUIDialog //	
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );



	// 기부랭킹목록 갱신.
	void RefreshList();	

	// 기부하기 Dialog 갱신 - 현재소지금.
	void RefreshContributionGoldDlg();

private:

	//// 상위랭커 갱신.
	//void SetDataHighRanker( const struct SContributionRanker * aRanker, SHighRankerControls & aControls, int nItemID );
	
	
	// 랭킹목록갱신패킷 전송.	
	void OnSendRefreshRankerList();
};


#endif