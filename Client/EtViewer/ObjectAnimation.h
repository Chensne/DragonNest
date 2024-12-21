#pragma once

#include "ObjectBase.h"

class CObjectAnimation : public CObjectBase
{
public:
	CObjectAnimation();
	virtual ~CObjectAnimation();

protected:
	CString m_szFileName;
	bool m_bParentTypeSkin;
	int m_nTotalAni;
	int m_nCurAni;
	int m_nBoneCount;
	std::vector<int> m_nVecTotalFrame;
	std::vector<CString> m_szVecAniName;

	int m_nLoadedCurAni;
	float m_fLoadedCurFrame;

	void ChangeTreeName();
public:
	virtual void Activate();
	virtual void Process( LOCAL_TIME LocalTime );
	virtual CString GetClassName() { return CString("Animation"); }

	virtual void ExportObject( FILE *fp, int &nCount );
	virtual void ImportObject( FILE *fp );

	bool LoadAnimation( CString &szFileName );
	void RemoveAnimation();
	void SetTreeName( CString &szStr );

	void Play( int nIndex, int nLoopCount = 1, float fFrame = 0.f );
	void Stop();
	void Pause();
	void Resume();
	bool IsPlay();
	bool IsPause();
	void SetLoopCount( int nLoopCount );

	float GetCurFrame();
	void SetCurFrame( float fFrame );
	int GetTotalFrame();

	void LinkAniInfo( bool bEnable );

	int GetCurAniIndex() { return m_nCurAni; }
	int GetTotalAniIndex() { return m_nTotalAni; }

	CString GetFileName() { return m_szFileName; }
	virtual void GetChildFileName( std::vector<CString> &szVecResult );
	bool IsParentTypeSkin() { return m_bParentTypeSkin; }

	int GetLoadedCurAniIndex() { return m_nLoadedCurAni; }
	float GetLoadedCurFrame() { return m_fLoadedCurFrame; }

};