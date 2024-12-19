#include "StdAfx.h"
#include "DnPartsFace.h"
#include "DnTableDB.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartsFace::CDnPartsFace()
{
	m_nActorFaceTableID = 0;
}

CDnPartsFace::~CDnPartsFace()
{
}

bool CDnPartsFace::Initialize( int nPartsTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	if( CDnParts::Initialize( nPartsTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) return false;
	m_nActorFaceTableID = (DWORD)pSox->GetFieldFromLablePtr( nPartsTableID, "_ActorFaceID" )->GetInteger();
	if( m_nActorFaceTableID <= 0 )
		m_nActorFaceTableID = ( m_hActor ) ? m_hActor->GetClassID() : 0;
	return true;
}

bool CDnPartsFace::CreateObject( DnActorHandle hActor, const char *szAniName )
{
	bool bResult = CDnParts::CreateObject( hActor, szAniName );
	if( m_hActor ) {
		m_hActor->MAFaceAniBase::Initialize( m_nActorFaceTableID );
	}
	return bResult;
}