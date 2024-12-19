#pragma once

#include "EtUIDialog.h"
#include "DnChatTabDlg.h"
#include "DnPVPObserverDlg.h"
#include "DnPVPInfoDlg.h"
#include "DnPVPOptionDlg.h"

class CDnPVPLobbyChatTabDlg : public CDnChatTabDlg
{

protected:
	
	CEtUIListBoxEx *m_pObserverListBox;

public:

	struct sObserverSlot{
		int iJobID;
		BYTE    cLevel;
		BYTE    cPVPLevel;
		WCHAR   wszPlayerName[NAMELENMAX];
		UINT    uiSessionID;
		bool    isNeedUpdate;
		UINT	uiUserState;
#if defined( PRE_ADD_REVENGE )
		UINT uiRevengeID;
#endif	// #if defined( PRE_ADD_REVENGE )

		sObserverSlot()
			:iJobID(0)
			,cLevel(0)
			,cPVPLevel(0)
			,isNeedUpdate(true)
			,uiUserState(0)
			,uiSessionID(0)			
#if defined( PRE_ADD_REVENGE )
			, uiRevengeID(0)
#endif	// #if defined( PRE_ADD_REVENGE )
		{			
			SecureZeroMemory(wszPlayerName, sizeof(wszPlayerName));
		}
	};


protected:

	std::vector<sObserverSlot> m_sObserverSlot;
	bool m_SelectList;


	CEtUIRadioButton *m_pButtonCharInfo;
	CEtUIRadioButton *m_pButtonObserver;
	CEtUIRadioButton *m_pButtonConfig;

	CDnPVPObserverDlg *m_pPVPObserverDlg;
	CDnPVPInfoDlg *m_pPVPInfoDlg;
	CDnPVPOptionDlg *m_pPVPOptionDlg;

	bool m_bInitPvpInfo;      // 이니셜라이징이 pvp로비 에 접근하는 시점에 일어나지 않는다. = <pvp빌리지테크스가 없다> -> 테스크 유무에따른 시작이니셜을 따로해줘야한다.

#ifdef PRE_MOD_PVPOBSERVER
	bool m_bObserverMode;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_DWC
	bool m_bIsDWCUser;
#endif
	
public:
	CDnPVPLobbyChatTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLobbyChatTabDlg(void);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	CEtUIListBoxEx* GetObserverListBox(){return m_pObserverListBox;}
	
	void ShowObserverDlg(bool bTrue); // 밖에있을때는 배너가 , 안에들어갈때는 옵져버 리스트가 떠야됩니다~ 서로 체인징 하기위한 함수
	void InsertObserver(int team , UINT uiUserState , UINT    uiSessionID , int iJobID , BYTE  cLevel , BYTE  cPVPLevel , const WCHAR * szPlayerName );
	void RemoveObserver(int iSessionID ,bool bAll = false);
	void SetSelectedListBox(bool bTrue){m_SelectList = bTrue;}
	
	int GetSessionIDFromList(int Num){return m_sObserverSlot[Num].uiSessionID;};
	std::wstring GetNameFromList(int Num){return m_sObserverSlot[Num].wszPlayerName;}
	int GetObserverNumber(){return (int)m_sObserverSlot.size();}; // 옵져버가 몇명인지 알아냅니다.
	bool IsSelectedListBox(){return m_SelectList;};
	sObserverSlot GetObserverSlotFromSessionID(int nSessionID);
	void SetObserverSlotUserState( const int nSessionID, const UINT uiUserState );

	void RefreshMyPVPInfo();
	CDnPVPObserverDlg* GetPVPObserverDlg(){return m_pPVPObserverDlg;}
	CDnPVPOptionDlg* GetPVPOptionDlg(){return m_pPVPOptionDlg;}

#ifdef PRE_MOD_PVPOBSERVER
	bool IsObserverMode(){
		return m_bObserverMode;
	}
#endif // PRE_MOD_PVPOBSERVER

private:
#ifdef PRE_MOD_PVPOBSERVER
	void SetTextObserver();	
#endif // PRE_MOD_PVPOBSERVER


};
