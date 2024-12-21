#pragma once
#include "CSVWriter.h"
#include "DnDamageBase.h"
#include "Singleton.h"

// Rotha DPS ������
// ���ϰ� ����ٰ� �ϴ� ������ ����
// ������ ���̺�� ���� ����ϰ� ó���� �κ��� �����ϴ� 
// ���� ���鼭 ������ ��ȹ�Դϴ�.

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
		int nTime;                  // �ð�
		std::wstring strCharName;   // ĳ���� �̸�
		std::wstring strSkillName;  // ��ų �̸� or �߻�ü
		int nSkillLevel;            // ��ų ����
		int nWeaponTableID;         // ���� / �߻�ü ���̺� ID
		int nCostMP;                // �Ҹ� ����

		char cAttackType;            // ������ Ÿ��
		int nAttackElement;         // �Ӽ� Ÿ��
		INT64 nDamage;                // ������
		int nHitProb;               // �ñ׳ε�����
		int nSkillProb;             // ��ų ������
		int nSkillAbsolute;         // ��ų ���밪
		bool bCritical;             // ũ��Ƽ��
		bool bCritcalRes;           // ũ������

		float fSkillCoolTime;       // ��Ÿ��
		float fSkillDelay;          // ��ų ������            
		int nAverageDamage;         // ��� ������
		int nWeaponDamage;          // ���ݷ�

		int nAreaMonsterCount;      // �ش������ �ֺ� ���� ����

		INT64 nMaxHP;
		int nMaxSP;
		INT64 nCurrentHP;             // ���� ü��
		int nCurrentSP;             // ���� ����

		int arrPlayerBuff[MaxBlowCount];    // �÷��̾�� ���� �Ǿ��ִ� ����/�����
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
		std::wstring strTargetName;    // ��� �̸�
		int nTargetID;                 // ��� ���̵�
		int nTargetUniqueID;           // ��� �������̵�
		
		INT64 nTargetHP;                 // ü��
		int nTargetPhysicalDamage;     // ����
		int nTargetMagicDamage;        // ����
		int nTargetPhysicalDefense;    // ����
		int nTargetMagicalDefense;     // ����
		int nTargetCritical;           // ũ��
		int nTargetCriticalRes;        // ũ������

		int arrElementalDefense[MaxElementCount];
		int arrTargetBuff[MaxBlowCount];    // ��� ���� ����Ǿ��ִ� ����/�����
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

	CSVWriter *m_pCSVWriter_BASE; // ������ �����ϰ� ��°�찡 ����Ƿ� �ɹ��� ������.
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