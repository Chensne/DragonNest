#pragma once

class CEtBaseEffect;
typedef CSmartPtr< CEtBaseEffect >	EtEffectHandle;

class CEtBaseEffect : public CSmartPtrBase< CEtBaseEffect >
{
public:
	CEtBaseEffect(void);
	virtual ~CEtBaseEffect(void);

protected:
	CEtMeshStream m_MeshStream;
	EtTextureHandle m_hTexture;
	EtMaterialHandle m_hMaterial;
	std::vector< SCustomParam >	m_vecCustomParam;

	EtBlendOP m_BlendOP;
	EtBlendMode m_SourceBlend;
	EtBlendMode m_DestBlend;

public:
	virtual void Render( float fElapsedTime ) = 0;
	static void RenderEffectList( float fElapsedTime );

	EtBlendOP GetBlendOP() { return m_BlendOP; }
	EtBlendOP SetBlendOP( EtBlendOP BlendOP ) 
	{ 
		EtBlendOP Backup = m_BlendOP;
		m_BlendOP = BlendOP;
		return Backup;
	}
	EtBlendMode GetSrcBlend() { return m_SourceBlend; }
	EtBlendMode SetSrcBlend( EtBlendMode SrcBlend ) 
	{ 
		EtBlendMode Backup = m_SourceBlend;
		m_SourceBlend = SrcBlend;
		return Backup; 
	}
	EtBlendMode GetDestBlend() { return m_DestBlend; }
	EtBlendMode SetDestBlend( EtBlendMode DestBlend ) 
	{ 
		EtBlendMode Backup = m_DestBlend;
		m_DestBlend = DestBlend;
		return Backup; 
	}
};
