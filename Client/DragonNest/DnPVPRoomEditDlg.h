#pragma once
#include "DnPVPMakeRoomDlg.h"


class CDnPVPRoomEditDlg : public CDnPVPMakeRoomDlg
{

public:
	CDnPVPRoomEditDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPRoomEditDlg(void);
protected:

	int iMaxPlayerLevel; 
	int iMinPlayerLevel;
	int bIsEditRoom;
	virtual void SendMakeRoom();

public:
	void SetRoomName( const WCHAR * RoomName );
	void SetPassword();

	void SetMinMaxPlayerLevel(int MinLevel,int MaxLevel){iMinPlayerLevel = MinLevel; iMaxPlayerLevel = MaxLevel;} // 이부분은 룸을 에딧할때 방에있는 유저보다 랩이 낮게 설정하지 못하기위해 쓰인다.
	void SetEditState(bool bEdit){bIsEditRoom = bEdit;}
#ifdef PRE_ADD_PVP_TOURNAMENT
	void EnableMapDropDownList(bool bEnable);
	void EnableGameModeDropDownList(bool bEnable);
	void EnableRandomTeam(bool bEnable);
#endif

	virtual bool IsEditRoom() { return true; }
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
