#pragma once

#include "DnUnknownRenderObject.h"
#include "EtMatrixEx.h"
#include "DnBaseTransAction.h"
#include "TSmartPtrSignalImp.h"
#include "DnActionRenderBase.h"
#include "DnActionSignalImp.h"
#include "DnItem.h"

class NavigationCell;
class CDnDropItem : public CDnUnknownRenderObject<CDnDropItem>, 
					public CDnActionRenderBase,
					public CEtCustomRender, 
					public CDnBaseTransAction,
					public CDnActionSignalImp
{
public:
	CDnDropItem( DWORD dwUniqueID, bool bProcess );
	virtual ~CDnDropItem();

	enum DROPITEM_PROTOCOL {
		DPT_PICKUP = 0,
		DPT_NOPICKUP,
		DPT_PICKINSTANT,
	};

	static DWORD s_dwUniqueCount;

protected:
	static CEtOctree<DnDropItemHandle> *s_pOctree;
	static int s_nFontIndex;
	static std::vector<DnDropItemHandle> s_hVecPreLoadItemList;
	static bool s_bActive;
	static bool s_bShowToolTip;
	static bool s_bShowDropItems;
	static float s_fToolTipRange;
	static DnDropItemHandle s_hLastNearItem;
	float m_fDisappearTime;
	bool m_bDisappear;

	CEtOctreeNode<DnDropItemHandle> *m_pCurrentNode;

	int m_nItemID;
	tstring m_szItemName;
	int m_nOverlapCount;
	int m_nRandomSeed;
	char m_cOption;
	bool m_bShowToolTip;
	DWORD m_dwUniqueID;
	eItemRank m_Rank;
	DWORD m_dwItemNameColor;
	float m_fLifeTime;
	float m_fDistance;
	bool m_bFocus;
	DWORD m_dwOwnerUniqueID;

	NavigationCell *m_pCurCell;

	EtOutlineHandle m_hOutline;

protected:
	void ProcessToolTip();
	void ProcessAttributeMovement( float fDistance );
	void ProcessLifetime( LOCAL_TIME LocalTime, float fDelta );

	static void CalcCoinLump( int nCoin, int nMin, int nMax, std::vector<int> &nVecResult );
	static void CalcDropItems( int nDropItemTableID, std::vector<CDnItem::DropItemStruct> &VecResult, int &nDepth, bool bIncreaseUniqueID = true );

public:
	virtual bool Initialize( EtVector3 &vPos, int nItemID, int nRandomSeed, char cOption, int nCount, int nRotate, bool bPreInitialize = false );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void SyncClassTime( LOCAL_TIME LocalTime );
	virtual CDnRenderBase * GetRenderBase();

	int GetItemID() { return m_nItemID; }
	void ShowToolTip( bool bShow ) { m_bShowToolTip = bShow; }
	int GetOverlapCount() { return m_nOverlapCount; }

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	DWORD GetUniqueID() { return m_dwUniqueID; }

	void SetOwnerUniqueID( DWORD dwUniqueID ) { m_dwOwnerUniqueID = dwUniqueID; }
	DWORD GetOwnerUniqueID() { return m_dwOwnerUniqueID; };

	// Octree 를 위한..
	void InsertOctreeNode();
	void RemoveOctreeNode();

	virtual void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false );
	virtual void GetBoundingBox( SAABox &Box );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	// CEtCustomRender
	virtual void RenderCustom( float fElapsedTime );

	// CDnBaseTransAction
	virtual void Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	// CDnActionSignalImp
	virtual MatrixEx *GetObjectCross() { return &m_matExWorld; }

	void SetFocus( bool bFocus = true );
	// Static 함수들
	static bool IsActive() { return s_bActive; }
	static bool InitializeClass();
	static void ProcessClass( LOCAL_TIME LocalTime, float fDelta );
	static void ReleaseClass();

	static bool			PreInitializeItem( int nItemID );
	static CDnDropItem* DropItem( EtVector3 &vPos, DWORD dwUniqueID, int nItemID, int nSeed, char cOption, int nOverlapCount, int nRotate, DWORD dwOwnerUniqueID = -1 );
	static CDnDropItem* DropItem( const TDropItemSync& DropItem );
	// Range Scan 함수들
	static int ScanItem( EtVector3 &vPos, float fRadius, DNVector(DnDropItemHandle) &VecList );
	static DnDropItemHandle FindItemFromUniqueID( DWORD dwUniqueID );

	static void CalcDropItemList( int nItemDropTableID, std::vector<CDnItem::DropItemStruct> &VecList, bool bIncreaseUniqueID = true );
	static void CalcDropItemList( int nDifficulty, int nItemDropGroupTableID, std::vector<CDnItem::DropItemStruct> &VecList, bool bIncreaseUniqueID = true );

	static void ToggleToolTip();
	static bool IsShowToolTip();
	static void ShowDropItems( bool bShow );
	static bool IsShowDropItems();

	void AddCountString(std::wstring& resultString);
};