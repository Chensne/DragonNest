#include "StdAfx.h"
#include "DnHitPropComponent.h"
#include "DnWorldActProp.h"

CDnHitPropComponent::CDnHitPropComponent(void) : m_LastHitSignalTime( 0 ),
										 m_nLastHitSignalIndex( 0 ),
										 m_pPropMatrixEx( NULL ),
										 m_pDamageBase( NULL )
{
}

CDnHitPropComponent::~CDnHitPropComponent(void)
{
}

#ifdef PRE_ADD_HIT_PROP_ADD_SE
void CDnHitPropComponent::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
									LOCAL_TIME SignalEndTime, int nSignalIndex, DNVector(DnActorHandle)* pvlhHittedActor /*= NULL*/ )
#else
void CDnHitPropComponent::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
								    LOCAL_TIME SignalEndTime, int nSignalIndex )
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE
{
	if( m_hMonster ) *(m_hMonster->GetMatEx()) = *m_pPropMatrixEx;

	switch( Type ) {
		case STE_Hit:
			{
				if( !m_hMonster ) break;
				HitStruct *pStruct = (HitStruct *)pPtr;

				DNVector(DnActorHandle) hVecList;

				// 프랍이 갖고 있는 rotation 값까지 적용시키기 위해서.
				MatrixEx CrossTemp = *m_pPropMatrixEx;
				EtVector3 vOffset = *pStruct->vOffset * m_hProp->GetScale()->y;
				CrossTemp.MoveLocalZAxis( vOffset.z );
				CrossTemp.MoveLocalXAxis( vOffset.x );
				CrossTemp.MoveLocalYAxis( vOffset.y );

				CDnDamageBase::SHitParam HitParam;
				HitParam.hWeapon = m_hMonster->GetWeapon();
				int nWeaponLength = 0;
				if( pStruct->bIncludeWeaponLength && HitParam.hWeapon ) 
					nWeaponLength = HitParam.hWeapon->GetWeaponLength();

				EtVector3 vPos = CrossTemp.m_vPosition;

				float fDistanceMax = pStruct->fDistanceMax * m_hProp->GetScale()->y;
				float fDistanceMin = pStruct->fDistanceMin * m_hProp->GetScale()->y;
				float fHeightMax = pStruct->fHeightMax * m_hProp->GetScale()->y;
				float fHeightMin = pStruct->fHeightMin * m_hProp->GetScale()->y;

				float fDistance = max( fDistanceMax, (fHeightMax - fHeightMin) );
				float fXZDistanceSQ = fDistanceMax/* + nWeaponLength*/;
				float fXZDistanceMinSQ = fDistanceMin;

				//float fDistance = max( pStruct->fDistanceMax, (pStruct->fHeightMax - pStruct->fHeightMin) );
				//float fXZDistanceSQ = pStruct->fDistanceMax + nWeaponLength;
				//float fXZDistanceMinSQ = pStruct->fDistanceMin;

				fXZDistanceSQ *= fXZDistanceSQ;
				fXZDistanceMinSQ *= fXZDistanceMinSQ;

				CDnActor::ScanActor( m_hMonster->GetRoom(), vPos, fDistance, hVecList );

				EtVector3 vDir;
				EtVector3 vZVec = m_pPropMatrixEx->m_vZAxis;

				if( pStruct->fCenterAngle != 0.f ) {
					EtMatrix matRotate;
					EtMatrixRotationY( &matRotate, EtToRadian( pStruct->fCenterAngle ) );
					EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );

#if defined(PRE_FIX_63356)
					CrossTemp.m_vZAxis = vZVec;
#endif // PRE_FIX_63356
				}
				SAABox Box;
				float fDot;

				bool bHit = false;
				bool bFirstHit = true;
				if( m_LastHitSignalTime > LocalTime ) bFirstHit = false;
				if( m_nLastHitSignalIndex != nSignalIndex ) {
					bFirstHit = true;
					m_nLastHitSignalIndex = nSignalIndex;
				}

				HitParam.szActionName = pStruct->szTargetHitAction;
				HitParam.fDamage = pStruct->fDamageProb * 0.01f;
				HitParam.fDurability = pStruct->fDurabilityDamageProb * 0.01f;
				HitParam.vVelocity = *pStruct->vVelocity;
				HitParam.vResistance = *pStruct->vResistance;
				HitParam.vPosition = vPos;
				HitParam.fStiffProb = pStruct->fStiffProb * 0.01f;
				HitParam.RemainTime = SignalEndTime;
				HitParam.nDamageType = pStruct->nDamageType;
				HitParam.nSkillSuperAmmorIndex = pStruct->nApplySuperAmmorIndex - 1;
				HitParam.nSkillSuperAmmorDamage = pStruct->nApplySuperAmmorDamage;
				HitParam.DistanceType			= (pStruct->nDistanceType == 0) ? CDnDamageBase::DistanceTypeEnum::Melee : CDnDamageBase::DistanceTypeEnum::Range;
				HitParam.bIgnoreCanHit = ( pStruct->bIgnoreCanHit == TRUE );
				HitParam.bIgnoreParring = ( pStruct->bIgnoreParring == TRUE );

				// Actor 체크
				for( DWORD i=0; i<hVecList.size(); i++ ) {
					if( !hVecList[i]->IsHittable( m_hMonster, LocalTime, pStruct ) ) 
						continue;

					switch( hVecList[i]->GetHitCheckType() ) {
						case CDnActor::HitCheckTypeEnum::BoundingBox:
							{
								HitParam.vPosition = vPos;
								vDir = *hVecList[i]->GetPosition() - vPos;
								vDir.y = 0.0f;

								hVecList[i]->GetBoundingBox( Box );

								if( CDnActor::SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
								if( CDnActor::SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

								EtVec3Normalize( &vDir, &vDir );
								fDot = EtVec3Dot( &vZVec, &vDir );
								if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) continue;

								if( Box.Min.y < m_pPropMatrixEx->m_vPosition.y + pStruct->fHeightMin && 
									Box.Max.y < m_pPropMatrixEx->m_vPosition.y + pStruct->fHeightMin ) continue;
								if( Box.Min.y > m_pPropMatrixEx->m_vPosition.y + pStruct->fHeightMax && 
									Box.Max.y > m_pPropMatrixEx->m_vPosition.y + pStruct->fHeightMax ) continue;

								HitParam.vViewVec = -vDir;
							}
							break;
						case CDnActor::HitCheckTypeEnum::Collision:
							{
								SCollisionCapsule				Capsule;
								SCollisionResponse				CollisionResult;
								DNVector(SCollisionResponse)	vCollisionResult;
								//
								Capsule.Segment.vOrigin = vPos;
								float fHeight = pStruct->fHeightMax - pStruct->fHeightMin;
								Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( pStruct->fHeightMin + ( fHeight / 2.f ) );
								Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
								Capsule.fRadius = fDistanceMax;

								//int nParentBoneIndex = -1;
								EtVector3 vDestPos;
								if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) 
									continue;
								if( CollisionResult.pCollisionPrimitive )
								{
									for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
									{
										if( vCollisionResult[k].pCollisionPrimitive ) {
											vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
											if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < vPos.y + pStruct->fHeightMin ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}
											if( Box.Min.y > vPos.y + pStruct->fHeightMax && Box.Max.y > vPos.y + pStruct->fHeightMax ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}
											DNVector(EtVector3) vPointList;
											vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
											if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
												vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {
#else
											if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
												Box.GetVertices( vPointList );
											}
											bool bCheck = false;
											for( DWORD m=0; m<vPointList.size(); m++ ) {
#if defined(PRE_FIX_63356)
												if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == true ) {
#else
												if( hVecList[i]->CheckCollisionHitCondition(vPos, *m_pPropMatrixEx, vPointList[m], pStruct->fAngle) == true ) {
#endif // PRE_FIX_63356
													bCheck = true;
													break;
												}
											}

											if( bCheck == false ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}
											HitParam.vBoneIndex.push_back( hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive ) );
										}
									}
									if( vCollisionResult.empty() ) continue;
									//nParentBoneIndex = hVecList[i]->GetObjectHandle()->GetParentBoneIndex( CollisionResult.pCollisionPrimitive );
									GetCenterPos( *CollisionResult.pCollisionPrimitive, vDestPos );
									HitParam.vPosition = vDestPos;
								}
								else
								{
									ASSERT( 0 );
								}
								//////////////////////////////////////
								if( fDistanceMin > 100.f ) {
									vCollisionResult.clear();
									Capsule.fRadius = pStruct->fDistanceMin;
									if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == true ) {
										if( CollisionResult.pCollisionPrimitive )
										{
											for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
											{
												if( vCollisionResult[k].pCollisionPrimitive ) {
													vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
													if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < vPos.y + pStruct->fHeightMin ) {
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}
													if( Box.Min.y > vPos.y + pStruct->fHeightMax && Box.Max.y > vPos.y + pStruct->fHeightMax ) {
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}
													DNVector(EtVector3) vPointList;
													vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
														vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {
#else
													if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														Box.GetVertices( vPointList );
													}
													bool bCheck = false;
													for( DWORD m=0; m<vPointList.size(); m++ ) {
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														// 빼야하는 영역에서 한점이라도 안 걸리면 hit 영역과 걸쳐 있는 놈이므로 빼지 않는다...
#if defined(PRE_FIX_63356)
														if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == false ) {
#else
														if( hVecList[i]->CheckCollisionHitCondition(vPos, *m_pPropMatrixEx, vPointList[m], pStruct->fAngle) == false ) {
#endif // PRE_FIX_63356
#else
#if defined(PRE_FIX_63356)
														if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == true ) {
#else
														if( hVecList[i]->CheckCollisionHitCondition(vPos, *m_pPropMatrixEx, vPointList[m], pStruct->fAngle) == true ) {
#endif // PRE_FIX_63356
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
															bCheck = true;
															break;
														}
													}

#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													if( bCheck == true ) {
#else
													if( bCheck == false ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}
													int nBoneIndex = hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive );
													std::vector<int>::iterator it = std::find( HitParam.vBoneIndex.begin(), HitParam.vBoneIndex.end(), nBoneIndex );
													if( it != HitParam.vBoneIndex.end() ) {
														HitParam.vBoneIndex.erase( it );
													}
												}
											}
										}
									}
									if( HitParam.vBoneIndex.empty() ) continue;
								}
								//////////////////////////////////////

								//HitParam.nBoneIndex = nParentBoneIndex;

								HitParam.vViewVec = vPos - vDestPos;
								EtVec3Normalize( &HitParam.vViewVec, &HitParam.vViewVec );
							}
							break;
					}

					HitParam.bFirstHit = bFirstHit;
					bFirstHit = false;
					bHit = true;

					if( !HitParam.szActionName.empty() ) 
					{
						// this class user must have CDnDamageBase parent class.
						hVecList[i]->OnDamage( m_pDamageBase, HitParam, pStruct );

#ifdef PRE_ADD_HIT_PROP_ADD_SE
						if( pvlhHittedActor )
							pvlhHittedActor->push_back( hVecList[i] );
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE
					}
				}
				if( bHit ) {
					m_LastHitSignalTime = SignalEndTime;
				}
				else m_LastHitSignalTime = -1;

			}
			break;
	}
}