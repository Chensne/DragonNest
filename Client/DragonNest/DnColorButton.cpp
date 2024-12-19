#include "StdAfx.h"
#include "DnColorButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnColorButton::CDnColorButton( CEtUIDialog *pParent )
: CEtUIButton( pParent )
{
}

CDnColorButton::~CDnColorButton(void)
{
}

void CDnColorButton::Render( float fElapsedTime )
{
	CEtUIButton::Render( fElapsedTime );

	// 이렇게 렌더링하면 버튼의 글자 위에다가 렌더링을 덮어버리겠지만,
	// 컬러버튼에 글씨가 들어가지 않을테고, 전부 재정의하는건 코드 복사라 하지 않겠다.
	SUIElement *pElement = GetElement( 1 );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
}

void CDnColorButton::MouseEnter( bool bEnter )
{
	// Enter상태로 바꾸지 않는다.
	return;
}