#include "Stdafx.h"
#include "ObjectLightFolder.h"
#include "ObjectLightDir.h"
#include "ObjectLightPoint.h"
#include "ObjectLightSpot.h"
#include "GlobalValue.h"


CObjectLightFolder::CObjectLightFolder()
{
	m_Type = CActionBase::LIGHTFOLDER;
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
		{ "All", "Disable All Cast Shadow", CUnionValueProperty::Boolean, "모든 라이트의 캐스팅 쉐도우 옵션을 끈다.", TRUE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CObjectLightFolder::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	int nDir = 0, nPoint = 0, nSpot = 0;
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		switch( m_pVecChild[i]->GetType() ) {
			case CActionBase::LIGHTDIR: nDir++;	break;
			case CActionBase::LIGHTPOINT: nPoint++;	break;
			case CActionBase::LIGHTSPOT: nSpot++;	break;
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
						case CActionBase::LIGHTDIR:
							((CObjectLightDir*)m_pVecChild[i])->OnChangePropertyValue( 3, pVariable );
							break;
						case CActionBase::LIGHTPOINT:
							((CObjectLightPoint*)m_pVecChild[i])->OnChangePropertyValue( 4, pVariable );
							break;
						case CActionBase::LIGHTSPOT:
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
		CActionBase *pChild = m_pVecChild[i];
		switch( pChild->GetType() ) {
			case LIGHTDIR: VecDir.push_back( ((CObjectLightDir*)pChild)->GetLightInfo() );	break;
			case LIGHTPOINT: VecPoint.push_back( ((CObjectLightPoint*)pChild)->GetLightInfo() );	break;
			case LIGHTSPOT: VecSpot.push_back( ((CObjectLightSpot*)pChild)->GetLightInfo() );	break;
		}
	}

	if( szStr.IsEmpty() == true ) {
		CString szSubKey = REG_SUBKEY;
		szSubKey += "\\Light";

		// 일단 모든 라이트 정보를 전부 지워주구
		RegDeleteKey( HKEY_CURRENT_USER, szSubKey );


		// 저장 다시 한다.
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
	// 기존 라이트 다 지우구~
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		CGlobalValue::GetInstance().RemoveLightFromTreeID( m_pVecChild[i]->GetTreeItemID() );
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

	 
	CActionBase *pBase;
	for( DWORD i=0; i<VecDir.size(); i++ ) {
		pBase = new CObjectLightDir;
		pBase->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTDIR ) );
		CGlobalValue::GetInstance().AddLight( pBase );
		((CObjectLightDir*)pBase)->SetLightInfo( VecDir[i] );
	}

	for( DWORD i=0; i<VecPoint.size(); i++ ) {
		pBase = new CObjectLightPoint;
		pBase->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTPOINT ) );
		CGlobalValue::GetInstance().AddLight( pBase );
		((CObjectLightPoint*)pBase)->SetLightInfo( VecPoint[i] );
	}

	for( DWORD i=0; i<VecSpot.size(); i++ ) {
		pBase = new CObjectLightSpot;
		pBase->SetName( CGlobalValue::GetInstance().GetUniqueName( CActionBase::LIGHTSPOT ) );
		CGlobalValue::GetInstance().AddLight( pBase );
		((CObjectLightSpot*)pBase)->SetLightInfo( VecSpot[i] );
	}
}