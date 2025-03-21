#pragma once
#include "EtResource.h"
#include "EtTexture.h"

enum EffectParamType {
	EPT_INT			= 0,
	EPT_FLOAT		= 1,
	EPT_VECTOR		= 2,
	EPT_TEX			= 3,
	EPT_MATRIX		= 4,
	EPT_VARIABLE	= 5,
	EPT_INT_PTR		= 6,
	EPT_FLOAT_PTR	= 7,
	EPT_VECTOR_PTR	= 8,
	EPT_MATRIX_PTR	= 9,
	EPT_UNKNOWN		= 0xffffffff
};

typedef int		EtParameterHandle;

typedef void *( *EffectParamCallBack )( void *, void * );
typedef int ( *VariableCountCallBack )( void *, void *);
struct SCustomParam {
	EtParameterHandle hParamHandle;
	EffectParamType Type;
	int nVariableCount;
	union {
		int nInt;
		float fFloat;
		float fFloat4[ 4 ];
		int nTextureIndex;
		void *pPointer;
		struct {
			EffectParamType VariableType;
			EffectParamCallBack pCallBack;
			VariableCountCallBack pCountCallBack;
		};
	};
	void *pUserStruct;
	SCustomParam() {
		pUserStruct = NULL;
		Type = EPT_UNKNOWN;
		hParamHandle = 0;
		nVariableCount = 0;
	}
};

struct SGlobalParam
{
	char szParamName[ 32 ];
	EffectParamCallBack pCallBack;
	EffectParamType VariableType;
	VariableCountCallBack pCountCallBack;
	int nAddedIndex;
};

struct SHADER_COMPILE_INFO
{
	char szFileName[64];
	int nQualityLevel;
	DWORD dwBufferSize;
	BYTE *pCompiledBuffer;	
};

class CEtMaterial :	public CEtResource, public CLostDeviceProcess
{
public:
	CEtMaterial(void);
	virtual ~CEtMaterial(void);
	void Clear();

protected:
	EtEffect *m_pEffect;
	static LPD3DXEFFECTPOOL	s_pEffectPool;
	static std::vector< SHADER_COMPILE_INFO > s_CompiledShaders;
	static int s_nShaderQuality;

	std::vector< SCustomParam >		m_GlobalEffectParam;
	std::vector< SCustomParam >		m_WorldMatEffectParam;
	std::vector< SCustomParam >		m_PreComputeParam;
	std::vector< SCustomParam >		m_CustomEffectParam;
	EtHandle										m_hWorldMatArray;
	std::vector< std::vector< int > >	m_vecVertexDecl;
	std::vector< std::vector< int > >	m_vecMergedVertexDecl;
	bool											m_bUseTangentSpace;
	bool											m_bUseBackBuffer;
	std::vector< EtHandle >				m_vecParamHandle;
	std::vector< std::string >				m_vecParamName;
	std::vector< int >						m_vecParamUsed;
	int												m_nCurrentTechnique;
	std::vector< void* >					m_vecUserAllocMemList;
	LPD3DXEFFECTSTATEMANAGER		m_StateManager;

public:
	int LoadResource( CStream *pStream );
	void Reload();
	DWORD GetParameterUsedFlag( EtHandle hHandle );
	void SaveParamHandle();
	virtual void OnLostDevice();
	virtual void OnResetDevice();

	int GetTechniqueCount();
	int GetPassCount( EtHandle hTechnique );
	int GetParameterCount();
	EtParameterHandle GetParameterByName( const char *pName );
	const char *GetParameterName( EtParameterHandle hParam );
	D3DXPARAMETER_TYPE GetParameterType( EtParameterHandle hHandle );
	EtTextureType GetTextureType( EtParameterHandle hHandle );
	const char *GetSemantic( EtParameterHandle hHandle );

	int SetTechnique( int nIndex );
	void BeginEffect( int &nPasses );
	void BeginEffect();
	void EndEffect();
	void BeginPass( int nPass ) { m_pEffect->BeginPass( nPass ); }
	void EndPass() { m_pEffect->EndPass(); }
	void CommitChanges() { m_pEffect->CommitChanges(); }

	int CreateVertexDecl( EtHandle hTechnique, EtHandle hPass, bool bMergeType = false );
	void CreateAllVertexDecl();
	int GetVertexDeclIndex( int nTechniqueIndex, int nPassIndex, bool bMergeType = false );

	static void CreateEffectPool();
	static void DeleteEffectPool() { SAFE_RELEASE( s_pEffectPool ); }
	static bool IsLoadCompiledShaders() { return !s_CompiledShaders.empty(); }
	static void LoadCompiledShaders();
	static void DeleteCompiledShaders();
	static void SetShaderQuality( int nQuality ) { s_nShaderQuality = nQuality; }
	static std::vector< SHADER_COMPILE_INFO > & GetShaderCompileInfo() { return s_CompiledShaders; }

	void CreateGlobalParam();
	bool AddGlobalParam( EtParameterHandle hParam, const char *pParamName );
	bool AddWorldMatParam( EtParameterHandle hParam, const char *pParamName );
	bool AddPreComputeParam( EtParameterHandle hParam, const char *pParamName );	
	void AddCustomParam( EtParameterHandle hParam, D3DXPARAMETER_DESC &Desc );
	int GetCustomParamCount() { return ( int )m_CustomEffectParam.size(); }
	EtParameterHandle GetCustomParamHandle( int nIndex ) { return m_CustomEffectParam[ nIndex ].hParamHandle; }
	EffectParamType GetCustomParamType( int nIndex ) { return m_CustomEffectParam[ nIndex ].Type; }

	void SetCustomParamList( std::vector< SCustomParam > &vecCustomParam );
	void SetCustomParam( SCustomParam *pParam, void *pAddParam = NULL, void *pAddParam2 = NULL );
	void SetGlobalParams();
	void SetWorldMatParams( EtMatrix *pWorldMat, EtMatrix *pPrevWorldMat, int nSaveMatIndex );
	void SetWorldMatParams( EtMatrix *pWorldMat, EtMatrix *pPrevWorldMat );
	void SetPreComputeParams( std::vector< SCustomParam > &vecCustomParam );
	void SetWorldMatArray( EtMatrix *pWorldMat, int nSaveMatIndex, int nBoneCount, const int *pBoneIndex );

	bool UseTangentSpace() { return m_bUseTangentSpace; }
	bool IsUseBackBuffer() { return m_bUseBackBuffer; }
	bool IsParameterName( EtParameterHandle hParam, const char *pParamName ) {  return ( m_vecParamName[ hParam ] == pParamName ); }
	void AddUserAllocMem( void *pPtr ) { m_vecUserAllocMemList.push_back( pPtr ); }
	static void ReloadMaterial();
};

class CEtShaderInclude : public ID3DXInclude
{
public:
	CEtShaderInclude();

	STDMETHOD(Open)( D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes );
	STDMETHOD(Close)( LPCVOID pData );
};

typedef CSmartPtr< CEtMaterial > EtMaterialHandle;

int AddCustomParam( std::vector< SCustomParam >	&ParamList, EffectParamType Type, EtMaterialHandle hMaterial, 
				   const char *pParamName, void *pValue, int nParamCount = 1 );
