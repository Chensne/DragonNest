#pragma once

#include "Timer.h"
#include "MAActorRenderBase.h"
#include "DnParts.h"

class MAPartsBody : virtual public MAActorRenderBase {
public:
	MAPartsBody();
	virtual ~MAPartsBody();

	enum PartsColorEnum {
		HairColor = 0,
		SkinColor = 1,
		EyeColor = 2,
		PartsColorAmount = 3,
	};

protected:
	DnPartsHandle m_hPartsObject[CDnParts::PartsTypeEnum_Amount];
	bool m_bSelfDelete[CDnParts::PartsTypeEnum_Amount];
	std::string m_szSkinName[CDnParts::PartsTypeEnum_Amount];

	DnPartsHandle m_hCashPartsObject[CDnParts::CashPartsTypeEnum_Amount];
	bool m_bCashSelfDelete[CDnParts::CashPartsTypeEnum_Amount];
	std::string m_szCashSkinName[CDnParts::CashPartsTypeEnum_Amount];

	// 무기를 제외한 전환버튼
	bool m_bPartsViewOrder[CDnParts::SwapParts_Amount];
	bool m_bHideHelmet;

	bool m_bIgnoreDeleteWaitOnepiece;	// 몇몇 상황에선 Onepiece 오브젝트를 DeleteWait에 안넣길 원한다.
	EtAniObjectHandle m_hDeleteWaitOnepieceCashPartsObject;

	bool m_bCombineParts[5];
	bool m_bCombineCashParts[5];

	DWORD m_dwPartsColor[PartsColorAmount];

	EtTextureHandle		m_hFaceDecoTexture;

	EtAniObjectHandle m_MergedObject;
	std::vector<EtSkinHandle>		m_NeedReleasePartsList;

	static CSyncLock s_PartsLock;

	volatile long m_nCombinePartsState;

	int m_nActorClassID;

	int m_nCurrentHairLevel;
	bool m_bIsShowPVPHelmet;


	CSyncLock m_PartsLock;

	bool m_bIgnoreCombine;

protected:
	virtual bool ChangeSocialTexture( int nSocialIndex, int nFrameIndex ) override;

	void UpdateFaceDeco();
	void RefreshHairLevel( int nLevel );

	void UpdateColorParam( PartsColorEnum ColorType );
	int GetHairLevel( DnPartsHandle hParts );

	int GetCurrentHairLevel(){return m_nCurrentHairLevel;};

public:
	void LoadSkin( const char *szDummySkinName, const char *szAniName );
	void FreeSkin();

	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
//	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	// 노말템
	virtual bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true);
	virtual bool DetachParts( CDnParts::PartsTypeEnum Index );

	DnPartsHandle GetParts( CDnParts::PartsTypeEnum Index ) { return m_hPartsObject[Index]; }
	std::string GetSkinName(CDnParts::PartsTypeEnum Index ) {return m_szSkinName[Index]; }

	// 캐쉬템
	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false, bool bUseCombine = true);
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );
	void LinkCashParts( CDnParts::PartsTypeEnum Index );

	DnPartsHandle GetCashParts( CDnParts::PartsTypeEnum Index ) { return m_hCashPartsObject[Index]; }
	std::string GetCashSkinName(CDnParts::PartsTypeEnum Index ) {return m_szCashSkinName[Index]; }

	// 기타
	void CombineParts();
	EtAniObjectHandle GetCombineObject() {return m_MergedObject;}
	void SetCombineObject(EtAniObjectHandle hObject){m_MergedObject = hObject;} 

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
	virtual void OnCombineParts() {}
	virtual void OnCombineParts( EtSkinHandle hSkin, CEtSkinInstance **ppSkinInstance ) { if( *ppSkinInstance ) *ppSkinInstance = NULL; }
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

	bool IsCombiningParts() { return ( m_nCombinePartsState != 0 ); }
	CSyncLock &GetMemberPartsLock() { return m_PartsLock; }

	void ShowParts( bool bShow );

	void SetPartsColor( PartsColorEnum ColorType, DWORD dwColor );
	DWORD GetPartsColor( PartsColorEnum ColorType ) { return m_dwPartsColor[ColorType]; }

	// 강제로 Face텍스처 셋팅해줘야하는 상황에서 호출.
	void ForceUpdateFaceDeco() { UpdateFaceDeco(); }

	void SetIgnoreDeleteWaitOnepiece( bool bFlag ) { m_bIgnoreDeleteWaitOnepiece = bFlag; }

	// 전환 스위치
	void RefreshPartsViewOrder( int nEquipIndex, bool bSkipCombine = false );
	void SetPartsViewOrder( int nEquipIndex, bool bShowCash ) { m_bPartsViewOrder[nEquipIndex] = bShowCash; }
	bool IsViewPartsOrder( int nEquipIndex ) { return m_bPartsViewOrder[nEquipIndex]; }
	void RefreshHideHelmet( bool bChangeParts = false );
	void SetHideHelmet( bool bHide ) { m_bHideHelmet = bHide; }
	bool IsHideHelmet() { return m_bHideHelmet; }

	void SetPVPShowHelmet(bool bTrue){m_bIsShowPVPHelmet = bTrue;};

	void SetIgnoreCombine( bool bValue ) { m_bIgnoreCombine = bValue; }
};
