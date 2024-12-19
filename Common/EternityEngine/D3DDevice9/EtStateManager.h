#pragma once
#include "Singleton.h"

//#define _CHECK_STATE_PERFORMANCE
struct SStateBlock
{
	const static int MAX_SAMPLER_COUNT = 16;

	struct RenderStateValue
	{
		D3DRENDERSTATETYPE	State;
		DWORD	Value;
		bool operator < ( RenderStateValue &rhs ) 
		{
			return State < rhs.State;
		}
	};

	struct SamplerStateValue
	{
		D3DSAMPLERSTATETYPE	State;
		DWORD	Value;
		DWORD	Sampler;
		bool operator < ( SamplerStateValue &rhs ) 
		{
			return State < rhs.State;
		}
	};

	RenderStateValue m_RenderStates[ MAX_SAMPLER_COUNT ];
	SamplerStateValue m_SamplerStates[ MAX_SAMPLER_COUNT ];
	int	m_RenderCount;
	int m_SamplerCount;	
	DWORD m_OldRenderValue[MAX_SAMPLER_COUNT];
	DWORD m_OldSamplerValue[MAX_SAMPLER_COUNT];

	SStateBlock()
	{
		Clear();
		memset(m_RenderStates, 0, sizeof(m_RenderStates));
		memset(m_SamplerStates, 0, sizeof(m_SamplerStates));
	}

	bool operator == (SStateBlock &rhs ) 
	{
		if(m_RenderCount != rhs.m_RenderCount || m_SamplerCount != rhs.m_SamplerCount ) return false;
		for( int i = 0; i < m_RenderCount; i++) {
			if( m_RenderStates[i].State != rhs.m_RenderStates[i].State )
				return false;
			if( m_RenderStates[i].Value != rhs.m_RenderStates[i].Value ) 
				return false;
		}
		for( int i = 0; i < m_SamplerCount; i++) {
			if( m_SamplerStates[i].State != rhs.m_SamplerStates[i].State )
				return false;
			if( m_SamplerStates[i].Value != rhs.m_SamplerStates[i].Value ) 
				return false;
		}
		return true;
	}

	void Clear()
	{
		m_RenderCount = 0;
		m_SamplerCount = 0;
		for( int i = 0; i < MAX_SAMPLER_COUNT; i++) {
			m_OldRenderValue[ i ] = 0;
			m_OldSamplerValue[ i ] = 0;
		}
	}

	void AddRenderState( D3DRENDERSTATETYPE State, DWORD Value)
	{
		int i;
		for( i = 0; i < m_RenderCount; i++) {
			if( m_RenderStates[ i ].State == State ) {
				m_RenderStates[ i ].Value = Value;
				break;
			}
		}
		if( i == m_RenderCount ) {
			assert( m_RenderCount < MAX_SAMPLER_COUNT );
			if( m_RenderCount < MAX_SAMPLER_COUNT )
			{
				m_RenderStates[ m_RenderCount ].State = State;
				m_RenderStates[ m_RenderCount ].Value = Value;
				m_RenderCount++;
				std::sort( m_RenderStates, m_RenderStates + m_RenderCount );
			}
		}
	}

	void AddSamplerState( D3DSAMPLERSTATETYPE State, DWORD Value, DWORD Sampler = 0 )
	{
		int i;
		for( i = 0; i < m_SamplerCount; i++) {
			if( m_SamplerStates[ i ].State == State ) {
				m_SamplerStates[ i ].Value = Value;
				break;
			}
		}
		if( i == m_SamplerCount ) {
			assert( m_SamplerCount < MAX_SAMPLER_COUNT );
			if( m_SamplerCount < MAX_SAMPLER_COUNT )
			{
				m_SamplerStates[ m_SamplerCount ].State = State;
				m_SamplerStates[ m_SamplerCount ].Value = Value;
				m_SamplerStates[ m_SamplerCount ].Sampler = Sampler;
				m_SamplerCount++;
				std::sort( m_SamplerStates, m_SamplerStates + m_SamplerCount );
			}
		}
	}

	void SetState() const
	{
		for( int j = 0; j < m_RenderCount; j++) {
			GetEtDevice()->GetRenderState( m_RenderStates[ j ].State, (DWORD*)&m_OldRenderValue[ j ] );
			GetEtDevice()->SetRenderState( m_RenderStates[ j ].State, m_RenderStates[ j ].Value);
		}
		for( int j = 0; j < m_SamplerCount; j++) {
			GetEtDevice()->GetSamplerState( m_SamplerStates[ j ].Sampler, m_SamplerStates[ j ].State, (DWORD*)&m_OldSamplerValue[ j ] );
			GetEtDevice()->SetSamplerState( m_SamplerStates[ j ].Sampler, m_SamplerStates[ j ].State, m_SamplerStates[ j ].Value);
		}
	}

	void RestoreState() const
	{
		for( int j = 0; j < m_RenderCount; j++) {				
			GetEtDevice()->SetRenderState( m_RenderStates[ j ].State, m_OldRenderValue[ j ]);
		}
		for( int j = 0; j < m_SamplerCount; j++) {				
			GetEtDevice()->SetSamplerState( m_SamplerStates[ j ].Sampler, m_SamplerStates[ j ].State, m_OldSamplerValue[ j ]);
		}
	}
};

struct CEtStateBlockManager : public ID3DXEffectStateManager, public CSingleton < CEtStateBlockManager >
{
public:
	enum {
		RS_MAX = D3DRS_BLENDOPALPHA + 1,
		SS_MAX = D3DSAMP_DMAPOFFSET + 1,
		TS_MAX = D3DTSS_CONSTANT + 1,
		DIRTY_MAX = 32,										// 한번의 DrawCall 에 32개 이상의 Setting 은 안될거라 생각하는데 혹시 모자르면 추후에 늘린다.
		STAGE_MAX = 8,		
	};

private:
	IDirect3DDevice9 *m_pD3DDevice;

	enum {
		RESET_MAGIC_NUMBER = 0xBAADCAFE							// 0 이나 0xFFFFFFFF 같은값 말고, 일반적으로 사용이 되지 않는 값을 선택한다. 
	};	

	DWORD		m_RenderState[ 4 ][ RS_MAX ];
	DWORD		m_SamplerState[ 4 ][ STAGE_MAX ][ SS_MAX ];
	DWORD		m_TextureStageState[ 4 ][ STAGE_MAX ][ TS_MAX ];

	DWORD		m_DirtyRenderState[ DIRTY_MAX ];
	DWORD		m_DirtySamplerState[ STAGE_MAX ][ DIRTY_MAX ];
	DWORD		m_DirtyTextureStageState[ STAGE_MAX ][ DIRTY_MAX ];

	bool		m_DirtyRenderFlag[ RS_MAX ];
	bool		m_DirtySamplerFlag[ STAGE_MAX ][ SS_MAX ];
	bool		m_DirtyTextureStageFlag[ STAGE_MAX ][ TS_MAX ];

	int			m_nDirtyRenderCount;
	int			m_DirtySamplerCount[ STAGE_MAX ];
	int			m_DirtyTextureStageCount[ STAGE_MAX ];
#if defined( _CHECK_STATE_PERFORMANCE )
	int			m_nStateTotal;
	int			m_nStateSkip;
#endif

	LPDIRECT3DVERTEXSHADER9			m_currVertexShader;
	LPDIRECT3DPIXELSHADER9				m_currPixelShader;
	LPDIRECT3DBASETEXTURE9			m_currTexture[ STAGE_MAX ];

	bool		m_bEnable;
#if defined( _CHECK_STATE_PERFORMANCE )
	int			m_nShaderSkip;
	int			m_nShaderTotal;

	int			m_nTextureSkip;
	int			m_nTextureTotal;	
#endif
	
	std::vector< SStateBlock > m_stateBlocks;

public:
	CEtStateBlockManager();
	virtual ~CEtStateBlockManager();

	void Initialize( CEtDevice *pDevice );
	void Reset();
	void SetDefaultState();

public:
	void GetRenderStateDefered( D3DRENDERSTATETYPE State, DWORD *pdwValue );
	void SetRenderStateDefered( D3DRENDERSTATETYPE State, DWORD dwValue );
	void GetSamplerStateDefered( DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD *pdwValue );
	void SetSamplerStateDefered( DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value );
	void GetTextureStageStateDefered( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwValue);
	void SetTextureStageStateDefered( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	void FlushDeferedStates();
	void DebugDumpState();
	bool IsEnable() { return m_bEnable; }
	void SetEnable( bool bEnable ) { m_bEnable = bEnable; }
	void Diff();
	void Capture();
	void Restore();
	
public:
	STDMETHOD(QueryInterface)( REFIID iid, LPVOID *ppv)  {
		return S_OK;
	}
	STDMETHOD_(ULONG, AddRef)()  {
		return S_OK;
	}
	STDMETHOD_(ULONG, Release)() {
		return S_OK;
	}
	STDMETHOD(SetTransform)( D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) {
		return m_pD3DDevice->SetTransform( State, pMatrix);		
	}
	STDMETHOD(SetMaterial)( CONST D3DMATERIAL9 *pMaterial ) {
		return m_pD3DDevice->SetMaterial( pMaterial );
	}
	STDMETHOD(SetLight)( DWORD Index, CONST D3DLIGHT9 *pLight ) {
		return m_pD3DDevice->SetLight( Index, pLight);
	}
	STDMETHOD(LightEnable)( DWORD Index, BOOL Enable ) {
		return m_pD3DDevice->LightEnable( Index, Enable);		
	}
	// Use EtDevice
	STDMETHOD(SetRenderState)( D3DRENDERSTATETYPE State, DWORD Value ) {
		if( m_bEnable ) {
			SetRenderStateDefered( State, Value);		
			return S_OK;
		}
		else {
			return m_pD3DDevice->SetRenderState( State, Value);	
		}
	}
	STDMETHOD(SetTexture)( DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture ) {
		if( m_bEnable ) {
#if defined( _CHECK_STATE_PERFORMANCE )
			m_nTextureTotal++;
#endif
			//ASSERT( Stage < STAGE_MAX );
			if( m_currTexture[ Stage ] != pTexture ) {
				m_currTexture[ Stage ] = pTexture;
				return m_pD3DDevice->SetTexture( Stage, pTexture );
			}
			else {
#if defined( _CHECK_STATE_PERFORMANCE )
				m_nTextureSkip++;
#endif
				return S_OK;
			}			
		}
		else {
			return m_pD3DDevice->SetTexture( Stage, pTexture);
		}
	}
	// Use EtDevice
	STDMETHOD(SetTextureStageState)( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value ) {
		if( m_bEnable ) {
			SetTextureStageStateDefered( Stage, Type, Value);
			return S_OK;		
		}
		else {
			return m_pD3DDevice->SetTextureStageState( Stage, Type, Value);
		}
	}
	// Use EtDevice
	STDMETHOD(SetSamplerState)( DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value ) {
		if( m_bEnable ) {
			SetSamplerStateDefered( Sampler, Type, Value);
			return S_OK;
		}
		else {
			return m_pD3DDevice->SetSamplerState( Sampler, Type, Value);
		}
	}
	STDMETHOD(SetNPatchMode)( FLOAT NumSegments ) {
		return m_pD3DDevice->SetNPatchMode( NumSegments );		
	}
	STDMETHOD(SetFVF)( DWORD FVF ) {	
		return m_pD3DDevice->SetFVF( FVF );	
	}
	STDMETHOD(SetVertexShader)( LPDIRECT3DVERTEXSHADER9 pShader ) {
		if( m_bEnable ) {
#if defined( _CHECK_STATE_PERFORMANCE )
			m_nShaderTotal++;
#endif
			if( m_currVertexShader != pShader ) {
				m_currVertexShader = pShader;
				return m_pD3DDevice->SetVertexShader( pShader );
			}
			else {
#if defined( _CHECK_STATE_PERFORMANCE )
				m_nShaderSkip++;
#endif
				return S_OK;
			}
		}
		else {
			return m_pD3DDevice->SetVertexShader( pShader );
		}
	}
	STDMETHOD(SetVertexShaderConstantF)( UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetVertexShaderConstantF( RegisterIndex, pConstantData, RegisterCount);
	}
	STDMETHOD(SetVertexShaderConstantI)( UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetVertexShaderConstantI( RegisterIndex, pConstantData, RegisterCount);		
	}
	STDMETHOD(SetVertexShaderConstantB)( UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetVertexShaderConstantB( RegisterIndex, pConstantData, RegisterCount);		
	}
	STDMETHOD(SetPixelShader)( LPDIRECT3DPIXELSHADER9 pShader ) {
		if( m_bEnable ) {
#if defined( _CHECK_STATE_PERFORMANCE )
			m_nShaderTotal++;
#endif
			if( m_currPixelShader  != pShader ) {
				m_currPixelShader = pShader;
				return m_pD3DDevice->SetPixelShader( pShader );
			}
			else {
#if defined( _CHECK_STATE_PERFORMANCE )
				m_nShaderSkip++;
#endif
				return S_OK;
			}
		}
		else {
			return m_pD3DDevice->SetPixelShader( pShader);
		}
	}
	STDMETHOD(SetPixelShaderConstantF)( UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetPixelShaderConstantF( RegisterIndex, pConstantData, RegisterCount);		
	}
	STDMETHOD(SetPixelShaderConstantI)( UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetPixelShaderConstantI( RegisterIndex, pConstantData, RegisterCount);		
	}
	STDMETHOD(SetPixelShaderConstantB)( UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount ) {
		return m_pD3DDevice->SetPixelShaderConstantB( RegisterIndex, pConstantData, RegisterCount);		
	}

	// StateBlock
	int CreateStateBlock( SStateBlock *StateBlock );
	SStateBlock* GetStateBlock( int index ) { return &m_stateBlocks[index]; }
};
extern CEtStateBlockManager s_EtStateManager;
CEtStateBlockManager* GetEtStateManager();

