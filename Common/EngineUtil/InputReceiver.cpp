#include "StdAfx.h"
#include "InputReceiver.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CInputReceiver::CInputReceiver( bool bUseEventMsg )
{
	m_State = 0;
	m_bUseEventMsg = bUseEventMsg;
	m_pDeviceKeyboard = NULL;
	m_pDeviceMouse = NULL;
	m_pDeviceJoypad = NULL;

	memset( m_bPrevKeyState, 0, sizeof(m_bPrevKeyState) );
	memset( m_PrevKeyTime, 0, sizeof(m_PrevKeyTime) );
	memset( m_PrevButtonTime, 0, sizeof(m_PrevButtonTime) );
	memset( m_bPrevJoypadState, 0, sizeof(m_bPrevJoypadState) );
	memset( m_PrevJoypadTime, 0, sizeof(m_PrevJoypadTime) );

	if( bUseEventMsg ) {
		 UseEventMessage( true );
	}
	ResetDevicePointer( CInputDevice::KEYBOARD | CInputDevice::MOUSE | CInputDevice::JOYPAD );
}

CInputReceiver::~CInputReceiver()
{
	if( m_bUseEventMsg ) {
		UseEventMessage( false );
	}
}

D3DXVECTOR2 CInputReceiver::GetMousePosByScreen()
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return D3DXVECTOR2( 0.f, 0.f );

	return *m_pDeviceMouse->GetMousePos();
}

D3DXVECTOR3 CInputReceiver::GetMouseVariation()
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return D3DXVECTOR3( 0.f, 0.f, 0.f );

	return *m_pDeviceMouse->GetMouseVariatioin();
}

bool CInputReceiver::IsPushJoypadButton(std::pair<BYTE, BYTE> & cKeys)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) || CInputJoyPad::NULL_VALUE == cKeys.first ) return false;

	if( CInputJoyPad::NULL_VALUE == cKeys.second )
	{
		return m_pDeviceJoypad->IsPushButton( cKeys.first ) ? m_pDeviceJoypad->IsPushEnable(cKeys) : false;
	}
	else if( m_pDeviceJoypad->IsPushButton( cKeys.first ) && m_pDeviceJoypad->IsPushButton( cKeys.second ) )
	{
		LOCAL_TIME mainTime = m_pDeviceJoypad->GetPushButtonTime( cKeys.first );
		LOCAL_TIME assistTime = m_pDeviceJoypad->GetPushButtonTime( cKeys.second );

		return mainTime <= assistTime ? true : false;
	}

	return false;
}

void CInputReceiver::ReleaseJoypadButton(std::pair<BYTE, BYTE> cKeys)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	if( CInputJoyPad::NULL_VALUE != cKeys.second ) 
		m_pDeviceJoypad->ReleasePushButton( cKeys.second );

	m_pDeviceJoypad->ReleasePushButton( cKeys.first );
}

void CInputReceiver::SetPushJoypadButtonTime(std::pair<BYTE, BYTE> cKeys, LOCAL_TIME LocalTime)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;
	
	if( CInputJoyPad::NULL_VALUE != cKeys.second ) 
		m_pDeviceJoypad->SetPushButtonTime( cKeys.second, LocalTime );
	
	m_pDeviceJoypad->SetPushButtonTime( cKeys.first, LocalTime );
}

void CInputReceiver::SetReleaseJoypadButtonTime(std::pair<BYTE, BYTE> cKeys, LOCAL_TIME LocalTime)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	if( CInputJoyPad::NULL_VALUE != cKeys.second ) 
		m_pDeviceJoypad->SetReleaseButtonTime( cKeys.second, LocalTime );

	m_pDeviceJoypad->SetReleaseButtonTime( cKeys.first, LocalTime );
}

LOCAL_TIME CInputReceiver::GetPushJoypadButtonTime( std::pair<BYTE, BYTE> cKeys )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0;

	if( CInputJoyPad::NULL_VALUE == cKeys.second )
		return m_pDeviceJoypad->GetPushButtonTime( cKeys.first );
	else
		return m_pDeviceJoypad->GetPushButtonTime( cKeys.first ) <= m_pDeviceJoypad->GetPushButtonTime( cKeys.second ) ?
		m_pDeviceJoypad->GetPushButtonTime( cKeys.second ) : m_pDeviceJoypad->GetPushButtonTime( cKeys.first );
}

LOCAL_TIME CInputReceiver::GetReleaseJoypadButtonTime( std::pair<BYTE, BYTE> cKeys )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0;

	if( CInputJoyPad::NULL_VALUE == cKeys.second )
		return m_pDeviceJoypad->GetReleaseButtonTime( cKeys.first );
	else
		return m_pDeviceJoypad->GetReleaseButtonTime( cKeys.first ) <= m_pDeviceJoypad->GetReleaseButtonTime( cKeys.second ) ?
		m_pDeviceJoypad->GetReleaseButtonTime( cKeys.second ) : m_pDeviceJoypad->GetReleaseButtonTime( cKeys.first );
}

float CInputReceiver::GetPushJoypadButtonDelta( std::pair<BYTE, BYTE> cKeys, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0.f;

	float fDelta;

	if( CInputJoyPad::NULL_VALUE == cKeys.second )
	{
		if( m_PrevJoypadTime[cKeys.first] == 0 ) fDelta = 0.f;
		else fDelta = ( LocalTime - m_PrevJoypadTime[cKeys.first] ) / 1000.f;

		m_PrevJoypadTime[cKeys.first] = LocalTime;
	}
	else
	{
		if( m_PrevJoypadTime[cKeys.second] == 0 ) fDelta = 0.f;
		else fDelta = ( LocalTime - m_PrevJoypadTime[cKeys.second] ) / 1000.f;

		m_PrevJoypadTime[cKeys.second] = LocalTime;
	}
	return fDelta;
}

void CInputReceiver::ResetPushJoypadButtonDelta( std::pair<BYTE, BYTE> cKeys, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	if( CInputJoyPad::NULL_VALUE != cKeys.second ) 
		m_PrevJoypadTime[cKeys.second] = LocalTime;

	m_PrevJoypadTime[cKeys.first] = LocalTime;
}

bool CInputReceiver::IsPushKey( BYTE cKey )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return false;

	bool bResult = m_pDeviceKeyboard->IsPushKey( cKey );
	if( bResult == false ) {
		m_bPrevKeyState[cKey] = bResult;
		return false;
	}

	if( m_bPrevKeyState[cKey] == false ) m_PrevKeyTime[cKey] = 0;

	m_bPrevKeyState[cKey] = bResult;
	return true;
}

void CInputReceiver::ReleasePushKey( BYTE cKey )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return;
	m_pDeviceKeyboard->ReleasePushKey( cKey );
}

float CInputReceiver::GetPushKeyDelta( BYTE cKey, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return 0.f;

	float fDelta;

	if( m_PrevKeyTime[cKey] == 0 ) fDelta = 0.f;
	else fDelta = ( LocalTime - m_PrevKeyTime[cKey] ) / 1000.f;

	m_PrevKeyTime[cKey] = LocalTime;
	return fDelta;
}

LOCAL_TIME CInputReceiver::GetPushKeyTime( BYTE cKey )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return 0;
	return m_pDeviceKeyboard->GetPushKeyTime( cKey );
}

LOCAL_TIME CInputReceiver::GetReleaseKeyTime( BYTE cKey )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return 0;
	return m_pDeviceKeyboard->GetReleaseKeyTime( cKey );
}

void CInputReceiver::ResetPushKeyDelta( BYTE cKey, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return;
	m_PrevKeyTime[cKey] = LocalTime;
}

bool CInputReceiver::IsPushMouseButton( BYTE cButtonIndex )
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return false;

	return m_pDeviceMouse->IsPushButton( cButtonIndex );
}

void CInputReceiver::ReleasePushButton( BYTE cButtonIndex )
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return;
	m_pDeviceMouse->ReleasePushButton( cButtonIndex );
}

float CInputReceiver::GetPushButtonDelta( BYTE cButtonIndex, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return 0.f;

	float fDelta;

	if( m_PrevButtonTime[cButtonIndex] == 0 ) fDelta = 0.f;
	else fDelta = ( LocalTime - m_PrevKeyTime[cButtonIndex] ) / 1000.f;

	m_PrevButtonTime[cButtonIndex] = LocalTime;
	return fDelta;
}

void CInputReceiver::ResetPushButtonDelta( BYTE cButtonIndex, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return;
	m_PrevKeyTime[cButtonIndex] = LocalTime;
}

DWORD CInputReceiver::GetEventKeyCount()
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return 0;

	std::vector<BYTE> *pVecList = m_pDeviceKeyboard->GetEventKeyList();
	return (DWORD)pVecList->size();
}

BYTE CInputReceiver::GetEventKey( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return 0;

	std::vector<BYTE> *pVecList = m_pDeviceKeyboard->GetEventKeyList();
	if( dwIndex >= pVecList->size() ) return 0;
	return (*pVecList)[dwIndex];
}

bool CInputReceiver::GetEventKeyState( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return false;

	std::vector<BYTE> *pVecList = m_pDeviceKeyboard->GetEventKeyList();
	if( dwIndex >= pVecList->size() ) return false;
	return m_pDeviceKeyboard->IsPushKey( (*pVecList)[dwIndex] );
}

LOCAL_TIME CInputReceiver::GetEventKeyTime( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return false;
	return m_pDeviceKeyboard->GetEventTime( dwIndex );
}

void CInputReceiver::SetEventKeyTime( DWORD dwIndex, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return;
	return m_pDeviceKeyboard->SetEventTime( dwIndex, LocalTime );
}

DWORD CInputReceiver::GetEventJoypadCount()
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0;

	std::vector<BYTE> *pVecList = m_pDeviceJoypad->GetEventList();
	return (DWORD)pVecList->size();
}

BYTE CInputReceiver::GetEventJoypad( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0;

	std::vector<BYTE> *pVecList = m_pDeviceJoypad->GetEventList();
	if( dwIndex >= pVecList->size() ) return 0;
	return (*pVecList)[dwIndex];
}

bool CInputReceiver::GetEventJoypadState( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return false;

	std::vector<BYTE> *pVecList = m_pDeviceJoypad->GetEventList();
	if( dwIndex >= pVecList->size() ) return false;
	return m_pDeviceJoypad->IsPushButton( (*pVecList)[dwIndex] );
}

LOCAL_TIME CInputReceiver::GetEventJoypadTime( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return false;
	return m_pDeviceJoypad->GetEventTime( dwIndex );
}

void CInputReceiver::SetEventJoypadTime( DWORD dwIndex, LOCAL_TIME LocalTime )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;
	return m_pDeviceJoypad->SetEventTime( dwIndex, LocalTime );
}

DWORD CInputReceiver::GetEventMouseCount()
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return 0;

	std::vector<int> *pVecList = m_pDeviceMouse->GetEventList();
	return (DWORD)pVecList->size();
}

int CInputReceiver::GetEventMouse()
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return 0;
	return m_pDeviceMouse->GetEvent();
}

int CInputReceiver::GetEventMouse( DWORD dwIndex )
{
	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return 0;

	std::vector<int> *pVecList = m_pDeviceMouse->GetEventList();
	if( dwIndex >= pVecList->size() ) return 0;
	return (*pVecList)[dwIndex];
}

LOCAL_TIME CInputReceiver::GetEventMouseTime( IR_STATE State )
{
	switch( State ) {
		case IR_MOUSE_MOVE:		return m_pDeviceMouse->GetEventTime(0);
		case IR_MOUSE_WHEEL:	return m_pDeviceMouse->GetEventTime(1);
		case IR_MOUSE_LB_DOWN:	return m_pDeviceMouse->GetEventTime(2);
		case IR_MOUSE_LB_UP:	return m_pDeviceMouse->GetEventTime(3);
		case IR_MOUSE_RB_DOWN:	return m_pDeviceMouse->GetEventTime(4);
		case IR_MOUSE_RB_UP:	return m_pDeviceMouse->GetEventTime(5);
		case IR_MOUSE_WB_DOWN:	return m_pDeviceMouse->GetEventTime(6);
		case IR_MOUSE_WB_UP:	return m_pDeviceMouse->GetEventTime(7);
		case IR_MOUSE_LB_DBCLK: return m_pDeviceMouse->GetEventTime(8);
		case IR_MOUSE_RB_DBCLK:	return m_pDeviceMouse->GetEventTime(9);
		case IR_MOUSE_WB_DBCLK: return m_pDeviceMouse->GetEventTime(10);
	}
	return -1;
}

void CInputReceiver::SetEventMouseTime( IR_STATE State, LOCAL_TIME LocalTime )
{
	switch( State ) {
		case IR_MOUSE_MOVE:		return m_pDeviceMouse->SetEventTime(0, LocalTime);
		case IR_MOUSE_WHEEL:	return m_pDeviceMouse->SetEventTime(1, LocalTime);
		case IR_MOUSE_LB_DOWN:	return m_pDeviceMouse->SetEventTime(2, LocalTime);
		case IR_MOUSE_LB_UP:	return m_pDeviceMouse->SetEventTime(3, LocalTime);
		case IR_MOUSE_RB_DOWN:	return m_pDeviceMouse->SetEventTime(4, LocalTime);
		case IR_MOUSE_RB_UP:	return m_pDeviceMouse->SetEventTime(5, LocalTime);
		case IR_MOUSE_WB_DOWN:	return m_pDeviceMouse->SetEventTime(6, LocalTime);
		case IR_MOUSE_WB_UP:	return m_pDeviceMouse->SetEventTime(7, LocalTime);
		case IR_MOUSE_LB_DBCLK: return m_pDeviceMouse->SetEventTime(8, LocalTime);
		case IR_MOUSE_RB_DBCLK:	return m_pDeviceMouse->SetEventTime(9, LocalTime);
		case IR_MOUSE_WB_DBCLK: return m_pDeviceMouse->SetEventTime(10, LocalTime);
	}
}

bool CInputReceiver::ResetDevicePointer( int nDeviceType )
{
	bool bResult = false;
	if( !CInputDevice::IsActive() ) return false;

	if( ( nDeviceType & CInputDevice::KEYBOARD ) && m_pDeviceKeyboard != NULL ) bResult = true;
	if( ( nDeviceType & CInputDevice::MOUSE ) && m_pDeviceMouse != NULL ) bResult = true;
	if( ( nDeviceType & CInputDevice::JOYPAD ) && m_pDeviceJoypad != NULL ) bResult = true;
	if( bResult == true ) return true;

	m_pDeviceKeyboard = (CInputKeyboard *)CInputDevice::GetInstance().GetDeviceObject( CInputDevice::KEYBOARD );
	m_pDeviceMouse = (CInputMouse *)CInputDevice::GetInstance().GetDeviceObject( CInputDevice::MOUSE );
	m_pDeviceJoypad = (CInputJoyPad *)CInputDevice::GetInstance().GetDeviceObject( CInputDevice::JOYPAD );

	if( ( nDeviceType & CInputDevice::KEYBOARD ) && m_pDeviceKeyboard == NULL ) return false;
	if( ( nDeviceType & CInputDevice::MOUSE ) && m_pDeviceMouse == NULL ) return false;
	if( ( nDeviceType & CInputDevice::JOYPAD ) && m_pDeviceJoypad == NULL ) return false;

	return true;
}

void CInputReceiver::ReleaseAllButton()
{
	if( !ResetDevicePointer( CInputDevice::KEYBOARD ) ) return;
	m_pDeviceKeyboard->ResetDeviceState();

	if( !ResetDevicePointer( CInputDevice::MOUSE ) ) return;
	m_pDeviceMouse->ResetDeviceState();

	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;
	m_pDeviceJoypad->ResetDeviceState();
}

void CInputReceiver::UseEventMessage( bool bUse )
{
	if( !CInputDevice::IsActive() ) return;

	if( bUse ) CInputDevice::GetInstance().AddReceiverObject( this );
	else CInputDevice::GetInstance().RemoveReceiverObject( this );

	m_bUseEventMsg = bUse;
}

void CInputReceiver::SetPadWrappingButton(BYTE * pWrappingButton, BYTE * pWrappingAssistButton, BYTE * pUIWrappingButton, BYTE * pUIWrappingAssistButton, int wrappingSize, int uiWrappingSize)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	m_pDeviceJoypad->SetWrappingButton(pWrappingButton, pWrappingAssistButton, pUIWrappingButton, pUIWrappingAssistButton, wrappingSize, uiWrappingSize);
}

void CInputReceiver::SetWrappingKey( BYTE * pWrappingKey, BYTE * pUIWrappingKey )
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	m_pDeviceJoypad->SetWrappingKey( pWrappingKey, pUIWrappingKey );
}

float CInputReceiver::GetCursorSpeed()
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0.0f;

	return m_pDeviceJoypad->GetCursorSpeed();
}

float CInputReceiver::GetCameraSpeed()
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return 0.0f;

	return m_pDeviceJoypad->GetCameraSpeed();
}

void CInputReceiver::SetCursorSpeed(float fSpeed)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	m_pDeviceJoypad->SetCursorSpeed(fSpeed);
}

void CInputReceiver::SetCameraSpeed(float fSpeed)
{
	if( !ResetDevicePointer( CInputDevice::JOYPAD ) ) return;

	m_pDeviceJoypad->SetCameraSpeed(fSpeed);
}