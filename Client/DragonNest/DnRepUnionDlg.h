#pragma once

#include "EtUIDialog.h"
#include "ReputationSystem.h"

#define NPC_UNION_REPUTATION_COUNT_PER_PAGE 4
#define NPC_UNION_MEMBER_COUNT_PER_PAGE		9

class CDnRepUnionDlg : public CEtUIDialog, public CEtUICallback
{
	enum eUnionFigureType
	{
		eUFT_MIN,
		eUFT_EXPLAIN = eUFT_MIN,
		eUFT_MEMBER,
		eUFT_REWARD,
		eUFT_MAX
	};

	struct SUnionUnitUI
	{
		int			 tableItemID;
		int			 unionID;
		CEtUIStatic* pEventReceiver;
		CEtUIStatic* pSelect;
		CEtUITextureControl* pEmblem;
		CEtUIStatic* pName;
		CEtUIStatic* pPointText;
		CEtUIStatic* pPoint;

		SUnionUnitUI() : tableItemID(-1), unionID(-1), pEventReceiver(NULL), pSelect(NULL), pEmblem(NULL), pName(NULL), pPointText(NULL), pPoint(NULL) {}
		void Reset();
		void SetName(const std::wstring& name);
		void SetEmblem(EtTextureHandle hEmblem, int iconIdx);
		void ShowSelect(bool bShow);
		void SetPoint(UNIONPT_TYPE point);
		bool IsEmpty() const;
		bool IsSelected() const { return pSelect->IsShow(); }
		void ShowPoint(bool bShow);
		void ShowAll(bool bShow);
	};

	struct SUnionMemberUI
	{
		int				npcID[NPC_UNION_MEMBER_COUNT_PER_PAGE];
		int				npcNameStringID[NPC_UNION_MEMBER_COUNT_PER_PAGE];
		CEtUIStatic*	pBase;
		CEtUITextureControl*	pFace[NPC_UNION_MEMBER_COUNT_PER_PAGE];
		CEtUIProgressBar*		pFavorValue[NPC_UNION_MEMBER_COUNT_PER_PAGE];

		SUnionMemberUI() : pBase(NULL)
		{
			SecureZeroMemory(npcID, sizeof(npcID));
			SecureZeroMemory(npcNameStringID, sizeof(npcNameStringID));
			SecureZeroMemory(pFace, sizeof(pFace));
			SecureZeroMemory(pFavorValue, sizeof(pFavorValue));
		}
		void ShowAll(bool bShow);
		void ShowBase(bool bShow) { pBase->Show(bShow); }
		void ShowNpc(bool bShow, int idx);
		void SetFace(EtTextureHandle hFace, int iconIdx, int arrayIndex);
		bool IsShow() const;
		int  GetNpcID(const std::string& ctrlName) const;
		int  GetNpcIDByPos(float fX, float fY) const;
	};

	struct SUnionMemberInfo
	{
		int		tableId;
		int		npcId;
		bool	bIsOpened;
		REPUTATION_TYPE	favorValue;
		REPUTATION_TYPE	favorMax;

		SUnionMemberInfo() : tableId(0), npcId(-1), bIsOpened(false), favorMax(0), favorValue(0) {}
	};

public:
	CDnRepUnionDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnRepUnionDlg(void);

	void Initialize(bool bShow);
	void InitialUpdate();

	void Show(bool bShow);
	void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void Reset();

	void UpdateAll(bool bIsInit);
	void UpdateGroupPageBtn();
	void UpdateFigurePage();
	void UpdateMemberPage(bool bMakeMemberList);
	void UpdateMemberPageBtn();

	void ShowMemberPageBtn(bool bShow);

	void ProcessPage(bool bNext);

	eUnionFigureType	GetCurUnionTabType() const;
	const SUnionUnitUI*	GetCurSelectedUnionUI() const;

	static bool SortByQuestOpened(const SUnionMemberInfo& a, const SUnionMemberInfo& b);
	static bool SortByNpcIndex(const SUnionMemberInfo& a, const SUnionMemberInfo& b);

	SUnionUnitUI	m_UnionGroupUIs[NPC_UNION_REPUTATION_COUNT_PER_PAGE];
	SUnionMemberUI	m_UnionMemberUI;

	CEtUIStatic*	m_pUnionGroupPageCount;
	CEtUIButton*	m_pUnionGroupBtnPrevPage;
	CEtUIButton*	m_pUnionGroupBtnNextPage;

	CEtUIStatic*	m_pUnionMemberPageCount;
	CEtUIButton*	m_pUnionMemberBtnPrevPage;
	CEtUIButton*	m_pUnionMemberBtnNextPage;

	CEtUITextBox*	m_pUnionExplainText;

	CEtUIRadioButton* m_pUnionTab[eUFT_MAX];

	EtTextureHandle	 m_hEmblemTexture;
	EtTextureHandle	 m_hNpcFaceTexture;
	EtTextureHandle	 m_hHideNpcFaceTexture;

	int	m_MaxGroupPageCount;
	int m_CurGroupPageCount;

	int m_MaxMemberPageCount;
	int m_CurMemberPageCount;

	std::map<int, std::vector<SUnionMemberInfo> > m_UnionNpcList;
};