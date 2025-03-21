#pragma once

#include "Singleton.h"
#include "InputReceiver.h"

class CDnMouseCursor : public CSingleton<CDnMouseCursor>, public CInputReceiver, public CLostDeviceProcess
{
public:
	CDnMouseCursor();
	virtual ~CDnMouseCursor();

	enum CursorStateEnum
	{
		NOASSIGN = -1,
		NORMAL = 0,
		CLICK = 1,
		SIZE = 2,
		CursorStateEnum_Amount,
	};

private:
	CursorStateEnum	m_CursorState;
	CursorStateEnum	m_LastCursorState;
	HCURSOR	m_CursorArray[CursorStateEnum_Amount];
	EtTextureHandle m_CursorTexture[CursorStateEnum_Amount];
	float m_fMouseSensitivity;
	bool m_bShowCursor;
	int m_nShowRefCnt;

	// State마다 다른 기준점을 가집니다. 기본값은 1, 1
	UINT m_nXHotSpotArray[CursorStateEnum_Amount];
	UINT m_nYHotSpotArray[CursorStateEnum_Amount];

	RECT m_prevClipCursorRect;
	POINT m_prevCursorPos;

	//blondy
	bool m_bStatic; //UI로만 구성된 씬에서 다른 태스크들이나 UI에서 마우스간섭을 없애기 위해 항상 보이는 기능을 추가
	//blondy end

	bool m_bForceClipCursorNull;

public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();

	bool Create();
	void SetCursorState( CursorStateEnum State );
	CursorStateEnum GetCursorState() { return m_CursorState; }
	void SetHotSpot( UINT nStateIndex, UINT nXHotSpot = 1, UINT nYHotSpot = 1 );
	void UpdateCursor();
	
	float GetMouseSensitivity() { return m_fMouseSensitivity; }
	void SetMouseSensitivity( float fValue );

	void ShowCursor( bool bShow, bool bForce = false );
	bool IsShowCursor();

	int GetShowCursorRefCount() { return m_nShowRefCnt; }

	void AddShowRefCnt();
	void ReleaseShowRefCnt();

	// UI의 마우스커서 자동이동을 위해 만든 함수로,
	// 클라이언트 영역의 좌표값을 인자로 사용합니다.
	void SetCursorPos( int nX, int nY );
	void GetCursorPos( int *pX, int *pY );

	void SetStaticMouse( bool isStatic ){m_bStatic = isStatic;};

	void ClipCursor( RECT *pRect );
	void SetCursorPosToDevice( int nX, int nY );
	void SetForceClipCursorNull( bool bForceClipCursorNull ) { m_bForceClipCursorNull = bForceClipCursorNull; }

	float GetCursorWidth();
	float GetCursorHeight();
};