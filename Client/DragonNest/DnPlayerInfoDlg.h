#pragma once
#include "DnCustomDlg.h"

class CDnPlayerInfoDlg : public CDnCustomDlg
{
public:
	CDnPlayerInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPlayerInfoDlg(void);

protected:
	CEtUIStatic *m_pPlayerLevel;
	CEtUIStatic *m_pPlayerName;
	CEtUIStatic *m_pVillageName;
	CDnJobIconStatic *m_pJobIcon;
	CEtUIStatic *m_pStaticSlotStateBoard;
	CEtUIStatic *m_pStaticSlotStateStr;
	int m_nJobClassID;

public:
	void SetInfomation( LPCWSTR pwszPlayerName, LPCWSTR pwszVillageName, int nLevel, int nJobClassID );
	bool IsInfoValid();
	void ClearInfo();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};