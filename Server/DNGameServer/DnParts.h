#pragma once

#include "DnUnknownRenderObject.h"
#include "DnActionBase.h"
#include "DnItem.h"

class MAPartsBody;

class CDnParts : public CDnUnknownRenderObject< CDnParts, MAX_SESSION_COUNT >, public CDnActionBase, public CDnItem, public TBoostMemoryPool< CDnParts >
{
public:
	CDnParts( CMultiRoom *pRoom );
	virtual ~CDnParts();

	enum PartsTypeEnum {
		Face = 0,
		Hair = 1,
		Helmet = 2,
		Body = 3,
		Leg = 4,
		Hand = 5,
		Foot = 6,
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
	std::string m_szSkinName;
	int m_nClassID;

	int m_nDurability;
	int m_nMaxDurability;
	int m_nSetItemID;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool m_bOverLapSet;
#endif

protected:

public:
	bool Initialize( int nPartsTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false );

	void SetActor( DnActorHandle hActor );
	int GetClassID() { return m_nClassID; }
	PartsTypeEnum GetPartsType() { return m_PartsType; }
	const char *GetSkinName( int nActorTableID );

	EtAniObjectHandle GetObjectHandle() { return m_hObject; }
	bool CreateObject( DnActorHandle hActor, const char *szAniName );
	bool FreeObject();

	EtAniObjectHandle GetObject() { return m_hObject; }
	void SetObject( EtAniObjectHandle hObject ) { m_hObject = hObject; }

	bool IsInfinityDurability() { return ( m_nMaxDurability == 0 ) ? true : false ; }
	int GetDurability() { return m_nDurability; }
	void SetDurability( int nValue ) { m_nDurability = nValue; }
	int GetMaxDurability() { return m_nMaxDurability; }

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	DWORD GetUniqueID() { return m_dwUniqueID; }
	int GetSetItemID() { return m_nSetItemID; }
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool GetOverLapSet() { return m_bOverLapSet; }
#endif

	static DnPartsHandle CreateParts( CMultiRoom *pRoom, int nPartsTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false);

	virtual CDnItem &operator = ( TItem &e );
};