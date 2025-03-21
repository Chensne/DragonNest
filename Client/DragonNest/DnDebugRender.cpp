#include "stdafx.h"
#include "DnDebugRender.h"
#include "DnFreeCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDebugRender* g_pDnDebugRender;

CDnDebugRender::CDnDebugRender()
: CEtCustomRender()
{
	m_hTexture.Identity();

}

CDnDebugRender::~CDnDebugRender()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CDnDebugRender::RenderCustom( float fElapsedTime )
{
	if ( !m_hTexture )
		m_hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestComplete.dds" ).c_str(), RT_TEXTURE );

	CDnFreeCamera* pFreeCam = dynamic_cast<CDnFreeCamera*>(CDnCamera::GetActiveCamera().GetPointer());
	if ( pFreeCam )
	{
		EtVector3 vOutPos = CGlobalValue::GetInstance().m_vOutPos;
		bool bPicked = CGlobalValue::GetInstance().m_bPicked;
		if ( bPicked ) 
		{
			static DWORD dwClr = 0xffff0000;
			//EtMatrix mat;
			//EtMatrixIdentity( &mat);
			//EternityEngine::DrawLine3D( vOutPos, pFreeCam->GetMatEx()->m_vPosition , dwClr, &mat );
			WCHAR buff[256] = {0,};
			//swprintf_s( buff, L"Cam %0.2f %0.2f %0.2f", pFreeCam->GetMatEx()->m_vPosition.x, pFreeCam->GetMatEx()->m_vPosition.y, pFreeCam->GetMatEx()->m_vPosition.z );
			swprintf_s( buff, L"%0.2f %0.2f %0.2f", vOutPos.x, vOutPos.y, vOutPos.z );

			int nFontIndex = 5;
			vOutPos.y += 50.0f;
			
			SFontDrawEffectInfo FontEffectInfo;
			FontEffectInfo.dwFontColor = dwClr;
			FontEffectInfo.nDrawType = SFontDrawEffectInfo::SHADOW;
			FontEffectInfo.nWeight = 2;
			FontEffectInfo.dwEffectColor = 0xff000000;
			CEtFontMng::GetInstance().DrawTextW3D( nFontIndex, 16, buff, vOutPos, FontEffectInfo, -1, 1.0f );


			EtVector2 vSize;
			vSize.x = m_hTexture->Width() * 0.5f;
			vSize.y = m_hTexture->Height() * 0.5f;


			CEtSprite::GetInstance().DrawSprite3D( (EtTexture*)m_hTexture->GetTexturePtr(), m_hTexture->Width(), m_hTexture->Height(), 
				0xffffffff, vSize, vOutPos, 0.0f);

				
		}
	}
	
}