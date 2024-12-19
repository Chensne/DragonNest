#pragma once
#include "DnCustomDlg.h"
#include "DnJobIconStatic.h"

class CDnPVPLadderObserverInfoDlg : public CDnCustomDlg, public CEtUICallback
{
public:

	CDnPVPLadderObserverInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLadderObserverInfoDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	enum eLadderUserInfoCommon
	{
		TeamMax = 4,
		MaxPlayer = 8,
	};
	struct sLadderUserInfo
	{
		CEtUIStatic *pName;
		CDnJobIconStatic *pJobIcon;

		sLadderUserInfo()
		{
			pName = NULL;
			pJobIcon = NULL;
		}
	};

public:
	void InitializeLadderUserInfoUI();
	void ClearLadderUserInfo();
	void SetLadderUserInfo(int nLadderType,int nSelectedCount,LadderSystem::SC_PLAYING_ROOMLIST *pData);

protected:

	sLadderUserInfo m_sLadderInfo_A[eLadderUserInfoCommon::TeamMax]; // 팀구분을 합시다.
	sLadderUserInfo m_sLadderInfo_B[eLadderUserInfoCommon::TeamMax];
};
