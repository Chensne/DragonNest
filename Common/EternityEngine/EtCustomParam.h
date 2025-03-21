#pragma once

class CEtCustomParam
{
public:
	CEtCustomParam(void);
	virtual ~CEtCustomParam(void);

protected:
	EtSkinHandle m_hSkin;
	std::vector< std::vector< SCustomParam > > m_vecCustomParams;

public:
	void Initialize( EtSkinHandle hSkin );
	int AddCustomParam( const char *pParamName, EffectParamType Type );
	void SetCustomParam( int nParamIndex, EffectParamType Type, void *pValue, int nSubMeshIndex );
	std::vector< SCustomParam > &GetCustomParamList( int nIndex ) { return m_vecCustomParams[ nIndex ]; }
};
