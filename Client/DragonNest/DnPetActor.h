#pragma once

#include "DnActor.h"
#include "DnSimpleParts.h"
#include "MASingleBody.h"
#include "MAWalkMovement.h"


#define PET_RANDOM_STAND_TIME	120.0f

enum ePetParts{ PET_PARTS_BODY = 0, PET_PARTS_NOSE };

class CDnPetActor : public CDnActor, public MASingleBody 
{
public:
	CDnPetActor( int nClassID, bool bProcess = true );
	virtual ~CDnPetActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAWalkMovement();
		return pMovement;
	}

	virtual bool Initialize();
	virtual bool LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha );
	virtual void RenderCustom( float fElapsedTime );		
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void SetPartsColor( EtAniObjectHandle hObject, char* Type, EtColor Color ); // 파츠에 색상을 지정합니다.
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce );
	virtual void CmdLook( EtVector2 &vVec, bool bForce );
	virtual bool IsPetActor() { return true; }

public:
	UINT GetItemID() { return m_nItemID; }
	void SetItemID(UINT nID) { m_nItemID = nID; }
	void SetDefaultParts();
//	void EquipItem( int nPartIndex );
	void EquipItem( TVehicleItem tInfo );
	void UnEquipItem( Pet::Parts::ePetParts Type );
	DnSimplePartsHandle GetPetParts( Vehicle::Parts::eVehicleParts Slot ) { return m_hSimpleParts[Slot]; }
	void SetMyMaster( CDnPlayerActor* pMaster ) { m_pMaster = pMaster; }
	CDnPlayerActor* GetMyMaster() { return m_pMaster; }
	void ChangeColor( ePetParts parts, DWORD dwColor );
	void SetPetInfo( TVehicleCompact VehicleInfo ) { m_sPetCompact = VehicleInfo; }
	TVehicleCompact& GetPetInfo() { return m_sPetCompact; }
	bool HaveAnyDoNotGetItem( const DNVector(DnDropItemHandle)& vecItemList );
	void AddNotEatItemList(const DWORD& dropItemUniqueID);
	bool IsNotEatItemList(const DWORD& dropItemUniqueID) const;
	void ClearNotEatItemList() { m_NotEatItemList.clear(); }
	void SetDefaultMesh();
	void DoPetGesture( int nGestureID );

protected:
	UINT					m_nItemID;
	DnSimplePartsHandle		m_hSimpleParts[2];
	float					m_fPetRandomStandTime;

	CDnPlayerActor*			m_pMaster;
	TVehicleCompact			m_sPetCompact;
	std::set<INT64>			m_DoNotGetItemList;	// 인벤 부족으로 얻지 못한 아이템 리스트
	std::set<INT64>			m_NotEatItemList;	// 가위바위보 취소로 포기한 아이템 리스트
};

