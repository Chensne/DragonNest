#include "StdAfx.h"
#include "DnCpJudgeControl.h"
#include "DnCustomControlCommon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCpJudgeControl::CDnCpJudgeControl( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_JudgeType = JUDGE_NONE;
	m_Color		= 0xffffffff;
}

CDnCpJudgeControl::~CDnCpJudgeControl(void)
{
}

void CDnCpJudgeControl::Render( float fElapsedTime )
{
	if( !IsShow() ) return;

	SUIElement *pElement	= NULL;
	float fWidth			= 0.f;
	int nIndex				= -1;

	if (m_JudgeType > JUDGE_MAX)
	{
		_ASSERT(0);
		return;
	}

	if (m_JudgeType == JUDGE_NONE)
		return;

	nIndex = int(m_JudgeType);
	pElement = GetElement(nIndex);
	if( pElement == NULL )
	{
		_ASSERT(0);
		return;
	}

	SUICoord drawCoord;
	GetUICoord(drawCoord);
	drawCoord.fWidth	= pElement->fTextureWidth;
	drawCoord.fHeight	= pElement->fTextureHeight;

	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, m_Color, drawCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, m_Color, drawCoord);
}

void CDnCpJudgeControl::SetJudge(eCpJudgeType type)
{
	m_JudgeType = type;
}