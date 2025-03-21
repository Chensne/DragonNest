#pragma once
#include "DnEnemyGaugeDlg.h"
#include "DnPlayerGaugeDlg.h"
#include "SLMemPool.h"

class CDnEnemyGaugeMng
{
public:
	CDnEnemyGaugeMng(void);
	virtual ~CDnEnemyGaugeMng(void);

protected:
	std::list<CDnEnemyGaugeDlg*> m_listGaugeDlg;
	CSLMemPool<CDnEnemyGaugeDlg> m_GaugeDlgMemPool;

	std::vector<CDnPlayerGaugeDlg*> m_vecPlayerGaugeDlg;
//	CSLMemPool<CDnPlayerGaugeDlg> m_GaugePLayerDlgMemPool;

public:
	void Process( float fElapsedTime );
	void Clear();

public:
	void SetEnenmy( DnActorHandle hActor );
	void SetPlayer( DnActorHandle hActor );

	void DeleteEnemy( DnActorHandle hActor );
	void DeletePlayer( DnActorHandle hActor );
#ifdef PRE_ADD_SHOW_MONACTION
	bool IsEnemyGaugeInList(DnActorHandle hActor) const;
#endif
};