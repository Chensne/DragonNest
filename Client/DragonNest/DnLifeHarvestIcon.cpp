#include "stdafx.h"
#include "DnLifeHarvestIcon.h"
#include "TaskManager.h"
#include "DnLifeSkillPlantTask.h"
#include "DnTableDB.h"


SUICoord TexCoord = SUICoord( 0.f, 0.f, 1.f, 1.f );

CDnLifeHarvestIcon::CDnLifeHarvestIcon()
{
	//m_hHarvestTex = LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGrant.dds" ).c_str(), RT_TEXTURE );
	DNTableFileFormat* pFarmSkinSox = GetDNTable( CDnTableDB::TFARMSKIN );
	for( int itr = 1; itr <= pFarmSkinSox->GetItemCount(); ++itr )
	{
		char * szHarvestIconName = pFarmSkinSox->GetFieldFromLablePtr( itr, "_HarvestIcon" )->GetString();

		if( NULL == szHarvestIconName[0] )
			continue;

		EtTextureHandle hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( szHarvestIconName ).c_str(), RT_TEXTURE );

		m_vHarvestTex.push_back( hTexture );
	}
}

CDnLifeHarvestIcon::~CDnLifeHarvestIcon()
{
	//SAFE_RELEASE_SPTR( m_hHarvestTex );
	SAFE_RELEASE_SPTRVEC( m_vHarvestTex );
	Finalize();
}

void CDnLifeHarvestIcon::RenderCustom(float fElapedTime)
{
	static int STATE_MARK_SCALE_TIME  = 250;
	static float STATE_MARK_SCALE_RANGE  = 0.1f;

	if( m_vHarvestTex.empty() )
		return;

	m_TexSize.x = m_vHarvestTex[0]->Width() * 0.5f;
	m_TexSize.y = m_vHarvestTex[0]->Height() * 0.5f;

	CDnLifeSkillPlantTask * pTask = (CDnLifeSkillPlantTask *)CTaskManager::GetInstance().GetTask( "LifeSkillPlantTask" );

	int nTemp = (DWORD)pTask->m_pFrameSync->GetMSTime()%(STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME);
	if( nTemp < STATE_MARK_SCALE_TIME )
		m_TexSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * nTemp;
	else
		m_TexSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * ( (STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME) - nTemp );

	CEtSprite::GetInstance().Begin(0);
	for( std::map<int, std::pair<EtVector3, EtTextureHandle>>::iterator itor = m_mRenderList.begin(); itor != m_mRenderList.end(); ++itor )
	{
		EtVector3 vTemp = (*itor).second.first;
		EtTextureHandle hTexture= (*itor).second.second;
		CEtSprite::GetInstance().DrawSprite3D( (EtTexture*)hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), D3DCOLOR_ARGB(255,255,255,255), 
			m_TexSize, vTemp, 0.0f, &TexCoord );
	}
	CEtSprite::GetInstance().End();
}

void CDnLifeHarvestIcon::InsertList( int nIndex, EtVector3 etVector3, char * szHarvestIconName )
{
	EtTextureHandle hTexture;
	for( int itr = 0; itr < (int)m_vHarvestTex.size(); ++itr )
	{
		if( stricmp( szHarvestIconName, m_vHarvestTex[itr].GetPointer()->GetFileName() ) == 0 )
		{
			hTexture = m_vHarvestTex[itr];
			break;
		}
	}

	if( hTexture )
		m_mRenderList.insert( std::make_pair( nIndex, std::make_pair(etVector3, hTexture) ) );
}

void CDnLifeHarvestIcon::DeleteList(int nIndex )
{
	std::map<int, std::pair<EtVector3,EtTextureHandle>>::iterator itor = m_mRenderList.find(nIndex );
	if( itor == m_mRenderList.end() )
		return;

	m_mRenderList.erase( itor );
}

void CDnLifeHarvestIcon::Finalize()
{
	m_mRenderList.clear();
}

