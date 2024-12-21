#pragma once
#include "DnCustomDlg.h"

class CDnCharStatusLadderInfoDlg : public CDnCustomDlg
{
public:
	CDnCharStatusLadderInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharStatusLadderInfoDlg(void);
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

	struct sLadderScoreInfo_UI
	{
		CEtUIStatic *pLadderGrade;
		CEtUIStatic *pLadderStatus;
		CEtUIStatic *pLadderPlayCount;

		sLadderScoreInfo_UI()
		{
			pLadderGrade = NULL;
			pLadderStatus= NULL;
			pLadderPlayCount= NULL;
		}
	};

protected:

	CEtUIStatic *m_pLadderPoint;
	CEtUIStatic *m_pExpectingLadderPoint;
	std::vector<sLadderScoreInfo_UI> m_vecLadderScoreInfo;
	
public:

	void SetPvpLadderInfo(LadderSystem::SC_SCOREINFO* pInfo);
	void RefreshLadderInfo();
	void InitializeLadderScoreInfo();
};
