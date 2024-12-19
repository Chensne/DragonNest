#include "stdafx.h"
#include "DnClimaxEffect.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnClimaxEffect::CDnClimaxEffect()
: m_pBaseTexture( NULL )
, m_pScaleTexture( NULL )
, m_bScaleProcess( false )
, m_fScaleAfterShowTime( 1.f )
, m_fScaleAfterShowElapsedTime( 0.f )
, m_fScaleShowTime( 0.1f )
, m_fScaleTime( 0.f )
, m_fScaleAccel( 0.f )
, m_fScaleInit( 1.5f )
, m_fHalfTime( 0.f )
, m_fHalfShowTime( 0.f )
, m_bContinue( false )
{

}

CDnClimaxEffect::~CDnClimaxEffect()
{
	SAFE_RELEASE_SPTR( m_hClimaxTexture );
}

void CDnClimaxEffect::Init( float fScaleShowTime, float fScaleAccel, float fScaleInit, float fScaleAfterShowTime, float fHalfShowTime, bool bContinue )
{
	m_fScaleShowTime = fScaleShowTime;
	m_fScaleAccel = fScaleAccel;
	m_fScaleInit = fScaleInit;
	m_fScaleAfterShowTime = fScaleAfterShowTime;
	m_fHalfShowTime = fHalfShowTime;
	m_bContinue = bContinue;

	m_pBaseTexture->Show( false );
	m_pScaleTexture->Show( false );
}

void CDnClimaxEffect::Process( float fElapsedTime )
{
	if( m_bScaleProcess )
	{
		if( m_fScaleTime < m_fScaleShowTime )
		{
			if( 0.f == m_fScaleTime )
			{
				m_pScaleTexture->SetTextureColor(m_pScaleTexture->GetTextureColor());
				m_pScaleTexture->SetManualControlColor(true);
				m_pScaleTexture->Show( true );
			}

			m_fScaleTime += fElapsedTime;

			float ratio = 1.f - ((m_fScaleShowTime - m_fScaleTime) / m_fScaleShowTime);

			float scale = m_fScaleInit - ((m_fScaleInit - 1.f) * ratio) * m_fScaleAccel;
			m_fScaleAccel += 0.2f;
			if (scale < 1.f)
				scale = 1.f;
			m_pScaleTexture->SetScale(scale);

			EtColor color(m_pScaleTexture->GetTextureColor());
			color.a = float(pow(4 * m_fScaleTime, 2));
			if (color.a > 1.f)
				color.a = 1.f;
			m_pScaleTexture->SetTextureColor(color);

			EtVector2 prev, cur, result;
			GetUICtrlCenter(prev, m_pScaleTexture, 1.f);
			GetUICtrlCenter(cur, m_pScaleTexture, scale);
			result = prev - cur;

			SUICoord curCoord;
			m_pScaleTexture->GetUICoord(curCoord);
			m_pScaleTexture->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
		}
		else if( m_fHalfTime < m_fHalfShowTime )
		{
			m_fHalfTime += fElapsedTime;
		}
		else if( m_fScaleAfterShowElapsedTime < m_fScaleAfterShowTime )
		{
			if (m_fScaleAfterShowElapsedTime == 0.f)
			{
				m_pScaleTexture->SetScale(1.f);
				m_pScaleTexture->Show( m_bContinue );
				m_pBaseTexture->SetTextureColor(m_pBaseTexture->GetTextureColor());
				m_pBaseTexture->SetManualControlColor(true);
				m_pBaseTexture->Show(true);
				m_fScaleAccel = 1.f;
			}

			m_fScaleAfterShowElapsedTime += fElapsedTime;
			float ratio = (m_fScaleAfterShowTime - m_fScaleAfterShowElapsedTime) / m_fScaleAfterShowTime;
			EtColor color(m_pBaseTexture->GetTextureColor());
			color.a = 0.7f - float(pow(4 * m_fScaleAfterShowElapsedTime, 2));
			if (color.a < 0.f)
				color.a = 0.f;
			m_pBaseTexture->SetTextureColor(color);
			float scale = 1.f + log(10.f * m_fScaleAfterShowElapsedTime + 1);
			m_pBaseTexture->SetScale(scale);

			EtVector2 prev, cur, result;
			GetUICtrlCenter(prev, m_pBaseTexture, 1.f);
			GetUICtrlCenter(cur, m_pBaseTexture, scale);
			result = prev - cur;

			SUICoord curCoord;
			m_pBaseTexture->GetUICoord(curCoord);
			m_pBaseTexture->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
		}
		else
		{
			if( m_bContinue )
			{
				m_fScaleTime = 0.f;
				m_fScaleAfterShowElapsedTime = 0.f;
				m_fHalfTime = 0.f;
			}
			else
			{
				m_pBaseTexture->Show( m_bContinue );
				m_pScaleTexture->Show( m_bContinue );
				m_bScaleProcess = false;
			}
		}
	}
}

void CDnClimaxEffect::GetUICtrlCenter(EtVector2& result, CEtUIControl* ctrl, float scale)
{
	if (ctrl == NULL)
	{
		_ASSERT(0);
		return;
	}
	SUICoord curCoord;
	ctrl->GetUICoord(curCoord);
	curCoord.fWidth *= scale;
	curCoord.fHeight *= scale;
	result.x = (curCoord.fX + curCoord.fWidth) * 0.5f;
	result.y = (curCoord.fY + curCoord.fHeight) * 0.5f;
}

bool CDnClimaxEffect::IsShow()
{
	return m_bScaleProcess;
}

void CDnClimaxEffect::Start()
{
	m_bScaleProcess = true;
	m_fScaleTime = 0.f;
	m_fScaleAfterShowElapsedTime = 0.f;
	m_fHalfTime = 0.f;

	m_pBaseTexture->Show( false );
	m_pScaleTexture->Show( false );
}