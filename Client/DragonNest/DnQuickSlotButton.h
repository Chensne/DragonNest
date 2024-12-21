#pragma once
#include "DnSlotButton.h"

class CDnQuickSlotButton : public CDnSlotButton
{
	enum
	{
		typeItemOutline,
		typeSkillOutline,
		typeItemBlack,
		typeItemRed,
		typeSkillBlack,
		typeSkillRed,
		typeSkillToggle01,
		typeSkillToggle02,
		typeIconBeam,
		typeItemCount,
		typeSymbolOutline,
		typeItemDisable,
		typeItemNew,
		typeItemUnusable,
		typeItemOutlineC,
		typeItemOutlineB,
		typeItemOutlineA,
		typeItemOutlineS,
		typeItemOutlineR,
		typeRepair01,
		typeRepair02,
		typeRepair03,
		typeMiscItemOutlineC,	// 잡템 테두리.
		typeMiscItemOutlineB,
		typeMiscItemOutlineA,
		typeMiscItemOutlineS,
		typeMiscItemOutlineR,
		typeMiscItemOutlineN,
		typeSkillPassive,		// 스킬 쿨타임동안엔 붉은색으로 렌더링. 파일명이 Passive라 이름이 이렇다.
		typeItemCashMark,
		typeSkillOutlineDisabled,	// 얻지 못한 스킬에서 사용할 회색 아웃 라인.
		typeItemButton_Seal,
		typeItemOutlineLG,
		typeCooltimeCount,
		typeActiveSkill,
	};

public:
	CDnQuickSlotButton( CEtUIDialog *pParent );
	virtual ~CDnQuickSlotButton(void);

protected:
	int m_nButtonOrder;
	int m_nItemCondition;
	int m_nTabIndex;		// Note : 버튼이 속한 QuickSlotDlg의 탭 인덱스

	int m_nTypeOutline;
	int m_nTypeBlack;
	int m_nTypeRed;

	SUICoord m_ShortCutUV;
	SUICoord m_ShortCutCoord;
	SUICoord m_IconUVCoord;

	std::vector<EtTextureHandle> m_hVecTextureList;
	
	EtTextureHandle m_hCurIcon;
	EtTextureHandle m_hCurIconLight;

	EtTextureHandle m_hShortCutIcon;
	EtTextureHandle m_hSkillIconLight;
	EtTextureHandle m_hItemIconLight;

	// 퀵슬롯 사용 연출 아예 빼는거면 MainBar의 SkillOnEffect.dds도 빼야한다.
#define _QUICKSLOT_USE_EFFECT
#ifdef _QUICKSLOT_USE_EFFECT
	EtTextureHandle m_hQuickSlotUse;
#endif
	
	float m_fItemCoolTime;
	float m_fPrevCoolTime;
	float m_fCompleteCoolTime;
	float m_fConst;
	float m_fProgressSize;
	float m_fToggleTime;

#ifdef _QUICKSLOT_USE_EFFECT
	float m_fSizeConst;
	float m_fSizeTime;
	float m_fSizeReadyTime;
	float m_fSizeRate;
	float m_fSizeAlpha;
	SUICoord m_uiDefaultCoord;
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	int m_nEndBlinkTime;
	int m_nProcessBlinkTime;
#endif 

	EtColor m_CompleteCoolTimeColor;

	SUIElement *m_pElementToggle1;
	SUIElement *m_pElementToggle2;
	UI_CONTROL_STATE m_uiControlState1;
	UI_CONTROL_STATE m_uiControlState2;
	bool m_bToggleBegin;

	bool m_bForceUsable;
	bool m_bForceUnUsable;
	int m_iRootJobID;

	bool m_bDragFlag;

protected:
	void RenderToggle( float fElapsedTime );
	
public:
	void SetButtonOrder( int nOrder );
	void SetTabIndex( int nIndex );
	int GetTabIndex() { return m_nTabIndex; }
	void SetQuickItem( MIInventoryItem *pItem );
	int GetItemSlotIndex();

	void SetCompleteCoolTime( float fTime );
#ifdef _QUICKSLOT_USE_EFFECT
	void SetStartCoolTime( float fReadyTime, float fTime );
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	void EndBlinkTime( int nTime )	{ m_nEndBlinkTime = nTime; }
#endif

	virtual void SetHotKey( int nKey );
	int	GetItemCondition() const { return m_nItemCondition; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
	virtual void RenderDrag( float fElapsedTime );
	//virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );

	void SetForceUsable( bool bForceUsable ) { m_bForceUsable = bForceUsable; if( bForceUsable ) m_bForceUnUsable = false; }
	void SetForceUnUsable( bool bForceUnUsable ) { m_bForceUnUsable = bForceUnUsable; if( bForceUnUsable ) m_bForceUsable = false; }

private:
#ifdef PRE_ADD_SKILLCOOLTIME
	void RenderCoolTimeCounter(const float& fElapsedTime, const float& fMaxCoolTime, const float& fPresentCoolTime);
#endif
};
