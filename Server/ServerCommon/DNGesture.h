
#pragma once
#include "DNServerDef.h"
#include "Util.h"

class CDNUserSession;
class CDNUserSendManager;
class CDNGesture
{
public:
	CDNGesture(CDNUserSession * pSession);
	~CDNGesture() {}

	void AddCashGesture( int nGestureID );
	void DelCashGesture( int nGestureID );
	void GetCashGestureList( TGesture * pGestureArr );

	bool HasCashGesture( int nGestureID );

	void AddEffectItemGesture( int nGestureID );
	void DelEffectItemGesture( int nGestureID );
	void GetEffectItemGestureList( TGesture * pGestureArr );

	bool HasEffectItemGesture( int nGestureID );

private:
	CDNUserSendManager * m_pSendManager;

	std::map <int, bool>	m_MapCashGestureList;
	std::map <int, bool>	m_MapEffectItemGestureList;
};