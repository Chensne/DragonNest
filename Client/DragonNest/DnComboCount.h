#pragma once
#include "DnCountCtl.h"

class CDnComboCount : public CDnCountCtl
{
public:
	CDnComboCount( CEtUIDialog *pParent );
	virtual ~CDnComboCount(void);

protected:
	int		m_nRedIndex;

public:
	virtual void SetValue( int nValue );
	virtual float GetControlWidth();
	virtual float GetControlHeight();

public:
	virtual void Render( float fElapsedTime );
};