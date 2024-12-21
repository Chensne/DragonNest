#pragma once

#include "EtResource.h"
#include "EtMesh.h"

#define SKIN_FILE_STRING "Eternity Engine Skin File"
#define SKIN_HEADER_RESERVED	( 1024 - sizeof( SSkinFileHeader ) )
struct SSkinFileHeader
{
	SSkinFileHeader() 
	{
		memset( szHeaderString, 0, 256 );
		memset( szMeshName, 0, 256 );
		nVersion = 0;
		nSubMeshCount = 0;
	};

	char szHeaderString[ 256 ];
	char szMeshName[ 256 ];
	int nVersion;
	int nSubMeshCount;
};

#define SUB_SKIN_HEADER_RESERVED	( 1024 - sizeof( SSubSkinHeader ) )
struct SSubSkinHeader
{
	SSubSkinHeader() 
	{
		memset( szMaterialName, 0, 256 );
		memset( szEffectName, 0, 256 );
		fAlphaValue = 1.0f;
		bEnableAlphablend = false;
	}

	char szMaterialName[ 256 ];
	char szEffectName[ 256 ];
	float fAlphaValue;
	bool bEnableAlphablend;
};

class CEtCustomParam;
class CEtSkin;
typedef CSmartPtr< CEtSkin > EtSkinHandle;

class CEtSkin : public CEtResource
{
	friend class CEtSkinInstance;
	friend class CEtMergedSkin;
public:
	CEtSkin();
	virtual ~CEtSkin();
	void Clear();

protected:
	SSkinFileHeader m_SkinHeader;
	std::vector< SSubSkinHeader > m_vecSubSkinHeader;
	std::vector< EtMaterialHandle > m_vecMaterialHandle;
	std::vector< std::vector< SCustomParam > > m_vecCustomParam;

	CEtCustomParam *m_pAdditionalCustomParam;
	int m_nAlphaParamIndex;
	std::vector< int > m_DiffuseTexIndexList;
	std::vector< int > m_vecTehiniqueIndex;
	EtMeshHandle m_MeshHandle;
	EtMaterialHandle m_hShadowMaterial;

	// 외부에서 이 스킨을 원하는 머터리얼로 렌더 하고 싶을때 사용한다.
	EtMaterialHandle m_hExternalMaterial;
	std::vector< SCustomParam > *m_pvecExternalParam;
	std::vector< std::vector< SCustomParam > >  *m_pvecMultiExternalParam;

	std::vector< int > m_MergeTextureSlot;
	std::vector< BOOL > m_IsAlphaTextureList;
	EtColor m_EmmisiveColor[5];

public:
	virtual int LoadResource( CStream *pStream );

	int Save( const char *pFileName );
	int SaveSkin( CStream *pStream );
	int LoadMesh( const char *pMeshName );
	int LoadEffect( const char *pEffectName );
	int LoadTexture( const char *pTextureName, EtTextureType Type );

	void CheckTechnique();
	void CheckMeshStream();

	EtMaterialHandle GetCurrentMaterial( int nMtlIndex );	
	void SetExternalMaterial( EtMaterialHandle hMaterial, std::vector< SCustomParam > *pvecParam, std::vector< std::vector< SCustomParam > > *pvecMultiParam = NULL );
	void ClearExternalMaterial();
	const int& GetDiffuseTexIndex( int nSubMesh ) { return m_DiffuseTexIndexList[ nSubMesh ]; }

	EtMaterialHandle GetMaterialHandle( int nSubMeshIndex ) { return m_vecMaterialHandle[ nSubMeshIndex ]; }
	EtMeshHandle GetMeshHandle() { return m_MeshHandle; }
	int GetMaterialCount() { return (int)m_vecMaterialHandle.size(); }

	int GetEffectCount( int nSubMeshIndex ) { return (int)m_vecCustomParam[ nSubMeshIndex ].size(); }
	void SetEffect( int nSubMeshIndex, int nEffectIndex, SCustomParam *pParam );
	SCustomParam *GetCustomParam( int nSubMeshIndex, int nEffectIndex ) { return &m_vecCustomParam[nSubMeshIndex][nEffectIndex]; }
	void SetTexture( int nSubMeshIndex, int nEffectIndex, const char *pTexName );

	void SetAlphaValue( int nSubMeshIndex, float fAlphaValue );
	float GetAlphaValue( int nSubMeshIndex );
	void EnableAlphaBlend( int nSubMeshIndex, bool bEnable );
	bool IsEnableAlphaBlend( int nSubMeshIndex );

	void RenderSubMesh( int nSubMeshIndex, EtMatrix &WorldMat, EtMatrix &PrevWorldMat, bool bShadowReceive, int nSaveMatIndex, float fAlpha );
	void Render( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, bool bShadowReceive, std::vector< bool > *pvecShowSubmesh = NULL, CEtCustomParam *pCustomParam = NULL, int nSaveMatIndex = -1 );
	void RenderAlpha( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, bool bShadowReceive, std::vector< bool > *pvecShowSubmesh = NULL, CEtCustomParam *pCustomParam = NULL, int nSaveMatIndex = -1 );
	void RenderShadow( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, std::vector< bool > *pvecShowSubmesh = NULL, int nSaveMatIndex = -1 );
	void SetCustomParams( int nMaterialIndex );	
	bool IsAlphaTexture( int nSubMesh ) {return ( m_IsAlphaTextureList[nSubMesh] == TRUE ); }
};