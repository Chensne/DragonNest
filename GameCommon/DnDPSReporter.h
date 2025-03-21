#pragma once
#include "CSVWriter.h"
#include "DnDamageBase.h"
#include "Singleton.h"

// Rotha DPS 측정기
// 급하게 만든다고 일단 빠르게 구현
// 구조상 테이블로 빼면 깔끔하게 처리될 부분이 많습니다 
// 추후 보면서 수정할 계획입니다.

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION

class CDnDPSReporter  : public CSingleton<CDnDPSReporter>
{
public:

	enum eReport
	{
		MaxBlowCount = 10,
		MaxUserCount = 4,
		MaxElementCount = 4,
	};

	struct sBasicData
	{
		int nTime;                  // 시간
		std::wstring strCharName;   // 캐릭터 이름
		std::wstring strSkillName;  // 스킬 이름 or 발사체
		int nSkillLevel;            // 스킬 레벨
		int nWeaponTableID;         // 무기 / 발사체 테이블 ID
		int nCostMP;                // 소모 마나

		char cAttackType;            // 데미지 타입
		int nAttackElement;         // 속성 타입
		INT64 nDamage;                // 데미지
		int nHitProb;               // 시그널데미지
		int nSkillProb;             // 스킬 데미지
		int nSkillAbsolute;         // 스킬 절대값
		bool bCritical;             // 크리티컬
		bool bCritcalRes;           // 크리저항

		float fSkillCoolTime;       // 쿨타임
		float fSkillDelay;          // 스킬 딜레이            
		int nAverageDamage;         // 평균 데미지
		int nWeaponDamage;          // 공격력

		int nAreaMonsterCount;      // 해당시점의 주변 몬스터 숫자

		INT64 nMaxHP;
		int nMaxSP;
		INT64 nCurrentHP;             // 현재 체력
		int nCurrentSP;             // 현재 마나

		int arrPlayerBuff[MaxBlowCount];    // 플레이어에게 적용 되어있는 버프/디버프
		int arrPlayerDebuff[MaxBlowCount];

	
		void clear()
		{
			nTime = 0;                  
			strCharName.clear();
			strSkillName.clear();

			nSkillLevel = 0;          
			nWeaponTableID = 0;
			nCostMP = 0;       

			cAttackType = 0;       
			nAttackElement = 0;
			nDamage = 0;     
			nHitProb = 0;
			nSkillProb = 0;
			nSkillAbsolute = 0;
 
			bCritical = false;
			bCritcalRes = false;   

			fSkillCoolTime = 0.f;
			fSkillDelay = 0.f;
			nAverageDamage = 0;    
			nWeaponDamage = 0;   

			nAreaMonsterCount = 0;

			nMaxHP = 0;
			nMaxSP = 0;
			nCurrentHP = 0;   
			nCurrentSP = 0;   

			for(int i=0; i<MaxBlowCount; i++)
			{
				arrPlayerBuff[i] = 0;
				arrPlayerDebuff[i] = 0;
			}
		}
	};

	struct sTargetData
	{
		std::wstring strTargetName;    // 대상 이름
		int nTargetID;                 // 대상 아이디
		int nTargetUniqueID;           // 대상 고유아이디
		
		INT64 nTargetHP;                 // 체력
		int nTargetPhysicalDamage;     // 물공
		int nTargetMagicDamage;        // 마공
		int nTargetPhysicalDefense;    // 물방
		int nTargetMagicalDefense;     // 마방
		int nTargetCritical;           // 크리
		int nTargetCriticalRes;        // 크리저항

		int arrElementalDefense[MaxElementCount];
		int arrTargetBuff[MaxBlowCount];    // 대상 에게 적용되어있는 버프/디버프
		int arrTargetDebuff[MaxBlowCount];

		void clear()
		{
			strTargetName.clear();
			nTargetID = 0;   
			nTargetUniqueID = 0;
			nTargetHP = 0;
			nTargetPhysicalDamage = 0;   
			nTargetMagicDamage = 0;
			nTargetPhysicalDefense = 0; 
			nTargetMagicalDefense = 0;
			nTargetCritical = 0;
			nTargetCriticalRes = 0;   

			for(int i=0; i<MaxElementCount; i++)
				arrElementalDefense[i] = 0;

			for(int i=0; i<MaxBlowCount; i++)
			{
				arrTargetBuff[i] = 0;
				arrTargetDebuff[i] = 0;
			}
		}
	};

	CSVWriter *m_pCSVWriter_BASE; // 정보를 수집하고 찍는경우가 생기므로 맴버로 가진다.
	CSVWriter *m_pCSVWriter_SKILL;
	CSVWriter *m_pCSVWriter_USER; 

	sBasicData m_sBaseData;
	sTargetData m_sTargetData;

	std::vector<INT64> m_vecEnableUserList;
	bool m_bValidated;

public:

	CDnDPSReporter();
	virtual ~CDnDPSReporter();

	int GetEnabledUserCount();
	bool EnableUser( INT64 nDBID );
	bool IsEnabledUser( INT64 nDBID );
	void DisableAllUser();

	void StartReport();
	void EndReport();
	void ReportDPS();

	void ClearData();

	void ApplyBaseData( CDnDamageBase::SHitParam *HitParam , float fDamage);
	void ApplyTargetData( DnActorHandle hActor );
	void ApplyDotDamageData(int nDamage, DnActorHandle hActor , DnBlowHandle hFromBlow );
	
	void ApplyAreaMonsterCount(int nCount);

	void ReportBaseInfo();
	void ReportTargetInfo();
	void ReportUserInfo( DnActorHandle hActor );
	void ReportSkillInfo( DnSkillHandle hSkill ,  int nAreaMonsterCount );

	void MakeBaseColumnName(std::wstring &str);
	void MakeTargetColumnName(std::wstring &str);
	void MakeUserInfoColumName(std::wstring &str);
	void MakeSkillInfoColumName(std::wstring &str);

};

extern CDnDPSReporter g_DPSReporter;

#endif