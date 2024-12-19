#include "StdAfx.h"
#include "DnLoadingButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define DEFAULT_LOADINGBTN_ANIMATION_INTERVAL 0.3f
#define MAX_ANIMATION_SEC_FOR_ONE_ROUND 10.f

CDnLoadingButton::CDnLoadingButton( CEtUIDialog *pParent )
: CEtUIButton( pParent )
, m_FrameCount( 0 )
, m_Interval( 0.f )
, m_CurrentElemIdx( 0)
, m_AnimationTimer( 0.f )
, m_bAnimation( false )
{
}

CDnLoadingButton::~CDnLoadingButton(void)
{
}

void CDnLoadingButton::Initialize(SUIControlProperty *pProperty)
{
	CEtUIButton::Initialize(pProperty);

	m_FrameCount = GetElementCount();
}

void CDnLoadingButton::SetAnimation(bool bSet)
{
	SetAnimation(bSet, DEFAULT_LOADINGBTN_ANIMATION_INTERVAL);
}

void CDnLoadingButton::SetAnimation(bool bSet, float sec)
{
	if (m_FrameCount <= 0)
	{
		m_bAnimation = false;
		m_CurrentElemIdx = 0;
		m_Interval = 0.f;
		m_AnimationTimer = 0.f;
	}
	else
	{
		m_bAnimation = bSet;
		m_CurrentElemIdx = 0;
		m_Interval = (bSet) ? (sec / m_FrameCount) : (DEFAULT_LOADINGBTN_ANIMATION_INTERVAL / m_FrameCount);
		m_AnimationTimer = 0.f;
	}
}

void CDnLoadingButton::IncreseAnimationSpeed(float incSecForOneRound)
{
	float newInterval = m_Interval - incSecForOneRound;
	if (newInterval > 0.f)
		m_Interval = newInterval;
}

void CDnLoadingButton::DecreaseAnimationSpeed(float decSecForOneRound)
{
	float newInterval = m_Interval + decSecForOneRound;
	if (newInterval < MAX_ANIMATION_SEC_FOR_ONE_ROUND)
		m_Interval = newInterval;
}

void CDnLoadingButton::Render( float fElapsedTime )
{
	m_CurrentState = UI_STATE_NORMAL;
	m_MoveButtonCoord = m_Property.UICoord;

	if (!IsShow())
		m_CurrentState = UI_STATE_HIDDEN;
	else if (!IsEnable())
		m_CurrentState = UI_STATE_DISABLED;
	else if (m_bPressed)
		m_CurrentState = UI_STATE_PRESSED;

	UpdateBlendRate();

	if (m_bAnimation && (m_FrameCount > 0))
	{
		if (m_AnimationTimer > m_Interval)
		{
			m_CurrentElemIdx = (++m_CurrentElemIdx % m_FrameCount);
			m_AnimationTimer = 0.f;
		}

		m_AnimationTimer += fElapsedTime;
	}
	else
	{
		m_CurrentElemIdx = 0;
	}

	SUIElement *pElement = GetElement(m_CurrentElemIdx);
	if (pElement)
	{
		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
	}
}