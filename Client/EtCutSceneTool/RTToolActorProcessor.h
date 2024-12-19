#pragma once

#include "EternityEngine.h"
#include "ICustomActorProcessor.h"


class CFade;
class CDnCutSceneActProp;

class CRTToolActorProcessor : public ICustomActorProcessor
{
private:
	vector<float>			m_vlPrevFrame;
	vector<int>				m_vlPrevActionIndex;
	CFade*					m_pFade;
	vector<CDnCutSceneActProp*> m_vlpActProp;

public:
	CRTToolActorProcessor(void);
	virtual ~CRTToolActorProcessor(void);

	// from ICustomActorProcessor
	void AddObjectToPlay( S_CS_ACTOR_INFO* pNewObject );
	void AddPropToPlay( S_CS_PROP_INFO* pPropInfo );
	void CalcAniDistance( S_CS_ACTOR_INFO* pActorInfo, float fCurFrame, float fPrevFrame, EtVector3& DistVec );
	void SetPropAction( CEtWorldProp* pProp, const char* pActionName, LOCAL_TIME AbsoluteTime, float fFrame );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	int GetActionIndex( int iActorIndex, const char* pActionName );		// ���ΰ� ĳ���͵��� �޶����� �Ǵ� ��쵵 �����Ƿ� Ȯ���ϰ� �ε����� ���´�.
	float GetActorScale( int iActorIndex );
	void ShowProp( CEtWorldProp* pProp, bool bShow );

	void SetFadeObject( CFade* pFade ) { m_pFade = pFade; }
	void FadeIn( float fSpeed, DWORD dwColor );
	void FadeOut( float fSpeed, DWORD dwColor );

	void ShowSubtitle( const wchar_t* pSubtitle );
	void HideSubtitle( void );

	virtual void ShowEffect( void * pActor, bool bShow ); // #54681 - �÷��̾�ĳ������ Hideó��.
};
