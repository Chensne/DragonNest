#pragma once

#include "ActionBase.h"

#define _DIRECT_NEXT_ACTION_NAME

class CActionElement : public CActionBase {
public:
	CActionElement();
	virtual ~CActionElement();

protected:
	CString m_szLinkAniName;
	DWORD m_dwLength;
	CString m_szNextActionName;
	DWORD m_dwBlendFrame;
	DWORD m_dwNextActionFrame;

	DWORD dwUnkAct2;
	DWORD dwUnkAct3;
	bool bUnkAct4;
	DWORD dwUnkAct5;

	CString m_szAniNameTemp; 
	CString m_szActionNameTemp;
	bool m_bLoop;

	bool IsOverlapOtherChild( DWORD dwChildIndex );
#ifdef _DIRECT_NEXT_ACTION_NAME
	void SetDirectActionName(bool bSet);
#endif

public:
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );

	virtual bool ExportObject( FILE *fp, int &nCount );
	virtual bool ImportObject( FILE *fp , int nVersion);
	bool CheckImportLinkAni( FILE *fp , int nVersion);
#ifdef _DIRECT_NEXT_ACTION_NAME
	bool ImportForCheck(FILE *fp , int nVersion);
#endif

	CString GetLinkAniName() { return m_szLinkAniName; }
	DWORD GetLength() { return m_dwLength; }
	CString GetNextActionName() { return m_szNextActionName; }
	DWORD GetBlendFrame() { return m_dwBlendFrame; }
	DWORD GetNextActionFrame() { return m_dwNextActionFrame; }

	void ChangeNextActionName( const char *szPrevName, const char *szNewName );

	bool IsCanPlay();
	void SetPlay();
	void Play( int nLoopCount = 1, float fFrame = 0.f );
	bool IsStop();
	bool IsPause();
	void Stop();
	void Pause();
	void Resume();
	void SetCurFrame( float fFrame );
	float GetCurFrame();
	float GetPrevFrame();

	void RequeueSignal( int nSourceLength, int nModifyLength, int nType );

	bool IsLoop();
	void SetLoop( bool bLoop );
	CActionElement &operator = ( CActionElement &e );

	void ReloadSCR();	// �� �׼ǿ� ����ִ� SCR���� �������ؼ�, �ٽ� �ε��� �� �ְ� �Ѵ�.
};