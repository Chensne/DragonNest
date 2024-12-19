#pragma once

#include "Timer.h"
#include "MAActorRenderBase.h"
#include "DnParts.h"

class MAPartsBody : virtual public MAActorRenderBase {
public:
	MAPartsBody();
	virtual ~MAPartsBody();

protected:
	CDnActor *m_pActor;
	bool m_bIsValidActor;

	DnPartsHandle m_hPartsObject[CDnParts::PartsTypeEnum_Amount];
	bool m_bSelfDelete[CDnParts::PartsTypeEnum_Amount];

	DnPartsHandle m_hCashPartsObject[CDnParts::CashPartsTypeEnum_Amount];
	bool m_bCashSelfDelete[CDnParts::CashPartsTypeEnum_Amount];
	std::string m_szCashSkinName[CDnParts::CashPartsTypeEnum_Amount];

	// 무기를 제외한 전환버튼
	bool m_bPartsViewOrder[CDnParts::SwapParts_Amount];

protected:
	bool IsValidActor();

public:
	virtual void LoadSkin( const char *szDummySkinName, const char *szAniName );

	// 노말템
	bool AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	bool DetachParts( CDnParts::PartsTypeEnum Index );
	DnPartsHandle GetParts( CDnParts::PartsTypeEnum Index ) { return m_hPartsObject[Index]; }

	// 캐쉬템
	virtual bool AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete = false );
	virtual bool DetachCashParts( CDnParts::PartsTypeEnum Index );
	DnPartsHandle GetCashParts( CDnParts::PartsTypeEnum Index ) { return m_hCashPartsObject[Index]; }

	void RefreshPartsViewOrder( int nEquipIndex );
	void SetPartsViewOrder( int nEquipIndex, bool bShowCash ) { m_bPartsViewOrder[nEquipIndex] = bShowCash; }

	virtual bool bIsPartsBody(){ return true; }
};