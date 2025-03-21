#pragma once

#include "DnUnknownRenderObject.h"
#include "DnRenderBase.h"
#include "DnActionBase.h"
#include "DnItem.h"


// Rotha //  SimpleParts
// 애니메이션 파츠의 기본만 구성한 클레스 - 랜더를 붙이는 목적으로 사용하기 위해 제작했습니다.  //
// PET 이나 , NPC , 탈것같은 Actor 에도 파츠를 간단히 붙일 수 있습니다. //



class CDnSimpleParts : public CDnUnknownRenderObject< CDnSimpleParts , MAX_SESSION_COUNT >, 
	public CDnActionBase , 
	public CDnItem ,
	public CDnRenderBase
{
public:
	
	CDnSimpleParts(CMultiRoom *pRoom);
	virtual ~CDnSimpleParts();
	
protected:
	
	DnActorHandle m_hActor;
	EtAniObjectHandle m_hObject;
	DWORD m_dwUniqueID;
	
protected:

public:
	
	virtual bool Initialize( int nPartsTableID , const char *szSkinName , const char *szAniName );
	virtual bool CreateObject( const char *szSkinName , const char *szAniName );

	static DnPartsHandle CreateParts( CMultiRoom *pRoom, int nPartsTableID , const char *szSkinName , const char *szAniName );

	virtual EtAniObjectHandle GetAniObjectHandle() { return m_hObject; }

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	DWORD GetUniqueID() { return m_dwUniqueID; }
	EtAniObjectHandle GetObjectHandle() { return m_hObject; }	
	
	void SetActor( DnActorHandle hActor ){ m_hActor = hActor; }
	void InitializeItem( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false);
	bool FreeObject();

};


