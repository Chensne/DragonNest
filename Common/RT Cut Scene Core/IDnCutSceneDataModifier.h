#pragma once

#include "IDnCutSceneDataReader.h"


// �ƽ� �����͸� ������ �� �ִ� �������̽� ����
class IDnCutSceneDataModifier
{
private:

public:
	IDnCutSceneDataModifier(void);
	virtual ~IDnCutSceneDataModifier(void);

	virtual bool RegisterResource( const char* pResName, int iResourceKind ) = 0;
	virtual bool UnRegisterResource( const char* pResName, int iResourceKind ) = 0;
	virtual bool SetMonsterTableID( const char* pActorName, int iMonsterTableID ) = 0;
	virtual DWORD GenerateID( void ) = 0;

	// �� ��ü �Ӽ� ���� ����..
	virtual bool ModifySceneInfo( SceneInfo* pSceneInfo ) = 0;
	
	// �׼� ����..
	virtual bool InsertAction( ActionInfo* pActionInfo ) = 0;
	virtual bool ModifyActionInfo( ActionInfo& Action ) = 0;
	virtual bool RemoveAction( int iID ) = 0;

	virtual bool InsertKey( KeyInfo* pKeyInfo ) = 0;
	virtual bool ModifyKeyInfo( KeyInfo& Key ) = 0;
	virtual bool RemoveKey( int iID ) = 0;

	// �̺�Ʈ ����..
	virtual bool InsertEvent( const EventInfo* pEventInfo ) = 0;
	virtual bool ModifyEventInfo( const EventInfo* pEventInfo ) = 0;
	virtual bool RemoveEvent( int iID ) = 0; 

	virtual bool ChangeRegResName( const char* pOriResName, const char* pNewResName ) = 0;
	virtual bool AddActorsAnimation( const char* pActorName, ActionEleInfo* pAnimationElement ) = 0;
	virtual bool SetActorsFitYPosToMap( const char* pActorName, bool bFitYPosToMap ) = 0;
	virtual bool SetActorsInfluenceLightmap( const char* pActorName, bool bInfluenceLightmap ) = 0;
	virtual bool SetActorsScaleLock( const char* pActorName, bool bScaleLock ) = 0;
	virtual bool ClearActorsAnimations( const char* pActorName ) = 0;
	virtual bool SetRegResPos( const char* pResName, EtVector3& vPos ) = 0;
	virtual bool SetRegResRot( const char* pResName, float fRot ) = 0;

	// ������ ����
	virtual bool SaveToFile( const char* pFileName ) = 0;

	// ������ Ŭ����
	virtual void Clear( void ) = 0;
};
