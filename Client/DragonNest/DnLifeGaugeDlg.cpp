#include "Stdafx.h"
#include "DnLifeSkillPlantTask.h"
#include "DnTableDB.h"
#include "DnLifeGaugeDlg.h"
#include "DnInterface.h"
#include "DnWorldOperationProp.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildTask.h"


CDnLifeGaugeDlg::CDnLifeGaugeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( UI_TYPE_BOTTOM, pParentDialog, nID, pCallback  )
, m_pTimeBar( NULL )
, m_pWaterBar( NULL )
, m_pStaticName( NULL )
, m_pStaticGuildName( NULL )
, m_pMarkTexture( NULL )
, m_pProp( NULL )
, m_nItemID( 0 )
, m_eGrowLevel( 0 )
, m_bGuild( false )
{
}

CDnLifeGaugeDlg::~CDnLifeGaugeDlg()
{
}

void CDnLifeGaugeDlg::Initialize(bool bShow)
{
#if defined(PRE_REMOVE_FARM_WATER)
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeGaugeDlg.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeGauge_WDlg.ui" ).c_str(), bShow );
#endif	// #if defined(PRE_REMOVE_FARM_WATER)
}

void CDnLifeGaugeDlg::InitialUpdate()
{
	m_pTimeBar  = GetControl<CEtUIProgressBar>( "ID_PRB_TIME" );
	m_pWaterBar = GetControl<CEtUIProgressBar>( "ID_PRB_WATER" );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT1" );
	m_pStaticGuildName = GetControl<CEtUIStatic>( "ID_TEXT0" );
	m_pMarkTexture = GetControl<CEtUITextureControl>( "ID_TEXTURE_MARK" );

	m_pStaticName->SetTextColor( textcolor::PLAYERNAME );
	m_pStaticGuildName->SetTextColor( textcolor::PLAYERNAME );

#if defined(PRE_REMOVE_FARM_WATER)
	m_pWaterBar->Show( false );
#endif // PRE_REMOVE_FARM_WATER

	SetPosition( -100.0f, -100.0f );
}

void CDnLifeGaugeDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnLifeGaugeDlg::SetTimeBar(float fProgress)
{
	m_pTimeBar->SetProgress( fProgress );
}

void CDnLifeGaugeDlg::SetWaterBar(float fProgress)
{
	m_pWaterBar->SetProgress( fProgress );
}

void CDnLifeGaugeDlg::UpdateGauge( int nItemID, int eGrowLevel )
{
	m_nItemID = nItemID;
	m_eGrowLevel = eGrowLevel;
}

void CDnLifeGaugeDlg::SetGuildInfo( bool bGuild, const TGuildView &GuildView, WCHAR * wszName )
{
	m_bGuild = bGuild;

	m_pStaticName->Show( bGuild );
	m_pStaticGuildName->Show( bGuild );
	m_pMarkTexture->Show( bGuild );

	if( bGuild )
	{
		m_pStaticName->SetText(wszName);
		WCHAR wszGuildName[GUILDNAME_MAX+2];
		swprintf_s( wszGuildName, _countof(wszGuildName), L"<%s>", GuildView.wszGuildName );
		m_pStaticGuildName->SetText( wszGuildName );

		if( GetGuildTask().IsShowGuildMark( GuildView ) )
		{
			EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( GuildView );
			m_pMarkTexture->SetTexture( hGuildMark );
			m_pMarkTexture->Show( true );
		}
	}
	else
	{
		// 길드는 없지만 자신것은 표시 해 준다.
		if (!CDnLocalPlayerActor::s_hLocalActor)	return;
		CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
		if( !pLocalActor )	return;

		DnActorHandle hActor = CDnActor::FindActorFromName( wszName );
		if( !hActor )	return;

		if( hActor->GetUniqueID() == pLocalActor->GetUniqueID() )
		{
			m_pStaticName->SetText( wszName);	
			m_pStaticName->Show( true );
		}
	}
}

void CDnLifeGaugeDlg::Render(float fElapsedTime)
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	EtVector3 etVector = localActor->GetMatEx()->m_vPosition - m_pProp->GetMatEx()->m_vPosition;

	if( 1000.f < EtVec3Length( &etVector ) )
		return;

	if( !m_pProp )
		return;

	if( GetInterface().IsOpenBlind() )
		return;

	if( !CEtCamera::GetActiveCamera() ) 
		return;

	SAABox box;
	EtVector4 vUIPos, vSample;
	EtVector3 vPos;
	float fHeight = 0.f;
	static float fMovePos = 50.0f;
	static float fMinY = 0.025f;
	SUICoord dlgCoord;

	DNTableFileFormat* pFarmSox = GetDNTable( CDnTableDB::TFARMCULTIVATE );
	DNTableFileFormat* pSkinSox = GetDNTable( CDnTableDB::TFARMSKIN );

	int nPropID = pFarmSox->GetFieldFromLablePtr( m_nItemID, "_SkinID" )->GetInteger();
	if( CDnLifeSkillPlantTask::GROW_LEVEL1 == m_eGrowLevel )
		fHeight = pSkinSox->GetFieldFromLablePtr( nPropID, "_1StepHead" )->GetFloat();
	else if( CDnLifeSkillPlantTask::GROW_LEVEL2 == m_eGrowLevel )
		fHeight = pSkinSox->GetFieldFromLablePtr( nPropID, "_2StepHead" )->GetFloat();

	m_pProp->GetBoundingBox(box);
	fHeight *= m_pProp->GetScale()->y;
	vPos = m_pProp->GetMatEx()->m_vPosition;
	vPos.y += fHeight+30.0f;

	EtVec4Transform( &vUIPos, &EtVector4(vPos, 1.0f), CEtCamera::GetActiveCamera()->GetViewMat() );
	if( vUIPos.z < 0.0f ) {
		// 화면 안쪽이라면..(이 코드 없어도 되긴 한데 그냥 안전용으로 넣어둔다.)
		SetPosition( -100.0f, -100.0f );
		return;
	}

	vSample = vUIPos;
	vSample.z -= fMovePos;			// 몬스터에 가리는거 방지하기 위해서 조금 앞으로 땡긴다.
	EtVec4Transform( &vUIPos, &vUIPos, CEtCamera::GetActiveCamera()->GetProjMat() );	
	EtVec4Transform( &vSample, &vSample, CEtCamera::GetActiveCamera()->GetProjMat() );	
	vUIPos.x /= vUIPos.w;
	vUIPos.y /= vUIPos.w;
	vUIPos.z = vSample.z / vSample.w;

	m_pTimeBar->SetZValue( vUIPos.z );
	m_pWaterBar->SetZValue( vUIPos.z );

	vUIPos.x = (vUIPos.x*0.5f)+0.5f;
	vUIPos.x *= GetScreenWidthRatio();
	vUIPos.y = (vUIPos.y*0.5f)+0.5f;
	vUIPos.y = 1.0f - vUIPos.y;
	vUIPos.y *= GetScreenHeightRatio();

	GetDlgCoord( dlgCoord );
	dlgCoord.SetPosition( vUIPos.x-(dlgCoord.fWidth * 0.5f), vUIPos.y );
	SetDlgCoord( dlgCoord );

	CEtUIDialog::Render( fElapsedTime );
}

