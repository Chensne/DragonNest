#pragma once

#include "DnUnknownRenderObject.h"
#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnItem.h"
#include "DnActionSignalImp.h"

class MAPartsBody;
class CDnParts : public CDnUnknownRenderObject< CDnParts >, 
				 public CDnActionSignalImp,
				 public CDnRenderBase, 
				 public CDnItem 
{
public:
	CDnParts();
	virtual ~CDnParts();

	enum PartsTypeEnum {
		Face = 0,
		Hair = 1,
		Helmet = 2,
		Body = 3,
		DefaultPartsType_Min = Body,
		Leg = 4,
		Hand = 5,
		Foot = 6,
		DefaultPartsType_Max = Foot,
		Necklace = 7,
		Earring = 8,
		Ring = 9,
		Ring2 = 10,

		CashHelmet = 0,
		CashBody = 1,
		CashLeg = 2,
		CashHand = 3,
		CashFoot = 4,
		CashNecklace = 5,
		CashEarring = 6,
		CashRing = 7,
		CashRing2 = 8,
		CashWing = 9,
		CashTail = 10,
		CashFaceDeco = 11,
		CashFairy = 12,

		DefaultPartsTypeEnum_Amount = 4,		// Body ~ Foot
		OverlapPartsTypeEnum_Amount = 5,		// Helmet ~ Foot
		PartsTypeEnum_Amount = 11,
		CashPartsTypeEnum_Amount = 13,
		AllParts_Amount = PartsTypeEnum_Amount + CashPartsTypeEnum_Amount,
		SwapParts_Amount = 12,
	};

protected:
	DnActorHandle m_hActor;
	EtAniObjectHandle m_hObject;
	DWORD m_dwUniqueID;

	PartsTypeEnum m_PartsType;
	bool m_bPermitPlayer[10];

	int m_nDurability;
	int m_nMaxDurability;
	int m_nDurabilityRepairCoin;

	int m_nEmptyDurabilitySoundIndex;
	int m_nSetItemID;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool m_bOverLapSet;
#endif
	bool m_bSimpleSetItemDescription;
	int m_nSimpleSetItemDescriptionID;

	bool m_bLinkBone;
	bool m_bExistAction;
	std::string m_szLinkDummyBoneName;

	EtMatrix m_matWorld;
	MatrixEx m_ObjectCross;
	int m_nAniIndex;
	int m_nBlendAniIndex;
	float m_fBlendStartFrame;
	float m_fBlendFrame;
	float m_fBlendAniFrame;
	bool m_bShow;
	bool m_bIgnoreCombine;

	std::vector<PartsTypeEnum> m_VecSubPartsList;
protected:

public:
	virtual bool Initialize( int nPartsTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = 0 );
	virtual bool CreateObject( DnActorHandle hActor, const char *szAniName );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetActor( DnActorHandle hActor );
	PartsTypeEnum GetPartsType() const { return m_PartsType; }
	void SetPartsType( PartsTypeEnum emPartsType ) { m_PartsType = emPartsType; }
	const char *GetSkinName( int nActorTableID );
	const char *GetDecalName( int nActorTableID );

	EtAniObjectHandle GetObjectHandle() { return m_hObject; }	
	virtual bool FreeObject();

	void SetObjectHandle( EtAniObjectHandle hObject ) { m_hObject = hObject; }

	bool IsInfinityDurability() { return  ( m_nMaxDurability == 0 ) ? true : false; }
	int GetDurability() const { return m_nDurability; }
	void SetDurability( int nValue ) { m_nDurability = nValue; }
	int GetMaxDurability() { return m_nMaxDurability; }
	int GetDurabilityRepairCoin() { return m_nDurabilityRepairCoin; }
	bool IsIgnoreCombine() { return m_bIgnoreCombine; }

	// 현재 이 파츠를 수리하는데 드는 비용
	float GetRepairPrice();

	void OnEmptyDurability();

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	DWORD GetUniqueID() { return m_dwUniqueID; }
	int GetSetItemID() { return m_nSetItemID; }
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool GetOverLapSet() { return m_bOverLapSet; }
#endif
	bool IsSimpleSetItemDescription() { return m_bSimpleSetItemDescription; }
	int GetSimpleSetItemDescriptionID() { return m_nSimpleSetItemDescriptionID; }

	virtual bool IsLoadPartsAni();

	// CDnRenderBase 
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f );
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual int GetAniIndex( const char *szAniName );
	virtual int GetBoneIndex( const char *szBoneName );
	virtual int GetDummyBoneIndex( const char *szBoneName );
	virtual void ShowRenderBase( bool bShow );
	virtual bool IsShow();
	virtual EtAniObjectHandle GetAniObjectHandle() { return m_hObject; }

	// CDnActionSignalImp
	virtual MatrixEx *GetObjectCross() { m_ObjectCross = m_matWorld; return &m_ObjectCross; }
	virtual CDnRenderBase *GetRenderBase() { return this; }

	static DnPartsHandle CreateParts( int nPartsTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );
	static PartsTypeEnum GetPartsType( int nPartsTableID );

	virtual CDnItem &operator = ( TItem &e );

	void UpdateSkinColorParam( DWORD dwSkinColor );
	void UpdateEyeColorParam( DWORD dwEyeColor );

	void LinkParts( DnActorHandle hActor, const char *szBoneName );
	void UnlinkParts();

	bool IsExistSubParts() { return !m_VecSubPartsList.empty(); }
	int GetSubPartsCount() { return (int)m_VecSubPartsList.size(); }
	PartsTypeEnum GetSubPartsIndex( int nIndex );

	static void ConvertFloatToR10G10B10( DWORD *dwOutColor, float *pSourceColor );
	static void ConvertR10G10B10ToFloat( float *pOutColor, DWORD dwSourceColor );
	static void ConvertD3DCOLORToR10G10B10( DWORD *pOutColor, D3DCOLOR dwSource, float fIntensity = 1.0f );
	static void ConvertInt3ToR10G10B10( DWORD *pOutColor, int *pSource );

#ifdef PRE_ADD_TRANSPARENCY_COSTUME
	static bool IsTransparentSkin( int nPartsTableID, int nActorTableID );
#endif	// #ifdef PRE_ADD_TRANSPARENCY_COSTUME
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	static bool IsAccessoryItem(PartsTypeEnum type, bool bIsCash);
#endif
};
