#pragma once

#include "EtType.h"

class CEtPostProcessFilter;

class CEtPostProcessMng
{
public:
	CEtPostProcessMng();
	virtual ~CEtPostProcessMng();
	void Clear();

protected:
	std::vector< CEtPostProcessFilter * > m_vecFilter;

public:
	void Initialize();
	void Render( float fElapsedTime );
	CEtPostProcessFilter *CreateFilter( ScreenFilter Filter );
	void RemoveFilter( CEtPostProcessFilter *pFilter );

	CEtPostProcessFilter *GetFilter( ScreenFilter Filter );
};

extern CEtPostProcessMng g_EtPostProcessMng;
inline CEtPostProcessMng *GetEtPostProcessMng() { return &g_EtPostProcessMng; }
