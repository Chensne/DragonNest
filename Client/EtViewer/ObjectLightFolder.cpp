#include "Stdafx.h"
#include "ObjectLightFolder.h"
#include "GlobalValue.h"


CObjectLightFolder::CObjectLightFolder()
{
	m_Type = CObjectBase::LIGHTFOLDER;
}

CObjectLightFolder::~CObjectLightFolder()
{
}


void CObjectLightFolder::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Total", CUnionValueProperty::Integer, "Light Count", FALSE },
		{ "Common", "Directional", CUnionValueProperty::Integer, "Directional Light Count", FALSE },
		{ "Common", "Point", CUnionValueProperty::Integer, "Point Light Count", FALSE },
		{ "Common", "Spot", CUnionValueProperty::Integer, "Spot Light Count", FALSE },
		{ "Common", "Ambient", CUnionValueProperty::Vector4Color, "Ambient Light", TRUE },
		{ "All", "Disable All Cast Shadow", CUnionValueProperty::Boolean, "��� ����Ʈ�� ĳ���� ������ �ɼ��� ����.", TRUE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CObjectLightFolder::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	int nDir = 0, nPoint = 0, nSpot = 0;
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		switch( m_pVecChild[i]->GetType() ) {
			case CObjectBase::LIGHTDIR: nDir++;	break;
			case CObjectBase::LIGHTPOINT: nPoint++;	break;
			case CObjectBase::LIGHTSPOT: nSpot++;	break;
		}
	}

	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( nDir + nPoint + nSpot );
			break;
		case 1:
			pVariable->SetVariable( nDir );
			break;
		case 2:
			pVariable->SetVariable( nPoint );
			break;
		case 3:
			pVariable->SetVariable( nSpot );
			break;
		case 4:
			{
				EtColor Color = EternityEngine::GetGlobalAmbient();
				pVariable->SetVariable( (D3DXVECTOR4)Color );
			}
			break;
		case 5:
			{
				pVariable->SetVariable( FALSE );
			}
			break;
	};
}

void CObjectLightFolder::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 4:
			{
				EtVector4 vColor = pVariable->GetVariableVector4();
				EternityEngine::SetGlobalAmbient( (EtColor*)&vColor );
			}
			break;
		case 5:
			{
				for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
					switch( m_pVecChild[i]->GetType() ) {
						case CObjectBase::LIGHTDIR:
							((CObjectLightDir*)m_pVecChild[i])->OnChangePropertyValue( 3, pVariable );
							break;
						case CObjectBase::LIGHTPOINT:
							((CObjectLightPoint*)m_pVecChild[i])->OnChangePropertyValue( 4, pVariable );
							break;
						case CObjectBase::LIGHTSPOT:
							((CObjectLightSpot*)m_pVecChild[i])->OnChangePropertyValue( 8, pVariable );
							break;
					}
				}
			}
			break;
	}
}

void CObjectLightFolder::SaveLightSetting( CString szStr )
{
	std::vector<SLightInfo> VecDir;
	std::vector<SLightInfo> VecPoint;
	std::vector<SLightInfo> VecSpot;

	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CObjectBase *pChild = m_pVecChild[i];
		switch( pChild->GetType() ) {
			case LIGHTDIR: VecDir.push_back( ((CObjectLightDir*)pChild)->GetLightInfo() );	break;
			case LIGHTPOINT: VecPoint.push_back( ((CObjectLightPoint*)pChild)->GetLightInfo() );	break;
			case LIGHTSPOT: VecSpot.push_back( ((CObjectLightSpot*)pChild)->GetLightInfo() );	break;
		}
	}

	if( szStr.IsEmpty() == true ) {
		CString szSubKey = REG_SUBKEY;
		szSubKey += "\\Light";

		// �ϴ� ��� ����Ʈ ������ ���� �����ֱ�
		RegDeleteKey( HKEY_CURRENT_USER, szSubKey );


		// ���� �ٽ� �Ѵ�.
		EtColor Color = EternityEngine::GetGlobalAmbient();
		SetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Ambient", &Color, sizeof(EtColor) );

		SetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Directional", (DWORD)VecDir.size() );
		SetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Point", (DWORD)VecPoint.size() );
		SetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Spot", (DWORD)VecSpot.size() );

		CString szStr;
		for( DWORD i=0; i<VecDir.size(); i++ ) {
			szStr.Format( "Directional-%d", i );
			SetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &VecDir[i], sizeof(SLightInfo) );
		}
		for( DWORD i=0; i<VecPoint.size(); i++ ) {
			szStr.Format( "Point-%d", i );
			SetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &VecPoint[i], sizeof(SLightInfo) );
		}
		for( DWORD i=0; i<VecSpot.size(); i++ ) {
			szStr.Format( "Spot-%d", i );
			SetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &VecSpot[i], sizeof(SLightInfo) );
		}
	}
	else {
	}
}

void CObjectLightFolder::LoadLightSetting( CString szStr )
{
	// ���� ����Ʈ �� ���챸~
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( m_pVecChild[i]->GetTreeItemID() );
		i--;
	}

	std::vector<SLightInfo> VecDir;
	std::vector<SLightInfo> VecPoint;
	std::vector<SLightInfo> VecSpot;

	if( szStr.IsEmpty() == true ) {
		CString szSubKey = REG_SUBKEY;
		szSubKey += "\\Light";

		DWORD dwDir = 0, dwPoint = 0, dwSpot = 0;
		EtColor Ambient = 0xFFFFFFFF;
		int nSize = sizeof(EtColor);

		GetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Ambient", &Ambient, nSize );
		EternityEngine::SetGlobalAmbient( &Ambient );

		GetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Directional", dwDir );
		GetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Point", dwPoint );
		GetRegistryNumber( HKEY_CURRENT_USER, szSubKey.GetBuffer(), "Spot", dwSpot );

		CString szStr;
		SLightInfo Info;
		nSize = sizeof(SLightInfo);
		for( DWORD i=0; i<dwDir; i++ ) {
			szStr.Format( "Directional-%d", i );
			GetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &Info, nSize );
			VecDir.push_back( Info );
		}
		for( DWORD i=0; i<dwPoint; i++ ) {
			szStr.Format( "Point-%d", i );
			GetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &Info, nSize );
			VecPoint.push_back( Info );
		}
		for( DWORD i=0; i<dwSpot; i++ ) {
			szStr.Format( "Spot-%d", i );
			GetRegistryBinary( HKEY_CURRENT_USER, szSubKey.GetBuffer(), szStr.GetBuffer(), &Info, nSize );
			VecSpot.push_back( Info );
		}
	}
	else {
	}

	 
	CObjectBase *pBase;
	for( DWORD i=0; i<VecDir.size(); i++ ) {
		pBase = new CObjectLightDir;
		CGlobalValue::GetInstance().AddObject( m_ItemID, pBase );
		((CObjectLightDir*)pBase)->SetLightInfo( VecDir[i] );
	}

	for( DWORD i=0; i<VecPoint.size(); i++ ) {
		pBase = new CObjectLightPoint;
		CGlobalValue::GetInstance().AddObject( m_ItemID, pBase );
		((CObjectLightPoint*)pBase)->SetLightInfo( VecPoint[i] );
	}

	for( DWORD i=0; i<VecSpot.size(); i++ ) {
		pBase = new CObjectLightSpot;
		CGlobalValue::GetInstance().AddObject( m_ItemID, pBase );
		((CObjectLightSpot*)pBase)->SetLightInfo( VecSpot[i] );
	}
}

void CObjectLightFolder::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_Type, sizeof(int), 1, fp );
	nCount++;

	EtColor Color = EternityEngine::GetGlobalAmbient();
	fwrite( &Color, sizeof(EtColor), 1, fp );

	CObjectBase::ExportObject( fp, nCount );
}

void CObjectLightFolder::ImportObject( FILE *fp )
{
	EtColor Color;
	fread( &Color, sizeof(EtColor), 1, fp );

	EternityEngine::SetGlobalAmbient( &Color );
}
