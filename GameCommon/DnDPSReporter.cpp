#include "stdafx.h"
#include "DnDPSReporter.h"
#include "DnStateBlow.h"
#include "DnMonsterActor.h"
#include "DNGameRoom.h"
#include "TimeSet.h"
#include "direct.h"

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION

CDnDPSReporter g_DPSReporter;

CDnDPSReporter::CDnDPSReporter()
{
	m_bValidated = false;
	std::wstring strColumnName;

	MakeBaseColumnName(strColumnName);
	MakeTargetColumnName(strColumnName);

	_mkdir("r:\\DPS_RESULT");

	m_pCSVWriter_BASE = new CSVWriter;
	m_pCSVWriter_BASE->Initialize( L"r:\\DPS_RESULT\\DPS_REPORT.csv" , strColumnName );

	strColumnName.clear();
	MakeSkillInfoColumName(strColumnName);

	m_pCSVWriter_SKILL = new CSVWriter;
	m_pCSVWriter_SKILL->Initialize( L"r:\\DPS_RESULT\\SKILL_USAGE_REPORT.csv" , strColumnName );

	strColumnName.clear();
	MakeUserInfoColumName(strColumnName);
	m_pCSVWriter_USER = new CSVWriter;
	m_pCSVWriter_USER->Initialize( L"r:\\DPS_RESULT\\USER_REPORT.csv" , strColumnName );
	
	ClearData();
}

CDnDPSReporter::~CDnDPSReporter()
{
	SAFE_DELETE(m_pCSVWriter_BASE);
	SAFE_DELETE(m_pCSVWriter_SKILL);
	SAFE_DELETE(m_pCSVWriter_USER);
}

void CDnDPSReporter::ClearData()
{
	m_sBaseData.clear();
	m_sTargetData.clear();
}

int CDnDPSReporter::GetEnabledUserCount()
{
	return (int)m_vecEnableUserList.size();
}

bool CDnDPSReporter::EnableUser( INT64 nDBID )
{
	if( GetEnabledUserCount() > eReport::MaxUserCount )
		return false;

	if( m_vecEnableUserList.empty() )
	{
		m_pCSVWriter_BASE->CheckExistFile();
		m_pCSVWriter_SKILL->CheckExistFile();
		m_pCSVWriter_USER->CheckExistFile();
	}

	m_vecEnableUserList.push_back(nDBID);
	return true;
}

bool CDnDPSReporter::IsEnabledUser( INT64 nDBID )
{
	for(int i=0; i<(int)m_vecEnableUserList.size(); i++ )
	{
		if( m_vecEnableUserList[i] == nDBID )
			return true;
	}

	return false;
}

void CDnDPSReporter::DisableAllUser()
{
	m_vecEnableUserList.clear();

	m_pCSVWriter_BASE->AddField();
	m_pCSVWriter_BASE->WriteColumnName();
	m_pCSVWriter_SKILL->AddField();
	m_pCSVWriter_SKILL->WriteColumnName();
	m_pCSVWriter_USER->AddField();
	m_pCSVWriter_USER->WriteColumnName();
}

void CDnDPSReporter::StartReport()
{
	m_bValidated = false;
}

void CDnDPSReporter::EndReport()
{
	ReportDPS();
}

void CDnDPSReporter::ReportDPS()
{
	if(!m_bValidated)
		return;

	m_pCSVWriter_BASE->StartWrite();
	ReportBaseInfo();
	ReportTargetInfo();
	m_pCSVWriter_BASE->AddField(); // End
	m_pCSVWriter_BASE->EndWrite();
}

void CDnDPSReporter::ApplyBaseData( CDnDamageBase::SHitParam *HitParam , float fDamage )
{
	if(!HitParam )
		return;
	if(!HitParam->hHitter || !HitParam->hHitter->IsPlayerActor() )
		return;

	m_sBaseData.nDamage = (int)fDamage;
	m_sBaseData.nHitProb = (int)(HitParam->fDamage * 100);

	m_sBaseData.strCharName = HitParam->hHitter->GetName();

	m_sBaseData.nCurrentHP = HitParam->hHitter->GetHP();
	m_sBaseData.nCurrentSP = HitParam->hHitter->GetSP();

	m_sBaseData.nMaxHP = HitParam->hHitter->GetMaxHP();
	m_sBaseData.nMaxSP = HitParam->hHitter->GetMaxSP();

	m_sBaseData.cAttackType = HitParam->cAttackType;
	m_sBaseData.nAttackElement = HitParam->HasElement;

	m_sBaseData.bCritical = (HitParam->HitType == CDnWeapon::HitTypeEnum::Critical) ? true : false;
	m_sBaseData.bCritcalRes = (HitParam->HitType == CDnWeapon::HitTypeEnum::CriticalRes) ? true : false;

	if( HitParam->hWeapon )
		m_sBaseData.nWeaponTableID = HitParam->hWeapon->GetClassID();

	if( HitParam->hHitter->GetProcessSkill() )
	{
		DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );

		int nSkillID = HitParam->hHitter->GetProcessSkill()->GetClassID();
		
		if( pSkillTable->IsExistItem( nSkillID ) )
		{
			int iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
			m_sBaseData.strSkillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );
		}

		m_sBaseData.nCostMP =      HitParam->hHitter->GetProcessSkill()->GetDecreaseMP();
		m_sBaseData.nSkillLevel =  HitParam->hHitter->GetProcessSkill()->GetLevel();
		m_sBaseData.fSkillCoolTime = HitParam->hHitter->GetProcessSkill()->GetDelayTime();
		m_sBaseData.fSkillDelay =  HitParam->hHitter->GetProcessSkill()->GetElapsedDelayTime();
	}


	if(HitParam->hHitter->GetStateBlow())
	{
		int nSize = HitParam->hHitter->GetStateBlow()->GetNumStateBlow();
		int nBuffCount = 0;
		int nDebuffCount = 0;

		for ( int i = 0 ; i < nSize ; i++ )
		{
			DnBlowHandle hBlow = HitParam->hHitter->GetStateBlow()->GetStateBlow(i);
			if(!hBlow)
				continue;

			const CDnSkill::SkillInfo* pSkillinfo = hBlow->GetParentSkillInfo();

			if( pSkillinfo )
			{
				if( nBuffCount < MaxBlowCount &&  pSkillinfo->eTargetType != CDnSkill::TargetTypeEnum::Enemy  )
				{
					m_sBaseData.arrPlayerBuff[nBuffCount] = hBlow->GetBlowIndex();
					nBuffCount++;
				}
				else if( nDebuffCount < MaxBlowCount )
				{
					m_sBaseData.arrPlayerDebuff[nDebuffCount] = hBlow->GetBlowIndex();
					nDebuffCount++;
				}

				if( HitParam->hHitter->GetProcessSkill() )
				{
					if( pSkillinfo->iSkillID == HitParam->hHitter->GetProcessSkill()->GetClassID() )
					{
						if ( hBlow->GetBlowIndex() == STATE_BLOW::BLOW_001 || hBlow->GetBlowID() == STATE_BLOW::BLOW_028 )
							m_sBaseData.nSkillAbsolute += (int)hBlow->GetFloatValue();

						if ( hBlow->GetBlowIndex() == STATE_BLOW::BLOW_002 || hBlow->GetBlowID() == STATE_BLOW::BLOW_029 )
							m_sBaseData.nSkillProb += 100 + (int)( (float)hBlow->GetFloatValue() * 100 );
					}
				}
			}
		
		}
	}

	m_bValidated = true;
}


void CDnDPSReporter::ApplyAreaMonsterCount(int nCount)
{
	m_sBaseData.nAreaMonsterCount = nCount;
}


void CDnDPSReporter::ReportBaseInfo()
{
	CTimeSet CurTime;
	m_pCSVWriter_BASE->AddColumn(FormatW( L"%d : %d : %d",CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond() ).c_str());
	m_pCSVWriter_BASE->AddColumn(m_sBaseData.strCharName.c_str()); // 이름
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nCurrentHP).c_str()); // HP
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nMaxHP).c_str()); // HP
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nCurrentSP).c_str()); // MP
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nMaxSP).c_str()); // MP
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nDamage).c_str());
	
	if(m_sBaseData.strSkillName.empty())
		m_pCSVWriter_BASE->AddColumn(L"기본 공격");
	else
		m_pCSVWriter_BASE->AddColumn(m_sBaseData.strSkillName.c_str());               // 스킬 이름

	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nSkillLevel).c_str()); // 스킬 레벨

	if( m_sBaseData.cAttackType == 0 )
		m_pCSVWriter_BASE->AddColumn(L"물리"); // 스킬 타입
	else
		m_pCSVWriter_BASE->AddColumn(L"마법"); // 스킬 타입

	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nAttackElement).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nHitProb).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nSkillProb).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nSkillAbsolute).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.bCritical ? 1 : 0 ).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.bCritcalRes ? 1 : 0 ).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nCostMP).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%f",m_sBaseData.fSkillCoolTime).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%f",m_sBaseData.fSkillDelay).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nWeaponTableID).c_str());
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.nAreaMonsterCount).c_str());
	
	for(int i=0; i<MaxBlowCount; i++)
		m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.arrPlayerBuff[i]).c_str());

	for(int i=0; i<MaxBlowCount; i++)
		m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sBaseData.arrPlayerDebuff[i]).c_str());

	m_sBaseData.clear();
}

void CDnDPSReporter::MakeBaseColumnName(std::wstring &str)
{
	str += L"시간;";
	str += L"이름;";
	str += L"체력;";
	str += L"체력MAX;";
	str += L"마나;";
	str += L"마나Max;";
	str += L"데미지;";
	str += L"스킬이름;";
	str += L"스킬레벨;";
	str += L"공격타입;";
	str += L"공격속성;";
	str += L"히트데미지비율;";
	str += L"스킬비율;";
	str += L"스킬절대값;";
	str += L"크리;";
	str += L"크리저항;";
	str += L"소모마나;";
	str += L"스킬쿨타임;";
	str += L"스킬딜레이;";
	str += L"무기번호;";
	str += L"몬스터 숫자;";

	for(int i=0; i<MaxBlowCount; i++)
		str += FormatW( L"버프_%d;" , i ).c_str();

	for(int k=0; k<MaxBlowCount; k++)
		str += FormatW( L"디버프_%d;" ,k ).c_str();
}

void CDnDPSReporter::ApplyTargetData( DnActorHandle hActor )
{
	if(!hActor)
		return;

	m_sTargetData.strTargetName = hActor->GetName();  

	if(hActor->IsMonsterActor())
	{
		CDnMonsterActor *pMonster = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		m_sTargetData.nTargetID = pMonster->GetMonsterClassID();
	}
	else
		m_sTargetData.nTargetID = hActor->GetClassID(); 

	m_sTargetData.nTargetUniqueID = hActor->GetUniqueID();
	m_sTargetData.nTargetHP = hActor->GetHP();

	m_sTargetData.nTargetPhysicalDamage =  (hActor->GetAttackPMin() + hActor->GetAttackPMax()) / 2;   
	m_sTargetData.nTargetMagicDamage = (hActor->GetAttackMMin() + hActor->GetAttackMMax()) / 2;        
	m_sTargetData.nTargetPhysicalDefense = hActor->GetDefenseP();   
	m_sTargetData.nTargetMagicalDefense = hActor->GetDefenseM();    
	m_sTargetData.nTargetCritical = hActor->GetCritical();         
	m_sTargetData.nTargetCriticalRes = hActor->GetCriticalResistance();       

	for(int i=0;i<MaxElementCount; i++)
		m_sTargetData.arrElementalDefense[i] = (int)(hActor->GetElementDefense((CDnState::ElementEnum)i) * 100);

	if(hActor->GetStateBlow())
	{
		int nSize = hActor->GetStateBlow()->GetNumStateBlow();
		int nBuffCount = 0;
		int nDebuffCount = 0;

		for ( int i = 0 ; i < nSize ; i++ )
		{
			DnBlowHandle hBlow = hActor->GetStateBlow()->GetStateBlow(i);
			if(!hBlow)
				continue;

			const CDnSkill::SkillInfo* pSkillinfo = hBlow->GetParentSkillInfo();
			if( nBuffCount < MaxBlowCount &&  ( pSkillinfo && pSkillinfo->eTargetType != CDnSkill::TargetTypeEnum::Enemy ) )
			{
				m_sTargetData.arrTargetBuff[nBuffCount] = hBlow->GetBlowIndex();
				nBuffCount++;
			}
			else if( nDebuffCount < MaxBlowCount )
			{
				m_sTargetData.arrTargetDebuff[nDebuffCount] = hBlow->GetBlowIndex();
				nDebuffCount++;
			}
		}
	}
}

void CDnDPSReporter::ReportTargetInfo()
{
	m_pCSVWriter_BASE->AddColumn(m_sTargetData.strTargetName.c_str()); // 이름
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetID).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetUniqueID).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetHP).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetPhysicalDamage).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetMagicDamage).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetPhysicalDefense).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetMagicalDefense).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetCritical).c_str()); 
	m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.nTargetCriticalRes).c_str()); 

	for(int i=0; i<MaxElementCount; i++)
		m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.arrElementalDefense[i]).c_str());

	for(int i=0; i<MaxBlowCount; i++)
		m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.arrTargetBuff[i]).c_str());

	for(int i=0; i<MaxBlowCount; i++)
		m_pCSVWriter_BASE->AddColumn(FormatW(L"%d",m_sTargetData.arrTargetDebuff[i]).c_str());

	m_sTargetData.clear();
}

void CDnDPSReporter::MakeTargetColumnName(std::wstring &str )
{
	str += L"대상 이름;";
	str += L"대상 번호;";
	str += L"대상 고유번호;";
	str += L"대상 체력;";
	str += L"대상 물공;";
	str += L"대상 마공;";
	str += L"대상 물리방어;";
	str += L"대상 마법방어;";
	str += L"대상 크리;";
	str += L"대상 크리저항;";
	str += L"대상 불방어;";
	str += L"대상 물방어;";
	str += L"대상 빛방어;";
	str += L"대상 암흑방어;";
	
	for(int i=0; i<MaxBlowCount; i++)
		str += FormatW( L"대상 버프_%d;" ,i ).c_str();

	for(int k=0; k<MaxBlowCount; k++)
		str += FormatW( L"대상 디버프_%d;" ,k ).c_str();
}


void CDnDPSReporter::ApplyDotDamageData(int nDamage, DnActorHandle hActor , DnBlowHandle hFromBlow )
{
	if(!hFromBlow || !hActor)
		return;
	CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hFromBlow->GetParentSkillInfo());
	if( !pSkillInfo || !pSkillInfo->iSkillID )
		return;

	m_sBaseData.nDamage = nDamage;

	if( pSkillInfo->hSkillUser )
		m_sBaseData.strCharName = pSkillInfo->hSkillUser->GetName();

	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int nSkillID = pSkillInfo->iSkillID;
	if( pSkillTable->IsExistItem( nSkillID ) )
	{
		int iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
		m_sBaseData.strSkillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );
	}

	m_sBaseData.nCurrentHP = pSkillInfo->hSkillUser->GetHP();
	m_sBaseData.nCurrentSP = pSkillInfo->hSkillUser->GetSP();

	m_sBaseData.nMaxHP = pSkillInfo->hSkillUser->GetMaxHP();
	m_sBaseData.nMaxSP = pSkillInfo->hSkillUser->GetMaxSP();

	m_sBaseData.cAttackType = 1;
	m_sBaseData.nAttackElement = pSkillInfo->eSkillElement;

	m_sBaseData.nSkillLevel =   pSkillInfo->iLevel;
	m_sBaseData.arrPlayerDebuff[0] = hFromBlow->GetBlowIndex();

	ApplyTargetData(hActor);

	m_bValidated = true;
}


void CDnDPSReporter::ReportUserInfo( DnActorHandle hActor )
{
	if( !hActor || !hActor->GetRoom() )
		return;

	m_pCSVWriter_USER->StartWrite();
	CTimeSet CurTime;
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d : %d : %d",CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond() ).c_str() );
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetGameRoom()->m_iMapIdx ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , (int)hActor->GetGameRoom()->m_StageDifficulty ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( hActor->GetName() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->OnGetJobClassID() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetLevel() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetMaxHP() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetMaxSP() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetStrength() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetAgility() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetIntelligence() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetStamina() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetMoveSpeed() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetAttackPMin() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetAttackPMax() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetAttackMMin() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetAttackMMax() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetDefenseP() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetDefenseM() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetCritical() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetStun() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetStiff() ).c_str() ); 
	m_pCSVWriter_USER->AddColumn( FormatW( L"%d" , hActor->GetFinalDamage() ).c_str() );

	for(int n=0; n<MaxElementCount; n++)
		m_pCSVWriter_USER->AddColumn( FormatW( L"%f" , hActor->GetElementAttack((CDnState::ElementEnum)n) ).c_str() );

	for(int n=0; n<MaxElementCount; n++)
		m_pCSVWriter_USER->AddColumn( FormatW( L"%f" , hActor->GetElementDefense((CDnState::ElementEnum)n) ).c_str() ); 

	m_pCSVWriter_USER->AddField(); // End
	m_pCSVWriter_USER->EndWrite();

}

void CDnDPSReporter::MakeUserInfoColumName(std::wstring &str)
{
	str += L"시간;";
	str += L"맵번호;";
	str += L"난이도;";
	str += L"이름;";
	str += L"직업;";
	str += L"레벨;";
	str += L"HP;";
	str += L"SP;";
	str += L"힘;";
	str += L"민첩;";
	str += L"지능;";
	str += L"건강;";
	str += L"이속;";
	str += L"물공MIN;";
	str += L"물공MAX;";
	str += L"마공MIN;";
	str += L"마공MAX;";
	str += L"물리방어;";
	str += L"마법방어;";
	str += L"크리;";
	str += L"스턴;";
	str += L"경직;";
	str += L"파뎀;";
	str += L"불공격;";
	str += L"물공격;";
	str += L"빛공격;";
	str += L"암흑공격;";
	str += L"불방어;";
	str += L"물방어;";
	str += L"빛방어;";
	str += L"암흑방어;";
}


void CDnDPSReporter::ReportSkillInfo( DnSkillHandle hSkill , int nAreaMonsterCount )
{
	if( !hSkill )
		return;

	m_pCSVWriter_SKILL->StartWrite();

	CTimeSet CurTime;
	m_pCSVWriter_SKILL->AddColumn( FormatW( L"%d : %d : %d",CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond() ).c_str() );
	
	if(hSkill->GetActor())
		m_pCSVWriter_SKILL->AddColumn( hSkill->GetActor()->GetName() ); 
	else
		m_pCSVWriter_SKILL->AddColumn( L"" ); 
		
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int nSkillID = hSkill->GetClassID();
	if( pSkillTable && pSkillTable->IsExistItem( nSkillID ) )
	{
		int iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
		m_pCSVWriter_SKILL->AddColumn( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID ) ); 
	}
	else
		m_pCSVWriter_SKILL->AddColumn( L"" );

	m_pCSVWriter_SKILL->AddColumn( FormatW( L"%d" , hSkill->GetLevel() ).c_str() );
	m_pCSVWriter_SKILL->AddColumn( FormatW( L"%d" , hSkill->GetDecreaseMP() ).c_str() );
	m_pCSVWriter_SKILL->AddColumn( FormatW( L"%d" , nAreaMonsterCount).c_str() );


	m_pCSVWriter_SKILL->AddField(); // End
	m_pCSVWriter_SKILL->EndWrite();
}

void CDnDPSReporter::MakeSkillInfoColumName(std::wstring &str)
{
	str += L"시간;";
	str += L"사용자;";
	str += L"스킬이름;";
	str += L"스킬레벨;";
	str += L"소모MP;";
	str += L"몬스터숫자;";
}

#endif