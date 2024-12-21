#pragma once

#include "DnUIDefine.h"

#ifdef PRE_ADD_PVPRANK_INFORM

class CDnPvPRankInformDlg : public CEtUIDialog
{
public:
	struct SUserInfo
	{
		std::wstring userName;
		int userRank;
		int userExp;

		SUserInfo() { userRank = -1; userExp = -1; }
		void Clear() { userName.clear(); userRank = -1; userExp = -1; }
		bool IsEmpty() const { return userName.empty(); }
	};

	CDnPvPRankInformDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false);
	virtual ~CDnPvPRankInformDlg();

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool IsShowExpRankType(const int& rankType) const;
	void SetBasicInfo();
	void MakeRankInfos();
	void UpdateExpStatus();
	void Clear();

	CEtUIStatic* m_pUserName;
	CEtUIStatic* m_pUserRank;
	CEtUIStatic* m_pUserExp;
	CEtUITextureControl* m_pUserRankIcon;
	SUserInfo m_UserInfo;
	int m_ScrollStartIndex;

	int m_UserExpCache;
	int m_NextLevelExpCache;
	std::wstring m_ExpTooltipCache;

	CEtUIListBoxEx* m_pRankInfoList;
	std::vector<PvPRankInfoUIDef::SRankUnit> m_RankInfos;
};

#endif // PRE_ADD_PVP_RANKING