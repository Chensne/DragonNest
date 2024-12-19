#pragma once

#include "ActionBase.h"
#include "EtMatrixEx.h"

#define _CHECK_WALKFRONT

class CActionObject : public CActionBase {
public:
	CActionObject();
	virtual ~CActionObject();

	struct ActionHeader {
		char szHeaderString[32];
		int nVersion;
		int nActionCount;
	};

#ifdef _CHECK_WALKFRONT
	enum eCheckWalkFrontResult
	{
		eERROR_NOACTION,
		eOK_EXISTACTION,
		eOK_NONEED
	};
#endif
protected:
	// ����Ʈ ������ ���۷��� ī���� 
	EtAniObjectHandle m_ObjectHandle;

	
	std::vector<CString> m_szVecAniNameList;

	CString m_szSkinName;
	CString m_szAniName;
	CString m_szActName;
	CString m_szPath;

	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_AniTime;
	int m_nAniIndex;
	int m_nLoopCount;
	float m_fCurFrame;
	float m_fPrevFrame;
	bool m_bStop;
	bool m_bPause;
	MatrixEx m_matExWorld;
	int m_nTotalFrame;

	EtVector3 m_vVelocity;
	EtVector3 m_vResistance;

	// #48950 �̽� ������ Y ���� �̵���Ű�� �� �߰�.
	float m_fMoveYDistancePerSec;
	float m_fLeftMoveYDistance;
	float m_fOriginalYPos;
	//bool m_bAppliedYDistance;
	//bool m_bMaintainYDistanceOnArriveDestPosition;


	// #56216.	
	bool m_bRotate;	
	//EtMatrix m_AxisRotation;// ȸ����. 		
	EtVector3 m_vAxisPos; // ȸ������ǥ.
	float m_fSpeed;

protected:
	void ProcessVelocity( LOCAL_TIME LocalTime, float fDelta );

public:
//	virtual void Activate();
	virtual void Process( LOCAL_TIME LocalTime );
	virtual void InitPropertyInfo();
	virtual void OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );
	virtual void OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable );

	bool LoadSkin( const char *pFileName );
	bool LoadAni( const char *pFileName );
#ifdef _CHECK_WALKFRONT
	bool LoadAction( const char *pFileName, bool bCheckOnly = false);
#else
	bool LoadAction( const char *pFileName );
#endif
	bool CheckExistLinkAniAction( const char *pFileName );
	bool SaveAction();
	void SetPath( CString szStr );
	CString GetSkinName() { return m_szSkinName; }
#ifdef _CHECK_WALKFRONT
	eCheckWalkFrontResult CheckActionExistWithLoadFile(const std::string& fileName, const std::string& checkActionName);
	eCheckWalkFrontResult CheckActionExist(const std::vector<std::string>& checkFullPathArray, const std::string& checkActionName) const;
	bool IsActionExist(const std::string& checkActionName) const;
#endif

	EtAniObjectHandle GetObjectHandle() { return m_ObjectHandle; }

	int GetAniCount();
	CString GetAniName( int nAniIndex );
	int GetAniLength( CString szAniName );
	int GetAniIndex( CString szAniName );

	bool IsCanPlay( CString szAniName );

	void SetPlay( CString szAniName );
	void Play( CString szAniName, int nLoopCount = 1, float fFrame = 0.f );
	void Stop();
	void Pause();
	void Resume();
	bool IsStop();
	bool IsPause();
	void SetCurFrame( float fFrame );
	float GetCurFrame();
	float GetPrevFrame();
	void SetLoopCount( int nLoopCount );
	int GetLoopCount();
	void SetTotalFrame( int nFrame ) { m_nTotalFrame = nFrame; }

	MatrixEx *GetMatEx() { return &m_matExWorld; }
	EtVector3 GetAniDistance();

	void SetVelocity( EtVector3 &vVec ) { m_vVelocity = vVec; }
	void SetResistance( EtVector3 &vVec ) { m_vResistance = vVec; }
	EtVector3 *GetVelocity() { return &m_vVelocity; }
	EtVector3 *GetResistance() { return &m_vResistance; }

	void SetMoveYDistancePerSec( float fVelocityY, float fWholeMoveYDistance ) { m_fMoveYDistancePerSec = fVelocityY; m_fLeftMoveYDistance = fWholeMoveYDistance; };
	void ResetMoveYDistance( void ) { m_fMoveYDistancePerSec = 0.0f; m_fLeftMoveYDistance = 0.0f; m_fOriginalYPos = 0.0f; };


	// #56216.	
	void SetRotate( bool b ){
		m_bRotate = b;
	}
	void SetRotationSpeed( float fSpeed ){
		m_fSpeed = fSpeed;
	}
	void SetAxisPosition( EtVector3 & vPos );	
	void RotateObject( float fdelta );
	
};