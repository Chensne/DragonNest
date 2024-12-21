#pragma once

#include "LostDeviceProcess.h"

class CEtOcclusionQuery : public CLostDeviceProcess
{
public:
	CEtOcclusionQuery(void);
	virtual ~CEtOcclusionQuery(void);

protected:
	bool m_bHasPrev;
	LPDIRECT3DQUERY9 m_pQuery;
	bool m_bCreateFail;
	bool m_bAsyncType;

public:
	void Begin();
	void End();
	bool GetResult( DWORD &dwResult );
	void SetAsyncType( bool bType ) { m_bAsyncType = bType; }

	virtual void OnLostDevice();
	virtual void OnResetDevice();
};
