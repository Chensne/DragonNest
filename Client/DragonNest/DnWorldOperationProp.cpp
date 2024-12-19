#include "StdAfx.h"
#include "DnWorldOperationProp.h"
#include "DNProtocol.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldOperationProp::CDnWorldOperationProp()
{
	m_bEnableOperator = true;
	m_nCrosshairType = CDnLocalPlayerActor::CrossHairType::Lever;
}

CDnWorldOperationProp::~CDnWorldOperationProp()
{
}

void CDnWorldOperationProp::CmdOperation()
{
	if( !m_bEnableOperator ) return;
	Send( eProp::CS_CMDOPERATION, NULL );
}

void CDnWorldOperationProp::OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) {
		case eProp::SC_CMDENABLEOPERATOR:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				Stream.Read( &m_bEnableOperator, sizeof(bool) );
			}
			break;
	}
	CDnWorldActProp::OnDispatchMessage( dwProtocol, pPacket );
}

bool CDnWorldOperationProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() ) {
		OperationStruct *pStruct = (OperationStruct *)GetData();

		switch( pStruct->nIconType ) {
			case 0: m_nCrosshairType = CDnLocalPlayerActor::CrossHairType::Lever; break;
			case 1: m_nCrosshairType = CDnLocalPlayerActor::CrossHairType::Pickup; break;
			case 2: m_nCrosshairType = CDnLocalPlayerActor::CrossHairType::Seed;	break;
			case 3: m_nCrosshairType = CDnLocalPlayerActor::CrossHairType::Chest_UnLock; break;
		}
	}

	return true;
}

int CDnWorldOperationProp::GetCrosshairType()
{
	return m_nCrosshairType;
}

void CDnWorldOperationProp::SetCrosshairType( int nCrosshairType )
{
	m_nCrosshairType = nCrosshairType;
}