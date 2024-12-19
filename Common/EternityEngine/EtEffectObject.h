#pragma once

#include "EtEffectData.h"

class CEtEffectElement;
class CEtEffectObject;
typedef CSmartPtr< CEtEffectObject >	EtEffectObjectHandle;

class CEtEffectObject : public CSmartPtrBase< CEtEffectObject >
{
public:
	CEtEffectObject(void);
	virtual ~CEtEffectObject(void);

protected:
	bool m_bDelete;
	EtMatrix m_WorldMat;
	float m_fScale;
	EtEffectDataHandle m_hEffectData;
	std::vector< CEtEffectElement * > m_vecEffectElement;

	int m_nPrevTick;
	int m_nEffectTick;
	float m_fFloatTick;
	RenderType m_RenderType;
	bool	m_bLoop;

	EtMatrix m_LinkWorldMat;
	int m_LinkBoneIndex;
	int m_nLinkType;
	EtVector3 m_LinkPos;
	EtVector3 m_LinkRotate;
	EtAniObjectHandle m_hLinkParent;
	bool m_bLink;
	bool m_bShow;

public:
	void Initialize( EtEffectDataHandle hEffectData, EtMatrix *pInitMat );
	void SetLoop( bool bLoop ) {m_bLoop = bLoop;}
	void ProcessLink( EtMatrix &WorldMat );
	void Render( float fElapsedTime );
	void RenderImmediate( float fElapsedTime );
	bool CalcTick(  float fElapsedTime  );
	void SetWorldMat( EtMatrix *mat );
	void SetScale( float fScale );
	void SetAlpha( float fAlpha );
	const char *GetEffectFileName();
	void Show( bool bShow ) { m_bShow = bShow;}
	bool IsShow() { return m_bShow;}
	void EnableCull( bool bEnable );

	RenderType SetRenderType( RenderType Type );
	RenderType GetRenderType() { return m_RenderType; }

	void SetLink( EtMatrix WorldMat, char *szBoneName, EtVector3 vPos, EtVector3 vRotate, EtAniObjectHandle hParent );
	void UnLink() { m_bLink = false;}

	static void RenderEffectObjectList( float fElapsedTime );
	static void ProcessDeleteEffectObjectList( float fElapsedTime );
};
