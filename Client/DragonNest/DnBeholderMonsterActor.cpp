#include "StdAfx.h"
#include "DnBeholderMonsterActor.h"
#include "MAAiBase.h"
#include "DnTableDB.h"
#include "DnActorClassDefine.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

/*
º»-------------Ãæµ¹Ä¸½¶---------ÃË¼ö¸Þ½¬
bone39        Capsule03        feeler03
bone89        Capsule06        feeler11
bone33        Capsule02        feeler12
bone65        Capsule04        feeler13
bone85        Capsule09        feeler09
bone69        Capsule14        feeler10
bone73        Capsule15        feeler02
bone57        Capsule11        feeler05
bone48        Capsule05        feeler04
bone81        Capsule07        feeler14
bone77        Capsule08        feeler07
bone24        Capsule10        feeler06
bone61        Capsule12        feeler01
bone12        Capsule13        feeler08
*/


struct __FeelerInfo 
{
	std::string szBoneName;
	int	nBoneIndex;
	std::string szSubMeshName;
	int nSubMeshIndex;
};

// key º»ÀÎµ¦½º, 
static std::map<int, __FeelerInfo>	gs_FeelerInfoMap;

__FeelerInfo __FindFeelerInfo(int nBoneName)
{
	std::map<int, __FeelerInfo>::iterator it;
	it = gs_FeelerInfoMap.find(nBoneName);
	if ( it != gs_FeelerInfoMap.end() )
		return it->second;

	// ¾øÀ¸¸é 
	__FeelerInfo _dummy;
	_dummy.nBoneIndex = -1;
	_dummy.nSubMeshIndex = -1;
	_dummy.szBoneName = "";
	_dummy.szSubMeshName = "";
	return _dummy;
}

CDnBeholderMonsterActor::CDnBeholderMonsterActor( int nClassID, bool bProcess )
: CDnPartsMonsterActor( nClassID, bProcess )
, m_nEyeBoneIndex( 0 )
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
	m_DummyBeholderEye = CreateActor(99, false, false, false );
	if( m_DummyBeholderEye ) {
		m_DummyBeholderEye->SetProcess( false );
		m_DummyBeholderEye->Show(false);
	}
}

CDnBeholderMonsterActor::~CDnBeholderMonsterActor()
{
	SAFE_RELEASE_SPTR( m_DummyBeholderEye );
}

bool CDnBeholderMonsterActor::Initialize()
{ 
	CDnPartsMonsterActor::Initialize();

	m_nEyeBoneIndex = GetBoneIndex("~eye");

	if ( gs_FeelerInfoMap.empty() )
	{
		__FeelerInfo info;
	
		info.szBoneName = "bone61";
		info.szSubMeshName = "feeler01";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone73";
		info.szSubMeshName = "feeler02";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone39";
		info.szSubMeshName = "feeler03";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone48";
		info.szSubMeshName = "feeler04";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone57";
		info.szSubMeshName = "feeler05";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone24";
		info.szSubMeshName = "feeler06";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone77";
		info.szSubMeshName = "feeler07";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone12";
		info.szSubMeshName = "feeler08";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone85";
		info.szSubMeshName = "feeler09";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone69";
		info.szSubMeshName = "feeler10";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone89";
		info.szSubMeshName = "feeler11";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone33";
		info.szSubMeshName = "feeler12";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone65";
		info.szSubMeshName = "feeler13";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));

		info.szBoneName = "bone81";
		info.szSubMeshName = "feeler14";
		info.nBoneIndex = GetBoneIndex(info.szBoneName.c_str());
		info.nSubMeshIndex = GetSubMeshIndex(info.szSubMeshName.c_str());
		gs_FeelerInfoMap.insert( make_pair( info.nBoneIndex, info));
	}

	SetUseDamageColor(true);

	return true;
}

int ___g_bool = 1;

void CDnBeholderMonsterActor::PreProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPartsMonsterActor::PreProcess( LocalTime, fDelta );
	
/*
	if ( ___g_bool )
	{
		LookTarget(CDnActor::Identity());
	}
*/
	DnActorHandle hAggroActor = GetAggroTarget();
	if ( hAggroActor )
	{
		D3DXVECTOR3 vRot(0.0f,0.0f,0.0f);
		EtVector3 v = *(hAggroActor->GetPosition()) - *(GetPosition());
		v.y = 0.0f;

		
		EtVec3Normalize(&v, &v);
		float fDot = EtVec3Dot(&v, &GetMatEx()->m_vZAxis );
		
		float fAngle = EtToDegree( EtAcos(min(fDot, 1.0f)));
		if ( fAngle != 0.0f )
			vRot.y = -fAngle;
		vRot.x = 6.0f;

		//EtVec3Normalize(&v, &v);
		float fDot2 = EtVec3Dot(&v, &GetMatEx()->m_vXAxis );
		float fAngle2 = EtToDegree( EtAcos(min(fDot2, 1.0f)));
		if ( fAngle2 <= 90.0f )
		{
			vRot.y = fAngle;
		}
		if ( vRot.y > 42.0f )	vRot.y = 42.0f;
		if ( vRot.y < -42.0f )	vRot.y = -42.0f;

	
		m_hObject->SetBoneRotation( m_nEyeBoneIndex, vRot );
	}

}


void CDnBeholderMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPartsMonsterActor::Process(LocalTime, fDelta);

	if( !IsDie() ) {
		DNVector(DnActorHandle) hVecList;
		EtVector3 vAniDist = *GetPosition();
		vAniDist.x -= GetAniDistance()->x;
		vAniDist.z -= GetAniDistance()->z;

		int nCount =  ScanActor( vAniDist, (float)GetUnitSize(), hVecList );
		if( nCount > 0 ) {
			EtVector2 vDir;
			int nSumSize;
			for( int i=0; i<nCount; i++ ) {
				if( hVecList[i] == GetMySmartPtr() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				vDir = EtVec3toVec2( *hVecList[i]->GetPosition() - *GetPosition() );
				nSumSize = GetUnitSize() + hVecList[i]->GetUnitSize();
				float fLength = D3DXVec2Length( &vDir );
				if( fLength > (float)nSumSize ) continue;

				D3DXVec2Normalize( &vDir, &vDir );
				MAMovementBase *pMovement = (hVecList[i]->GetMovement());
				if( pMovement ) pMovement->MoveToWorld( vDir * (float)( nSumSize - fLength ) );
			}
		}
	}
}


/*
º»-------------Ãæµ¹Ä¸½¶---------ÃË¼ö¸Þ½¬
bone39        Capsule03        feeler03
bone89        Capsule06        feeler11
bone33        Capsule02        feeler12
bone65        Capsule04        feeler13
bone85        Capsule09        feeler09
bone69        Capsule14        feeler10
bone73        Capsule15        feeler02
bone57        Capsule11        feeler05
bone48        Capsule05        feeler04
bone81        Capsule07        feeler14
bone77        Capsule08        feeler07
bone24        Capsule10        feeler06
bone61        Capsule12        feeler01
bone12        Capsule13        feeler08
*/

void CDnBeholderMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	CDnPartsMonsterActor::OnDamage( pHitter, HitParam );

#if 0
	__FeelerInfo info = __FindFeelerInfo(HitParam.nBoneIndex);

	if ( info.nSubMeshIndex > -1 )
	{
		OutputDebug("OnDamage Hide SubMesh : %d\n", info.nSubMeshIndex );
		m_hObject->ShowSubmesh( info.nSubMeshIndex, false);
	}
#endif
}