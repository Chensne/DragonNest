#include "StdAfx.h"
#include "DnWorldProp.h"
#include "DnWorldSector.h"
#include "DnTableDB.h"

DECL_MULTISMART_PTR_STATIC( CDnWorldProp, MAX_SESSION_COUNT, 500 )

CDnWorldProp::CDnWorldProp( CMultiRoom *pRoom )
: CMultiSmartPtrBase< CDnWorldProp, MAX_SESSION_COUNT >( pRoom )
{
	EtMatrixIdentity( &m_matWorld );

	m_PropType = PTE_Static;
	m_bIsStaticCollision = true;
	m_bShow = true;
	m_bProjectileSkip = false;
	m_bEnableAggro = false;
	m_bEnableOperator = true;

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	m_bRandomVisibleProp = false;
	#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	ClearVisibleOptions();
	#else
	m_bRandomResultIsVisible = true;
	#endif
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE
}

CDnWorldProp::~CDnWorldProp()
{
	SAFE_RELEASE_SPTR( m_Handle );
}

bool CDnWorldProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	ClearVisibleOptions();
#endif

	bool bResult = CEtWorldProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult == false ) return false;

	int nTableID = ((CDnWorldSector*)pParentSector)->GetPropTableID( szPropName );
	if( nTableID != -1 ) {
		if( InitializeTable( nTableID ) == false ) return false;
	}
	if( CreateObject() == false ) return false;

	if( m_Handle ) {
		m_Handle->SetCollisionScale( max( max( vScale.x, vScale.y ), vScale.z ) );
		if( m_bIsStaticCollision ) m_Handle->SetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) );
		else m_Handle->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 2 ) );
		m_Handle->SetTargetCollisionGroup( 0 );
		UpdateMatrix();
	}

	return true;
}

bool CDnWorldProp::InitializeTable( int nTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );

	m_PropType = (PropTypeEnum)pSox->GetFieldFromLablePtr( nTableID, "_ClassID" )->GetInteger();

	// CDnWorldImp 이면 Preload 이기 때문에 무조건 true 리턴해준다.
	CDnWorld *pWorld = dynamic_cast<CDnWorld *>(GetSector()->GetParentGrid()->GetWorld());
	if( pWorld == NULL ) return true;

	// 찍힐 확률 체크해서 리턴
	int nValue = (int)( pSox->GetFieldFromLablePtr( nTableID, "_VisibleProb" )->GetFloat() * 100.f );

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	if( 100 != nValue )
	{
		m_bRandomVisibleProp = true;
	#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
		m_VisibleOptions[PropDef::Option::eRandomCreate] = PropDef::Option::True;
		if (_rand(GetRoom())%100 > nValue)
			m_VisibleOptions[PropDef::Option::eRandomCreate] = PropDef::Option::False;
	#else
		m_bRandomResultIsVisible = true;
		if( _rand(GetRoom())%100 > nValue )
			m_bRandomResultIsVisible = false;
	#endif
	}
#else
	if( _rand(GetRoom())%100 > nValue ) return false;
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE

	// 날씨 체크해서 리턴
	std::string szWeather;
	szWeather = pSox->GetFieldFromLablePtr( nTableID, "_VisibleWeather" )->GetString();
	char *szWeatherString[EWorldEnum::WeatherEnum_Amount] = { "FineDay", "FineGlow", "FineNight", "FineDawn", "CloudyDay", "CloudyGlow", "CloudyNight", "CloudyDawn", "RainDay", "RainNight", "HeavyrainDay", "HeavyrainNight" };
	bool bVisible = false;
	if( strcmp( szWeather.c_str(), "All" ) == NULL ) bVisible = true;
	else { // All 이 아니라면 날씨체크
		EWorldEnum::WeatherEnum Weather = CDnWorld::GetInstance(GetRoom()).GetCurrentWeather();
		for( int i=0; ; i++ ) {
			//char *szTemp = _GetSubStrByCount( i, (char*)szWeather.c_str(), '+' );
			std::string strTemp = _GetSubStrByCountSafe( i, (char*)szWeather.c_str(), '+' );
			if( strTemp.size() == 0 ) break;
			if( strcmp( strTemp.c_str(), szWeatherString[Weather] ) == NULL ) {
				bVisible = true;
				break;
			}
		}
	}
	if( bVisible == false ) return false;

	m_bIsStaticCollision	= ( pSox->GetFieldFromLablePtr( nTableID, "_IsStaticCollision" )->GetInteger() == TRUE ) ? true : false;
	m_bProjectileSkip		= ( pSox->GetFieldFromLablePtr( nTableID, "_IsProjectileSkip" )->GetInteger() == TRUE ) ? true : false;
	m_bEnableAggro			= ( pSox->GetFieldFromLablePtr( nTableID, "_IsMonsterAggro" )->GetInteger() == TRUE ) ? true : false;

	return true;
}

bool CDnWorldProp::CreateObject()
{
	m_Handle = EternityEngine::CreateStaticObject( GetRoom(), GetPropName() );
	if( !m_Handle ) return false;
	return true;
}

void CDnWorldProp::UpdateMatrix()
{
	EtVector3 vOffset = *m_pParentSector->GetOffset();
	vOffset.x -= ( m_pParentSector->GetTileWidthCount() * m_pParentSector->GetTileSize() ) / 2.f;
	vOffset.z -= ( m_pParentSector->GetTileHeightCount() * m_pParentSector->GetTileSize() ) / 2.f;
	vOffset.y = 0.f;

	m_Cross.m_vPosition = m_vPosition + vOffset;
	m_Cross.RotatePitch( m_vRotation.x );
	m_Cross.RotateRoll( m_vRotation.z );
	m_Cross.RotateYaw( m_vRotation.y );

	EtMatrix matScale;
	m_matWorld = *m_Cross;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &m_matWorld, &matScale, &m_matWorld );

	m_Handle->Update( &m_matWorld );
}

DnPropHandle CDnWorldProp::FindPropFromUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID )
{
	DnPropHandle hProp;
	for( int i=0; i<GetItemCount(pRoom); i++ ) {
		hProp = GetItem(pRoom, i);
		if( hProp ) {
			if( hProp->GetUniqueID () == dwUniqueID ) return hProp;
		}
	}
	return CDnWorldProp::Identity();
}

// pBreakIntoGameSession != NULL 인 경우에는 pBreakIntoGameSession 에게만 해당 패킷을 보낸다.
void CDnWorldProp::CmdShow( bool bShow, CDNUserSession* pBreakIntoGameSession/*=NULL*/ )
{
	m_bShow = bShow;
	EnableCollision( bShow );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &bShow, sizeof(bool) );

	if( pBreakIntoGameSession )
		Send( eProp::SC_CMDSHOW, &Stream, pBreakIntoGameSession );
	else
		Send( eProp::SC_CMDSHOW, &Stream );
}

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
bool CDnWorldProp::SetShowInfo(PropDef::Option::eTypeWithPriority option, PropDef::Option::eValue value)
{
	if (option < PropDef::Option::eMinVisibleOption || option >= PropDef::Option::eMaxVisibleOption)
		return false;

	m_VisibleOptions[option] = value;

	SendPropShowInfo();

	return true;
}

void CDnWorldProp::SendPropShowInfo()
{
	const DWORD dwVisibleOptionSize = (sizeof(PropDef::Option::eValue) * PropDef::Option::eMaxVisibleOption);
	BYTE pBuffer[128];
	CPacketCompressStream Stream(pBuffer, 128);

	Stream.Write(m_VisibleOptions, dwVisibleOptionSize);

	Send(eProp::SC_SEND_SHOWINFO, &Stream);
}

bool CDnWorldProp::IsVisible() const
{
	int i = PropDef::Option::eMaxVisibleOption - 1;
	for (; i >= PropDef::Option::eMinVisibleOption && i >= 0; --i)
	{
		if (m_VisibleOptions[i] != PropDef::Option::NotSet)
		{
			return (m_VisibleOptions[i] == PropDef::Option::True);
		}
	}

	return true;
}
#endif

void CDnWorldProp::CmdChatBalloon( int nUIStringIndex )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nUIStringIndex, sizeof(int) );	

	Send( eProp::SC_CMDCHATBALLOON_AS_INDEX, &Stream );
}

void CDnWorldProp::EnableCollision( bool bEnable )
{
	if( IsIgnoreBuildColMesh() ) return;
	if( m_Handle ) m_Handle->EnableCollision( bEnable );
}

void CDnWorldProp::EnableIgnoreBuildColMesh( bool bEnable ) 
{
	CEtWorldProp::EnableIgnoreBuildColMesh( bEnable );
	if( m_Handle ) m_Handle->EnableCollision( !bEnable );
}

void* CDnWorldProp::AllocPropData(int& usingCount)
{
#ifdef WIN64
	usingCount = GetPropSignalDataUsingCount64(m_nClassID);
#else
	usingCount = GetPropSignalDataUsingCount(m_nClassID);
#endif
	if (usingCount == 0)
		return NULL;

	return new int[usingCount];
}

void CDnWorldProp::EnableOperator( bool bEnable )
{
	m_bEnableOperator = bEnable;
}

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
void CDnWorldProp::ClearVisibleOptions()
{
	int i = 0;
	for (; i < PropDef::Option::eMaxVisibleOption; ++i)
	{
		m_VisibleOptions[i] = PropDef::Option::NotSet;
	}
}
#endif