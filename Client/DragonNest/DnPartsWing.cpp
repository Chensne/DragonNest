#include "StdAfx.h"
#include "DnPartsWing.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnGuildTask.h"

CDnPartsWing::CDnPartsWing()
{
	m_WingType = WingType::None;
	m_nGuildMarkSubMeshIndex = -1;
	m_nTextureIndex = -1;
}

CDnPartsWing::~CDnPartsWing()
{
}

bool CDnPartsWing::CreateObject( DnActorHandle hActor, const char *szAniName )
{
	bool bResult = CDnParts::CreateObject( hActor, szAniName );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWING );
	if( !pSox ) return bResult;

	const DNTableCell* pWingTypeField = pSox->GetFieldFromLablePtr( m_nClassID, "_WingType" );
	if( !pWingTypeField ) return bResult;

	m_WingType = (WingType)pWingTypeField->GetInteger();


	if( m_WingType == GuildFlag && m_hObject ) {
		std::string szSubMesh;
		for( int i=0; i<m_hObject->GetSubMeshCount(); i++ ) {
			szSubMesh = m_hObject->GetSubMeshName(i);
			ToLowerA( szSubMesh );
			if( strstr( szSubMesh.c_str(), "guildmark" ) ) {
				m_nGuildMarkSubMeshIndex = i;
				m_nTextureIndex = -1;
				break;
			}
		}
	}

	return bResult;
}

void CDnPartsWing::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnParts::Process( LocalTime, fDelta );

	if( m_nGuildMarkSubMeshIndex != -1 && m_hObject && m_hActor->IsPlayerActor() ) {
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor ) {
			if( CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark( pPlayerActor->GetGuildSelfView() ) ) {
				EtTextureHandle hTexture = GetGuildTask().GetGuildMarkTexture( pPlayerActor->GetGuildSelfView() );
				if( hTexture ) {
					int nTextureIndex = hTexture.GetIndex();
					if( m_nTextureIndex != nTextureIndex ) {
						int nCustomParam = m_hObject->AddCustomParam( "g_DiffuseTex" );
						if( nCustomParam != -1 )
							m_hObject->SetCustomParam( nCustomParam, &nTextureIndex, m_nGuildMarkSubMeshIndex );
						m_nTextureIndex = nTextureIndex;
					}
				}
			}
		}
	}
}