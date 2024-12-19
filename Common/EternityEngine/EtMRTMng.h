#pragma once

#include "Singleton.h"

class CEtMRTMng : public CSingleton< CEtMRTMng >
{
public:
	CEtMRTMng(void);
	virtual ~CEtMRTMng(void);

protected:
	EtTextureHandle m_hDepthTarget;
	EtTextureHandle m_hVelocityTarget;
	EtSurface *m_pBackupSurface;
	bool m_bEnable;
	bool m_bInit;
	bool m_bClearNeeded;

public:
	void Initialize();

	void Enable( bool bEnable ) { m_bEnable = bEnable; }
	bool IsEnable() { return m_bEnable; }

	EtTextureHandle GetDepthTarget() { return m_hDepthTarget; }
	EtTextureHandle GetVelocityTarget() { return m_hVelocityTarget; }

	void ClearDepthTarget();
	bool BeginDepthTarget();
	void EndDepthTarget();
};
