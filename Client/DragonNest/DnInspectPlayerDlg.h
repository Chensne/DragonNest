#pragma once
#include "EtUIDialog.h"
#include "DnCustomDlg.h"
#include "DnWeapon.h"
#include "DnRenderAvatar.h"
#include "DnInspectPlateDlg.h"
#ifdef PRE_ADD_TALISMAN_SYSTEM
#include "DnInspectTalismanDlg.h"
#endif

class CDnInspectGuildInfoDlg;

class CDnInspectPlayerDlg : public CDnCustomDlg
{
public:
	// 살펴보기에서의 세트아이템 리스트 얻기 위한 장비인덱스, 해당 장비의 세트아이템인덱스
	struct SEquipInfo {
		int nClassID;
		int nSetItemID;
	};
public:
	CDnInspectPlayerDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnInspectPlayerDlg(void);

protected:	
	std::vector< CDnItemSlotButton * > m_vecSlotButton;
	std::vector< CDnItemSlotButton * > m_vecCashSlotButton;

	CDnItem* m_pSourceItem;
	CDnItemSlotButton* m_pSourceItemSlot;

	DWORD m_dwSessionID;

	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticLevel;
	CEtUIStatic *m_pStaticJob;
	CEtUIStatic *m_pStaticAppellation;
	CDnInspectPlateDlg	*m_pInspectPlateDlg;
#ifdef PRE_ADD_TALISMAN_SYSTEM
	CDnInspectTalismanDlg *m_pInspectTalismanDlg;
#endif

#ifdef PRE_ADD_COSTUME_SKILL
	CDnQuickSlotButton *m_pCostumeSkillSlotButton;
#endif

	float			m_fMouseX;
	float			m_fMouseY;	
	bool			m_bShowCashEquip;

	CEtUIButton	*m_pRotate;
	CEtUIButton	*m_pRotateLeft;
	CEtUIButton	*m_pRotateRight;

	CEtUIButton	*m_pRotateFront;
	CEtUIButton	*m_pRotateBack;	

	//blondy pvp 정보 표시 
	CEtUITextureControl	*m_pPVPIconTexCon;
	CEtUIStatic	*m_pPVPRankNameStCon;
	//blondy end

	std::vector<SEquipInfo> m_vecEquipInfo;

	CDnRenderAvatarNew m_RenderAvatar;

	typedef CDnCustomDlg	BaseClass;

	std::vector<CDnItem*> m_vecInspectItemList;

	CDnInspectGuildInfoDlg* m_pInspectGuildInfoDlg;


#ifdef PRE_ADD_BESTFRIEND
	INT64 m_BFserial;
	TCHAR * m_pBestfriendName;
#endif

protected:
	void RefreshOnepieceBlock();

public:
	void SetSessionID( DWORD dwSessionID );
	int GetSessionID();
	void RefreshItemSlots();
	void GetEquipInfo( std::vector<SEquipInfo> &vecEquipInfo );
	void SwapEquipButtons( bool bShowCashEquip );
	bool CheckValidActor();
	void SwapPlateDlg( bool bPlate );
	void ShowEquipModeUIControl( bool bShow );

	void SwapGuildInfoDlg(bool bShow);
	void ShowBaseItem(bool bShow);
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	void SwapTalismanDlg( bool bShow );
	CDnInspectTalismanDlg* GetInspectTalismanDlg() { return m_pInspectTalismanDlg; }
#endif

#ifdef PRE_ADD_BESTFRIEND
	INT64 GetBFserial() { return m_BFserial; }
	TCHAR * GetBestfriendName(){ return m_pBestfriendName; }
#endif

public:
	void InitCustomControl( CEtUIControl *pControl ) override;
	void InitialUpdate() override;
	void Initialize(bool bShow) override;
	void Show( bool bShow ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void Process( float fElapsedTime ) override;
	void Render( float fElapsedTime ) override;
};
