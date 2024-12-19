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

	// �̷��� �������ϸ� ��ư�� ���� �����ٰ� �������� �������������,
	// �÷���ư�� �۾��� ���� �����װ�, ���� �������ϴ°� �ڵ� ����� ���� �ʰڴ�.
	SUIElement *pElement = GetElement( 1 );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
}

void CDnColorButton::MouseEnter( bool bEnter )
{
	// Enter���·� �ٲ��� �ʴ´�.
	return;
}