#pragma once
#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"
#include "DnRenderAvatar.h"
#include "DnLocalPlayerActor.h"
#include "DnCharPetPreviewDlg.h"

class CDnCashShopPreviewDlg : public CDnCustomDlg
{
public:
	CDnCashShopPreviewDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopPreviewDlg(void);

	virtual void		Initialize(bool bShow);
	virtual void		Show(bool bShow);

	bool				AttachParts(CASHITEM_SN sn, ITEMCLSID itemId, bool bCheckPackageCashRing2 = false);
	void				DetachPartsAll(int classId);
	void				DetachPartsWithItemId(int classId, ITEMCLSID itemId);
	void				DetachPartsWithSN(int classId, CASHITEM_SN itemId);
	void				ClearParts(int classId);
	eRetPreviewChange	ChangePreviewAvatar(int classId,bool bForce = false);

	void                ChangeVehicleParts(int nClassID = 0 ,int nPartClassID = 0, bool bDefault=false);
	void                SetVehicleDefaultParts();
	void				InitRenderAvatar();
	void				ResetRenderAvatar() { m_RenderAvatar.ResetActor(); }

protected:
	virtual void	InitialUpdate();
	virtual void	Render( float fElapsedTime );
	virtual void	Process( float fElapsedTime );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void			ToggleAction(bool bBattleMode);
	void			HoldRenderAvatar(bool bHold) { m_bHoldRender = bHold; }
	void			MakeAvatarToRender(int classId, bool bDefault);
	void			MakeDefaultParts();

	ITEMCLSID		GetDefaultPartsItemId(DNTableFileFormat* pSox, int classId, CDnParts::PartsTypeEnum partsType, int partsIndex) const;
	ITEMCLSID		GetBasicPartsItemId(DNTableFileFormat* pSox, int classId, CDnParts::PartsTypeEnum partsType, int partsIndex) const;
	DWORD			GetBasicPartsColor(DNTableFileFormat*  pDefaultSox, MAPartsBody::PartsColorEnum colorType, int nColorIndex, int nClassIndex) const;
	int				GetPartsIndex(int classId) const;

	void			AttachParts(ITEMCLSID itemId, bool bCashRing2 = false);

	CEtUIButton*	m_pReduceBtn;
	CEtUIButton*	m_pRotateLeftBtn;
	CEtUIButton*	m_pRotateRightBtn;
	CEtUIButton*	m_pRotateFrontBtn;
	CEtUIButton*	m_pRotateBackBtn;
	CEtUIButton*	m_pEnlargeBtn;

	CEtUIButton*	m_pBuyAllBtn;
	CEtUIButton*	m_pGiftAllBtn;
	CEtUIButton*	m_pRollbackBtn;
	CEtUIButton*	m_pSaveCoordi;

	CEtUIButton*	m_pDrawFrameBtn;

	CEtUIButton*	m_pNormalActionBtn;
	CEtUIButton*	m_pBattleActionBtn;
	bool			m_bHoldRender;

	//std::vector<DnPlayerActorHandle>	m_hVecCreateActorList;
	std::vector<DefaultPartsStruct>		m_VecCreateDefaultPartsList;
	std::vector<bool>					m_VecOnepieceList;
	std::map<CASHITEM_SN, ITEMCLSID>	m_IdMatchingList[CLASSKINDMAX+1];
	//CDnPlayerActor*						m_pCurrentActor;
	CDnRenderAvatarNew					m_RenderAvatar;

	float							m_fMouseX;
	float							m_fMouseY;
};

class CDnCashShopPetPreviewDlg : public CDnCharPetPreviewDlg
{
public:
	CDnCashShopPetPreviewDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCashShopPetPreviewDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	virtual void RefreshPetEquip();
	void SetPetAvatar( int nPetItemID );
	CDnActor* GetPetAvatar() { return m_hPet; }
	void SetPetAvatarParts( int nPartsType, int nPartsItemID );
	void SetPetAvatarColor( eItemTypeEnum eItemType, DWORD dwColor );
	void DetachPartsWithType( int nPartsSlot );
	void ResetBasePet();

private:
	CEtUIButton*	m_pBuyAllBtn;
	CEtUIButton*	m_pGiftAllBtn;
	CEtUIButton*	m_pRollbackBtn;

	DnActorHandle	m_hPet;
};
