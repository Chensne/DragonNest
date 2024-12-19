#include "StdAfx.h"
#include "DnBubble.h"

namespace BubbleSystem
{

CDnBubble::CDnBubble( void ) : m_iTypeID( -1 ), m_fDurationTime( 0.0f ), m_fRemainTime( 0.0f ), m_iIconIndex( 0 )
{

}

CDnBubble::~CDnBubble( void )
{

}

void CDnBubble::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_fRemainTime -= fDelta;
	if( m_fRemainTime < 0.0f )
		m_fRemainTime = 0.0f;
}

bool CDnBubble::IsEnd( void )
{
	bool bResult = false;

	if( m_fRemainTime <= 0.0f )
		bResult = true;

	return bResult;
}

};