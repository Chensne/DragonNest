#pragma once
#include "DnCustomDlg.h"
#include "DnBlindDlg.h"
#include "DnItemTask.h"
#include "DnDungeonClearImp.h"

class CDnDungeonClearMoveDlg;
class CDnStageClearMoveDlg;
class CDnStageClearCPRankDlg;
class CDnStageClearDlg : public CDnCustomDlg, public CDnBlindCallBack
{
	// 코드를 보면 알겠지만, 여기 아이템은 임시로 보여주기만 하는 아이템 슬롯일 뿐이다.
	// 실제로는 RefreshInven을 통해 인벤으로 들어가게 된다.
	struct SBonusBoxButton
	{
		CDnBonusBoxButton *m_pBonusBoxButton;
		CDnItem *m_pItem;
		CDnItemSlotButton *m_pItemSlotButton;
		int m_nBoxType;

		SBonusBoxButton()
			: m_pBonusBoxButton(NULL)
			, m_pItem(NULL)
			, m_pItemSlotButton(NULL)
			, m_nBoxType(0)
		{
		}

		~SBonusBoxButton()
		{
			SAFE_DELETE( m_pItem );
		}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		void SetBoxType(int nBoxType)
		{
			m_nBoxType = nBoxType;
		}

		void SetBoxItem(TItem &ItemInfo)
		{
			if( ItemInfo.nItemID > 0 )
			{
				SAFE_DELETE( m_pItem );

				m_pItem = GetItemTask().CreateItem( ItemInfo );

				m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
			}
			else
			{
				m_pItemSlotButton->SetBlankItem();
			}
		}
#else
		void SetBoxInfo(int nBoxType, TItem &ItemInfo)
		{
			m_nBoxType = nBoxType;

			if( ItemInfo.nItemID > 0 )
			{
				SAFE_DELETE( m_pItem );

				m_pItem = GetItemTask().CreateItem( ItemInfo );

				m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
			}
			else
			{
				m_pItemSlotButton->SetBlankItem();
			}
		}
#endif

		void Clear()
		{
			SAFE_DELETE( m_pItem );
			m_pItemSlotButton->ResetSlot();
		}
	};

	struct SStageClearList
	{
		CDnGaugeFace *m_pFace;
		CEtUITextureControl *m_pTextureRank;
		CEtUIStatic *m_pName;
		CEtUIStatic *m_pBase;
		SBonusBoxButton m_sBonusBoxButton[4];

		int m_nSessionID;
		int m_nBoxCount;

		SStageClearList()
			: m_pFace(NULL)
			, m_pTextureRank(NULL)
			, m_pName(NULL)
			, m_pBase(NULL)
			, m_nSessionID(0)
			, m_nBoxCount(0)
		{
		}

		void Show( bool bShow )
		{
			m_pFace->Show( bShow );
			m_pTextureRank->Show( bShow );
			m_pName->Show( bShow );
			m_pBase->Show( bShow );
		}

		void Clear()
		{
			m_nSessionID = 0;
			m_nBoxCount = 0;
		}
	};

	struct SoundType 
	{
		enum 
		{
			RECOMPENSE_WINDOW =0,
			BOX_MOUSEON,			//  보상상자에 마우스 오버
			BOX_CLICK,				//  보상상자에 클릭
			BOX_OPEN,				//  보상상자 열렸을때
			MAX
		};
	};

	enum ePhase
	{
		PHASE_NONE,
		PHASE_CLOSED_BOX,
		PHASE_OPEN_BOX
	};

	enum
	{
		REWARDIDENTITY_ITEMID = 1107312640,
	};

	enum{
		PLAYER_UI_NUMBER = 4,
	};

public:
	CDnStageClearDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStageClearDlg(void);

protected:
#if defined( PRE_ADD_CP_RANK )
	CDnStageClearCPRankDlg * m_pStageClearCPRankDlg;
#endif	// #if defined( PRE_ADD_CP_RANK )

	EtTextureHandle m_hRankTexture;

	CDnGaugeFace *m_pFace;
	CEtUITextureControl *m_pTextureRank;
	CEtUIStatic *m_pName;
	SBonusBoxButton m_sBonusBoxButton[4];
	CDnItemSlotButton *m_pIdentifySlot;
	CEtUIStatic *m_pIdentifySlotBase;
	CEtUIStatic *m_pIdentifyMsg;
	CEtUIStatic *m_pIdentifyBack;
	CEtUIStatic *m_pStaticRemainSelectBox;
	CEtUIStatic *m_pStaticSelectBoxNotice;
	CDnItem *m_pIdentifyItem;
	bool m_bRequestIdentify;
	int m_nBoxCount;

	CEtUIStatic *m_pTime;
	CEtUIStatic *m_pTimeBase;

#if defined( PRE_ADD_CP_RANK )
	CEtUIStatic * m_pBestPlayer[PLAYER_UI_NUMBER];
	CEtUIStatic * m_pNewRecord[PLAYER_UI_NUMBER];

	SUICoord m_pBestPlayer_UICoord[PLAYER_UI_NUMBER];
	SUICoord m_pNewRecord_UICoord[PLAYER_UI_NUMBER];
#endif	// #if defined( PRE_ADD_CP_RANK )

	std::vector<SStageClearList> m_vecStageClearList;

	float m_fElapsedTime;
	int m_nCount;
	bool m_bTimer;
	int m_nSeletBonusBoxIndex;
	int m_nSelectRemainCount;
	int m_nCurPhase;

	int	m_nSoundIndexArray[SoundType::MAX];

	int		m_CurBoxIdxPresented;
	float	m_fPhaseTime;
	ePhase	m_Phase;

protected:
	void SetClearInfo();
	void CheckIdentifyItem();
#if defined ( PRE_ADD_NAMEDITEM_SYSTEM )
	void SetRewardBoxOrRewardItemInfo(bool bBoxInfo);
#else
	void SetRewardItemInfo();
#endif

#if defined( PRE_ADD_CP_RANK )
	void ResetCPRank();
	void SetCPRank( std::vector<CDnDungeonClearImp::SDungeonClearInfo> & vecDungeonClearInfo );
	void GetNewRecordPlayer( CDnDungeonClearImp::SDungeonClearInfo & sDungeonClearInfo );
	void SetCPRankUI( const int nIndex, bool bNewRecord, bool bBestPlayer );
	bool IsAbyssMinLevel( const int nLevel );
#endif	// #if defined( PRE_ADD_CP_RANK )

public:
	void SetAllQuestionBox();
	void SetAllBonusBox();
	void SetAllOpenBonusBox();

	void SetClosedBonusBox(int idx);
	void SetOpenBonusBox(int idx);
	void SetAllPartyBonusBox();
	void SetAllPartyOpenBonusBox();

	void SelectBonusBox( int nSessionID, char cSelectIndex );
	void OpenBonusBox();
	void OpenBonusBox( char cSlotIndex, char cBoxType );
	void OpenBonusItem();
	void SetMyPortraitTexture( EtTextureHandle hTexture );

	void ResetCount() { m_nCount = 0; }

#if defined ( PRE_ADD_NAMEDITEM_SYSTEM )
	void ShowBonusBox();
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	virtual void OnBlindClose();
	virtual void OnBlindClosed();
};
