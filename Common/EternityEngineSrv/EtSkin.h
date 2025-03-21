#pragma once

#include "EtResource.h"
#include "EtMesh.h"

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

class CEtSkin : public CEtResource, public TBoostMemoryPool< CEtSkin >
{
public:
	CEtSkin( CMultiRoom *pRoom );
	virtual ~CEtSkin();
	void Clear();

protected:
	SSkinFileHeader m_SkinHeader;
	EtMeshHandle m_MeshHandle;

public:
	int LoadResource( CStream *pStream );
	int LoadMesh( const char *pMeshName );

	EtMeshHandle GetMeshHandle() { return m_MeshHandle; }
};
