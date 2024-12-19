#include "StdAfx.h"
#include "InputDevice.h"
#include "InputReceiver.h"
#include "CountryByCountryKeyboard.h"
#include "SundriesFunc.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//#define _KAL_TEST

SWrappingData g_EnableWrappingData[] = {
	{ DIK_ESCAPE, VK_ESCAPE },
	{ DIK_1,	'1' },
	{ DIK_2,	'2' },
	{ DIK_3,	'3' },
	{ DIK_4,	'4' },
	{ DIK_5,	'5' },
	{ DIK_6,	'6' },
	{ DIK_7,	'7' },
	{ DIK_8,	'8' },
	{ DIK_9,	'9' },
	{ DIK_0,	'0' },
	{ DIK_BACK,	VK_BACK },
	{ DIK_Q,	'Q' },
	{ DIK_W,	'W' },
	{ DIK_E,	'E' },
	{ DIK_R,	'R' },
	{ DIK_T,	'T' },
	{ DIK_Y,	'Y' },
	{ DIK_U,	'U' },
	{ DIK_I,	'I' },
	{ DIK_O,	'O' },
	{ DIK_P,	'P' },
	{ DIK_A,	'A' },
	{ DIK_S,	'S' },
	{ DIK_D,	'D' },
	{ DIK_F,	'F' },
	{ DIK_G,	'G' },
	{ DIK_H,	'H' },
	{ DIK_J,	'J' },
	{ DIK_K,	'K' },
	{ DIK_L,	'L' },
	{ DIK_SEMICOLON,	VK_OEM_1 },
	{ DIK_APOSTROPHE,	VK_OEM_7 },
	{ DIK_GRAVE,		VK_OEM_3 },
	{ DIK_BACKSLASH,	VK_OEM_5 },
	{ DIK_Z,	'Z' },
	{ DIK_X,	'X' },
	{ DIK_C,	'C' },
	{ DIK_V,	'V' },
	{ DIK_B,	'B' },
	{ DIK_N,	'N' },
	{ DIK_M,	'M' },
	{ DIK_COMMA,	VK_OEM_COMMA },
	{ DIK_PERIOD,	VK_OEM_PERIOD },
	{ DIK_SLASH,	VK_OEM_2 },
	{ DIK_MULTIPLY,	VK_MULTIPLY },
	{ DIK_SPACE,	VK_SPACE },
	{ DIK_NUMPAD7,	VK_NUMPAD7 },
	{ DIK_NUMPAD8,	VK_NUMPAD8 },
	{ DIK_NUMPAD9,	VK_NUMPAD9 },
	{ DIK_SUBTRACT,	VK_SUBTRACT },
	{ DIK_NUMPAD4,	VK_NUMPAD4 },
	{ DIK_NUMPAD5,	VK_NUMPAD5 },
	{ DIK_NUMPAD6,	VK_NUMPAD6 },
	{ DIK_ADD,		VK_ADD },
	{ DIK_NUMPAD1,	VK_NUMPAD1 },
	{ DIK_NUMPAD2,	VK_NUMPAD2 },
	{ DIK_NUMPAD3,	VK_NUMPAD3 },
	{ DIK_NUMPAD0,	VK_NUMPAD0 },
	{ DIK_DECIMAL,	VK_DECIMAL },
	{ DIK_DIVIDE,	VK_DIVIDE },
	{ DIK_HOME,		VK_HOME },
	{ DIK_UP,		VK_UP },
	{ DIK_PRIOR,	VK_PRIOR },
	{ DIK_LEFT,		VK_LEFT },
	{ DIK_RIGHT,	VK_RIGHT },
	{ DIK_END,		VK_END },
	{ DIK_DOWN,		VK_DOWN },
	{ DIK_NEXT,		VK_NEXT },
	{ DIK_INSERT,	VK_INSERT },
	{ DIK_DELETE,	VK_DELETE },
	{ DIK_LCONTROL, VK_LCONTROL },
	{ DIK_MINUS,	VK_OEM_MINUS },
	{ DIK_EQUALS,	VK_OEM_PLUS },
};

BYTE _ToDIK( BYTE cVK )
{
	for( int i = 0; i < _countof(g_EnableWrappingData); ++i )
	{
		if( g_EnableWrappingData[i].cVK == cVK )
			return g_EnableWrappingData[i].cDIK;
	}
	return 0;
}

BYTE _ToVK( BYTE cDIK )
{
	for( int i = 0; i < _countof(g_EnableWrappingData); ++i )
	{
		if( g_EnableWrappingData[i].cDIK == cDIK )
			return g_EnableWrappingData[i].cVK;
	}
	return 0;
}

BYTE _NoNumLockVKToNumLockVK( BYTE cVK )
{
	switch( cVK )
	{
	case VK_HOME:	return VK_NUMPAD7;
	case VK_UP:		return VK_NUMPAD8;
	case VK_PRIOR:	return VK_NUMPAD9;
	case VK_LEFT:	return VK_NUMPAD4;
	case 0x0c:		return VK_NUMPAD5;
	case VK_RIGHT:	return VK_NUMPAD6;
	case VK_END:	return VK_NUMPAD1;
	case VK_DOWN:	return VK_NUMPAD2;
	case VK_NEXT:	return VK_NUMPAD3;
	case VK_INSERT:	return VK_NUMPAD0;
	case VK_DELETE:	return VK_DECIMAL;
	}
	return cVK;
}

bool _IsEnableWrappingDIK( BYTE cDIK )
{
	for( int i = 0; i < _countof(g_EnableWrappingData); ++i )
	{
		if( g_EnableWrappingData[i].cDIK == cDIK )
			return true;
	}
	return false;
}

BYTE _GetCurrentPushKey( bool bTransNumLock )
{
	BYTE cVK = 0;
	for( int i = 0; i < _countof(g_EnableWrappingData); ++i )
	{
		if( GetKeyState( g_EnableWrappingData[i].cVK )&0x80 )
		{
			cVK = g_EnableWrappingData[i].cVK;
			break;
		}
	}
	if( cVK == 0 ) return 0;

	// 넘락키 토글 안켜있는채로 Numpad7누르면 Home으로 온다. 그래서 현재 numlock상태에 따라 변환 처리를 해준다.
	if( bTransNumLock ) cVK = _NoNumLockVKToNumLockVK( cVK );
	return _ToDIK(cVK);
}

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

int CInputDevice::ms_InverseModeCount = 0;
int CInputDevice::ms_InverseMode = 0; //입력 반전 모드(1:키보드&마우스, 2: 키보드, 3: 마우스)

void CInputDevice::SetInverseMode(int nMode)
{
	ms_InverseMode = nMode;	
}

bool CInputDevice::IsInverseKeyboard()
{
	return (ms_InverseModeCount > 0 ? (ms_InverseMode == 1 || ms_InverseMode == 2) : false);
}

bool CInputDevice::IsInverseMouse()
{
	return (ms_InverseModeCount > 0 ? (ms_InverseMode == 1 || ms_InverseMode == 3) : false);
}

void CInputDevice::ResetInverseMode()
{
	ms_InverseModeCount = 0;
	ms_InverseMode = 0;
}

CInputDevice::CInputDevice()
{
	m_pDI = NULL;
	m_bCheckWndFocus = false;
	m_bForcePassProcess = false;
	m_hWnd = NULL;
	int i = 0;
	for (; i < eABSENCE_MAX; ++i)
	{
		m_AbsenceCheckTimers[i].fAbsenceCheckTimer = eABSENCE_MIN;
		m_AbsenceCheckTimers[i].bHoldAbsenceCheckTimer = false;
	}
}

CInputDevice::~CInputDevice()
{
	Finalize();
}

void CInputDevice::Finalize()
{
	SAFE_DELETE_PVEC( m_pVecDevice );
	SAFE_DELETE_VEC( m_pVecReceiverList );
	SAFE_RELEASE( m_pDI );
}

bool CInputDevice::IsValidAbsenceChecktype(eAbsenceCheckTimerType type) const
{
	return (type >= eABSENCE_MIN && type < eABSENCE_MAX);
}

float CInputDevice::GetAbsenceCheckTimer(eAbsenceCheckTimerType type) const
{
	if (IsValidAbsenceChecktype(type) == false)
		return 0.f;

	return m_AbsenceCheckTimers[type].fAbsenceCheckTimer;
}

void CInputDevice::ResetAbsenceCheckTimer(eAbsenceCheckTimerType type)
{
	if (IsValidAbsenceChecktype(type) == false)
		return;

	m_AbsenceCheckTimers[type].fAbsenceCheckTimer = 0.f;
}

void CInputDevice::HoldAbsenceCheckTimer(eAbsenceCheckTimerType type, bool bHold)
{
	if (IsValidAbsenceChecktype(type) == false)
		return;

	m_AbsenceCheckTimers[type].bHoldAbsenceCheckTimer = bHold;
}

void CInputDevice::Process( LOCAL_TIME LocalTime, float fDelta )
{
	int i = 0;
	for (; i < eABSENCE_MAX; ++i)
	{
		if (m_AbsenceCheckTimers[i].bHoldAbsenceCheckTimer == false)
		{
			m_AbsenceCheckTimers[i].fAbsenceCheckTimer += fDelta;
			if (m_AbsenceCheckTimers[i].fAbsenceCheckTimer < 0.f)
				m_AbsenceCheckTimers[i].fAbsenceCheckTimer = 0.f;
		}
	}

	//m_PrevLocalTime = LocalTime;

	if( m_bCheckWndFocus && GetFocus() != m_hWnd  ) return;

	int nEvent = 0;
	bool bOnMsg = false;
	for( DWORD i=0; i<m_pVecDevice.size(); i++ ) {
		if( m_pVecDevice[i]->Process( LocalTime ) == true ) {
			nEvent |= m_pVecDevice[i]->GetEvent();
			bOnMsg = true;

			int j = 0;
			for (; j < eABSENCE_MAX; ++j)
			{
				m_AbsenceCheckTimers[j].fAbsenceCheckTimer = 0.f;
			}
		}
	}

	if( m_bForcePassProcess ) {
		m_bForcePassProcess = false;
		return;
	}
	if( bOnMsg ) {
		for( DWORD i=0; i<m_pVecReceiverList.size(); i++ ) {
			m_pVecReceiverList[i]->OnInputReceive( nEvent, LocalTime );
		}
	}
}

bool CInputDevice::Initialize( HWND hWnd, int nDeviceType, bool bCheckFocus )
{
	HRESULT hResult;
	bool bResult = true;

	m_hWnd = hWnd;
	m_bCheckWndFocus = bCheckFocus;
	hResult = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL );
	ADD_D3D_RES( m_pDI );
	if( FAILED( hResult ) ) return false;

	if( nDeviceType & KEYBOARD ) bResult = CreateDevice( KEYBOARD );
	if( nDeviceType & MOUSE ) bResult = CreateDevice( MOUSE );
	if( nDeviceType & JOYPAD ) CreateDevice( JOYPAD );

	if( ((CInputJoyPad *)GetDeviceObject( JOYPAD )) )
	{
		((CInputJoyPad *)GetDeviceObject( JOYPAD ))->SetKeyboard( ((CInputKeyboard *)GetDeviceObject( KEYBOARD )) );
		((CInputJoyPad *)GetDeviceObject( JOYPAD ))->SetMouse( ((CInputMouse *)GetDeviceObject( MOUSE )) );
	}

	return bResult;
}

bool CInputDevice::CreateDevice( DEVICE_TYPE Type )
{
	CInputDeviceBase *pDevice = NULL;
	switch( Type ) {
		case KEYBOARD:
			pDevice = new CInputKeyboard;
			break;
		case MOUSE:
			pDevice = new CInputMouse;
			break;
		case JOYPAD:
			pDevice = new CInputJoyPad;
			((CInputJoyPad *)pDevice)->SetHWND( m_hWnd );
			break;
		default:
			return false;
	}
	if( pDevice->Initialize() == false ) {
		SAFE_DELETE( pDevice );
		return false;
	}
	m_pVecDevice.push_back( pDevice );
	return true;
}

CInputDeviceBase *CInputDevice::GetDeviceObject( DEVICE_TYPE DeviceType )
{
	for( DWORD i=0; i<m_pVecDevice.size(); i++ ) {
		if( m_pVecDevice[i]->GetDeviceType() == DeviceType ) return m_pVecDevice[i];
	}
	return NULL;
}

void CInputDevice::ResetAllInput()
{
	for( DWORD i=0; i<m_pVecDevice.size(); i++ ) {
		m_pVecDevice[i]->ResetDeviceState();
	}
}

CInputKeyboard::CInputKeyboard()
: CInputDeviceBase( CInputDevice::KEYBOARD )
{
	m_pDIDevice = NULL;
	m_dwApplyBufferCount = 0;
	m_dwBufferCount = 8;
	m_pDidod = new DIDEVICEOBJECTDATA[m_dwBufferCount];
	memset( m_bKeyState, 0, sizeof(m_bKeyState) );
	memset( m_PushKeyTime, 0, sizeof(m_PushKeyTime) );
	memset( m_ReleaseKeyTime, 0, sizeof(m_ReleaseKeyTime) );
	m_nEvent = 0;

	SecureZeroMemory( &m_StartupStickyKeys, sizeof(m_StartupStickyKeys) );
	SecureZeroMemory( &m_StartupToggleKeys, sizeof(m_StartupToggleKeys) );
	SecureZeroMemory( &m_StartupFilterKeys, sizeof(m_StartupFilterKeys) );
}

CInputKeyboard::~CInputKeyboard()
{
	Release();
}

void CInputKeyboard::Release()
{
	if( m_pDIDevice ) m_pDIDevice->Unacquire();
	SAFE_DELETEA( m_pDidod );
	SAFE_RELEASE( m_pDIDevice );

	AllowAccessibilityShortcutKeys( true );
}

bool CInputKeyboard::Initialize()
{
	HRESULT hResult;
	LPDIRECTINPUT8 pDI = CInputDevice::GetInstance().GetDI();
	DIPROPDWORD dipdw;

	hResult = pDI->CreateDevice( GUID_SysKeyboard, &m_pDIDevice, NULL );
	ADD_D3D_RES( m_pDIDevice );
	if( FAILED( hResult ) ) return false;

	hResult = m_pDIDevice->SetDataFormat( &c_dfDIKeyboard );
	if( FAILED( hResult ) ) return false;

	hResult = m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY );
	if( FAILED( hResult ) ) return false;

	ZeroMemory( &dipdw, sizeof(DIPROPDWORD) );
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = m_dwBufferCount;

	hResult = m_pDIDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
	if( FAILED( hResult ) ) return false;

	m_pDIDevice->Acquire();

	ZeroMemory(&m_StartupStickyKeys, sizeof(STICKYKEYS));
	ZeroMemory(&m_StartupToggleKeys, sizeof(TOGGLEKEYS));
	ZeroMemory(&m_StartupFilterKeys, sizeof(FILTERKEYS));
	m_StartupStickyKeys.cbSize = sizeof(STICKYKEYS);	
	m_StartupToggleKeys.cbSize = sizeof(TOGGLEKEYS);
	m_StartupFilterKeys.cbSize = sizeof(FILTERKEYS);

	// Save the current sticky/toggle/filter key settings so they can be restored them later
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &m_StartupStickyKeys, 0);
	SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &m_StartupToggleKeys, 0);
	SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &m_StartupFilterKeys, 0);

	AllowAccessibilityShortcutKeys( false );

	return true;
}

//	CInputMouse 와 중복 코드. m_pDIDevice를 base 레벨로 올리는 것에 대해 고려 by kalliste
HRESULT CInputKeyboard::SetCooperativeLevel(DWORD flag)
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	return m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), flag );
}

HRESULT CInputKeyboard::GetDeviceData()
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	m_dwApplyBufferCount = m_dwBufferCount;
	return m_pDIDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), m_pDidod, &m_dwApplyBufferCount, 0 );
}

bool CInputKeyboard::Process( LOCAL_TIME LocalTime )
{
	HRESULT hResult;

	// Release Event
	m_nEvent = 0;
	m_VecKeyList.clear();
	m_VecKeyEventTime.clear();

	hResult = GetDeviceData();
	if( FAILED( hResult ) ) {
		switch( hResult ) {
			case DIERR_NOTFOUND:
				Release();
				Initialize();
				return false;
			default:
				hResult = m_pDIDevice->Acquire();
				ResetDeviceState();
//				if( hResult == DIERR_OTHERAPPHASPRIO || hResult == DIERR_NOTACQUIRED ) return false;
				return false;
		}
	}

	for( DWORD i=0; i<m_dwApplyBufferCount; i++ )
	{
		DWORD dwOfs = m_pDidod[i].dwOfs;
#if defined( PRE_ADD_MULTILANGUAGE )
		DWORD dwOfs = GetCountryByCountryKeyboard().ChangeKey( m_pDidod[i].dwOfs );
#endif	// #if defined( PRE_ADD_MULTILANGUAGE )

		m_bKeyState[dwOfs] = ( m_pDidod[i].dwData & 0x80 ) ? true : false;

		// Event 위해 저장~
		if( m_bKeyState[dwOfs] == true ) {
			m_nEvent |= CInputReceiver::IR_KEY_DOWN;
			m_PushKeyTime[dwOfs] = LocalTime;
		}
		else {
			m_nEvent |= CInputReceiver::IR_KEY_UP;
			m_ReleaseKeyTime[dwOfs] = LocalTime;
		}

		m_VecKeyList.push_back( (BYTE)dwOfs );
		m_VecKeyEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );
	}

	if( m_dwApplyBufferCount > 0 ) return true;
	return false;
}

void CInputKeyboard::SetKey( int index, bool bPush, LOCAL_TIME LocalTime, DWORD pushTime )
{
//	if( m_bKeyState[index] )
//		return;

	m_bKeyState[index] = bPush;

	// Event 위해 저장~
	if( m_bKeyState[index] == true ) {
		m_nEvent |= CInputReceiver::IR_KEY_DOWN;
		m_PushKeyTime[index] = LocalTime;
	}
	else {
		m_nEvent |= CInputReceiver::IR_KEY_UP;
		m_ReleaseKeyTime[index] = LocalTime;
	}

	m_VecKeyList.push_back( (BYTE)index );
	m_VecKeyEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - pushTime ) );
}

// 전체화면 모드에서 Shift 키 연타 등의 윈도우 숏컷키로 인해서 화면 전환되서
// 게임 플레이에 지장을 받지 않도록 한다.  by realgaia
void CInputKeyboard::AllowAccessibilityShortcutKeys( bool bAllowKeys )
{
	if( bAllowKeys )
	{
		// Restore StickyKeys/etc to original state and enable Windows key      
		STICKYKEYS sk = m_StartupStickyKeys;
		TOGGLEKEYS tk = m_StartupToggleKeys;
		FILTERKEYS fk = m_StartupFilterKeys;

		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
		SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tk, 0);
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
	}
	else
	{
		// Disable StickyKeys/etc shortcuts but if the accessibility feature is on, 
		// then leave the settings alone as its probably being usefully used

		STICKYKEYS skOff = m_StartupStickyKeys;
		if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
			skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
		}

		TOGGLEKEYS tkOff = m_StartupToggleKeys;
		if( (tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
			tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
		}

		FILTERKEYS fkOff = m_StartupFilterKeys;
		if( (fkOff.dwFlags & FKF_FILTERKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
			fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
		}
	}
}

CInputMouse::CInputMouse()
: CInputDeviceBase( CInputDevice::MOUSE )
{
	m_pDIDevice = NULL;
	m_dwApplyBufferCount = 0;
	m_dwBufferCount = 32;
	m_pDidod = new DIDEVICEOBJECTDATA[m_dwBufferCount];

	m_MousePos = D3DXVECTOR2( 0.f, 0.f );
	m_MouseVariation = D3DXVECTOR3( 0.f, 0.f, 0.f );
	memset( m_bButtonState, 0, sizeof(m_bButtonState) );
	memset( m_ButtonLastTime, 0, sizeof(m_ButtonLastTime) );
	memset( m_MouseEventTime, 0, sizeof(m_MouseEventTime) );
	m_nEvent = 0;
}

CInputMouse::~CInputMouse()
{
	Release();
}

void CInputMouse::Release()
{
	if( m_pDIDevice ) m_pDIDevice->Unacquire();
	SAFE_RELEASE( m_pDIDevice );
	SAFE_DELETEA( m_pDidod );
}

bool CInputMouse::Initialize()
{
	HRESULT hResult;
	DIPROPDWORD dipdw;
	LPDIRECTINPUT8 pDI = CInputDevice::GetInstance().GetDI();

	hResult = pDI->CreateDevice( GUID_SysMouse, &m_pDIDevice, NULL );
	if( FAILED( hResult ) ) return false;
	ADD_D3D_RES( m_pDIDevice );

	hResult = m_pDIDevice->SetDataFormat( &c_dfDIMouse2 );
	if( FAILED( hResult ) ) return false;

	hResult = m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND | DISCL_NOWINKEY );
	if( FAILED( hResult ) ) return false;


	ZeroMemory( &dipdw, sizeof(DIPROPDWORD) );
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = m_dwBufferCount;

	hResult = m_pDIDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
	if( FAILED( hResult ) ) return false;

	m_pDIDevice->Acquire();

	return true;
}

HRESULT CInputMouse::GetDeviceData()
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	m_dwApplyBufferCount = m_dwBufferCount;
	return m_pDIDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), m_pDidod, &m_dwApplyBufferCount, 0 );
}

bool CInputMouse::Process( LOCAL_TIME LocalTime )
{
	HRESULT hResult;

	// Release Event
	m_nEvent = 0;
	m_VecEventList.clear();

	hResult = GetDeviceData();
	if( FAILED( hResult ) ) {
		switch( hResult ) {
			case DI_OK:	break;
			case DIERR_NOTFOUND:
				Release();
				Initialize();
				return false;
			default:
				hResult = m_pDIDevice->Acquire();
				ResetDeviceState();
//				if( hResult == DIERR_OTHERAPPHASPRIO || hResult == DIERR_NOTACQUIRED ) return false;
				return false;
		}
	}

	m_MouseVariation = D3DXVECTOR3( 0.f, 0.f, 0.f );
	DWORD dwLocalTime = timeGetTime();
	for( DWORD i=0; i<m_dwApplyBufferCount; i++ ) {

		emMOUSE_BUTTON mouseButton(buttonMax);

		switch( m_pDidod[i].dwOfs ) {
			case DIMOFS_X:
				m_MousePos.x += (float)(int)m_pDidod[i].dwData;
				m_MouseVariation.x += (float)(int)m_pDidod[i].dwData;
				m_nEvent |= CInputReceiver::IR_MOUSE_MOVE;
				m_MouseEventTime[0] = (LOCAL_TIME) LocalTime - (LOCAL_TIME)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				break;
			case DIMOFS_Y:
				m_MousePos.y += (float)(int)m_pDidod[i].dwData;
				m_MouseVariation.y += (float)(int)m_pDidod[i].dwData;
				m_nEvent |= CInputReceiver::IR_MOUSE_MOVE;
				m_MouseEventTime[0] = (LOCAL_TIME) LocalTime - (LOCAL_TIME)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				break;
			case DIMOFS_Z:
				m_MouseVariation.z += (float)(int)m_pDidod[i].dwData;
				m_nEvent |= CInputReceiver::IR_MOUSE_WHEEL;
				m_MouseEventTime[1] = (LOCAL_TIME) LocalTime - (LOCAL_TIME)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				break;
			case DIMOFS_BUTTON0:
				m_bButtonState[0] = ( m_pDidod[i].dwData & 0x80 ) ? true : false;
				m_nEvent |= ( m_bButtonState[0] == true ) ? CInputReceiver::IR_MOUSE_LB_DOWN : CInputReceiver::IR_MOUSE_LB_UP;
				m_MouseEventTime[ 2 + ( ( m_bButtonState[0] == true ) ? 0 : 1 ) ] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				mouseButton = buttonLeft;
				break;
			case DIMOFS_BUTTON1:
				m_bButtonState[1] = ( m_pDidod[i].dwData & 0x80 ) ? true : false;
				m_nEvent |= ( m_bButtonState[1] == true ) ? CInputReceiver::IR_MOUSE_RB_DOWN : CInputReceiver::IR_MOUSE_RB_UP;
				m_MouseEventTime[ 4 + ( ( m_bButtonState[1] == true ) ? 0 : 1 ) ] = (LOCAL_TIME) LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				mouseButton = buttonRight;
				break;
			case DIMOFS_BUTTON2:
				m_bButtonState[2] = ( m_pDidod[i].dwData & 0x80 ) ? true : false;
				m_nEvent |= ( m_bButtonState[2] == true ) ? CInputReceiver::IR_MOUSE_WB_DOWN : CInputReceiver::IR_MOUSE_WB_UP;
				m_MouseEventTime[ 6 + ( ( m_bButtonState[2] == true ) ? 0 : 1 ) ] = (LOCAL_TIME) LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
				mouseButton = buttonMiddle;
				break;
		}

		m_VecEventList.push_back( m_pDidod[i].dwOfs );

		// Note : 마우스 버튼의 더블클릭 체크
		//
		if( mouseButton != buttonMax )
		{
			if( m_bButtonState[mouseButton] )
			{
				if( (m_pDidod[i].dwTimeStamp - m_ButtonLastTime[mouseButton]) < 330 )
				{
					switch( mouseButton )
					{
					case buttonLeft:	
						m_nEvent |= CInputReceiver::IR_MOUSE_LB_DBCLK; 
						m_MouseEventTime[8] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
						break;
					case buttonRight:	
						m_nEvent |= CInputReceiver::IR_MOUSE_RB_DBCLK; 
						m_MouseEventTime[9] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
						break;
					case buttonMiddle:	
						m_nEvent |= CInputReceiver::IR_MOUSE_WB_DBCLK; 
						m_MouseEventTime[10] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp );
						break;
					}

					m_ButtonLastTime[mouseButton] = 0;
				}
				else
				{
					m_ButtonLastTime[mouseButton] = m_pDidod[i].dwTimeStamp;
				}
			}
		}
	}

	if( m_dwApplyBufferCount > 0 ) return true;
	return false;
}

void CInputMouse::SetLeftButton( bool bPush, LOCAL_TIME LocalTime, DWORD pushTime )
{
	m_bButtonState[0] = bPush;
	m_nEvent |= ( m_bButtonState[0] == true ) ? CInputReceiver::IR_MOUSE_LB_DOWN : CInputReceiver::IR_MOUSE_LB_UP;
	m_MouseEventTime[ 2 + ( ( m_bButtonState[0] == true ) ? 0 : 1 ) ] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - pushTime );
}

void CInputMouse::SetRightButton( bool bPush, LOCAL_TIME LocalTime, DWORD pushTime )
{
	m_bButtonState[1] = bPush;
	m_nEvent |= ( m_bButtonState[1] == true ) ? CInputReceiver::IR_MOUSE_RB_DOWN : CInputReceiver::IR_MOUSE_RB_UP;
	m_MouseEventTime[ 4 + ( ( m_bButtonState[1] == true ) ? 0 : 1 ) ] = (LOCAL_TIME)LocalTime - (int)( timeGetTime() - pushTime );
}

//	CInputKeyboard 와 중복 코드. m_pDIDevice를 base 레벨로 올리는 것에 대해 고려 by kalliste
HRESULT CInputMouse::SetCooperativeLevel(DWORD flag)
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	return m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), flag );
}



CInputJoyPad::CInputJoyPad()
: CInputDeviceBase( CInputDevice::JOYPAD )
{
	m_pDIDevice = NULL;
	m_dwApplyBufferCount = 0;
	m_dwBufferCount = 32;
	m_pDidod = new DIDEVICEOBJECTDATA[m_dwBufferCount];

	memset( m_bButtonState, 0 , sizeof(m_bButtonState));
	memset( m_ReleaseJoypadTime, 0, sizeof(m_ReleaseJoypadTime) );
	memset( m_PushJoypadTime, 0, sizeof(m_PushJoypadTime) );

	memset( m_ButtonPush, 0, sizeof(m_ButtonPush) );
	memset( m_UIButtonPush, 0, sizeof(m_UIButtonPush) );

	m_nEvent = 0;

	m_pWrappingKey = NULL;
	m_pUIWrappingKey = NULL;
	m_pWrappingButton = NULL;
	m_pWrappingAssistButton = NULL;
	m_pUIWrappingButton = NULL;
	m_pUIWrappingAssistButton = NULL;

	m_pKeyboard = NULL;
	m_pMouse = NULL;

	m_iWrappingSize = 0;
	m_iUIWrappingSize = 0;

	m_fCameraSpeed = 0.27f;
	m_fCursorSpeed = 0.10f;

	m_hWnd = NULL;
}

CInputJoyPad::~CInputJoyPad()
{
	Release();
}

void CInputJoyPad::Release()
{
	if( m_pDIDevice ) m_pDIDevice->Unacquire();
	SAFE_RELEASE( m_pDIDevice );
	SAFE_DELETEA( m_pDidod );
}

bool CInputJoyPad::Initialize()
{
	HRESULT hResult;
	DIPROPDWORD dipdw;
	LPDIRECTINPUT8 pDI = CInputDevice::GetInstance().GetDI();

	hResult = pDI->CreateDevice( GUID_Joystick, &m_pDIDevice, NULL );
	if( FAILED( hResult ) ) return false;
	ADD_D3D_RES( m_pDIDevice );

	hResult = m_pDIDevice->SetDataFormat( &c_dfDIJoystick2 );
	if( FAILED( hResult ) ) return false;

	hResult = m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY );
	if( FAILED( hResult ) ) return false;


	ZeroMemory( &dipdw, sizeof(DIPROPDWORD) );
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = m_dwBufferCount;

	hResult = m_pDIDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
	if( FAILED( hResult ) ) return false;

	hResult = m_pDIDevice->EnumObjects( EnumObjectsCallback, (VOID *)m_pDIDevice, DIDFT_ALL );
	if( FAILED( hResult ) )	return false;

	m_pDIDevice->Acquire();

	return true;
}

HRESULT CInputJoyPad::GetDeviceData()
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	m_dwApplyBufferCount = m_dwBufferCount;
	return m_pDIDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), m_pDidod, &m_dwApplyBufferCount, 0 );
}

bool CInputJoyPad::Process( LOCAL_TIME LocalTime )
{
	HRESULT hResult;

	// Release Event
	m_nEvent = 0;
	m_VecEventList.clear();
	m_VecEventTime.clear();

	hResult = GetDeviceData();
	if( FAILED( hResult ) ) {
		switch( hResult ) {
			case DI_OK:	break;
			case DIERR_NOTFOUND:
				Release();
				Initialize();
				return false;
			default:
				hResult = m_pDIDevice->Acquire();
				//ResetDeviceState();
				//				if( hResult == DIERR_OTHERAPPHASPRIO || hResult == DIERR_NOTACQUIRED ) return false;
				return false;
		}
	}

	hResult = m_pDIDevice->Poll(); 
	if( FAILED(hResult) )  
	{
		hResult = m_pDIDevice->Acquire();
		while( hResult == DIERR_INPUTLOST ) 
			hResult = m_pDIDevice->Acquire();
		return false; 
	}

	DIJOYSTATE2 js;
	m_pDIDevice->GetDeviceState( sizeof(DIJOYSTATE2), &js );
	DWORD dwLocalTime = timeGetTime();
	for( DWORD i=0; i<m_dwApplyBufferCount; i++ ) {

		switch( m_pDidod[i].dwOfs ) {
			case DIJOFS_X :
				{
					js.lX > 500 ? m_bButtonState[LS_RIGHT] = true : m_bButtonState[LS_RIGHT] = false;
					js.lX < -500 ? m_bButtonState[LS_LEFT] = true : m_bButtonState[LS_LEFT] = false;

					if( m_bButtonState[LS_RIGHT] ) {
						m_PushJoypadTime[LS_RIGHT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_RIGHT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_RIGHT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_RIGHT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[LS_LEFT] ) {
						m_PushJoypadTime[LS_LEFT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_LEFT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_LEFT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_LEFT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}
					break;
				}

			case DIJOFS_Y :
				{
					js.lY > 500 ? m_bButtonState[LS_DOWN] = true : m_bButtonState[LS_DOWN] = false;
					js.lY < -500 ? m_bButtonState[LS_UP] = true : m_bButtonState[LS_UP] = false;

					if( m_bButtonState[LS_DOWN] ) {
						m_PushJoypadTime[LS_DOWN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_DOWN );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_DOWN] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_DOWN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[LS_UP] ) {
						m_PushJoypadTime[LS_UP] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_UP );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_UP] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_UP] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}
					break;
				}

			case DIJOFS_Z :
				{
					js.lZ > 500 ? m_bButtonState[LS_ZOOMIN] = true : m_bButtonState[LS_ZOOMIN] = false;
					js.lZ < -500 ? m_bButtonState[LS_ZOOMOUT] = true : m_bButtonState[LS_ZOOMOUT] = false;

					if( m_bButtonState[LS_ZOOMIN] ) {
						m_PushJoypadTime[LS_ZOOMIN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_ZOOMIN );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_ZOOMIN] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_ZOOMIN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[LS_ZOOMOUT] ) {
						m_PushJoypadTime[LS_ZOOMOUT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( LS_ZOOMOUT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[LS_ZOOMOUT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[LS_ZOOMOUT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					break;
				}

			case DIJOFS_RX :
				{
					js.lRx > 500 ? m_bButtonState[RS_RIGHT] = true : m_bButtonState[RS_RIGHT] = false;
					js.lRx < -500 ? m_bButtonState[RS_LEFT] = true : m_bButtonState[RS_LEFT] = false;

					if( m_bButtonState[RS_RIGHT] ) {
						m_PushJoypadTime[RS_RIGHT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_RIGHT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_RIGHT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_RIGHT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[RS_LEFT] ) {
						m_PushJoypadTime[RS_LEFT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_LEFT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_LEFT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_LEFT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}
					break;
				}

			case DIJOFS_RY :
				{
					js.lRy > 500 ? m_bButtonState[RS_DOWN] = true : m_bButtonState[RS_DOWN] = false;
					js.lRy < -500 ? m_bButtonState[RS_UP] = true : m_bButtonState[RS_UP] = false;

					if( m_bButtonState[RS_DOWN] ) {
						m_PushJoypadTime[RS_DOWN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_DOWN );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_DOWN] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_DOWN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[RS_UP] ) {
						m_PushJoypadTime[RS_UP] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_UP );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_UP] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_UP] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}
					break;
				}

			case DIJOFS_RZ :
				{
					js.lRz > 500 ? m_bButtonState[RS_ZOOMIN] = true : m_bButtonState[RS_ZOOMIN] = false;
					js.lRz < -500 ? m_bButtonState[RS_ZOOMOUT] = true : m_bButtonState[RS_ZOOMOUT] = false;

					if( m_bButtonState[RS_ZOOMIN] ) {
						m_PushJoypadTime[RS_ZOOMIN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_ZOOMIN );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_ZOOMIN] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_ZOOMIN] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( m_bButtonState[RS_ZOOMOUT] ) {
						m_PushJoypadTime[RS_ZOOMOUT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( RS_ZOOMOUT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[RS_ZOOMOUT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_ReleaseJoypadTime[RS_ZOOMOUT] = LocalTime; 
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					break;
				}

			
			case DIJOFS_POV(0) :
				{
					if( js.rgdwPOV[0] == 0 || js.rgdwPOV[0] == 4500 || js.rgdwPOV[0] == 31500 ) {
						m_bButtonState[MS_UP] = true;

						m_PushJoypadTime[MS_UP] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( MS_UP );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[MS_UP] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_bButtonState[MS_UP] = false;
						m_ReleaseJoypadTime[MS_UP] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( js.rgdwPOV[0] == 18000 || js.rgdwPOV[0] == 13500 || js.rgdwPOV[0] == 22500 ) {
						m_bButtonState[MS_DOWN] = true;

						m_PushJoypadTime[MS_DOWN] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( MS_DOWN );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[MS_DOWN] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_bButtonState[MS_DOWN] = false;
						m_ReleaseJoypadTime[MS_DOWN] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( js.rgdwPOV[0] == 9000 || js.rgdwPOV[0] == 4500 || js.rgdwPOV[0] == 13500 ) {
						m_bButtonState[MS_RIGHT] = true;

						m_PushJoypadTime[MS_RIGHT] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( MS_RIGHT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[MS_RIGHT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_bButtonState[MS_RIGHT] = false;
						m_ReleaseJoypadTime[MS_RIGHT] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}

					if( js.rgdwPOV[0] == 27000 || js.rgdwPOV[0] == 22500 || js.rgdwPOV[0] == 31500 ) {
						m_bButtonState[MS_LEFT] = true;

						m_PushJoypadTime[MS_LEFT] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN;

						m_VecEventList.push_back( MS_LEFT );
						m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

						m_JoypadTimeStamp[MS_LEFT] = m_pDidod[i].dwTimeStamp;
					}
					else {
						m_bButtonState[MS_LEFT] = false;
						m_ReleaseJoypadTime[MS_LEFT] = LocalTime;
						m_nEvent |= CInputReceiver::IR_JOYPAD_UP;
					}
					break;
				}
		}
		int index = B0;
		for( int itr = DIJOFS_BUTTON0 ; itr <= DIJOFS_BUTTON31; ++itr )
		{
			if( m_pDidod[i].dwOfs == itr )
			{
				m_bButtonState[index] = ( m_pDidod[i].dwData & 0x80 ) ? true : false;

				m_bButtonState[index] ? m_PushJoypadTime[index] = LocalTime, m_nEvent |= CInputReceiver::IR_JOYPAD_DOWN
									  : m_ReleaseJoypadTime[index] = LocalTime, m_nEvent |= CInputReceiver::IR_JOYPAD_UP;

				m_VecEventList.push_back( index );
				m_VecEventTime.push_back( (LOCAL_TIME)LocalTime - (int)( timeGetTime() - m_pDidod[i].dwTimeStamp ) );

				m_JoypadTimeStamp[index] = m_pDidod[i].dwTimeStamp;
			}
			++index;
		}
	}

	if( m_pWrappingButton && m_pKeyboard && m_pMouse )
	{
		bool bPush;
		INPUT input;

		for( int itr = 0; itr < IW_NORMALATTK; ++itr )
		{

			bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[itr], m_pWrappingAssistButton[itr]) );
			
			if( bPush )
			{
				m_pKeyboard->SetKey( m_pWrappingKey[itr], bPush, LocalTime, m_JoypadTimeStamp[ m_pWrappingAssistButton[itr] ] );
				m_ButtonPush[itr] = true;
			}
			else if( m_ButtonPush[itr] )
			{
				m_pKeyboard->SetKey( m_pWrappingKey[itr], bPush, LocalTime, m_JoypadTimeStamp[ m_pWrappingAssistButton[itr] ] );
				m_ButtonPush[itr] = false;
			}
											
		}

		//Rebirth
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_REBIRTH], m_pWrappingAssistButton[IW_REBIRTH]) );
		BYTE cVK = _ToVK(m_pWrappingKey[IW_REBIRTH]);
		if( bPush )
		{
			::ZeroMemory(&input, sizeof(input));
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = cVK;
			::SendInput(1, &input, sizeof(input));
			m_ButtonPush[IW_REBIRTH] = true;
		}
		else
		{
			::ZeroMemory(&input, sizeof(input));
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = cVK;
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			::SendInput(1, &input, sizeof(input));
			m_ButtonPush[IW_REBIRTH] = false;
		}

		//Shift-Key(키보드 셋팅에는 없는 것이라서 별도러 처리한다.)
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_AVOID], m_pWrappingAssistButton[IW_AVOID]) );
		if( bPush )
		{
			m_pKeyboard->SetKey( 42, bPush, LocalTime, m_JoypadTimeStamp[ m_pWrappingAssistButton[IW_AVOID] ] );
			m_ButtonPush[IW_AVOID] = true;
		}
		else if( m_ButtonPush[IW_AVOID] )
		{
			m_pKeyboard->SetKey( 42, bPush, LocalTime, m_JoypadTimeStamp[ m_pWrappingAssistButton[IW_AVOID] ] );
			m_ButtonPush[IW_AVOID] = false;
		}

		for( int itr = 0; itr < UIWrappingKeyIndex_Amount; ++itr )
		{
			if( IW_PVPTAB == itr )	continue;
			bPush = IsPushJoypadButton( std::make_pair(m_pUIWrappingButton[itr], m_pUIWrappingAssistButton[itr]) );
			BYTE cVK = _ToVK(m_pUIWrappingKey[itr]);
			ULONG ref = 0;
			
			if ( bPush )
			{
				::ZeroMemory(&input, sizeof(input));
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = cVK;
				::SendInput(1, &input, sizeof(input));
				m_UIButtonPush[itr] = true;
			}
			else
			{
				if( m_UIButtonPush[itr] )
				{
					::ZeroMemory(&input, sizeof(input));
					input.type = INPUT_KEYBOARD;
					input.ki.wVk = cVK;
					input.ki.dwFlags = KEYEVENTF_KEYUP;
					::SendInput(1, &input, sizeof(input));
					m_UIButtonPush[itr] = false;
				}
			}
		}

		//ESC - PVP
		bPush = IsPushJoypadButton( std::make_pair(m_pUIWrappingButton[IW_PVPTAB], m_pUIWrappingAssistButton[IW_PVPTAB]) );
		if( bPush )
		{
			m_pKeyboard->SetKey( m_pUIWrappingKey[IW_PVPTAB], bPush, LocalTime, m_JoypadTimeStamp[ m_pUIWrappingAssistButton[IW_PVPTAB] ] );
			m_UIButtonPush[IW_PVPTAB] = true;
		}
		else if( m_UIButtonPush[IW_PVPTAB] )
		{
			m_pKeyboard->SetKey( m_pUIWrappingKey[IW_PVPTAB], bPush, LocalTime, m_JoypadTimeStamp[ m_pUIWrappingAssistButton[IW_PVPTAB] ] );
			m_UIButtonPush[IW_PVPTAB] = false;
		}

		//마우스 처리
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_NORMALATTK], m_pWrappingAssistButton[IW_NORMALATTK]) );
		if( bPush )
		{
			if( !m_ButtonPush[IW_NORMALATTK] )
			{
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				::SendInput(1, &input, sizeof(input));
				m_ButtonPush[IW_NORMALATTK] = true;
			}
		}
		else
		{
			if( m_ButtonPush[IW_NORMALATTK] )
			{
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				::SendInput(1, &input, sizeof(input));
				m_ButtonPush[IW_NORMALATTK] = false;
			}
		}

		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_SPECIALATTK], m_pWrappingAssistButton[IW_SPECIALATTK]) );
		if( bPush )
		{
			if( !m_ButtonPush[IW_SPECIALATTK] )
			{
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				::SendInput(1, &input, sizeof(input));
				m_ButtonPush[IW_SPECIALATTK] = true;
			}
		}
		else
		{
			if( m_ButtonPush[IW_SPECIALATTK] )
			{
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				::SendInput(1, &input, sizeof(input));
				m_ButtonPush[IW_SPECIALATTK] = false;
			}
		}

		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_LOOKLEFT], m_pWrappingAssistButton[IW_LOOKLEFT]) );
		if( bPush )
		{
			POINT pt;
			GetCursorPos(&pt);
			pt.x -= (int)(m_fCursorSpeed * 100.0f);
			SetCursorPos(pt.x, pt.y);
		}
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_LOOKRIGHT], m_pWrappingAssistButton[IW_LOOKRIGHT]) );
		if( bPush )
		{
			POINT pt;
			GetCursorPos(&pt);
			pt.x += (int)(m_fCursorSpeed * 100.0f);
			SetCursorPos(pt.x, pt.y);
		}
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_LOOKUP], m_pWrappingAssistButton[IW_LOOKUP]) );
		if( bPush )
		{
			POINT pt;
			GetCursorPos(&pt);
			pt.y -= (int)(m_fCursorSpeed * 100.0f);
			SetCursorPos(pt.x, pt.y);
		}
		bPush = IsPushJoypadButton( std::make_pair(m_pWrappingButton[IW_LOOKDOWN], m_pWrappingAssistButton[IW_LOOKDOWN]) );
		if( bPush )
		{
			POINT pt;
			GetCursorPos(&pt);
			pt.y += (int)(m_fCursorSpeed * 100.0f);
			SetCursorPos(pt.x, pt.y);
		}
	}

	if( m_dwApplyBufferCount > 0 ) return true;
	return false;
}

bool CInputJoyPad::IsPushJoypadButton( std::pair<BYTE, BYTE> & cKeys )
{
	if( CInputJoyPad::NULL_VALUE == cKeys.second )
	{
		return IsPushButton( cKeys.first ) ? IsPushEnable(cKeys) : false;
	}
	else if( IsPushButton( cKeys.first ) && IsPushButton( cKeys.second ) )
	{
		LOCAL_TIME mainTime = GetPushButtonTime( cKeys.first );
		LOCAL_TIME assistTime = GetPushButtonTime( cKeys.second );

		return mainTime <= assistTime ? true : false;
	}

	return false;
}

//	CInputKeyboard 와 중복 코드. m_pDIDevice를 base 레벨로 올리는 것에 대해 고려 by kalliste
HRESULT CInputJoyPad::SetCooperativeLevel(DWORD flag)
{
	if( m_pDIDevice == NULL ) return DIERR_NOTFOUND;

	return m_pDIDevice->SetCooperativeLevel( CInputDevice::GetInstance().GetWndHandle(), flag );
}

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{

	LPDIRECTINPUTDEVICE8 pJoystic = (LPDIRECTINPUTDEVICE8)pContext;

	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg; 
		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin              = -1000; 
		diprg.lMax              = +1000; 

		// Set the range for the axis
		if( FAILED( pJoystic->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
			return DIENUM_STOP;

	}

	return true;
}

void CInputJoyPad::SetWrappingButton(BYTE * pWrappingButton, BYTE * pWrappingAssistButton, BYTE * pUIWrappingButton, BYTE * pUIWrappingAssistButton, int wrappingSize, int uiWrappingSize)
{
	m_pWrappingButton = pWrappingButton;
	m_pWrappingAssistButton = pWrappingAssistButton;
	m_pUIWrappingButton = pUIWrappingButton;
	m_pUIWrappingAssistButton = pUIWrappingAssistButton;

	m_iWrappingSize = wrappingSize;
	m_iUIWrappingSize = uiWrappingSize;
}

void CInputJoyPad::SetWrappingKey( BYTE *pWrappingKey, BYTE *pUIWrappingKey )
{
	m_pWrappingKey = pWrappingKey;
	m_pUIWrappingKey = pUIWrappingKey;
}

/*
	단일키는 복수 키에 씹히게 만들어야 한다.
*/
bool CInputJoyPad::IsPushEnable( std::pair<BYTE, BYTE> & cKeys )
{
	if( !m_pWrappingButton || !m_pWrappingAssistButton || !m_pUIWrappingButton || !m_pUIWrappingAssistButton )
		return false;

	//복수 키거나 메인키가 없으면 리턴한다.
	if( NULL_VALUE == cKeys.first || NULL_VALUE != cKeys.second)
		return false;

	for( int itr = 0; itr < m_iWrappingSize; ++itr )
	{
		if( cKeys.first == m_pWrappingAssistButton[itr] )
		{
			if( m_bButtonState[ m_pWrappingButton[itr] ] )
				return false;
		}
	}

	for( int itr = 0; itr < m_iUIWrappingSize; ++itr )
	{
		if( cKeys.first == m_pUIWrappingAssistButton[itr] )
		{
			if( m_bButtonState[ m_pUIWrappingButton[itr] ] )
				return false;
		}
	}

	return true;
}

void CInputJoyPad::SetKeyboard( CInputKeyboard * pKeyboard ) 
{ 
	m_pKeyboard = pKeyboard; 
}
void CInputJoyPad::SetMouse( CInputMouse * pMouse ) 
{ 
	m_pMouse = pMouse; 
}