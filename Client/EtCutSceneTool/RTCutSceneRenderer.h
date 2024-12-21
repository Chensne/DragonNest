#pragma once
#include "IRenderBase.h"
#include "ICommandListener.h"
#include "IDnCutSceneDataReader.h"
#include "DnCutSceneWeapon.h"


class CDnCutSceneWorld;
class CCamController;
class ICommand;
class CDnCutSceneActor;
class CDnCutScenePlayer;
class ICustomActorProcessor;
class CEtSoundEngine;
class CFade;
class CAxisRenderObject;
class CBlindCaptionDlg;


class CRTCutSceneRenderer : public IRenderBase
{
private:
	CEtSoundEngine*			m_pSoundEngine;

	CCamController*			m_pCamController;
	bool					m_bActivate;
	CDnCutScenePlayer*		m_pCutScenePlayer;

	ICustomActorProcessor*	m_pActorProcessor;

	LPD3DXSPRITE			m_pSprite;
	CFade*					m_pFade;

	int						m_iMode;

	//LOCAL_TIME				m_LocalTime;

	//MatrixEx			m_CameraCross;
	EtCameraHandle			m_hCamera;
	EtMatrix				m_matDefaultProj;
	CDnCutSceneWorld*		m_pWorld;

	vector<CDnCutSceneActor*> m_vlpActors;
	map<tstring, CDnCutSceneActor*> m_mapActors;
	
	tstring					m_strNowMapName;

	set<tstring>			m_setActors;

	EtVector3				m_avAxis[ 6 ];
	EtVector3				m_avAlphabetX[ 4 ];
	EtVector3				m_avAlphabetY[ 4 ];
	EtVector3				m_avAlphabetZ[ 6 ];
	
	bool					m_bCamOnPlay;
	bool					m_bShowSpotPos;

	CDnCutSceneActor*		m_pSelectedActor;	

	std::map<std::wstring, CDnCutSceneActor *> m_mapLockSelectedActor;	

	bool m_bLockSelect;

	int						m_iPrevClkXPos;
	int						m_iPrevClkYPos;

	// DIRECTX 메시..
	LPD3DXMESH				m_pSphereMesh;
	SPrimitiveDraw3D*		m_pSphereBuffer;
	vector<SPrimitiveDraw3D*> m_vlpBufferToDraw;
	DWORD					m_dwNumTriangle;

	KeyInfo					m_SelectedKeyInfo;	
	EtVector3*				m_pSelectedKeyPos;
	bool					m_bEditedKeyInfo;
	bool					m_bSelectedStartPos;
	int						m_iSelectedSubKey;

	int						m_iSelectedObjectType;

	// 파티클 select 관련
	EtBillboardEffectHandle	m_hSelectedParticleEvent;
	int						m_iSelectedParticleEventID;
	tstring					m_strSelectedParticleName;

	// 카메라 select 관련
	EtObjectHandle			m_hCameraMesh;
	bool					m_bShowMaxCameraPath;
	int						m_iSelectedCameraEventID;
	tstring					m_strSelectedCamEventName;
	EtVector3*				m_pSelectedMaxCamOffset;
	EtVector3				m_vOriMaxCamOffset;				// 편집 전의 원래 카메라 오프셋
	//CamEventInfo*			m_pSelectedCamEventInfo;

	// 렌더링 옵션 관련
	bool					m_bRenderFog;

	vector<string>			m_vlDefaultActionName;
	
	// 3d 축
	CAxisRenderObject*		m_pPropAxisObject;

	deque<CAxisRenderObject*> m_dqpAxisPool;
	map<CDnCutSceneActor*, CAxisRenderObject*> m_mapActorAxisObject;

	map<KeyInfo*, CAxisRenderObject*> m_mapKeyInfoAxisObject;
	bool m_bSelectedKeyAxis;
	bool m_bSelectKeyInfo;

	bool					m_bShowAxis;

	// DOF 필터
	CEtDOFFilter*			m_pDOFFilter;

	// 자막 출력용 UI 다이얼로그
	int						m_iWidth;
	int						m_iHeight;
	bool					m_bShowSubtitle;
	wstring					m_strSubtitle;
	LPD3DXFONT				m_pFont;

	// 레터박스 출력. 현재 클라이언트에서는 1024 기준으로 100 픽셀씩 레터박스를 보여준다.
	bool					m_bShowLetterBox;

private:
	void _LoadMap( const char* pMapName, const char* pEnvFilePath );
	void _LoadActor( const wchar_t* pActorName, const char* pActorResName );	// 편집자가 이름붙인 액터와 실제 액터 리소스 이름

	CDnCutSceneActor* _CreateActor( const wchar_t* pActorName, const char* pActorResName );
	CDnCutSceneActor* _LoadActorFromResource( const char* pActorResName );
	shared_ptr<CDnCutSceneWeapon> _LoadWeaponFromResource( int iWeaponTableID );

	void _InitToPlay( void );

	void _DrawAxis( const EtVector3& vPosition, float fRot );
	void _DrawKeyPath( const wchar_t* pActorName );

	void _DrawLetterBox( void );

	void _MakeSphereWorld( const EtVector3& vPos, EtMatrix& matWorld );

	CDnCutSceneActor* _FindActorByName( const wchar_t* pActorName );

	SPrimitiveDraw3D* _GetAvailablePBuffer( void );
	void _ResetPBufferUseMark( void );

	void _DrawCoordText2D( EtVector3& vPos, DWORD dwColor, const char* pName = NULL );

	void _FillActorsAniElementData( CDnCutSceneActor* pActor );
	CAxisRenderObject* _GetAxisRenderObject( void );		// 풀에서 갖고 오기.
	void _ReturnAxisRenderObject( CAxisRenderObject* pAxisRenderObject );
	void _SelectActor( CDnCutSceneActor* pActor );
	void _DeSelectActor( void );
	void SelectKeyInfo( const KeyInfo * pKey );
	void _DeSelectKeyAxis();
	void _ProcessAxisObject( LOCAL_TIME LocalTime, float fDelta );

public:
	CRTCutSceneRenderer( HWND hWnd );
	virtual ~CRTCutSceneRenderer(void);

	// RenderPanel 쪽에서 조작에 관련된 모든 일을 하고 렌더러에서는 쿼리 및 명령 수행을 합니다!

	// 바깥에서 적절한 위치 얻어오기
	void GetHeightPos( EtVector3& vPos );
	float GetMapHeight( float fXPos, float fYPos );

	void SetAnimation( const wchar_t* pActorName, int iAnimationIndex );
	void SetMode( int iMode );
	
	// IRenderBase 가상함수 구현
	void UpdateRenderObject( void );

	void _UpdateMap( void );
	void _UpdateActors( void );
	void OnResize( int iWidth, int iHeight );
	void SetActivate( bool bActive );
	void OnRender( LOCAL_TIME LocalTime, float fDelta );
	
	void OnLButtonDown( int iXPos, int iYPos );
	void OnCButtonDown( int iXPos, int iYPos );
	void OnRButtonDown( int iXPos, int iYPos );

	void OnLButtonDrag( int iXPos, int iYPos );
	void OnCButtonDrag( int iXPos, int iYPos );
	void OnRButtonDrag( int iXPos, int iYPos );

	void OnLButtonUp( int iXPos, int iYPos );
	void OnRButtonUp( int iXPos, int iYPos );

	void SeeThisActor( const wchar_t* pActorName );
	void SelectActor( const wchar_t* pActorName );

	//void SelectParticle( int iEventID );

	void ToggleCamMode( bool bToggleCamMode );
	void ToggleShowSpotPos( bool bToggleSpotPos );
	void ToggleLetterBox( bool bToggleLetterBox );
	
	void OnMouseWheel( int iWheelDelta );

	// bintitle.
	void ToggleWeapon( bool bWeapon );
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);

	bool IsObjectSelected( void );
	void UnSelectObject( void );

	void UpdateSelectedObject( void );

	const wchar_t* GetSelectedObjectName( void );
	int GetSelectedObjectType( void ) { return m_iSelectedObjectType; };
	int GetSelectedObjectID( void );
	void SetActorAlpha( const wchar_t* pActorName, float fAlpha );
	bool MoveObjectByDragging( const wchar_t* pActorName, int iMouseXPos, int iMouseYPos );
	void GetSelectedObjectPos( EtVector3* vPos/*const wchar_t* pActorName*/ );
	void RotateObjectByWheelDelta( int iWheelDelta );
	float GetRenderActorRotDegree( const wchar_t* pActorName );

	void OnPostLoadFile( void );

	void OnSelectEventUnit( int iEventType, int iID );

	// 렌더링 옵션 관련
	void SetRenderFog( bool bRenderFog );

	void OnSelecteProp( int iPropID );

	CDnCutSceneWorld* GetWorld( void ) { return m_pWorld; };

	void ShowSubtitle( const wchar_t* pSubtitle );
	void HideSubtitle( void );

	const EtVector3& GetLookAt( void );

	virtual void ChangeActor( const wchar_t* pOldActorName, const wchar_t* pNewActorResName );
	bool CanChangeActor( const wchar_t* pOldActorName, const wchar_t* pNewActorResName );

	void SetActorScale( const wchar_t* pActorName, float fScale );

	void SetLockSelect( bool b ){
		m_bLockSelect = b;
	}
	bool bIsLockSelect(){
		return m_bLockSelect;
	}

	void AddLockActor( CDnCutSceneActor * pActor );
	void ClearLockActor();
	bool IsLockActor( CDnCutSceneActor * pActor );

};
