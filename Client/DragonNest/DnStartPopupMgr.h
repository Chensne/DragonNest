#pragma once

#ifdef PRE_ADD_START_POPUP_QUEUE

class CEtUIDialog;
class CDnStartPopupMgr
{
public:
	enum ePopupType
	{
		eNormal,
		eEventToday,
		eStamp,
		eEventGuide,
	};

	struct SPopupUnit
	{
		ePopupType type;
		CEtUIDialog* pPopupDlg;
		std::string stringParam;

		SPopupUnit() { type = eNormal; pPopupDlg = NULL; }
		bool IsSame(const SPopupUnit& unit) const;
	};

	CDnStartPopupMgr();
	virtual ~CDnStartPopupMgr() {}

	void Process(float fElapsedTime);
	void RegisterTop(ePopupType type, CEtUIDialog* pDlg, const char* stringParam1 = NULL);
	void RegisterLow(ePopupType type, CEtUIDialog* pDlg, const char* stringParam1 = NULL);

	void StartShow();
	void StopAll();

private:
	void DoShow();
	void PopAndShow();

	std::list<SPopupUnit> m_Popups;
	bool m_bShowing;
};

#endif // PRE_ADD_START_POPUP_QUEUE