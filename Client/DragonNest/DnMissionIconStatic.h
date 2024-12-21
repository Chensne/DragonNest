#pragma once

class CDnMissionIconStatic : public CEtUIStatic
{
public:
	CDnMissionIconStatic( CEtUIDialog *pParent );
	virtual ~CDnMissionIconStatic(void);

protected:
	int m_nIconID;
	EtTextureHandle m_hCurrentIcon;
	std::vector<EtTextureHandle> m_hVecIconList;

	SUICoord m_IconUV;

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	void SetIconID( int nID );
	int GetIconID()  { return m_nIconID; }
};
