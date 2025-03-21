#pragma once
#include "Singleton.h"
#include "SignalHeader.h"


// 전역으로 사용할 액션 관련 상세 정보.
// 필요한대로 추가해서 사용하면 됨. 
// 현재는 임의의 액션 파일의 임의의 액션에 있는 발사체 시그널, 
// SendAction_Weapon 시그널의 갯수를 갖고 있는데,
// CDnActionBase::CheckPreSignal() 쪽에서 한 번 저장된 후에 서버 어플리케이션 종료될 때 까지 read only
// 따라서 스레드 동기화 없다.
// 게임서버에서만 사용되며 반드시 PreLoad 옵션으로 액션파일이 게임서버 어플리케이션이 뜰 때 
// 모두 로드되어있어야 한다.
class CDnActionSpecificInfo : public CSingleton<CDnActionSpecificInfo>
{
public:
	struct S_WEAPONACTION_INFO
	{
		int iWeaponIndex;
		int iFrame;		// 무기 액션 시그널이 박혀있는 프레임 정보
		std::string strActionName;
		
		S_WEAPONACTION_INFO( void ) : iWeaponIndex( 0 ), iFrame( 0 ) {};
	};

	struct S_PROJECTILE_COUNT_INFO
	{
		// 액션 인덱스, 발사체 시그널 갯수
		map<int, int> mapMaxProjectileCountInAction;

		// 액션 인덱스, 발사체에서 사용하는 무기 인덱스.
		map<int, multiset<int> > mapUsingProjectileWeaponTableIDs;

		// 해당 액션의 발사체 시그널이 박힌 프레임을 받아둔다.
		map<int, deque<int> > mapProjectileSignalFrameOffset;

		// 기본 슛 액션의 쿨타입을 input 시그널을 기반으로 뽑아낸다. (Shoot_ 문자열이 포함된 액션들)
		// Jump, Pick 액션을 제외한 가장 프레임이 빠른 인풋 시그널을 기준으로 한다.
		map<int, DWORD> mapBasicShootActionCoolTime;

		// 액션 인덱스, SendAction_Weapon 시그널 갯수
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
		// 스킬 체인 입력이 켜진 input 시그널을 갖고 있는 액션 인덱스
		set<int> setSkillChainAction;

		// 각 액션 마다 스킬 체인 입력이 켜진 input 시그널에 지정된 바뀔 액션 이름들을 받아둔다.
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
		// 각 액션에 inputhaspassiveskill 이 들어있다면 시그널 정보 중 필요한 것을 저장해둠.
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
