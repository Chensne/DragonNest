#pragma once

#include "EtCustomRender.h"

class CDnLifeHarvestIcon : public CEtCustomRender
{
public :
	CDnLifeHarvestIcon();
	~CDnLifeHarvestIcon();

	virtual void RenderCustom( float fElapedTime );

private:

	std::vector<EtTextureHandle>	m_vHarvestTex;
	EtVector2		m_TexSize;

	std::map< int, std::pair<EtVector3, EtTextureHandle> >	m_mRenderList;

public :

	void InsertList( int nIndex, EtVector3 etVector3, char * szHarvestIconName );
	void DeleteList( int nIndex );
	void Finalize();
};