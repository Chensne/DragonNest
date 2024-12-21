#pragma once
#include "EtPostProcessFilter.h"

class CEtTransitionFilter : public CEtPostProcessFilter
{
public:
	CEtTransitionFilter();
	virtual ~CEtTransitionFilter();
	virtual void Clear();

protected:
	int		m_nBackBufferIndex;
	bool	m_bCaptured;
	float	m_fElapsedTime;

public:
	void Capture() {m_bCaptured = true;}

	virtual void Initialize();
	virtual void Render( float fElapsedTime );
};