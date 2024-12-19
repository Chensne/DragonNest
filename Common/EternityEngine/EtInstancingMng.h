#pragma once

class CEtInstancingMng
{
public:
	CEtInstancingMng();
	~CEtInstancingMng();

private:
	bool						m_bInit;
	EtVertexBuffer			*m_pWorldViewMatVB;
	int							m_nVertexDecl[ 3 ];
	EtMaterialHandle		m_hMaterial;
	EtMaterialHandle		m_hSourceMaterial;
	bool						m_bEnable;

public:
	void Initialize();
	void ClassifyBlock( std::vector< SRenderStackElement* > &vecBlock, std::vector< SRenderStackElement* > &vecInstancingBlock );
	void RenderDepth( std::vector< SRenderStackElement* > &vecBlock );
	void Render( std::vector< SRenderStackElement* > &vecBlock );
	bool IsEnable() { return m_bEnable;}
	void Enable( bool bEnable );
};
