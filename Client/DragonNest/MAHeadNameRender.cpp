#include "StdAfx.h"
#include "MAHeadNameRender.h"
#include "DnActor.h"
#include "EtFontMng.h"
#include "DnCamera.h"
#include "DnPlayerActor.h"
#include "DnPartyTask.h"
#include "DnFriendTask.h"
#include "DnGuildTask.h"
#include "DnMasterTask.h"
#include "DnIsolate.h"
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#include "DnBridgeTask.h"
#ifdef PRE_ADD_SHOW_MONACTION
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAHeadNameRender::MAHeadNameRender()
{
	m_fScale = 0.4f;
	m_fIncreaseHeight = 5.f;
#ifdef PRE_ADD_SHOW_MONACTION
	memset(&m_HighestNameForDebug, 0, sizeof(HeadNameStruct));
#endif
}

MAHeadNameRender::~MAHeadNameRender()
{
}


//// ComeBack
//void MAHeadNameRender::AddHeadNameElement( char cLine, EtTextureHandle hTexture, EtVector2 &vSize, float fHeightRevision,  WCHAR *wszStr, int nFontIndex, int nFontHeight, DWORD dwFontColor, DWORD dwShadowColor )
//{
//	if( cLine < 0 || cLine >= HEADNAME_LINEMAX ) return;
//
//	HeadNameStruct Struct;
//
//	Struct.Type = HeadNameElementTypeEnum::TextureNString;
//
//	// Texture.	
//	Struct.hTexture = hTexture;	
//	Struct.vSize = vSize;
////	Struct.fHeightRevision = fHeightRevision;
//	Struct.Coord = SUICoord( 0.f, 0.f, 1.f, 1.f );
//	
//	// Font.
//	//Struct.Type = HeadNameElementTypeEnum::String;
//	Struct.szStr = wszStr;
//	Struct.nFontIndex = nFontIndex;
//	Struct.nFontHeight = nFontHeight;
//	Struct.dwFontColor = dwFontColor;
//	Struct.dwShadowColor = dwShadowColor;
////	if( !hTexture )
//		Struct.fHeightRevision = 0.f;
//	Struct.fWidth = -1.0f;
//
//	m_VecHeadNameList[cLine].push_back( Struct );
//}

void MAHeadNameRender::AddHeadNameElement( char cLine, WCHAR *wszStr, int nFontIndex, int nFontHeight, DWORD dwFontColor, DWORD dwShadowColor )
{
	if( cLine < 0 || cLine >= HEADNAME_LINEMAX ) return;

	HeadNameStruct Struct;

	Struct.Type = HeadNameElementTypeEnum::String;
	Struct.szStr = wszStr;
	Struct.nFontIndex = nFontIndex;
	Struct.nFontHeight = nFontHeight;
	Struct.dwFontColor = dwFontColor;
	Struct.dwShadowColor = dwShadowColor;
	Struct.fHeightRevision = 0.f;
	Struct.fWidth = -1.0f;

	m_VecHeadNameList[cLine].push_back( Struct );
}

void MAHeadNameRender::AddHeadNameElement( char cLine, EtTextureHandle hTexture, float fWidth, float fHeight, DWORD dwColor, float fHeightRevision )
{
	AddHeadNameElement( cLine, hTexture, EtVector2( fWidth, fHeight ), dwColor, fHeightRevision );
}

void MAHeadNameRender::AddHeadNameElement( char cLine, EtTextureHandle hTexture, EtVector2 &vSize, DWORD dwColor, float fHeightRevision )
{
	if( cLine < 0 || cLine >= HEADNAME_LINEMAX ) return;

	HeadNameStruct Struct;

	Struct.Type = HeadNameElementTypeEnum::Texture;
	Struct.hTexture = hTexture;
	Struct.dwFontColor = dwColor;
	Struct.vSize = vSize;
	Struct.fHeightRevision = fHeightRevision;
	Struct.Coord = SUICoord( 0.f, 0.f, 1.f, 1.f );
	Struct.fWidth = -1.0f;

	m_VecHeadNameList[cLine].push_back( Struct );
}

void MAHeadNameRender::AddHeadNameElement( char cLine, EtTextureHandle hTexture, EtVector2 &vSize, int nCountX, int nCountY, int nIndex, DWORD dwColor, float fHeightRevision )
{
	if( cLine < 0 || cLine >= HEADNAME_LINEMAX ) return;

	HeadNameStruct Struct;

	Struct.Type = HeadNameElementTypeEnum::Texture;
	Struct.hTexture = hTexture;
	Struct.dwFontColor = dwColor;
	Struct.vSize = vSize;
	Struct.fHeightRevision = fHeightRevision;
	Struct.fWidth = -1.0f;

	float fWidth = 1.f / (float)nCountX;
	float fHeight = 1.f / (float)nCountY;
	Struct.Coord = SUICoord( fWidth * (float)( nIndex % nCountX ), fHeight * (float)( nIndex / nCountX ), fWidth, fHeight );

	m_VecHeadNameList[cLine].push_back( Struct );
}

void MAHeadNameRender::AddHeadNameElement( char cLine, EtTextureHandle hTexture, float fWidth, float fHeight, int nCountX, int nCountY, int nIndex, DWORD dwColor, float fHeightRevision )
{
	AddHeadNameElement( cLine, hTexture, EtVector2( fWidth, fHeight ), nCountX, nCountY, nIndex, dwColor, fHeightRevision );
}

#ifdef PRE_ADD_SHOW_MONACTION
void MAHeadNameRender::AddHeadNameElement_Highest(WCHAR *wszStr, int nFontIndex, int nFontHeight, DWORD dwFontColor, DWORD dwShadowColor)
{
	HeadNameStruct& Struct = m_HighestNameForDebug;

	Struct.Type = HeadNameElementTypeEnum::String;
	Struct.szStr = wszStr;
	Struct.nFontIndex = nFontIndex;
	Struct.nFontHeight = nFontHeight;
	Struct.dwFontColor = dwFontColor;
	Struct.dwShadowColor = dwShadowColor;
	Struct.fHeightRevision = 0.f;
	Struct.fWidth = -1.0f;
}
#endif

void MAHeadNameRender::Render()
{
	CDnActor *pActor = dynamic_cast<CDnActor *>(this);
	if( !pActor ) return;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( !hCamera ) return;

	SAABox box;
	pActor->GetBoundingBox(box);
	float fHeight = box.Max.y - box.Min.y + m_fIncreaseHeight;

	EtVector3 vPos = pActor->GetMatEx()->m_vPosition;
	vPos.y += fHeight;

	EtVector3 vTemp;
	for( int i=0; i<HEADNAME_LINEMAX; i++ ) {
		float fTotalWidth = 0.f;
		static std::vector<float> fVecWidth;
		fVecWidth.clear();
		int nCount = (int)m_VecHeadNameList[i].size();

		for( int j=0; j<nCount; j++ ) {
			fVecWidth.push_back( GetWidth( &m_VecHeadNameList[i][j], ( j+1 == nCount ) ? false : true ) );
			fTotalWidth += fVecWidth[ fVecWidth.size() - 1 ];
		}

		fTotalWidth /= 2.f;
		float fValue = -fTotalWidth;
		for( int j=0; j<nCount; j++ ) {
			vTemp = vPos;
			vTemp.y += ( i * 8.f );

			vTemp += hCamera->GetMatEx()->m_vXAxis * fValue;
			fValue += fVecWidth[j];
			
			//// PRE_ADD_NEWCOMEBACK
			//HeadNameStruct & rStruct = m_VecHeadNameList[i][j];
			//if( rStruct.Type == HeadNameElementTypeEnum::TextureNString )
			//	DrawElementEx( &m_VecHeadNameList[i][j], vTemp, ( j+1 == nCount ) ? false : true );
			//else
				DrawElement( &m_VecHeadNameList[i][j], vTemp, ( j+1 == nCount ) ? false : true );
		}

		m_VecHeadNameList[i].clear();
	}

#ifdef PRE_ADD_SHOW_MONACTION
	if (m_HighestNameForDebug.szStr.empty() == false)
	{
		float fTotalWidth = 0.f;
		fTotalWidth = GetWidth(&m_HighestNameForDebug, false);
		fTotalWidth /= 2.f;
		float fValue = -fTotalWidth;
		vTemp = vPos;
		
		if (CDnInterface::GetInstance().IsShowEnemyGauge(pActor->GetActorHandle()))
			vTemp.y += 3 * 8.f;
		vTemp += hCamera->GetMatEx()->m_vXAxis * fValue;
		fValue += fTotalWidth;

		DrawElement( &m_HighestNameForDebug, vTemp, false);

		memset(&m_HighestNameForDebug, 0, sizeof(HeadNameStruct));
	}
#endif
}

float MAHeadNameRender::GetWidth( HeadNameStruct *pStruct, bool bIncreaseSpace )
{	
	if( pStruct->fWidth >= 0.0f ) {
		return pStruct->fWidth;
	}
	switch( pStruct->Type ) {
		case HeadNameElementTypeEnum::String:
			{
				SUICoord Coord;
				std::wstring wszStr = pStruct->szStr;
				if( bIncreaseSpace ) wszStr += L" ";
				CEtFontMng::GetInstance().CalcTextRect(pStruct->nFontIndex, pStruct->nFontHeight, wszStr.c_str(), DT_VCENTER | DT_CENTER, Coord, -1);
				pStruct->fWidth = Coord.fWidth * ( GetEtDevice()->Width() * m_fScale );
			}
			break;
		case HeadNameElementTypeEnum::Texture:
			{
				SUICoord Coord;
				if (bIncreaseSpace) CEtFontMng::GetInstance().CalcTextRect(CDnActor::s_nFontIndex, 16, L" ", DT_VCENTER | DT_CENTER, Coord, -1);
				pStruct->fWidth = pStruct->vSize.x + ( Coord.fWidth * ( GetEtDevice()->Width() * m_fScale ) );
			}
			break;

		//// COMEBACK
		//case HeadNameElementTypeEnum::TextureNString:
		//	{
		//		SUICoord CoordString;
		//		std::wstring wszStr( pStruct->szStr );
		//		if( bIncreaseSpace ) wszStr += L" ";
		//		CEtFontMng::GetInstance().CalcTextRect( pStruct->nFontIndex, pStruct->nFontHeight, wszStr.c_str(), DT_VCENTER | DT_CENTER, CoordString, -1 );
		//		float fStrWidth = CoordString.fWidth * ( GetEtDevice()->Width() * m_fScale );


		//		SUICoord CoordTexture;
		//		if( bIncreaseSpace ) CEtFontMng::GetInstance().CalcTextRect( CDnActor::s_nFontIndex, 16, L" ", DT_VCENTER | DT_CENTER, CoordTexture, -1 );
		//		float fTexWidth = pStruct->vSize.x + ( CoordTexture.fWidth * ( GetEtDevice()->Width() * m_fScale ) );

		//		pStruct->fWidth = ( fStrWidth > fTexWidth ? fStrWidth : fTexWidth );
		//	}
		//	break;
	}
	return pStruct->fWidth;
}

void MAHeadNameRender::DrawElement( HeadNameStruct *pStruct, EtVector3 vPos, bool bIncreaseSpace )
{
	switch( pStruct->Type ) {
		case HeadNameElementTypeEnum::String:
			{
				SFontDrawEffectInfo FontEffectInfo;
				FontEffectInfo.nDrawType = SFontDrawEffectInfo::SHADOW;
				FontEffectInfo.nWeight = 2;
				FontEffectInfo.dwFontColor = pStruct->dwFontColor;
				FontEffectInfo.dwEffectColor = pStruct->dwShadowColor;

				std::wstring wszStr = pStruct->szStr;
				if( bIncreaseSpace ) wszStr += L" ";

				CEtFontMng::GetInstance().DrawTextW3D( pStruct->nFontIndex, pStruct->nFontHeight, wszStr.c_str(), vPos, FontEffectInfo, -1, m_fScale );
			}
			break;
		case HeadNameElementTypeEnum::Texture:
			{
				if( !pStruct->hTexture ) break;
				// DrawSprite3D 가 센터점을 넘겨주도록 되있어서 포지션을 다시 마춰줍니다.
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				EtVector3 vTemp = vPos;
				vTemp += hCamera->GetMatEx()->m_vXAxis * ( pStruct->vSize.x / 2.f );
				vTemp.y += ( -8.f * m_fScale ) + pStruct->fHeightRevision; // ( pStruct->vSize.y / 2.f ) 
				CEtSprite::GetInstance().Begin(0);
				CEtSprite::GetInstance().DrawSprite3D( (EtTexture*)pStruct->hTexture->GetTexturePtr(), pStruct->hTexture->Width(), pStruct->hTexture->Height(), pStruct->dwFontColor, pStruct->vSize, vTemp, 0.0f, &pStruct->Coord );
				CEtSprite::GetInstance().End();
			}
			break;
	}
}


//void MAHeadNameRender::DrawElementEx( HeadNameStruct *pStruct, EtVector3 vPos, bool bIncreaseSpace )
//{		
//	if( pStruct->hTexture )
//	{
//		// DrawSprite3D 가 센터점을 넘겨주도록 되있어서 포지션을 다시 마춰줍니다.
//		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
//		EtVector3 vTemp = vPos;
//		vTemp += hCamera->GetMatEx()->m_vXAxis * ( pStruct->vSize.x / 2.f );
//		//vTemp.y += ( -8.f * m_fScale ) + pStruct->fHeightRevision; // ( pStruct->vSize.y / 2.f ) 
//		CEtSprite::GetInstance().Begin(0);
//		CEtSprite::GetInstance().DrawSprite3D( (EtTexture*)pStruct->hTexture->GetTexturePtr(), pStruct->hTexture->Width(), pStruct->hTexture->Height(), pStruct->dwFontColor, pStruct->vSize, vTemp, 0.0f, &pStruct->Coord );
//		CEtSprite::GetInstance().End();
//	}
//
//	pStruct->hTexture->Width()
//
//	SFontDrawEffectInfo FontEffectInfo;
//	FontEffectInfo.nDrawType = SFontDrawEffectInfo::SHADOW;
//	FontEffectInfo.nWeight = 2;
//	FontEffectInfo.dwFontColor = pStruct->dwFontColor;
//	FontEffectInfo.dwEffectColor = pStruct->dwShadowColor;
//
//	std::wstring wszStr = pStruct->szStr;
//	if( bIncreaseSpace ) wszStr += L" ";
//
//	CEtFontMng::GetInstance().DrawTextW3D( pStruct->nFontIndex, pStruct->nFontHeight, wszStr.c_str(), vPos, FontEffectInfo, -1, m_fScale );
//
//}

#ifdef PRE_ADD_VIP
void MAHeadNameRender::GetVIPHeadIconFontColor(CDnPlayerActor* pActor, DWORD &dwFontColor, DWORD &dwFontColorS, bool bCheckStage)
{
	if (pActor->IsVIP())
	{
		if (bCheckStage)
		{
			if (pActor->IsAlwaysVIPHeadRender())
			{
				dwFontColor = textcolor::VIPNAME;
				dwFontColorS = textcolor::VIPNAME_S;
			}
		}
		else
		{
			dwFontColor = textcolor::VIPNAME;
			dwFontColorS = textcolor::VIPNAME_S;
		}
	}
}
#endif

void MAHeadNameRender::GetHeadIconFontColor( CDnPlayerActor *pActor, HeadNameFontColorType Type, DWORD &dwFontColor, DWORD &dwFontColorS )
{
	if( !pActor ) return;
	if( !CDnPartyTask::IsActive() ) return;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	dwFontColor = 0;
	dwFontColorS = 0;

	// 길드전 일경우
	if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
	{
		switch( Type ) 
		{
		case Normal :
			if( pActor->GetTeam() == PvPCommon::Team::A ) 
			{
				dwFontColor = textcolor::PVP_F_PLAYER;
				dwFontColorS = textcolor::PVP_F_PLAYER_S;
			}
			else if( pActor->GetTeam() == PvPCommon::Team::B )
			{
				dwFontColor = textcolor::PVP_E_PLAYER ;
				dwFontColorS = textcolor::PVP_E_PLAYER_SH;
			}
			else
			{
				dwFontColor = textcolor::PVP_F_PLAYER;
				dwFontColorS = textcolor::PVP_F_PLAYER_S;
			}
			return;
		case Appellation :
			if( pActor->GetTeam() == PvPCommon::Team::A ) 
			{
				dwFontColor = textcolor::PVP_F_PLAYER;
				dwFontColorS = textcolor::PVP_F_PLAYER_S;
			}
			else if( pActor->GetTeam() == PvPCommon::Team::B )
			{
				dwFontColor = textcolor::PVP_E_PLAYER ;
				dwFontColorS = textcolor::PVP_E_PLAYER_SH;
			}
			else
			{
				dwFontColor = textcolor::PVP_F_PLAYER;
				dwFontColorS = textcolor::PVP_F_PLAYER_S;
			}
			return;
		}
	}

	if( pActor->IsLocalActor() ) {
		switch( Type ) {
			case Normal:
				switch( CDnPartyTask::GetInstance().GetPartyRole() ) {
					case CDnPartyTask::SINGLE:
						dwFontColor = textcolor::PLAYERNAME;
						dwFontColorS = textcolor::PLAYERNAME_S;
#ifdef PRE_ADD_VIP
						GetVIPHeadIconFontColor(pActor, dwFontColor, dwFontColorS, false);
#endif
						break;
					case CDnPartyTask::LEADER:
						dwFontColor = textcolor::PARTYMASTERNAME;
						dwFontColorS = textcolor::PARTYMASTERNAME_S;

						// 운영자 난입에 의해 파티리더가 된거라면,
						if( GetPartyTask().IsSingleToPartyByGMTrace() )
						{
							dwFontColor = textcolor::PLAYERNAME;
							dwFontColorS = textcolor::PLAYERNAME_S;
						}

#ifdef PRE_ADD_VIP
						GetVIPHeadIconFontColor(pActor, dwFontColor, dwFontColorS, true);
#endif
						break;
					case CDnPartyTask::MEMBER:
						if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
						{
							dwFontColor = textcolor::PLAYERNAME;
							dwFontColorS = textcolor::PLAYERNAME_S;
						}
						else
						{
							dwFontColor = textcolor::PARTYMEMBERNAME;
							dwFontColorS = textcolor::PARTYMEMBERNAME_S;
						}
#ifdef PRE_ADD_VIP
						GetVIPHeadIconFontColor(pActor, dwFontColor, dwFontColorS, true);
#endif
						break;
				}
				break;
			case Appellation:
				switch( CDnPartyTask::GetInstance().GetPartyRole() ) {
					case CDnPartyTask::SINGLE:
						dwFontColor = textcolor::APPELLATION_SINGLE;
						dwFontColorS = textcolor::APPELLATION_SINGLE_S;
						break;
					case CDnPartyTask::LEADER:
						dwFontColor = textcolor::APPELLATION_LEADER;
						dwFontColorS = textcolor::APPELLATION_LEADER_S;

						// 운영자 난입에 의해 파티리더가 된거라면,
						if( GetPartyTask().IsSingleToPartyByGMTrace() )
						{
							dwFontColor = textcolor::APPELLATION_SINGLE;
							dwFontColorS = textcolor::APPELLATION_SINGLE_S;
						}
						break;
					case CDnPartyTask::MEMBER:
						if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
						{
							dwFontColor = textcolor::APPELLATION_SINGLE;
							dwFontColorS = textcolor::APPELLATION_SINGLE_S;
						}
						else
						{
							dwFontColor = textcolor::APPELLATION_MEMBER;
							dwFontColorS = textcolor::APPELLATION_MEMBER_S;
						}
						break;
				}
				break;
		}
	}
	else {
		switch( Type ) {
			case Normal:
				if( CDnPartyTask::GetInstance().IsPvpGame() )
				{
					if( CDnPartyTask::GetInstance().IsPartyMember( pActor->GetUniqueID() ) ) {
						if( pActor->GetTeam() != CDnActor::s_hLocalActor->GetTeam() ) {

							dwFontColor = textcolor::PVP_E_PLAYER ;
							dwFontColorS = textcolor::PVP_E_PLAYER_SH;	

							if(pActor && CDnActor::s_hLocalActor && pActor->GetTeam() == PvPCommon::Team::A && CDnActor::s_hLocalActor->GetTeam() ==PvPCommon::Team::Observer) // 관전자일때의 처리
							{
								dwFontColor = textcolor::PVP_F_PLAYER;
								dwFontColorS = textcolor::PVP_F_PLAYER_S;
							}
						}
						else {
							dwFontColor = textcolor::PVP_F_PLAYER ;
							dwFontColorS = textcolor::PVP_F_PLAYER_S;					
						}
					}
					else {
						dwFontColor = textcolor::ORANGE;
						dwFontColor = 0xFF000000;
					}
					return;
				}
				else {
					if( CDnPartyTask::GetInstance().IsPartyMember( pActor->GetUniqueID() ) 
						&& (pGameTask ? pGameTask->GetGameTaskType() != GameTaskType::Farm : true) ) 
					{
						if( CDnPartyTask::GetInstance().GetMasterSessionID() == pActor->GetUniqueID() ) {
							dwFontColor = textcolor::PARTYMASTERNAME;
							dwFontColorS = textcolor::PARTYMASTERNAME_S;
						}
						else {
							dwFontColor = textcolor::PARTYMEMBERNAME;
							dwFontColorS = textcolor::PARTYMEMBERNAME_S;
						}
						if( CDnIsolateTask::GetInstance().IsBlackList( pActor->GetName() ) ) {
							dwFontColor = textcolor::ISOLATENAME;
							dwFontColorS = textcolor::ISOLATENAME_S;
						}
#ifdef PRE_ADD_VIP
						GetVIPHeadIconFontColor(pActor, dwFontColor, dwFontColorS, true);
#endif
					}
					else {
						dwFontColor = textcolor::CHARNAME;
						dwFontColorS = textcolor::CHARNAME_S;
						if( CDnFriendTask::GetInstance().GetFriendInfo( pActor->GetName() ) ) {
							dwFontColor = textcolor::FRIENDNAME;
							dwFontColorS = textcolor::FRIENDNAME_S;
						}
						if( CDnMasterTask::GetInstance().IsMasterPlayer( pActor->GetName() ) ) {
							dwFontColor = textcolor::MASTERNAME;
							dwFontColorS = textcolor::MASTERNAME_S;
						}
						if( CDnGuildTask::GetInstance().GetGuildMemberFromName( pActor->GetName() ) ) {
							dwFontColor = textcolor::GUILDMEMBERNAME;
							dwFontColorS = textcolor::GUILDMEMBERNAME_S;
						}
						if( CDnIsolateTask::GetInstance().IsBlackList( pActor->GetName() ) ) {
							dwFontColor = textcolor::ISOLATENAME;
							dwFontColorS = textcolor::ISOLATENAME_S;
						}
#ifdef PRE_ADD_VIP
						GetVIPHeadIconFontColor(pActor, dwFontColor, dwFontColorS, false);
#endif
					}
				}
				break;
			case Appellation:
				if( CDnPartyTask::GetInstance().IsPartyMember( pActor->GetUniqueID() ) 
					&& (pGameTask ? pGameTask->GetGameTaskType() != GameTaskType::Farm : true) ) 
				{
					if( CDnPartyTask::GetInstance().GetMasterSessionID() == pActor->GetUniqueID() ) {
						dwFontColor = textcolor::APPELLATION_LEADER;
						dwFontColorS = textcolor::APPELLATION_LEADER_S;
					}
					else {
						dwFontColor = textcolor::APPELLATION_MEMBER;
						dwFontColorS = textcolor::APPELLATION_MEMBER_S;
					}
					if( CDnIsolateTask::GetInstance().IsBlackList( pActor->GetName() ) ) {
						dwFontColor = textcolor::APPELLATION_ISOLATE;
						dwFontColorS = textcolor::APPELLATION_ISOLATE_S;
					}
				}
				else {
					dwFontColor = textcolor::APPELLATION_ANY;
					dwFontColorS = textcolor::APPELLATION_ANY_S;

					if( CDnFriendTask::GetInstance().GetFriendInfo( pActor->GetName() ) ) {
						dwFontColor = textcolor::APPELLATION_FRIEND;
						dwFontColorS = textcolor::APPELLATION_FRIEND_S;
					}
					if( CDnMasterTask::GetInstance().IsMasterPlayer( pActor->GetName() ) ) {
						dwFontColor = textcolor::APPELLATION_MASTER;
						dwFontColorS = textcolor::APPELLATION_MASTER_S;
					}
					if( CDnGuildTask::GetInstance().GetGuildMemberFromName( pActor->GetName() ) ) {
						dwFontColor = textcolor::APPELLATION_GUILD;
						dwFontColorS = textcolor::APPELLATION_GUILD_S;
					}
					if( CDnIsolateTask::GetInstance().IsBlackList( pActor->GetName() ) ) {
						dwFontColor = textcolor::APPELLATION_ISOLATE;
						dwFontColorS = textcolor::APPELLATION_ISOLATE_S;
					}
				}
				break;
		}
	}
}