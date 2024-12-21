#pragma once
#include "DnCustomDlg.h"
#include "Singleton.h"
#include "EtUIDialogGroup.h"
#include "DnCustomControlCommon.h"
#include "DnMinimap.h"

class CDnQuickSlotDlg;
class MIInventoryItem;
class CDnExpGauge;
class CDnExpDlg;
class CDnMessageStatic;
class CDnPassiveSkillSlotDlg;
class CDnPopupFatigueDlg;
class CDnGuildWarSkillSlotDlg;
#ifdef PRE_TEST_ANIMATION_UI
class CDnAniTestDlg;
#endif
class CDnItem;
class CDnMainBarMenuDlg;
//class CDnFarmPVPBtnDlg;
class CDnMainBar_MinimapDlg;
class CDnMainBar_EventDlg;
//class CDnMainBar_BubbleGaugeDlg;

class CDnMainDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnMainDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMainDlg(void);

	enum EMQUICKSLOT_DLG_ID
	{
		// 혹시 또 세페이지로 만들어달랄지도 모르니 주석처리 해두겠다.
		QUICKSLOT_DLG_00,
		QUICKSLOT_DLG_01,
		QUICKSLOT_SECOND_DLG_00,
		QUICKSLOT_SECOND_DLG_01, // 이중스킬로 인해서 늘어나는 슬롯이다. 메인슬롯이 늘어나면 같이늘어나야한다.
		QUICKSLOT_DLG_MAX,	// 일반 퀵슬롯은 여기까지만 순회 < DB및 서버에 정보가 저장되는 범위 >
		QUICKSLOT_EVENT,    // 클라이언트에서 강제로 Set해주는 퀵슬롯 서버에 저장하지않음.

#if defined( PRE_ADD_SKILLSLOT_EXTEND )
		QUICKSLOT_BUTTON_MAX = 12,
#else
		QUICKSLOT_BUTTON_MAX = 10,
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )

		QUICKSLOT_PAGE_COUNT = 2,

		QUICKSLOT_SKILL_PAGE_COUNT = QUICKSLOT_DLG_MAX / DualSkill::Type::MAX,
		RADIOMSG_DLG = 101,	// 별도의 라디오메세지 퀵슬롯 다이얼로그
		LIFESKILL_DLG,		// 별도의 라이프스킬 퀵슬롯 다이얼로그
		MAINMENU_DLG,
	};

	enum eMainDlgSoundType
	{
		eSOUND_MAILALARM,
		eSOUND_CASHSHOPGIFT,
		eSOUND_MAX
	};

	enum eNotifyBlinkTime
	{
		eMiniSiteBlinkTime = 10
	};

protected:

	CEtUIStatic *m_pStaticSlotNumber;
	CEtUIDialogGroup m_QuickSlgDlgGroup;

	std::vector<CDnMessageStatic*> m_vecStaticMessage;

	CDnPassiveSkillSlotDlg	*m_pPassiveSkillSlotDlg;

	//main bar menu 
	CDnMainBarMenuDlg  *m_pMainBarMenuDlg;

	//Love icon
	CEtUIButton * m_pLoveIcon;

	//farm pvp icon
	//CDnFarmPVPBtnDlg  *m_pFarmPVPBtnDlg;

	//Minimap 90cap
	CDnMainBar_MinimapDlg  *m_pMainBar_MinimapDlg;

	//Event Dialog 90cap
	CDnMainBar_EventDlg *m_pMainBar_EventDlg;

	//Bubble Dialog 90cap
	//CDnMainBar_BubbleGaugeDlg *m_pMainBar_BubbleGaugeDlg;



	CDnExpDlg *m_pExpDlg;
	CDnExpGauge *m_pExpGauge[5];
	CEtUIStatic *m_pStaticExp;

	CDnExpGauge *m_pFTGGauge;
	CEtUIStatic *m_pStaticFTGFont;
	CEtUIStatic *m_pStaticFTG;

	CDnExpGauge *m_pWeekFTGGauge;
	CEtUIStatic *m_pStaticWeekFTGFont;
	CEtUIStatic *m_pStaticWeekFTG;

	CDnExpGauge *m_pPCRoomFTGGauge;
	CEtUIStatic *m_pStaticPCRoomFTGFont;
	CEtUIStatic *m_pStaticPCRoomFTG;

	CDnExpGauge *m_pEventFTGGauge;
	CEtUIStatic *m_pStaticEventFTGFont;
	CEtUIStatic *m_pStaticEventFTG;

	CDnExpGauge *m_pVIPFTGGauge;
	CEtUIStatic *m_pStaticVIPFTGFont;
	CEtUIStatic *m_pStaticVIPFTG;


	CDnPopupFatigueDlg *m_pPopupFTGDlg;

	EtTextureHandle m_hNorthTexture;

	DWORD m_dwShowQuickDlgID;
	DWORD m_dwPrevQuickDlgID;
	DWORD m_dwOrignalShowQuickSlotDlgID;
	DWORD m_dwOrignalPrevQuickSlotDlgID;

	float m_fDegree;

	bool m_bExpUp;
	bool m_bLockQuickSlot;
	float m_fExpUpTime;
	SUICoord m_ExpCoord;
	bool m_bMoneyUp;
	float m_fMoneyUpTime;
	SUICoord m_MoneyCoord;

	EtTextureHandle m_hBackLight;
	CEtUIStatic *m_pStaticBackLight;
	EtTextureHandle m_hBackBlack;
	CEtUIStatic *m_pStaticBackBlack;

	CEtUIStatic* m_pStaticPost;
	CEtUIStatic* m_pStaticCashShopGift;
	CEtUIStatic* m_pStaticCashShopGiftBalloon;
	CEtUIStatic* m_pStaticCashShopGiftText;
	CEtUIStatic* m_pStaticHarvest;
	CEtUIStatic* m_pStaticCheckAttendanceBalloon;
	CEtUIStatic* m_pStaticCheckAttendanceText;
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	CEtUIButton* m_pStaticLevelUpNotifier;
#endif 

#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	CEtUIStatic* m_pStaticMailText;
	CEtUIStatic* m_pStaticMailBallon;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
	bool m_bShowQuestNotify;
	CEtUIStatic* m_pStaticQuestText;
	CEtUIStatic* m_pStaticQuestBallon;
#endif // PRE_ADD_REMOTE_QUEST
	bool m_bCheckAttendanceFirst;
	float m_fCheckAttendanceAlarmTime;
	bool m_bNotifyCheckAttendance;

#ifdef PRE_REMOVE_MINISITE
#else
	CEtUIButton* m_pNewMiniSiteBtn;
#endif
	bool m_bNotifyCheckMiniSite;
	float m_fMiniSiteAlarmTime;

	bool m_bCashShopGiftAlarm;
	float m_fCashShopGiftAlarmTime;
	bool m_bMailAlarm;
	float m_fMailAlarmTime;
	bool m_bHarvestAlarm;
	float m_fHarvestAlarmTime;
	int m_nHarvestCount;

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	float m_fLevelUpboxAlarmTime;
	bool m_bLevelUpBoxAlarm;
	int  m_nAlarmCount;
#endif

	bool m_bShowTimeEventAlarm;
	char m_cUpdateEventType;
	int m_nTimeEventID;
	float m_fTimeEventHideDelta;
	CEtUIButton* m_pStaticTimeEventIcon[6]; //CEtUIStatic
	CEtUIStatic* m_pStaticTimeEventText;

#ifdef PRE_ADD_LEVELUP_GUIDE
	CEtUIStatic* m_pStaticLevelUpSkillAlarm;
	CEtUIStatic* m_pStaticLevelUpSkillAlarmText;
	bool m_bLevelUpSkillAlarm;
	float m_fLevelUpSkillAlarmTime;
#endif

#ifdef PRE_TEST_ANIMATION_UI
	CEtUIButton*	m_pAniTestBtn;
	CDnAniTestDlg*	m_pAniTestDlg;
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	CEtUIStatic* m_pStaticNewAppellationAlarm;
	CEtUIStatic* m_pStaticNewAppellationAlarmText;
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	CEtUIButton * m_pAlteaDiceIcon;
	bool m_bAlteaDiceShow;
	bool m_bAlteaDiceAlarm;
	float m_fAlteaDiceAlarmTime;
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

	struct SButtonInfo
	{
		CDnMenuButton *pButton;
		int nDialogID;

		SButtonInfo()
			: pButton(NULL)
			, nDialogID(0)
		{
		}
	};

	std::vector<SButtonInfo> m_vecButtonInfo;

	CDnGuildWarSkillSlotDlg* m_pDnGuildWarSkillSlotDlg;

	bool m_bPVP;
	bool m_bShowOption;
	int m_nMainDlgSoundIndex[eSOUND_MAX];

	bool m_bEnableButtons;

#ifdef PRE_ADD_AUTO_DICE
	bool m_bShowDiceOption;
#endif
	int m_nSkillSlotPage;
#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	int m_HoldToggleShowDialogOnProcessButton;
#endif

	CDnItem*			m_pFoodItem;
	CDnItemSlotButton*	m_pFoodItemSlot;
	CEtUIStatic*		m_pStaticFoodItemCount;

#ifdef PRE_SPECIALBOX
	CEtUIButton* m_pButtonEventReceiverAlarm;
	CEtUIStatic* m_pStaticEventReceiverBalloon;
	CEtUIStatic* m_pStaticEventReceiverText;
	float m_fEventReceiverAlarmTime;
	bool m_bEventReceiverAlarm;
#endif

#ifdef PRE_ADD_STAMPSYSTEM
	class CDnNPCArrowDlg * m_NPCArrowDlg;
	int m_SearchedNpcID;
	bool m_bSearching; // 찾는중
	EtVector3 m_vNPCPos;
	EtVector3 m_camDir;
	float m_fRotDegree;
#endif // PRE_ADD_STAMPSYSTEM

protected:
	void ChangeQuickSlotUp();
	void ChangeQuickSlotDown();
	void ShowQuickSlotChangeMessage();

	void DrawMinimapOutInfo();
	void DrawIcon(CEtUIControl *pControl, float fAngle, float fRadius, float fValue);

	void ShowExpDialog(bool bShow, float fX = 0.0f, float fY = 0.0f);
	void ShowMailNotifyTooltip(float fX, float fY);
	void ShowHarvestNotifyTooltip(float fX, float fY);
	void ShowCheckAttendanceTooltip(float fX, float fY);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	void ShowLevelUpBoxNotifierToolTip(float fx, float fy);
	void UpdateLevelUpBoxNotifier(float fElapsedTime);
#endif 
	void UpdateMailNotify(float fElapsedTime);
	void UpdateCashShopGiftNotify(float fElapsedTime);
	void UpdateHarvestNotify(float fElapsedTime);
#ifdef PRE_ADD_LEVELUP_GUIDE
	void UpdateNewSkillNotify(float fElapsedTime);
#endif
#ifdef PRE_SPECIALBOX
	void UpdateSpecialBoxNotify(float fElapsedTime);
#endif
	float CalcCameraRotateValue();

	void RenderEXP();
	void RenderFTG();
	void RenderWeekFTG();
	void RenderPCRoomFTG();
	void RenderEventFTG();
#ifdef PRE_ADD_VIP
	void RenderVIPFTG();
#endif
	void RenderFTGStatic();
	void ShowFTGDialog(bool bShow, float fX = 0.0f, float fY = 0.0f);

public:
	// Note : QuickSlot
	bool InitQuickSlot(int nQuickSlotIndex, MIInventoryItem *pItem);	// Note : 로그인시 한번만 호출한다.
	int GetQuickSlotTabIndex();
	void OnRefreshQuickSlot();
	bool SetQuickSlot(int nIndex, MIInventoryItem *pItem);
	void AddPassiveSkill(DnSkillHandle hSkill);
	void ChangeQuickSlotButton(CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton);
	// LifeSkill 퀵슬롯
	bool InitLifeSkillQuickSlot(int nSlotIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType);
	void ResetLifeSkillQuickSlot(int nSlotIndex);
	bool ResetSecondarySkillQuickSlot(int nSecondarySkillID);

	bool IsShowGuildWarSkillSlotDlg();
	void ShowGuildWarSkillSlotDlg(bool bShow, bool bFixedGuildSkill);
	void UseGuildWarSkill(int nIndex);
	CDnGuildWarSkillSlotDlg* GetGuildWarSkillSlotDialog() { return m_pDnGuildWarSkillSlotDlg; }

	bool IsLockQuickSlot() { return m_bLockQuickSlot; }
	void LockQuickSlot(bool bTrue) { m_bLockQuickSlot = bTrue; } // 슬롯 페이지 스왑을 락시킵니다. < 강제로 스킬을 설정하는경우가 있기에설정 >
	void SwapEventSlot(bool bTrue);                            // 특정한 이벤트 슬롯을 활성화 / 비활성화 시켜줍니다
	void RefreshLifeSkillQuickSlot();

#ifdef PRE_SWAP_QUICKSLOT
	void SwapQuickSlot();
#endif

	bool SetQuickSlot(int nQuickSlotIndex, CDnSkill *pSkill);			// 스킬 습득시 자동 퀵슬롯 등록을 위한 함수
	int GetEmptyQuickSlotIndex();
	int GetSkillQuickSlotIndex(int nSkillID);
	void EnableQuickSlot(bool bEnable);

	void OnMailNotify();
	void OnHarvestNotify(int nCount);
	void OnCashShopGiftNotify(bool bSoundOnly = false);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	void OnLevelUpBoxNotify(bool bShow);
#endif 
	void ToggleMenuButton(DWORD dwDialogID, bool bShow);
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	void ToggleEnableAllButton(bool bEnable);
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

	//blondymarry start
	void ToggleEnableButton(DWORD dwDialogID, bool bEnable);
	void SetPVPMenu(bool isPVP) { m_bPVP = isPVP; }
	//blondymarry end
	void ProcessCommunityButton();
	void ProcessSystemMenuButton();
	void ProcessQuickSlot();
	void ProcessZoneMapButton();
	void ProcessChatRoomButton();
	void ProcessButton(DWORD dwDialogId);
	void ProcessPVPModeButton();
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	void ProcessPVPVillageAccessDlg();
#endif
	void BlinkMenuButton(DWORD dwDialogID);
	void ProcessTimeEventAlarm(float fElapsedTime);
	void CloseBlinkMenuButton(DWORD dwDialogID);
	SButtonInfo* GetMenuBotton(DWORD dwDialogID);
#ifdef PRE_ADD_PVP_RANKING
	void ProcessPvPRankButton();
#endif // PRE_ADD_PVP_RANKING
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	void ProcessPotentialJewelButton();
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

	void AddMessage(const wchar_t *szMsg, DWORD dTextColor = textcolor::WHITE);

	void ShowMinimapOption(bool bShow);
	void UpdateMinimapOption();

#ifdef PRE_ADD_AUTO_DICE
	void ShowAutoDiceOption(bool bShow);
	void UpateAutoDiceOption(bool bReset = false);
#endif

	// 키세팅
	void SetMainUIHotKey(int nUIWrappingKeyIndex, BYTE cVK);

	void EnableButtons(bool bEnable) { m_bEnableButtons = bEnable; }
	void ShowTimeEventAlarm(bool bShow);
	void UpdateTimeEventAlarm(char cType, int nLastEventID);
	CEtUIDialogGroup *GetQuickSlotDialogGroup() { return &m_QuickSlgDlgGroup; }

	void SetCheckAttendanceFirst(bool bCheckAttendanceFirst);
	void ShowCheckButton(bool bShow);
	void UpdateAttendanceNotify(float fElapsedTime);

	void ShowMiniSiteNotify(bool bShow, bool bBlink);
	void UpdateMiniSiteNotify(float fElapsedTime);
#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	void HoldToggleShowDialogOnProcessButton(bool bHold, DWORD dwDialogId);
#endif

#ifdef PRE_ADD_LEVELUP_GUIDE
	void SetLevelUpSkillAlarm(bool bSet);
#endif

#ifdef PRE_SPECIALBOX
	void SetEventReceiverAlarm(bool bNew);
	void CloseEventReceiverAlarm();
#endif

#ifdef PRE_TEST_ANIMATION_UI
	bool IsShowAniTestDlg() const;
#endif
	void ChangeSkillSlotPage(int nPage);
	bool IsCurrentSkillPageIndex(int nPageIndex);

	void SetPetFoodInfo(int nPetFoodItemID);
	void RefreshPetFoodCount();
	void ResetPetFoodInfo();

#if defined(PRE_FIX_61821)
	void ReplacePassiveSkill(DnSkillHandle hSkill);
#endif // PRE_FIX_61821

#ifdef PRE_ADD_REMOTE_QUEST
	bool IsShowQuestNotify() { return m_bShowQuestNotify; }
	void ShowQuestNotify(bool bShow, CDnQuestTree::eRemoteQuestState remoteQuestState = CDnQuestTree::REMOTEQUEST_NONE);
#endif // PRE_ADD_REMOTE_QUEST

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	void ShowAppellationNotify(bool bShow);
#endif // PRE_MOD_APPELLATIONBOOK_RENEWAL

	void SetAlteaDiceIcon();
#ifdef PRE_ADD_STAMPSYSTEM
	void NpcTalk(int npcID);
	void NPCSearch(int mapID, int npcID);
	void RotetaNPCArrow();
	void FindComplete();
#endif // PRE_ADD_STAMPSYSTEM

	void OnLevelUp(int nLevel);

	//90cap
	void SetBubble(int Count, float fExpireTime, float fTotalTime);


public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Render(float fElapsedTime);
	virtual void Process(float fElapsedTime);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg);

#if defined(PRE_ADD_68286)
	bool IsAlarmIconClick(POINT& MousePoint);
#endif // PRE_ADD_68286
};