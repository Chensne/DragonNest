#pragma once
#include "EtResource.h"
#include "EtBillboardEffectEmitter.h"

#define BILLBOARD_EFFECT_HEADER				"Eternity Engine Particle File"
#define BILLBOARD_EFFECT_HEADER_RESERVED	( 1024 - sizeof( SBillboardEffectHeader ) )

// 이 숫자 고치면 Particle.fx에 들어 있는 float4 g_IteratePos[ 40 ]; 값도 고쳐줘야 한다.
#define TRACE_POS_COUNT			40

struct SBillboardEffectHeader
{
	char szHeader[256];
	int nVersion;
	int nEmitterCount;
	SAABox BoundingBox;
};

class CEtBillboardEffectDataInstance;
class CEtBillboardEffectData : public CEtResource
{
public:
	CEtBillboardEffectData();
	~CEtBillboardEffectData();

	int LoadResource( CStream *pStream );
	int Save( const char *pFileName );
	void Clear();

	int AddEmitter( SEmitter *pEmitterInfo = NULL );
	int CopyEmitter( int nIndex );
	void DeleteEmitter( int nIndex );
	void ChangeEmitter( int nIndex1, int nIndex2 );
	CEtBillboardEffectEmitter *GetEmitter( int nIndex ) { return m_vecEmitter[ nIndex ]; }
	void CreateBillboardEffectBuffer();

	void LoadTexture( int nIndex, const char *pFileName );
	const char *GetTextureName( int nIndex ) { return m_vecEmitter[ nIndex ]->GetTextureName(); }

	int GetMaxBillboardEffectLife() { return m_nMaxBillboardEffectLife; }
	int GetEmitterCount() { return ( int )m_vecEmitter.size(); }
	void EnableDraw( int nIndex, bool bDraw ) { m_vecEmitter[ nIndex ]->EnableDraw( bDraw ); }

	CEtBillboardEffectDataInstance* GetInstance();
	void ReleaseInstance( CEtBillboardEffectDataInstance *pInstance );

protected:
	std::vector< CEtBillboardEffectEmitter * > m_vecEmitter;
	EtMaterialHandle m_hMaterial;

	int m_nTotalEffectCount;
	int m_nMaxBillboardEffectLife;
	bool m_bAlphaFog;

	std::vector< CEtBillboardEffectDataInstance* > m_InstancePool;
	std::vector< int > m_InstanceFreeSlots;

	void CheckLifeTime();

	friend class CEtBillboardEffectDataInstance;
};

typedef CSmartPtr< CEtBillboardEffectData > EtBillboardEffectDataHandle;

struct SBillboardEffectRenderInfo
{
	float fTime;
	EtVector4 Origin;
	float fGravitySpeed;
	EtVector3 vGravity;
	EtColor Color;
	float fScale;
	EtMatrix BillBoardMatrix;
	EtVector4 *pTracePos;
};

class CEtBillboardEffect;
class CEtBillboardEffectDataInstance
{
public:
	CEtBillboardEffectDataInstance();
	virtual ~CEtBillboardEffectDataInstance();

	void Reset();
	void CopyDataInfo( EtBillboardEffectDataHandle hData );
	void Render( EtMatrix &WorldMat, int nTick, bool bTracePos, std::vector< EtVector4 > &vecTracePos, EtColor &Color, float fScale, bool bReduceFillRate );
	void RenderImmediate( EtMatrix &WorldMat, int nTick, bool bTracePos, std::vector< EtVector4 > &vecTracePos, EtColor &Color, float fScale, bool bReduceFillRate );

protected:
	EtBillboardEffectDataHandle m_hData;
	std::vector< std::vector< SCustomParam > > m_vecCustomParams;
	std::vector< SBillboardEffectRenderInfo > m_vecRenderInfo;
	bool m_bAlphaFog;
	void *m_pTracePos;

	void SetTracePosCustomParam( std::vector< EtVector4 > &vecTracePos );
	int CalcTechnique( SEmitter *pEmitter, bool bTracePosMode );

	friend class CEtBillboardEffect;
};