#include "StdAfx.h"
#include "DnMessageStatic.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnMessageStatic::CDnMessageStatic( CEtUIDialog *pParent )
	: CEtUIStatic( pParent )
	, m_fMessageDisplayTime(0.0f)
{
}

CDnMessageStatic::~CDnMessageStatic(void)
{
}

void CDnMessageStatic::Process( float fElapsedTime )
{
	CEtUIStatic::Process( fElapsedTime );

	if( IsShow() )
	{
		if( m_fMessageDisplayTime <= 0.0f )
		{
			Show( false );
		}

		m_fMessageDisplayTime -= fElapsedTime;
	}
}