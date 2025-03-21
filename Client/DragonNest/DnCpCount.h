#pragma once
#include "DnCountCtl.h"

class CDnCpCount : public CDnCountCtl
{
public:
	CDnCpCount( CEtUIDialog *pParent );
	virtual ~CDnCpCount(void);

protected:
	int		m_StaticIndex;
	DWORD	m_CustomColor;
	bool	m_bCustomColor;

	const int	m_MinusSignIdx;

public:
	void			SetStaticIdx(int idx)				{ m_StaticIndex = idx; }
	void			EnableCustomColor(DWORD color)		{ m_bCustomColor = true; m_CustomColor = color; }
	void			DisableCustomColor()				{ m_bCustomColor = false; }
	virtual void	SetValue( int nValue );
	virtual float	GetControlWidth();
	virtual float	GetControlHeight();

	virtual void	Render( float fElapsedTime );
};