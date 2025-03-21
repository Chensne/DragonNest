#pragma once


#include "IDnCutSceneDataReader.h"



// 커맨드를 받는 인터페이스 정의
class ICmdReceiver
{
protected:
	

public:
	ICmdReceiver(void);
	virtual ~ICmdReceiver(void);

	virtual void SetModified( bool bModified ) = 0;
	virtual bool GetModified( void ) = 0;

	virtual bool RegisterResource(  const wxChar* pResName, int iResourceKind  ) = 0;
	virtual bool UnRegisterResource( const wxChar* pResName, int iResourceKind ) = 0;

	virtual bool SetMonsterTableID( const wxChar* pActorName, int iMonsterTableID ) = 0;
	virtual int GetMonsterTableID( const wxChar* pActorName ) = 0;

	// 신 전체 속성 관련
	virtual const SceneInfo* GetSceneInfo( void ) = 0;
	virtual bool ModifySceneInfo( SceneInfo* pSceneInfo ) = 0;

	virtual bool InsertAction( ActionInfo* pActionInfo ) = 0;
	virtual bool RemoveAction( int iID ) = 0;
	virtual bool ModifyActionInfo( ActionInfo& Action ) = 0;

	virtual bool InsertKey( KeyInfo* pKeyInfo ) = 0;
	virtual bool RemoveKey( int iID ) = 0;
	virtual bool ModifyKeyInfo( KeyInfo& Key ) = 0;

	virtual bool InsertEvent( EventInfo* pEventInfo ) = 0;
	virtual bool RemoveEvent( int iID ) = 0;
	virtual bool ModifyEventInfo( EventInfo* pEventInfo ) = 0;

	virtual int GetSelectedRegResIndex( void ) = 0;

	virtual int GenerateID( void ) = 0;
	virtual int GetNumActors( void ) = 0;
	virtual void GetActorNameByIndex( int iActorIndex, /*IN OUT*/ wxString& strActorName ) = 0;

	virtual const ActionInfo* GetActionInfoByID( int iID ) = 0;
	virtual int GetThisActorsActionNum( const wxChar* pActorName ) = 0;
	virtual const ActionInfo* GetThisActorsActionInfoByIndex( const wxChar* pActorName, int iActionIndex ) = 0;
	virtual bool GetThisActorsInfluenceLightmap( const wxChar* pActorName ) = 0;
	virtual bool GetThisActorsScaleLock( const wxChar* pActorName ) = 0;
	virtual bool SetActorsScaleLock( const wxChar* pActorName, bool bScaleLock ) = 0;

	//virtual void AddPropInfo( const S_PROP_INFO& PropInfo ) = 0;
	//virtual int GetNumActionProp( void ) = 0;
	//virtual const S_PROP_INFO* GetPropInfo( int iIndex ) = 0;
	//virtual void ClearPropInfo( void ) = 0;


	virtual const KeyInfo* GetKeyInfoByID( int iID ) = 0;
	virtual int GetThisActorsKeyNum( const wxChar* pActorName ) = 0;
	virtual const KeyInfo* GetThisActorsKeyInfoByIndex( const wxChar* pActorName, int iKeyIndex ) = 0;
	
	virtual int GetThisActorsAnimationNum( const wxChar* pActorName ) = 0;
	virtual const ActionEleInfo* GetThisActorsAnimation( const wxChar* pActorName, int iAnimationIndex ) = 0;

	virtual bool GetThisActorsFitYPosToMap( const wxChar* pActorName ) = 0;
	virtual bool SetActorsFitYPosToMap( const wxChar* pActorName, bool bFitYPosToMap ) = 0;
	virtual bool SetActorsInfluenceLightmap( const wxChar* pActorName, bool bInfluenceLightmap ) = 0;

	virtual const EventInfo* GetEventInfoByID( int iID ) = 0;
	virtual int GetThisTypesEventNum( int iEventType ) = 0;
	virtual const EventInfo* GetEventInfoByIndex( int iEventType, int iIndex ) = 0;

	virtual const EtVector3& GetRegResPos( const wxChar* pResName ) = 0;
	virtual float GetRegResRot( const wxChar* pResName ) = 0;
	virtual void GetRegResNameByIndex( int iIndex, /*IN OUT*/ wxString& wxResName ) = 0;
	virtual bool ChangeRegResName( const wxChar* pOriResName, const wxChar* pNewResName ) = 0;
	virtual bool AddActorsAnimation( const wxChar* pActorName, ActionEleInfo* pAnimationElement ) = 0;
	virtual bool ClearActorsAnimations( const wxChar* pActorName ) = 0;
	virtual bool SetRegResPos( const wxChar* pResName, EtVector3& vPos ) = 0;
	virtual bool SetRegResRot( const wxChar* pResName, float fRot ) = 0;
};
