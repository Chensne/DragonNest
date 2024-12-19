#pragma once




// 렌더링 코드와 wxWidget 을 분리 시키기 위한 인터페이스 선언.
// 실제 상속받을 놈은 wxWidget에 대한 존재를 전혀 모름..
class IRenderBase
{
public:
	// Renderer mode
	enum
	{
		EDITOR_MODE,
		STOP_MODE,
		PLAY_MODE,
		FREE_TIME_MODE,
		MODE_COUNT,
	};

	// Selected Object Type enumeration
	enum
	{
		SO_ACTOR,
		SO_PARTICLE,
		SO_MAX_CAMERA,
		SO_COUNT,
	};

public:
	IRenderBase( void );
	virtual ~IRenderBase(void);

	static IRenderBase* Create( HWND hWnd );

	virtual void UpdateRenderObject( void ) = 0;

	virtual void OnResize( int iWidth, int iHeight ) = 0;
	virtual void SetActivate( bool bActive ) = 0;
	virtual void OnRender( LOCAL_TIME LocalTime, float fDelta ) = 0;

	virtual void OnLButtonDown( int iXPos, int iYPos ) = 0;
	virtual void OnCButtonDown( int iXPos, int iYPos ) = 0;
	virtual void OnRButtonDown( int iXPos, int iYPos ) = 0;

	virtual void OnLButtonDrag( int iXPos, int iYPos ) = 0;
	virtual void OnCButtonDrag( int iXPos, int iYPos ) = 0;
	virtual void OnRButtonDrag( int iXPos, int iYPos ) = 0;

	virtual void OnLButtonUp( int iXPos, int iYPos ) = 0;
	virtual void OnRButtonUp( int iXPos, int iYPos ) = 0;

	virtual void OnMouseWheel( int iWheelDelta ) = 0;
	virtual void OnKeyDown(class wxKeyEvent& event) = 0;
	virtual void OnKeyUp(class wxKeyEvent& event) = 0;
	
	virtual void SeeThisActor( const wchar_t* pActorName ) = 0;
	virtual void SelectActor( const wchar_t* pActorName ) = 0;

	virtual void ToggleCamMode( bool bToggleCamMode ) = 0;
	virtual void ToggleShowSpotPos( bool bShowSpotPos ) = 0;
	virtual void SetRenderFog( bool bRenderFog ) = 0;
	virtual void ToggleLetterBox( bool bToggleLetterBox ) = 0;
	virtual void ToggleWeapon( bool bToggleWeapon ) = 0;

	virtual void SetMode( int iMode ) = 0;

	virtual bool IsObjectSelected( void ) = 0;
	virtual void UnSelectObject( void ) = 0;
	virtual void UpdateSelectedObject( void ) = 0;
	virtual const wchar_t* GetSelectedObjectName( void ) = 0;
	virtual int GetSelectedObjectType( void ) = 0;
	virtual int GetSelectedObjectID( void ) = 0;
	virtual void SetActorAlpha( const wchar_t* pActorName, float fAlpha ) = 0;
	virtual bool MoveObjectByDragging( const wchar_t* pActorName, int iMouseXPos, int iMouseYPos ) = 0;
	virtual void GetSelectedObjectPos( EtVector3* vPos/*const wchar_t* pActorName*/ ) = 0;
	virtual void RotateObjectByWheelDelta( int iWheelDelta ) = 0;
	virtual float GetRenderActorRotDegree( const wchar_t* pActorName ) = 0;
	virtual void ChangeActor( const wchar_t* pOldActorName, const wchar_t* pNewActorResName ) = 0;

	virtual void OnPostLoadFile( void ) = 0;

	virtual bool bIsLockSelect()=0;
};
