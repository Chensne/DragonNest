#pragma once
#include "DnDurabilityDlg.h"
#include "SLMemPool.h"

class CDnDurabilityGaugeMng
{
public:
	CDnDurabilityGaugeMng(void);
	virtual ~CDnDurabilityGaugeMng(void);

protected:
	std::list<CDnDurabilityDlg*> m_listGaugeDlg;
	CSLMemPool<CDnDurabilityDlg> m_GaugeDlgMemPool;

public:
	void Process( float fElapsedTime );
	void Clear();

public:
	void ShowDurability(DnActorHandle hActor, int nStateBlowID, float fDurability, bool bShow);

	void DeleteActor(DnActorHandle hActor);
};