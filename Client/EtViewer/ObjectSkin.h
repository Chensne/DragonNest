#pragma once

#include "ObjectBase.h"
#include "EtMatrixEx.h"

class CObjectAnimation;
class CObjectSkin : public CObjectBase
{
public:
	CObjectSkin();
	virtual ~CObjectSkin();

	static float s_fFramePerSec;
	enum eSkinType { SKIN_PARTS = 0, SKIN_WEAPON, SKIN_EARRING, SKIN_WING, SKIN_TAIL };

protected:
	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_AniTime;

	MatrixEx m_Cross;
	EtAniObjectHandle m_ObjectHandle;
	CString m_szSkinName;

	std::vector<CString> m_szVecAniList;
	std::vector<int> m_nVecAniOffset;
	int m_nAniIndex;
	int m_nLoopCount;
	float m_fCurFrame;
	bool m_bStop;
	bool m_bPause;
	bool m_bRegSubMesh;

	bool m_bLinkAniInfo;
	bool m_bShowDummy;

	EtVector3 m_vRotation;

	// Property ����
	DWORD m_dwVertexCount;
	std::vector<CString> m_szVecEffectList;

	char m_szSimFileName[MAX_PATH];

	eSkinType m_eSkinType;

	void RemakeSubmesh();
	void ReleaseSubmesh();

public:
	virtual void Activate();
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual CString GetClassName() { return CString("SkinObject"); }
	virtual void Show( bool bShow );

	virtual void ExportObject( FILE *fp, int &nCount );
	virtual void ImportObject( FILE *fp );

	void Play( const char *szFileName, int nAniIndex, int nLoopCount = 1, float fFrame = 0.f );
	void Stop();
	void Pause();
	void Resume();
	bool IsPlay() { return !m_bStop; }
	bool IsPause() { return m_bPause; }
	void SetLoopCount( int nLoopCount );

	float GetCurFrame() { return m_fCurFrame; }
	void SetCurFrame( float fFrame );

	void LoadSkin( const char *szFileName, bool bCreateObject );
	bool IsExistAnimation( const char *szFileName );
	bool QueueAnimation();
	EtAniObjectHandle GetObjectHandle() { return m_ObjectHandle; }

	bool LoadAnimation( const char *szFileName );
	void RemoveAnimation( const char *szFileName );

	void LoadSimulation( const char *szFileName );
	
	void LinkAniInfo( bool bEnable, CObjectAnimation *pChild );
	int GetChildAniIndex( const char *szFileName, int nAniIndex );
	int GetCurAniIndex() { return m_nAniIndex; }

	CString GetFileName() { return m_szSkinName; }

	void SaveSkin( CString szFileName );
	virtual void GetChildFileName( std::vector<CString> &szVecResult );

	void SetSkinType( eSkinType eSkinType ) { m_eSkinType = eSkinType; }
	eSkinType GetSkinType() { return m_eSkinType; }

	bool LinkAccessoryParts();
};