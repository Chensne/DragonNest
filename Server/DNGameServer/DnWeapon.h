#pragma once

#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnActorState.h"
#include "DnItem.h"
#include "DnUnknownRenderObject.h"

class CEtSoundChannel;

class CDnWeapon : public CDnUnknownRenderObject< CDnWeapon, MAX_SESSION_COUNT >, public CDnActionBase, public CDnRenderBase, public CDnItem
{
public:
	CDnWeapon( CMultiRoom *pRoom, bool bProcess = false, bool bIncreaseUniqueID = true );
	virtual ~CDnWeapon();

	enum WeaponTypeEnum {
		Unknown = 0x00000000,
		Weapon = 0x00000001,
		Projectile = 0x00000008,
	};

	enum HitTypeEnum {
		Normal,
		Critical,
		Stun,
		Defense,
		CriticalRes,
	};

	enum EquipTypeEnum {
		Sword,
		Gauntlet,
		Axe,
		Hammer,
		SmallBow,
		BigBow,
		CrossBow,
		Staff,
		Book,
		Orb,
		Puppet,
		Mace,
		Flail,
		Wand,
		Shield,
		Arrow,
		Cannon,
		BubbleGun,
		Glove,
		Fan,
		Chakram,
		Charm,
		EquipTypeEnum_Amount,
		EquipSlot_Amount = 2,
	};

	STATIC_DECL( DWORD s_dwUniqueCount );

private:
	DnActorHandle m_hActor;

protected:
	DWORD m_dwUniqueID;
	WeaponTypeEnum m_WeaponType;
	EquipTypeEnum m_EquipType;
	CDnWeapon *m_pParentWeapon;
	EtAniObjectHandle m_hObject;
	EtMatrix m_matWorld;
	int m_nClassID;
	int m_nLength;
	int m_nOriginalLength;
	int m_nDurability;
	int m_nMaxDurability;
	bool m_bLinkBone;
	bool m_bDestroyThis;
	bool m_bCreateObject;
	
	int m_nAniIndex;
	int m_nBlendAniIndex;
	float m_fBlendStartFrame;
	float m_fBlendFrame;
	float m_fBlendAniFrame;
	int m_nSetItemID;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool m_bOverLapSet;
#endif
	ElementEnum m_eElement;	// 아이템이 갖고 있는 속성

	EquipTypeEnum m_RecreateCashEquipType;
public:
	// CDnWeapon
	bool Initialize( int nWeaponTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	DnActorHandle GetActor() { return m_hActor; }

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	DWORD GetUniqueID() { return m_dwUniqueID; }

	void SetWeaponType( WeaponTypeEnum Type ) { m_WeaponType = Type; }
	WeaponTypeEnum GetWeaponType() { return m_WeaponType; }

	int GetClassID() { return m_nClassID; }
	EquipTypeEnum GetEquipType() { return m_EquipType; }

	void Update( EtMatrix *matWorld );
	EtMatrix *GetMatrix() { return &m_matWorld; }

	void SetWeaponLength( int nValue ) { m_nLength = nValue; }
	int GetWeaponLength() { return m_nLength; }
	int GetOriginalWeaponLength() { return m_nOriginalLength; }

	EtAniObjectHandle GetObjectHandle() { return m_hObject; }

	void LinkWeapon( DnActorHandle hActor, int nEquipIndex );
	void LinkWeapon( DnActorHandle hActor, CDnWeapon *pWeapon );
	void UnlinkWeapon();

	void ShowWeapon( bool bShow );

	void CreateObject();
	void FreeObject();
	bool IsCreateObject() { return m_bCreateObject; }
	void RecreateCashWeapon( DnActorHandle hActor, int nEquipIndex );

	bool IsInfinityDurability() { return ( m_nMaxDurability == 0 ) ? true : false ; }
	int GetDurability() { return m_nDurability; }
	void SetDurability( int nValue ) { m_nDurability = nValue; }
	int GetMaxDurability() { return m_nMaxDurability; }
	int GetSetItemID() { return m_nSetItemID; }
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool GetOverLapSet() { return m_bOverLapSet; }
#endif

	ElementEnum GetElementType( void ) { return m_eElement; }

	void OnHitSuccess( DnActorHandle hTarget, HitTypeEnum HitType, EtVector3 &vHitPosition, bool bPlaySound = true );

	// CDnRenderBase 
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f );
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual int GetAniIndex( const char *szAniName );
	virtual int GetBoneIndex( const char *szBoneName );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	CDnWeapon &operator = ( CDnWeapon &e );
	virtual CDnItem &operator = ( TItem &e );

	// Static 함수들
	static bool InitializeClass( CMultiRoom *pRoom );
	static void ProcessClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta );
	static void ReleaseClass( CMultiRoom *pRoom );
	static void ReleaseClass( CMultiRoom *pRoom, WeaponTypeEnum WeaponType );
	static DnWeaponHandle FindWeaponFromUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID );
	static DnWeaponHandle FindWeaponFromUniqueIDAndShooterUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID, DWORD dwShooterUniqueID );
	static DnWeaponHandle CreateWeapon( CMultiRoom *pRoom, int nWeaponTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, bool bProcess = false, bool bIncreaseUniqueID = true, char cSealCount = 0, bool bSoulBound = false );
	static bool IsSubWeapon( CDnWeapon::EquipTypeEnum emType );
};

class IBoostPoolDnWeapon:public CDnWeapon,public TBoostMemoryPool<IBoostPoolDnWeapon>
{
public:
	IBoostPoolDnWeapon( CMultiRoom *pRoom, bool bProcess = false, bool bIncreaseUniqueID = true ):CDnWeapon( pRoom, bProcess, bIncreaseUniqueID ){};
	virtual ~IBoostPoolDnWeapon(){}
};
