#include "StdAfx.h"
#include "DnSkillSlotButton.h"
#include "DnCustomControlCommon.h"
#include "DnSkill.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSkillSlotButton::CDnSkillSlotButton( CEtUIDialog *pParent )
	: CDnSlotButton( pParent ), m_ButtonState(SKILLSLOT_DISABLE)
{
}

CDnSkillSlotButton::~CDnSkillSlotButton(void)
{
}

void CDnSkillSlotButton::Initialize( SUIControlProperty *pProperty )
{
	CDnSlotButton::Initialize( pProperty );
}

//void CDnSkillSlotButton::UpdateRects()
//{
//	m_IconCoord.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.fY, SKILLSLOT_ICON_XSIZE / 1024.0f, m_Property.UICoord.fHeight );
//}

void CDnSkillSlotButton::Render( float fElapsedTime )
{
	if( !m_pItem || IsShow() == false)
		return;

	CDnSlotButton::Render(fElapsedTime);

	DWORD dwColor = m_dwIconColor;

	if( IsDragMode() || !IsEnable() || m_bRegist )
	{
		dwColor = 0xff808080;
	}

	m_pParent->DrawSprite( m_hSkillIcon, m_IconUV, dwColor, m_Property.UICoord );

	SUIElement *pElement = GetElement(0);
	if( pElement )
	{
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, dwColor, m_Property.UICoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, dwColor, m_Property.UICoord );
	}
}

void CDnSkillSlotButton::RenderDrag( float fElapsedTime )
{
	if (IsDragMode() == false)
		return;

	SUIElement *pElement;
	pElement = GetElement(0);

	POINT CursorPoint;
	GetCursorPos( &CursorPoint );
	ScreenToClient( GetEtDevice()->GetHWnd(), &CursorPoint );
	m_CursorCoord = m_Property.UICoord;
	m_CursorCoord.fWidth /= m_pParent->GetScreenWidthRatio();
	m_CursorCoord.fHeight /= m_pParent->GetScreenHeightRatio();
	m_CursorCoord.fX = CursorPoint.x / ( float )GetEtDevice()->Width() - m_CursorCoord.fWidth * 0.5f;
	m_CursorCoord.fY = CursorPoint.y / ( float )GetEtDevice()->Height() - m_CursorCoord.fHeight * 0.5f;

	CEtSprite::GetInstance().DrawSprite( ( EtTexture * )m_hSkillIcon->GetTexturePtr(), m_hSkillIcon->Width(), m_hSkillIcon->Height(), 
		m_IconUV, 0xffffffff, m_CursorCoord, 0.0f );

	EtTextureHandle hTexture = m_pParent->GetUITexture();
	if( m_bExistTemplateTexture )
		hTexture = m_Template.m_hTemplateTexture;
	if( !hTexture ) return;
	SUICoord UVCoord = pElement->UVCoord;
	if( m_bExistTemplateTexture )
		UVCoord = pElement->TemplateUVCoord;
	CEtSprite::GetInstance().DrawSprite( ( EtTexture * )hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), 
		UVCoord, pElement->TextureColor.dwCurrentColor, m_CursorCoord, 0.0f );
}