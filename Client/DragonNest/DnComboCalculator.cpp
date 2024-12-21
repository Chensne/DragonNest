#include "StdAfx.h"
#include "DnComboCalculator.h"
#include "InputWrapper.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnComboCalculator::CDnComboCalculator( int iCheckMethod ) : m_iCheckMethod( iCheckMethod ),
															 m_iLastKeyIndex( -1 ),
															 m_iLastPadIndex( -1 ),
															 m_fPadRatio( 0.0f ),
															 m_fPadAccum( 0.0f )
{
}

CDnComboCalculator::~CDnComboCalculator(void)
{

}


void CDnComboCalculator::SetPadsToCheck( std::vector<BYTE>& vlPadsToCheck, float DurationTime )
{ 
	m_vlPadsToCheck = vlPadsToCheck; 
	
	m_iLastPadIndex = -1; 
	
	m_fPadAccum = .0f;
	m_fPadRatio =  DurationTime / (float)vlPadsToCheck.size();
}


bool CDnComboCalculator::Process( LOCAL_TIME LocalTime, float fDelta, int* piComboCount )
{
	bool bComboSatisfy = false;
	*piComboCount = 0;

	//CInputJoyPad* pJoypad = static_cast<CInputJoyPad*>(CInputDevice::GetInstancePtr()->GetDeviceObject( CInputDevice::JOYPAD ));

	CInputKeyboard* pKeyboard = static_cast<CInputKeyboard*>(CInputDevice::GetInstancePtr()->GetDeviceObject( CInputDevice::KEYBOARD ));
	vector<BYTE>* pvlNowKeyEvents = pKeyboard->GetEventKeyList();
	// IsPushKey() 로 알아볼 수 있군..

#ifndef _FINAL_BUILD
	if( false == pvlNowKeyEvents->empty() )
	{
		OutputDebug( "[Freezing - KeyEventList(%d)] ", (int)pvlNowKeyEvents->size() );
		for( int i = 0; i < (int)pvlNowKeyEvents->size(); ++i )
		{
			if( pKeyboard->IsPushKey(pvlNowKeyEvents->at(i)) )
				OutputDebug( "%d ", pvlNowKeyEvents->at(i) );
		}
		OutputDebug( "\n" );
	}
#endif

	switch( m_iCheckMethod )
	{
		case CIRCULAR_CHECK:
			{
				// 조이패드 처리
				if( CDnActor::s_hLocalActor ) 
				{
					if( -1 == m_iLastPadIndex )
						m_iLastPadIndex = 0;

					CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
					std::vector<BYTE>::iterator Itor;
					if( localActor->IsJoypad() && !m_vlPadsToCheck.empty() )
					{
						bool bComboFailed = false;

						for( std::vector<BYTE>::iterator Itor = m_vlPadsToCheck.begin(); Itor != m_vlPadsToCheck.end(); ++Itor )
						{
							if ( localActor->IsPushJoypadButton( IW_PAD( (*Itor) ) ) )
							{
								// 이전 값과 같지 않아야 한다.
								if( m_iLastPadIndex != (*Itor) )
								{
									m_iLastPadIndex = (*Itor);
									m_vlPadsToCheck.erase(Itor);

									m_fPadAccum += m_fPadRatio;

									if( m_fPadAccum > 1.0f )
									{
										m_fPadAccum = 1.0f - m_fPadAccum;

										(*piComboCount) = 1;
									}
									return true;
								}
							}
						}
					}
				}

				if( -1 == m_iLastKeyIndex )
					m_iLastKeyIndex = 0;

				int iNumNowEventKeys = (int)pvlNowKeyEvents->size();
				for( int iKey = 0; iKey < iNumNowEventKeys; ++iKey )
				{
					BYTE KeyEvent = pvlNowKeyEvents->at( iKey );
					bool bComboFailed = false;

					// Key Up 이벤트는 패스 시킴
					if( false == pKeyboard->IsPushKey(KeyEvent) )
						continue;

					// 콤보 계산에 관계없는 키들은 skip 시킨다.
					vector<BYTE>::iterator iter = find( m_vlKeysToCheck.begin(), m_vlKeysToCheck.end(), KeyEvent );
					if( m_vlKeysToCheck.end() != iter )
					{
						if( m_vlKeysToCheck.at(m_iLastKeyIndex) == KeyEvent )
						{		
							// 콤보 성립, 진행시킴
							m_iLastKeyIndex = (m_iLastKeyIndex+1) % (int)m_vlKeysToCheck.size();

							if( piComboCount && m_iLastKeyIndex == 0 )
							{
								bComboSatisfy = true;
								++(*piComboCount);
							}
						}
						else
							bComboFailed = true;
					}
					else
					{
						bComboFailed = true;
					}

					if( bComboFailed )
					{
						// 엉뚱한 키가 들어오면 곧바로 리턴시킴. 처음부터 콤보 다시 해야 함.
						m_iLastKeyIndex = -1;
						return bComboSatisfy;
					}
				}
				
				//vector<BYTE>::iterator iter = find( m_vlKeysToCheck.begin(), m_vlKeysToCheck.end(), LastKey );
				//if( m_vlKeysToCheck.end() != iter )
				//	m_iLastKeyIndex = distance( m_vlKeysToCheck.begin(), iter );
			}
			break;
	}

	return bComboSatisfy;
}