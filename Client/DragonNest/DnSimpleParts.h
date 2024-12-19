#pragma once

#include "DnUnknownRenderObject.h"
#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnItem.h"
#include "DnActionSignalImp.h"


// Rotha //  SimpleParts
// 애니메이션 파츠의 기본만 구성한 클레스 //
// PET 이나 , NPC , 탈것같은 Actor 에도 파츠를 간단히 붙일 수 있습니다. //


class CDnSimpleParts : public CDnUnknownRenderObject< CDnSimpleParts >, 
	public CDnActionSignalImp,
	public CDnRenderBase, 
	public CDnItem 
{
public:
	
	CDnSimpleParts();
	virtual ~CDnSimpleParts();
	
protected:
	
	DnActorHandle m_hActor;
	EtAniObjectHandle m_hObject;

	DWORD m_dwUniqueID;

	bool m_bShow;
	bool m_bLinkBone;
	bool m_bExistAction;

	int m_nAniIndex;
	int m_nBlendAniIndex;
	float m_fBlendStartFrame;
	float m_fBlendFrame;
	float m_fBlendAniFrame;

	EtMatrix m_matWorld;
	std::string m_szLinkDummyBoneName;
	MatrixEx m_ObjectCross;

	
protected:

public:
	
	virtual bool Initialize( int nPartsTableID , const char *szSkinName , const char *szAniName );
	virtual bool CreateObject( const char *szSkinName , const char *szAniName );
	static DnPartsHandle CreateParts( int nPartsTableID , const char *szSkinName , const char *szAniName );

	bool FreeObject();

	// CDnRenderBase 
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f );
 	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ShowRenderBase( bool bShow );
 	virtual int GetAniIndex( const char *szAniName );
 	virtual int GetBoneIndex( const char *szBoneName );
 	virtual int GetDummyBoneIndex( const char *szBoneName );
	virtual bool IsShow();
	 	
	virtual EtAniObjectHandle GetAniObjectHandle() { return m_hObject; }

	// CDnActionSignalImp
 	virtual MatrixEx *GetObjectCross() { m_ObjectCross = m_matWorld; return &m_ObjectCross; }

	void LinkPartsToBone( DnActorHandle hActor, const char *szBoneName );                                       // 이건 Bone에다가 붙입니다.
	void UnlinkParts();

	void LinkPartsToObject(const char *szSkinName , const char *szAniName , EtObjectHandle hPlayerObject); // 이건 애니와 결합해서 사용합니다.
	virtual void InitializeItem( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false);


	DWORD GetUniqueID() { return m_dwUniqueID; }
	const char *GetSkinName( int nActorTableID ); // 
	EtAniObjectHandle GetObjectHandle() { return m_hObject; }	

	void SetUniqueID( DWORD dwUniqueID ) { m_dwUniqueID = dwUniqueID; }
	void SetObjectHandle( EtAniObjectHandle hObject ) { m_hObject = hObject; }
	void SetActor( DnActorHandle hActor ){ m_hActor = hActor; }

};


