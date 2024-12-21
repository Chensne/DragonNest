#pragma once
#include "DnCustomDlg.h"
#include "DnMovieControl.h"

class CDnJobChangeDlg : public CDnCustomDlg
{
public:
	CDnJobChangeDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnJobChangeDlg(void);

protected:
	CEtUIStatic *m_pSelectedClass[2];
	CEtUIButton *m_pSelectButton[2];

	CDnMovieControl *m_pMovieControl[2];

	CEtUIStatic *m_pClassName[2];
	CDnJobIconStatic *m_pJobIcon[2];
	CEtUIStatic *m_pClassInfoText[2];
	CDnSkillSlotButton *m_pSkillButtonA[2];
	CDnSkillSlotButton *m_pSkillButtonB[2];

	CEtUIButton *m_pButtonExClass;
	CEtUIButton *m_pButtonTakeClass;
	CEtUIStatic *m_pCover;

	int m_pClassID[2];
	int nSelectedIndex;
	char *m_pMovieFile[2];
public:
	struct JobData {
		int nJobID;
		int nJobName;
		int nJobDescriptionID;
		char *nJobVideo;
		char *nSkill;
		int nRecomm;
	};

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Show(bool bShow);

	void BtnSelectClass(int ClassID);
	void InitClass(int ClassID);
	std::vector<JobData> CalcNextClass(int ClassID);
	void Reset();

};
