#pragma once

#include "EtUIDialog.h"

class CDnDarklairInfoDlg : public CEtUIDialog 
{
public:
	CDnDarklairInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDarklairInfoDlg();

protected:
	CEtUIStatic *m_pStaticMapName;
	CEtUIStatic *m_pStaticRound;
	CEtUIStatic *m_pStaticBoss;
	CEtUIStatic *m_pStaticFinal;
	CEtUIStatic *m_pStaticBonus;

	SUICoord m_uiDefaultCoord[3];

	// 보스게이지 뜰때 이동할 임시 위치 컨트롤
	CEtUIStatic *m_pStaticMovePosition;
	SUICoord m_uiMovePositionOffset;
	bool m_bMoveOffset;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	void RefreshInfo();
	void RefreshRound( int nRound, bool bBoss, bool bFinal );
	void MoveOffset( bool bMove );
};