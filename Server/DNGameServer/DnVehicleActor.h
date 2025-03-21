#pragma once

#include "DnActor.h"
#include "DnSimpleParts.h"
#include "MASingleBody.h"
#include "MAWalkMovement.h"
#include "DnPlayerActor.h"
#include "DnVehicleState.h"

class CDnVehicleActor : public CDnActor,  public CDnVehicleState, virtual public MASingleBody
{

public:
	CDnVehicleActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnVehicleActor();

	virtual MAMovementBase* CreateMovement();

	virtual bool Initialize();
	virtual bool IsVehicleActor() { return true; }
	virtual int GetMoveSpeed();
	virtual void OnDrop( float fCurVelocity );
	virtual void OnFall( float fCurVelocity );
	virtual void CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false );
	virtual int CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit = false, bool bCheckCanBegin = true , bool bEternity = false );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true,	bool bFromStateBlow = false, bool bSkillChain = false );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket );
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );
	virtual void ProcessDie( LOCAL_TIME LocalTime, float fDelta );
	virtual void SendRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	virtual void SetMagnetPosition( EtVector3 &vPos );

public:

	DWORD	GetSyncDatumGap() const { return GetTickCount()-m_dwSyncDatumTick; }
	DnActorHandle GetMyPlayerActor(){return m_hMyActor;}; // 나를 타고있는 플레이어를 알 필요가 있을때가 있습니다. 

	bool IsLocalVehicle(){return m_bIsLocalVehicle;} // 내가 로컬플레이어의 탈것인가요?
	UINT GetItemID(){return m_nItemID;}
	void SetItemID(UINT nID){m_nItemID = nID;}
	void SetMyPlayerActor(DnActorHandle hActor){m_hMyActor = hActor;};
	void SetAttachToPlayer(bool bAttach){m_bAttachToPlayer = bAttach;} 
	void SetSyncDatumTick(){ m_dwSyncDatumTick = GetTickCount(); }
	void ChangeHairColor(DWORD dwColor);
	void SetDefaultHairColor();
	void SetDefaultParts();
	void EquipItem(TItem tInfo);
	void UnEquipItem(int nSlot);
#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	std::string GetVehicleActionString(){return m_strVehicleActionString;}
	void SetVehicleActionString(const char *szVehicleActionString) { m_strVehicleActionString = szVehicleActionString; }
#endif
#ifdef PRE_ADD_VEHICLE_SPECIAL_ACTION
	bool CheckSpecialAction( const char *szAction );
#endif

protected:

	EtVector3 m_vLastValidPos;
	EtVector2 m_vPrevMoveVector;
	std::string m_szPrevMoveAction;
	LOCAL_TIME m_LastSendMoveMsg;

	UINT m_nItemID;
	bool m_bAttachToPlayer;     // 현재 플레이어에게 붙어있는가?
	bool m_bIsLocalVehicle;  
	bool m_bResetMoveMsg;
	char m_cMovePushKeyFlag;
#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	std::string m_strVehicleActionString;
#endif
	
	DWORD m_dwHairColor;
	// 스핵관련
	DWORD		m_dwSyncDatumTick;

	DnActorHandle m_hMyActor; // 탈것에 붙어있는 액터<플레이어>
	DnSimplePartsHandle m_hSimpleParts[2]; // 일단 두개넣고 추가 여부에따라서 추가해줍시다.

};
