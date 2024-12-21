#pragma once
#include "Singleton.h"
#include "SignalHeader.h"


// �������� ����� �׼� ���� �� ����.
// �ʿ��Ѵ�� �߰��ؼ� ����ϸ� ��. 
// ����� ������ �׼� ������ ������ �׼ǿ� �ִ� �߻�ü �ñ׳�, 
// SendAction_Weapon �ñ׳��� ������ ���� �ִµ�,
// CDnActionBase::CheckPreSignal() �ʿ��� �� �� ����� �Ŀ� ���� ���ø����̼� ����� �� ���� read only
// ���� ������ ����ȭ ����.
// ���Ӽ��������� ���Ǹ� �ݵ�� PreLoad �ɼ����� �׼������� ���Ӽ��� ���ø����̼��� �� �� 
// ��� �ε�Ǿ��־�� �Ѵ�.
class CDnActionSpecificInfo : public CSingleton<CDnActionSpecificInfo>
{
public:
	struct S_WEAPONACTION_INFO
	{
		int iWeaponIndex;
		int iFrame;		// ���� �׼� �ñ׳��� �����ִ� ������ ����
		std::string strActionName;
		
		S_WEAPONACTION_INFO( void ) : iWeaponIndex( 0 ), iFrame( 0 ) {};
	};

	struct S_PROJECTILE_COUNT_INFO
	{
		// �׼� �ε���, �߻�ü �ñ׳� ����
		map<int, int> mapMaxProjectileCountInAction;

		// �׼� �ε���, �߻�ü���� ����ϴ� ���� �ε���.
		map<int, multiset<int> > mapUsingProjectileWeaponTableIDs;

		// �ش� �׼��� �߻�ü �ñ׳��� ���� �������� �޾Ƶд�.
		map<int, deque<int> > mapProjectileSignalFrameOffset;

		// �⺻ �� �׼��� ��Ÿ���� input �ñ׳��� ������� �̾Ƴ���. (Shoot_ ���ڿ��� ���Ե� �׼ǵ�)
		// Jump, Pick �׼��� ������ ���� �������� ���� ��ǲ �ñ׳��� �������� �Ѵ�.
		map<int, DWORD> mapBasicShootActionCoolTime;

		// �׼� �ε���, SendAction_Weapon �ñ׳� ����
		map<int, vector<S_WEAPONACTION_INFO> > mapSendActionWeapon;

		void Clear( void ) 
		{ 
			mapMaxProjectileCountInAction.clear();
			mapUsingProjectileWeaponTableIDs.clear();
			mapSendActionWeapon.clear();
			mapBasicShootActionCoolTime.clear();
		};
	};

	struct S_SKILLCHAIN_INPUT_INFO
	{
		// ��ų ü�� �Է��� ���� input �ñ׳��� ���� �ִ� �׼� �ε���
		set<int> setSkillChainAction;

		// �� �׼� ���� ��ų ü�� �Է��� ���� input �ñ׳ο� ������ �ٲ� �׼� �̸����� �޾Ƶд�.
		map<int, vector<string> > mapCanChainToThisAction;

		void Clear( void ) { setSkillChainAction.clear(); mapCanChainToThisAction.clear(); };
	};

	struct S_PASSIVESKILL_SIGNAL_INFO
	{
		int iSkillID;
		string strChangeActionName;
		string strEXSkillChangeActionName;
	};

	struct S_INPUTHASPASSIVESKILL_INFO
	{
		// �� �׼ǿ� inputhaspassiveskill �� ����ִٸ� �ñ׳� ���� �� �ʿ��� ���� �����ص�.
		map<int, vector<S_PASSIVESKILL_SIGNAL_INFO> > mapPassiveSkillInfo;

		void Clear( void ) { mapPassiveSkillInfo.clear(); };
	};

	struct S_BASIC_ATTACK_INPUT_SIGNAL_INFO
	{
		string strChangeActionName;
		DWORD dwStartFrame;
		DWORD dwEndFrame;

		S_BASIC_ATTACK_INPUT_SIGNAL_INFO( void ) : dwStartFrame( 0 ), dwEndFrame( 0 ) {};
	};

	struct S_BASIC_ATTACK_INFO
	{
		map<int, vector<S_BASIC_ATTACK_INPUT_SIGNAL_INFO> > mapBasicAttackInfo;
		void Clear( void ) { mapBasicAttackInfo.clear(); };
	};

private:
	map<std::string, S_PROJECTILE_COUNT_INFO*> m_mapProjectileSignalInfoByActionFile;
	S_PROJECTILE_COUNT_INFO* m_pCurrentLoadSignalInfo;

	map<std::string, S_SKILLCHAIN_INPUT_INFO*> m_mapSkillChainActionSetByActionFile;

	map<std::string, S_INPUTHASPASSIVESKILL_INFO*> m_mapPassiveSkillInfoByActionFile;

	map<std::string, S_BASIC_ATTACK_INFO*> m_mapBasicAttackInfoByActionFile;

public:
	CDnActionSpecificInfo( void );
	~CDnActionSpecificInfo( void );

	void AddProjectileSignalInfo( const std::string& strFullPath, const S_PROJECTILE_COUNT_INFO& ProjectileSignalInfo );
	const S_PROJECTILE_COUNT_INFO* FindProjectileSignalInfo( const std::string& strFullPath );

	void AddSkillChainActionSet( const std::string& strFullPath, const S_SKILLCHAIN_INPUT_INFO& SkillChainInputInfo );
	const S_SKILLCHAIN_INPUT_INFO* FindSkillChainActionSet( const std::string& strFullPath );

	void AddPassiveSkillInfo( const std::string& strFullPath, const S_INPUTHASPASSIVESKILL_INFO& PassiveSkillInfo );
	const S_INPUTHASPASSIVESKILL_INFO* FindPassiveSkillInfo( const std::string& strFullPath );

	void AddBasicAttackActionInfo( const std::string& strFullPath, const S_BASIC_ATTACK_INFO& BasicAttackInfo );
	const S_BASIC_ATTACK_INFO* FindBasicAttackInfo( const std::string& strFullPath );
};
