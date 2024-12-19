#pragma once

#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnActorState.h"
#include "DnItem.h"
#include "DnWeapon.h"
#include "DnUnknownRenderObject.h"
#include "DnActionSignalImp.h"

class CEtSoundChannel;

class CDnWeapon : public CDnUnknownRenderObject< CDnWeapon >, 
				  //public CDnActionBase, 
				  public CDnRenderBase, 
				  public CDnItem, 
				  public CDnActionSignalImp
{
public:
	CDnWeapon( bool bProcess = false, bool bIncreaseUniqueID = true );
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
		//Hp_Delta,
		Count,
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
		Scimiter,
		Dagger,
		Crook,
		Spear,
		LE_UNK,
		Bracelet,
		KnuckleGear,
		MC_UNK,	
		Claw,				
		EquipTypeEnum_Amount,
		EquipSlot_Amount = 2,
	};
//#pragma message("     --->DNWeapon mc/le unk fix")
	struct EffectStruct {
		char cType; // -1 : None, 0 : Particle, 1 : FX
		int nIndex;
		DnEtcHandle hEtcObject;

		EffectStruct() { cType = -1; nIndex = -1; hEtcObject = CDnEtcObject::Identity(); }
		EffectStruct( char Type, int Index ) { cType = Type; nIndex = Index; }
	};

	static DWORD s_dwUniqueCount;
protected:
	DWORD m_dwUniqueID;
	WeaponTypeEnum m_WeaponType;
	EquipTypeEnum m_EquipType;
	DnActorHandle m_hActor;
	CDnWeapon *m_pParentWeapon;
	int m_nEquipIndex;
	EtAniObjectHandle m_hObject;
	EtMatrix m_matWorld;
	int m_nLength;
	int m_nOriginalLength;
	int m_nDurability;
	int m_nMaxDurability;
	int m_nDurabilityRepairCoin;
	bool m_bLinkBone;
	bool m_bDestroyThis;
	int m_nLinkBoneIndex;
	int m_nEnchantActionValue;

	EtSwordTrailHandle m_hSwordTrail;
	AttachSwordTrailStruct *m_pSwordTrailStruct;

	EffectStruct m_NormalHitEffect[CDnActorState::ActorMaterialEnum_Amount];
	EffectStruct m_CriticalHitEffect[CDnActorState::ActorMaterialEnum_Amount];
	EffectStruct m_StunHitEffect[CDnActorState::ActorMaterialEnum_Amount];
	EffectStruct m_DefenseEffect;
	EffectStruct m_FinishEffect;

	std::vector<int> m_nVecNormalHitSound[CDnActorState::ActorMaterialEnum_Amount];
	std::vector<int> m_nVecCriticalHitSound;
	std::vector<int> m_nVecStunHitSound;
	std::vector<int> m_nVecDefenseSound;
	std::vector<int> m_nVecFinishSound;
	float m_fSoundRange;
	float m_fRollOff;

	int m_nAniIndex;
	int m_nBlendAniIndex;
	float m_fBlendStartFrame;
	float m_fBlendFrame;
	float m_fBlendAniFrame;
	bool m_bShow;
	int m_nSetItemID;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool m_bOverLapSet;
#endif
	bool m_bSimpleSetItemDescription;
	int m_nSimpleSetItemDescriptionID;
	ElementEnum m_eElement;
	bool m_bCreateObject;

	bool m_bOneType;

	int m_nEmptyDurabilitySoundIndex;

	static std::map<DWORD, DnWeaponHandle> s_dwMapUniqueSearch;
	static CSyncLock s_LockSearchMap;

	MatrixEx m_ObjectCross;

	EquipTypeEnum m_RecreateCashEquipType;
	std::string m_szDefaultAction; // 기타 이펙트 무기액션이 붙기이전의 디폴트 값을 저장해둔다.

protected:
	void LoadEffect( const char *szFileName, const char *szActName, EffectStruct *pStruct );
	void FreeEffect( EffectStruct *pStruct );
	void LoadSound( const char *szFileName, std::vector<int> &nVecResult );
	void FreeSound( std::vector<int> &nVecList );

	static void InsertUniqueSearchMap( CDnWeapon *pWeapon );
	static void RemoveUniqueSearchMap( CDnWeapon *pWeapon );

public:
	// CDnWeapon
	bool Initialize( int nWeaponTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE);
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetUniqueID( DWORD dwUniqueID );
	DWORD GetUniqueID() { return m_dwUniqueID; }

	void SetWeaponType( WeaponTypeEnum Type ) { m_WeaponType = Type; }
	WeaponTypeEnum GetWeaponType() { return m_WeaponType; }

	EquipTypeEnum GetEquipType() const { return m_EquipType; }

	int GetEnchantActionValue() { return m_nEnchantActionValue; }

	void Update( EtMatrix *matWorld );
	EtMatrix *GetMatrix() { return &m_matWorld; }

	void SetWeaponLength( int nValue ) { m_nLength = nValue; }
	int GetWeaponLength() { return m_nLength; }

	int GetOriginalWeaponLength() { return m_nOriginalLength; }

	EtAniObjectHandle GetObjectHandle() { return m_hObject; }

	void LinkWeapon( DnActorHandle hActor, int nEquipIndex, const char *szBoneName = NULL );
	void LinkWeapon( DnActorHandle hActor, CDnWeapon *pWeapon );
	void UnlinkWeapon();
	void SetLinkActor( DnActorHandle hActor ) { m_hActor = hActor; }
	DnActorHandle GetLinkActor() { return m_hActor; }
	void ChangeLinkBone( const char *szBoneName );
	int GetLinkBoneIndex() { return m_nLinkBoneIndex; }
	void ForceSetEquipIndex( int nEquipIndex ) { m_nEquipIndex = nEquipIndex; }	// 칼리 왼손무기 때문에 만든 함수.
	
	void ShowWeapon( bool bShow );
	void ShowTrail( LOCAL_TIME nLengthTime, const ShowSwordTrailStruct *pStruct );

	void CreateObject( CDnActor *pActor = NULL , bool bIgnoreSoundEffectForVillage= true );
	void FreeObject();

	void RecreateCashWeapon( CDnActor *pActor, int nEquipIndex );

	bool IsInfinityDurability() { return  ( m_nMaxDurability == 0 ) ? true : false; }
	int GetDurability() const { return m_nDurability; }
	void SetDurability( int nValue ) { m_nDurability = nValue; }
	void SetMaxDurability( int nValue ) { m_nMaxDurability = nValue; }
	int GetMaxDurability() { return m_nMaxDurability; }
	int GetDurabilityRepairCoin() { return m_nDurabilityRepairCoin; }

	bool IsCreateObject() { return m_bCreateObject; }
	// 현재 이 무기를 수리하는데 드는 비용
	float GetRepairPrice();

	void OnHitSuccess( DnActorHandle hTarget, HitTypeEnum HitType, EtVector3 &vHitPosition, bool bPlaySound = true );

	void OnEmptyDurability();

	int GetSetItemID() { return m_nSetItemID; }
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	bool GetOverLapSet() { return m_bOverLapSet; }
#endif
	bool IsSimpleSetItemDescription() { return m_bSimpleSetItemDescription; }
	int GetSimpleSetItemDescriptionID() { return m_nSimpleSetItemDescriptionID; }
	ElementEnum GetElementType( void ) { return m_eElement; }

	bool IsOneType() { return m_bOneType; }

	const char *GetCurrentDefaultAction(){ return m_szDefaultAction.c_str(); }
	const char *GetWeaponMatchAction( const char *szAction );

	// CDnRenderBase 
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f );
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual int GetAniIndex( const char *szAniName );
	virtual int GetBoneIndex( const char *szBoneName );
	virtual int GetDummyBoneIndex( const char *szBoneName );
	virtual void ShowRenderBase( bool bShow );
	virtual bool IsShow() { return m_bShow; }
	virtual EtAniObjectHandle GetAniObjectHandle() { return m_hObject;}

	// CDnActionSignalImp
	virtual CDnRenderBase *GetRenderBase() { return this; }

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop = false );
	virtual void SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f );
	virtual bool IsExistAction( const char *szAction );

	void LoadEtcObject();
	void CreateBillboardEffect( int nIndex, MatrixEx &Cross );
	void CreateEffectObject( int nIndex, MatrixEx &Cross );
	void CreateEtcObject( DnEtcHandle hHandle, DnActorHandle hTarget, MatrixEx &Cross );

	// CDnActionSignalImp
	virtual MatrixEx *GetObjectCross() { m_ObjectCross = m_matWorld; return &m_ObjectCross; }
	void ResetDefaultAction( int nEquipIndex );

#ifdef PRE_FIX_BOW_ACTION_MISMATCH
	bool IsEnableNormalStandAction();
#endif

	CDnWeapon &operator = ( CDnWeapon &e );
	virtual CDnItem &operator = ( TItem &e );

	// Static 함수들
	static bool InitializeClass();
	static void ProcessClass( LOCAL_TIME LocalTime, float fDelta );
	static void ReleaseClass();
	static void ReleaseClass( WeaponTypeEnum Type );
	static DnWeaponHandle FindWeaponFromUniqueID( DWORD dwUniqueID );
	static DnWeaponHandle FindWeaponFromSerialID( INT64 nSerialID );
	static DnWeaponHandle CreateWeapon( int nWeaponTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, bool bProcess = false, bool bIncreaseUniqueCount = true, int nLookItemID = ITEMCLSID_NONE );
	static CDnWeapon::EquipTypeEnum GetEquipType(ITEMCLSID itemId);
	static bool IsSubWeapon( CDnWeapon::EquipTypeEnum emType );
};