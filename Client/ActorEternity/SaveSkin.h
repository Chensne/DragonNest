#pragma once

#define SKIN_FILE_STRING	"Eternity Engine Skin File 0.1"
#define SKIN_FILE_VERSION	10

struct SSkinFileHeader
{
	char szHeaderString[ 256 ];
	char szMeshName[ 256 ];
	int nVersion;
	int nSubMeshCount;
	char cReserved[ 504 ];
};

struct SSubSkinHeader
{
	char szMaterialName[ 256 ];
	char szEffectName[ 256 ];
	float fAlphaValue;
	bool bEnableAlpha;
	char cReserved[ 507 ];
};

class CSaveSkin
{
public:
	CSaveSkin(void);
	virtual ~CSaveSkin(void);

protected:
	enum ExportParamType {
		PT_UNKNOWN = -1,
		PT_INT = 0,
		PT_FLOAT = 1,
		PT_FLOAT4 = 2,
		PT_TEX = 3,

		PT_FORCE_DWORD = 0xFFFFFFFF,
	};
	struct SParamStruct {
		char szParamName[256];
		ExportParamType Type;
		union {
			int nInt;
			float fFloat;
			float fFloat4[4];
			char *szStr;
		};
	};

	std::vector< SParamStruct > m_VecParamList;

public:
	void SaveSkin( FILE *fp, Mtl *pMtl );
	bool CheckDirectX9ShaderClass( Mtl *pMtl );
	void WriteString( FILE *fp, const char *pString );

};
