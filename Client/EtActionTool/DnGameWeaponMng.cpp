#include "StdAfx.h"
#include "DnGameWeaponMng.h"
#include "DnGameTable.h"
#include "DNTableFile.h"
#include "ActionObject.h"
#include "GlobalValue.h"
#include "EtResourceMng.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameWeaponMng::CDnGameWeaponMng(void)
{
	char szFileName[ _MAX_PATH ];
	FILE *fp;
	char szString[_MAX_PATH]={0};
	char szActionName[_MAX_PATH]={0};
	int nWeapon1, nWeapon2;
	SWeaponInfo Info;

	strcpy_s( szFileName, _MAX_PATH, "R:\\Gameres\\WorkSheet\\ToolData\\ActionToolWeapon.txt" );
	fopen_s( &fp, szFileName, "r" );
	if( fp )
	{
		while( fgets( szString, _MAX_PATH, fp ) )
		{
			if( szString[strlen(szString)-1] == '\n' ) szString[strlen(szString)-1] = '\0';
			sscanf_s( szString, "%s %d %d", szActionName, _MAX_PATH, &nWeapon1, &nWeapon2 );
			Info.szActionFile = szActionName;
			Info.nWeapon1 = nWeapon1;
			Info.nWeapon2 = nWeapon2;
			m_vecWeaponInfo.push_back( Info );
		}
		fclose( fp );
	}
	m_nCurrentInfoIndex = -1;
}

CDnGameWeaponMng::~CDnGameWeaponMng(void)
{
	char szFileName[ _MAX_PATH ];
	FILE *fp;
	strcpy_s( szFileName, _MAX_PATH, "R:\\Gameres\\WorkSheet\\ToolData\\ActionToolWeapon.txt" );
	fopen_s( &fp, szFileName, "w" );
	if( fp )
	{
		for( int i = 0; i < (int)m_vecWeaponInfo.size(); ++i )
		{
			if( m_vecWeaponInfo[i].nWeapon1 == 0 && m_vecWeaponInfo[i].nWeapon2 == 0 )
				continue;
			fprintf_s( fp, "%s %d %d\n", m_vecWeaponInfo[i].szActionFile.c_str(), m_vecWeaponInfo[i].nWeapon1, m_vecWeaponInfo[i].nWeapon2 );
		}
		fclose( fp );
	}
	SAFE_RELEASE_SPTR( m_hWeaponObject1 );
	SAFE_RELEASE_SPTR( m_hWeaponObject2 );
}

void CDnGameWeaponMng::OnLoadAction( CString szActionFileName )
{
	m_nCurrentInfoIndex = -1;
	char szFileName[512] = { 0, };
	_GetFileName( szFileName, _countof(szFileName), szActionFileName );
	m_szCurrentActionFileName = szFileName;
	for( int i = 0; i < (int)m_vecWeaponInfo.size(); ++i )
	{
		if( m_vecWeaponInfo[i].szActionFile == m_szCurrentActionFileName )
		{
			AttachWeapon( m_vecWeaponInfo[i].nWeapon1, m_vecWeaponInfo[i].nWeapon2 );
			m_nCurrentInfoIndex = i;
			return;
		}
	}
}

void CDnGameWeaponMng::GetAttachedWeapon( int &nWeapon1, int &nWeapon2 )
{
	if( m_nCurrentInfoIndex < 0 ) return;
	if( m_nCurrentInfoIndex >= (int)m_vecWeaponInfo.size() ) return;
	nWeapon1 = m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon1;
	nWeapon2 = m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon2;
}

void CDnGameWeaponMng::ChangeAttachWeapon( int nWeapon1, int nWeapon2 )
{
	if( 0 <= m_nCurrentInfoIndex && m_nCurrentInfoIndex < (int)m_vecWeaponInfo.size() )
	{
		if( m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon1 == nWeapon1 && m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon2 == nWeapon2 )
			return;

		m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon1 = nWeapon1;
		m_vecWeaponInfo[m_nCurrentInfoIndex].nWeapon2 = nWeapon2;
	}
	else 
	{
		if( nWeapon1 == 0 && nWeapon2 == 0 )
			return;

		SWeaponInfo Info;
		Info.szActionFile = m_szCurrentActionFileName;
		Info.nWeapon1 = nWeapon1;
		Info.nWeapon2 = nWeapon2;
		m_nCurrentInfoIndex = (int)m_vecWeaponInfo.size();
		m_vecWeaponInfo.push_back( Info );
	}
	AttachWeapon( nWeapon1, nWeapon2 );
}

void CDnGameWeaponMng::AttachWeapon( int nWeapon1, int nWeapon2 )
{
	DNTableFileFormat *pSox = CDnGameTable::GetInstance().GetTable( CDnGameTable::WEAPON_TABLE );
	CActionObject *pObject = (CActionObject*)CGlobalValue::GetInstance().GetRootObject();
	if( !pSox ) return;
	if( !pObject ) return;
	if( !pObject->GetObjectHandle() ) return;
	if( !pObject->GetObjectHandle()->GetAniHandle() ) return;

	std::string szSkinName;
	std::string szAniName;
	std::string szActName;
	std::string szSimName;
	std::string szLinkBoneName;

	// 먼저 기존 무기를 삭제하고,
	SAFE_RELEASE_SPTR( m_hWeaponObject1 );
	if( nWeapon1 > 0 ) {
		if( pSox->IsExistItem( nWeapon1 ) ) {
			//szSkinName = pSox->GetFieldFromLablePtr( nWeapon1, "_SkinName" )->GetString();
			//szAniName = pSox->GetFieldFromLablePtr( nWeapon1, "_AniName" )->GetString();
			//szActName = pSox->GetFieldFromLablePtr( nWeapon1, "_ActName" )->GetString();
			//szSimName = pSox->GetFieldFromLablePtr( nWeapon1, "_SimName" )->GetString();
			// Ext옵티마이즈 하면서 아래 함수 쓰는거로 변경.
			CommonUtil::GetFileNameFromFileEXT( szSkinName, pSox, nWeapon1, "_SkinName" );
			CommonUtil::GetFileNameFromFileEXT( szAniName, pSox, nWeapon1, "_AniName" );
			CommonUtil::GetFileNameFromFileEXT( szActName, pSox, nWeapon1, "_ActName" );
			CommonUtil::GetFileNameFromFileEXT( szSimName, pSox, nWeapon1, "_SimName" );

			if( !szSkinName.empty() ) {
				m_hWeaponObject1 = EternityEngine::CreateAniObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
				if( m_hWeaponObject1 ) {
					m_hWeaponObject1->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					if( !szSimName.empty() ) {
						m_hWeaponObject1->CreateSimulation( szSimName.c_str() );
					}
				}
			}

			if( m_hWeaponObject1 ) {
				szLinkBoneName = "~BoxBone01";
				int nBoneIndex = pObject->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szLinkBoneName.c_str() );
				m_hWeaponObject1->SetParent( pObject->GetObjectHandle(), nBoneIndex );
			}
		}
	}

	SAFE_RELEASE_SPTR( m_hWeaponObject2 );
	if( nWeapon2 > 0 ) {
		if( pSox->IsExistItem( nWeapon2 ) ) {
			//szSkinName = pSox->GetFieldFromLablePtr( nWeapon2, "_SkinName" )->GetString();
			//szAniName = pSox->GetFieldFromLablePtr( nWeapon2, "_AniName" )->GetString();
			//szActName = pSox->GetFieldFromLablePtr( nWeapon2, "_ActName" )->GetString();
			//szSimName = pSox->GetFieldFromLablePtr( nWeapon2, "_SimName" )->GetString();
			CommonUtil::GetFileNameFromFileEXT( szSkinName, pSox, nWeapon2, "_SkinName" );
			CommonUtil::GetFileNameFromFileEXT( szAniName, pSox, nWeapon2, "_AniName" );
			CommonUtil::GetFileNameFromFileEXT( szActName, pSox, nWeapon2, "_ActName" );
			CommonUtil::GetFileNameFromFileEXT( szSimName, pSox, nWeapon2, "_SimName" );

			if( !szSkinName.empty() ) {
				m_hWeaponObject2 = EternityEngine::CreateAniObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
				if( m_hWeaponObject2 ) {
					m_hWeaponObject2->GetSkinInstance()->SetRenderAlphaTwoPass( true );
					if( !szSimName.empty() ) {
						m_hWeaponObject2->CreateSimulation( szSimName.c_str() );
					}
				}
			}

			if( m_hWeaponObject2 ) {
				szLinkBoneName = "~BoxBone02";
				int nBoneIndex = pObject->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szLinkBoneName.c_str() );
				m_hWeaponObject2->SetParent( pObject->GetObjectHandle(), nBoneIndex );
			}
		}
	}
}