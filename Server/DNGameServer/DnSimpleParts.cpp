#include "StdAfx.h"
#include "DnSimpleParts.h"
#include "DNTableFile.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnPlayerActor.h"

DECL_DN_SMART_PTR_STATIC( CDnSimpleParts, MAX_SESSION_COUNT, 100 )


CDnSimpleParts::CDnSimpleParts(CMultiRoom *pRoom)
: CDnUnknownRenderObject( pRoom, false )
, CDnItem( pRoom )
{
	CDnActionBase::Initialize( this );
}

CDnSimpleParts::~CDnSimpleParts()
{
	FreeObject();
}

bool CDnSimpleParts::FreeObject()
{
	SAFE_RELEASE_SPTR( m_hObject );
	return true;
}


bool CDnSimpleParts::Initialize( int nPartsTableID , const char *szSkinName , const char *szAniName )
{
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PartsTable.ext failed\r\n");
		return false;
	}
	if( !pSox->IsExistItem( nPartsTableID ) ) return false;
	m_nClassID = nPartsTableID;

	CreateObject(szSkinName,szAniName);
	return true;
}

bool CDnSimpleParts::CreateObject( const char *szSkinName , const char *szAniName )
{
	m_hObject = EternityEngine::CreateAniObject( GetRoom(), szSkinName , szAniName );
	return true;
}

DnPartsHandle CDnSimpleParts::CreateParts(CMultiRoom *pRoom, int nPartsTableID , const char *szSkinName , const char *szAniName)
{
	if( nPartsTableID < 1 ) return CDnParts::Identity();

	DnSimplePartsHandle hParts;

	hParts = (new CDnSimpleParts(pRoom))->GetMySmartPtr();

	hParts->Initialize( nPartsTableID , szSkinName , szAniName);
	return hParts;
}

void CDnSimpleParts::InitializeItem( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound )
{
	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound );
}
