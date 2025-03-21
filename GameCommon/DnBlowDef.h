#pragma once

const int STATE_BLOW_ARGUMENT_MAX_SIZE = 64;

#define SB_NONE					0x00000000
#define SB_ONBEGIN				0x00000001
#define SB_ONEND				0x00000002
#define SB_ONCHANGEDWEAPON		0x00000004
#define SB_ONDEFENSEATTACK		0x00000008
#define SB_ONUSEMP				0x00000010
#define SB_ONTARGETHIT			0x00000020
#define SB_ONCALCDAMAGE			0x00000040
#define SB_AFTERONDEFENSEATTACK	0x00000080

// Note: 상태효과 인덱스는 기획서에서부터 영구적용 아이템에 이르기까지 광범위하게 의존 되어있는
// 스킬 시스템의 핵심 인덱스이므로 기존 인덱스 수정을 금합니다.
// 상태효과 추가시엔 여기에 번호를 추가하고, CDnCreateBlow 의 IsBasicBlow 함수와 클래스가 따로 정해져있는 상태효과들의 생성 switch 문에 추가해줘야 합니다.
// 안좋은 상태효과(디버프)를 추가하는 경우엔 DissolveDebufBlow 에도 해제하도록 따로 추가해줘야 합니다.
struct STATE_BLOW
{
	enum emBLOW_INDEX
	{
		BLOW_NONE = 0,
		BLOW_001 = 1,	// 1) 물리 공격력 절대
		BLOW_002 = 2,	// 2) 물리 공격력 비율
		BLOW_003 = 3,	// 3) 물리 방어력 절대
		BLOW_004 = 4,	// 4) 물리 방어력 비율
		BLOW_005 = 5,	// 5) 힘 증가
		BLOW_006 = 6,	// 6) 민첩 증가
		BLOW_007 = 7,	// 7) 지능 증가
		BLOW_008 = 8,	// 8) 건강 증가
		BLOW_009 = 9,	// 9) 최대 HP 증가
		BLOW_010 = 10,	// 10) 최대 MP 증가
		BLOW_011 = 11,	// 11) 지속 HP 증가 절대치
		BLOW_012 = 12,	// 12) 지속 HP 증가 비율
		BLOW_013 = 13,	// 12) 지속 MP 증가 절대치
		BLOW_014 = 14,	// 14) 지속 MP 증가 비율
		BLOW_015 = 15,	// 15) 현재 HP 증가 절대치
		BLOW_016 = 16,	// 16) 현재 HP 증가 비율
		BLOW_017 = 17,	// 17) 현재 MP 증가 절대치
		BLOW_018 = 18,	// 18) 현재 MP 증가 비율
		BLOW_019 = 19,	// 19) 경직력 증가
		BLOW_020 = 20,	// 20) 경직 저항 증가
		BLOW_021 = 21,	// 21) 크리티컬 증가
		BLOW_022 = 22,	// 22) 크리티컬 저항 증가
		BLOW_023 = 23,	// 23) 스턴 확률 증가
		BLOW_024 = 24,	// 24) 스턴 저항력 증가
		BLOW_025 = 25,	// 25) 프레임 증가
		BLOW_026 = 26,	// 26) 다운 딜레이 증가
		BLOW_028 = 28,	// 28) 마법 공격 절대
		BLOW_029 = 29,	// 29) 마법 공격 비율
		BLOW_030 = 30,	// 30) 블록
		BLOW_031 = 31,	// 31) 패링
		BLOW_032 = 32,	// 32) 화염 공격력 증가
		BLOW_033 = 33,	// 33) 냉기 공격력 증가
		BLOW_034 = 34,	// 34) 빛 공격력 증가
		BLOW_035 = 35,	// 35) 어둠 공격력 증가
		BLOW_036 = 36,	// 36) 화염 내성 증가
		BLOW_037 = 37,	// 37) 냉기 내성 증가
		BLOW_038 = 38,	// 38) 빛 내성 증가
		BLOW_039 = 39,	// 39) 어둠 내성 증가
		BLOW_040 = 40,	// 40) 힘 모으기
		BLOW_041 = 41,	// 41) 결빙
		BLOW_042 = 42,	// 42) 화상
		BLOW_043 = 43,	// 43) 감전
		BLOW_044 = 44,	// 44) 중독
		BLOW_045 = 45,	// 45) 수면
		BLOW_048 = 48,	// 48) 스킬 포인트
		BLOW_049 = 49,	// 49) 경험치 증가
		BLOW_050 = 50,	// 50) 데미지 증가.
		BLOW_051 = 51,	// 51) #53454 꼭두각시 상태효과 관련 데미지를 주인? 액터에게 전이 하는 상태효과..(꼭두각시 몬스터? 테이블에 기본으로 이 상태효과가 적용 되어 있어야 함)
		BLOW_055 = 55,	// 55) 가디언 포스(절대값 만큼의 데미지를 흡수하는 보호막)
		BLOW_056 = 56,	// 56) 패링 상태효과에 있는 확률에 값을 더해줌.
		BLOW_057 = 57,	// 57) 부활
		BLOW_058 = 58,	// 58) 최대 HP 비율
		BLOW_059 = 59,	// 59) 최대 SP 비율
		BLOW_060 = 60,	// 60) 체인 공격
		BLOW_061 = 61,	// 61) 슈퍼아머 절대 증가
		BLOW_062 = 62,	// 62) 차져 상태효과
		BLOW_063 = 63,	// 63) 석화
		BLOW_064 = 64,  // 64) 슈퍼아머 비율 증가
		BLOW_065 = 65,	// 65) 무기 사거리 대체.
		BLOW_066 = 66,	// 66) 어그로 조절 상태효과
		BLOW_067 = 67,	// 67) 액티브 스킬 사용 제한
		BLOW_068 = 68,	// 68) 페이백 마나. 인자로 받은 확률로 MP 소모 시에 비율 적용해서 소모시킴
		BLOW_069 = 69,	// 69) 디버프 상태 효과 해제
		BLOW_070 = 70,	// 70) 이동 불가
		BLOW_071 = 71,	// 71) 행동 불가
		BLOW_072 = 72,	// 72) 기 모으기(클러릭 임팩트 블로우 스킬에서만 쓰임)
		BLOW_073 = 73,	// 73) 투명화
		BLOW_074 = 74,	// 74) 최대 공격력
		BLOW_075 = 75,	// 75) 이동속도 변화 절대
		BLOW_076 = 76,	// 76) 이동속도 변화 비율
		BLOW_077 = 77,	// 77) 효과 면역(걸려 있는 상태효과엔 영향을 미치지 않는다)
		BLOW_078 = 78,	// 78) MP 소모하는 스킬을 사용하지 못하게 하는 상태효과
		BLOW_079 = 79,	// 79) 슈퍼아머 브레이크(2차 전직용(162)과는 동작이 조금 다름.. 슈퍼아머를 0으로 만들고 크리티컬 계산은 변경하지 않는다..)
		BLOW_082 = 82, //  82) 파이널 데미지 비율 증가.
		BLOW_083 = 83,	// 83) 버프 상태효과 해제.
		BLOW_084 = 84,	// 84) 강제 상태 이행
		BLOW_086 = 86,	// 86) 팀 반전
		BLOW_087 = 87,	// 87) 힘 비율 증가
		BLOW_088 = 88,	// 88) 민첩 비율 증가
		BLOW_089 = 89,	// 89) 지능 비율 증가
		BLOW_090 = 90,	// 90) 건강 비율 증가
		BLOW_091 = 91,	// 91) 공격자 HP 절대
		BLOW_092 = 92,	// 92) 공격자 HP 비율 
		BLOW_093 = 93,	// 93) 마법 방어 절대
		BLOW_094 = 94,	// 94) 마법 방어 비율
		BLOW_096 = 96,	// 96) 쿨타임 비율 조절 상태효과.
		BLOW_099 = 99,	// 99) 프로그램쪽에서만 쓰는 무적 상태효과
		BLOW_100 = 100,	// 100) 그래픽 이펙트만 붙여주는 상태효과
		BLOW_101 = 101, // 101) 물리 밀리 방어력 절대 변경
		BLOW_102 = 102, // 102) 물리 밀리 방어력 비율 변경
		BLOW_103 = 103, // 103) 물리 레인지 방어력 절대 변경
		BLOW_104 = 104, // 104) 물리 레인지 방어력 비율 변경
		BLOW_105 = 105, // 105) 마법 밀리 방어력 절대 변경
		BLOW_106 = 106, // 106) 마법 밀리 방어력 비율 변경
		BLOW_107 = 107, // 107) 마법 레인지 방어력 절대 변경
		BLOW_108 = 108, // 108) 마법 레인지 방어력 비율 변경
		BLOW_109 = 109, // 109) 피격자 물리 공격력 데미지 적용
		BLOW_110 = 110,	// 110) 피격자 마법 공격력 데미지 적용
		BLOW_111 = 111, // 111) 물리 근접 데미지 반사
		BLOW_112 = 112, // 112) 물리 레인지 데미지 반사
		BLOW_113 = 113, // 113) 마법 근접 데미지 반사
		BLOW_114 = 114, // 114) 마법 레인지 데미지 반사
		BLOW_115 = 115,	// 115) 중독 폭발
		BLOW_116 = 116, // 116) 화상 폭발
		BLOW_117 = 117,	// 117) 결빙 폭발
		BLOW_118 = 118,	// 118) 감전 폭발
		BLOW_120 = 120,	// 120) 운영자 난입에서 사용하는 아이템 사용불가 상태효과
		BLOW_121 = 121,	// 121) 기본 Stand 액션을 입력받은 액션이름으로 대체함.
		BLOW_123 = 123,	// 123) 경직력 비율 변경
		BLOW_124 = 124,	// 124) 경직력 저항 비율 변경
		BLOW_125 = 125,	// 125) 크리티컬 비율 변경
		BLOW_126 = 126,	// 126) 크리티컬 저항 비율 변경
		BLOW_127 = 127,	// 127) 스턴 비율 변경
		BLOW_128 = 128,	// 128) 스턴 저항 비율 변경
		BLOW_129 = 129, // 129) 액션 대체 skill processor 를 사용하여 액션 이름을 CDnActor::SetActionQueue 에서 변경하는 상태효과.
		BLOW_130 = 130,	// 130) 공격불가 상태효과. 현재는 스테이지 클리어 점수가 나올 때에만 사용됨.
		BLOW_131 = 131,	// 131) 무게값 변경 상태효과.
		BLOW_132 = 132,	// 132) 도발 상태효과.
		BLOW_133 = 133,	// 133) 강제 스턴 상태효과. 타격시 기존 스턴 공식 무시하고 상태효과 수치로 받은 확률로 스턴 계산함.
		BLOW_134 = 134, // 134) 물리 피격 데미지 비율 변경
		BLOW_135 = 135,	// 135) 마법 피격 데미지 비율 변경
		BLOW_136 = 136,	// 136) 전염병 상태효과.
		BLOW_137 = 137,	// 137) 몬스터의 체력이 0이 되어 죽을 때 죽음을 유예하고 특정 스킬을 발동
		BLOW_138 = 138,	// 138) CanHit 여부에 관계 없이 CDnActor::IsHittable() 내부에서 무조건 CanHit true 로 판단된다. (하지만 99번 무적상태효과가 우선순위 높음)
		BLOW_139 = 139,	// 139) MP 사용량 비율 변경 상태효과
		BLOW_140 = 140, // 140) 치료 상태효과. 사용자의 <마법 공격력>*<효과 비율>만큼 대상의 HP를 회복시켜준다.
		BLOW_141 = 141, // 141) 치료 비율 변경 상태효과. HP가 회복되는 행동(물약 섭취, 힐링 스킬 발동 등)에 의해 HP를 회복 시킬 경우 <효과 비율>만큼 회복량이 늘어납니다.
		BLOW_142 = 142,	// 142) 슈퍼아머 공격력 비율 변경 상태효과.  <효과 시간>동안 해당 스킬 효과가 적용된 대상의 슈퍼아머1에 대한 공격력 비율이 <효과 비율>만큼 증가합니다.
		BLOW_143 = 143,	// 143) 하이랜더 상태효과. 버프가 적용되어있는 동안 해당 버프를 지닌 플레이어의 HP가 0 또는 0이하로 내려갈 시 “하이랜더” 버프를 지우고 HP를 1로 강제 조정 후 <효과 시간2>동안 1 미만으로 HP가 내려가지 않습니다.
		BLOW_144 = 144,	// 144) 동상 상태효과. 효과 시간동안 정해진 수치만큼 "프레임/이동속도"가 감소되고 추가로 "냉기 저항"도 감소.
		BLOW_146 = 146, // 146) 시간을 완전히 멈추는 효과(프레임을 0으로 만드는 효과), 모든 프레임관련 효과들 중에 가장 우선시 된다. (다른 프레임 조절 효과를 무시한다.)
		BLOW_148 = 148, // 148)	상태효과가 적용된 순간 어그로를 리셋시킨다. 대상이 몬스터 액터일 경우에만 유효하다. (#21673)
		BLOW_149 = 149, // 149) 얼음감옥 상태효과 (#22364)
		BLOW_150 = 150,	// 150) ; 구분자의 인자로 받아놓은 상태효과 인덱스들을 제외한 모든 상태효과 인덱스에 대해 면역이 된다.
		BLOW_151 = 151,	// 151) 카메라 일그러지는 효과
		BLOW_152 = 152,	// 152) 마우스 감도 느려지는 효과
		BLOW_153 = 153, // 153) 쿨타임 있는 패링 상태효과 #22735 
		BLOW_154 = 154, // 154) 특정 스킬데 대해서 무적(특정ID스킬의 Hit시그널 무시)
		BLOW_155 = 155, // 155) 특정 상태효과에 대한 회복
		BLOW_156 = 156, // 156) 슈퍼 아머 깨지지 않는다.(접두어 관련 추가 상태 효과-방어용)
		BLOW_157 = 157, // 157) DOT 효과(지금은 출혈 효과에 사용. 일정 지속 시간동안 일정 간격으로 일정 데미지를 줌)
		BLOW_158 = 158, // 158) 데미지 한번 더 적용..
		BLOW_159 = 159, // 159) 피니쉬 어택 (설정된 HP비율 이하로 내려 가면 설정 비율 이하로 내려간 만큼에 따른 추가 데미지)
		BLOW_160 = 160, // 160) 어펜드 히트(공격 성공시 자신의 최고 물리 데미지 만큼의 추가 데미지 발생)
		BLOW_161 = 161, // 161) 본 크래쉬(일정시간동안 Hit당 추가 데미지(절대값) 발생)
		BLOW_162 = 162, // 162) 브레이킹 포인트( 슈퍼아머를 0으로 만들고, 해당 공격은 크리티컬 데미지를 입힌다.)
		BLOW_164 = 164, // 164) 저지스 파워(힘 1당 @만큼 마법 공격력 증가(절대값)
		BLOW_165 = 165, // 165)            (지능 1당 @만큼 물리 공격력 증가(절대값)
		BLOW_166 = 166, // 166) 저지먼트 헤머 (피격 당하는 상대의 상태이상에 따라 데미지를 증가 시킨다)(목표상태이상ID, 증가 데미지 %)
		BLOW_167 = 167,	// 167) 몬스터 소환시 해당 몬스터의 모든 소유 스킬의 레벨을 이 상태효과에 정의된 레벨로 바꿈.
		BLOW_168 = 168, // 168) 쇼크트랜지션(감전된 대상에게 빛 공격시 추가 범위 감전 발생)
		BLOW_169 = 169, // 169) 크리티컬 브레이크(공격이 크리티컬로 명중시 발생 데미지의 %만큼 추가 데미지 발생)
		BLOW_170 = 170, // 170) 공격이 명중 후 <일정시간>이 지난 후 해당 스킬에 부여된 폭발 데미지 발생
		BLOW_171 = 171, // 171) 쇼타임(목표종류(액티브(0), 패시브(1))의 스킬 <쿨타임 시간 변경>)
		BLOW_172 = 172, // 172) 익스플로전(<목표상태이상>에 걸린 목표를 <특정속성>으로 공격시 <확률>로 <특정상태효과발생>)
		BLOW_173 = 173, // 173) 아이스 베리어(마법공격력의 %비율만큼 방어막 생성, 사라질때 주변 반경 @m이내 적들을 @초 동안 결빙시킨다)
		BLOW_174 = 174, // 174) 버프 훔치기(갯수;지속시간, 갯수만큼을 지우고, 무기를 필요로 하지 않는 스킬에 대해서 설정한 지속시간만큼 스킬 사용자에게 상태효과 적용)
		BLOW_175 = 175, // 175) 밀어내기(0:넉백, 1:밀어내기)
		BLOW_176 = 176, // 176) 구울모드에서 사용됨. 입력된 스킬Id에 해당하는 스킬만 사용 가능 제한함.
		BLOW_177 = 177, // 177) 추가 데미지 표시를 위해 프로그램상에서만 사용되는 단순 상태 효과
		BLOW_178 = 178, // 178) 홀리 쉴드(피격시 목표 상태 이상을 발동 시킵니다.
		BLOW_179 = 179, // 179) 이 상태효과가 들어간 후 최초로 대상에 hit 되면 상태효과로 정의된 액션으로 전이. 허리케인 댄스(해당 스킬의 첫 액션에 부여된 공격이 명중시 자동으로 다음 액션으로 전환합니다.)
		BLOW_180 = 180, // 180) 메지컬 브리즈(사용자가 시전한 모든 공격의 데미지 타입을 마법 데미지로 변경합니다)
		BLOW_181 = 181, // 181) 구울모드에서 사용될 상태 효과(성수)
		BLOW_182 = 182, // 182) 접두어 - 공격 속성을 변경 시키는 상태 효과
		BLOW_183 = 183, // 183) 구울모드에서 접두어 상태효과 적용 되지 않도록 하는 상태효과
		BLOW_184 = 184, // 184) 공격자가 가지고 있는 상태효과. 최종 데미지에서 물리 공격일 경우 인자값으로 받은 비율을 그대로 곱함. (0.3 이면 30% 증가)
		BLOW_185 = 185, // 185) 공격자가 가지고 있는 상태효과. 최종 데미지에서 마법 공격일 경우 인자값으로 받은 비율을 그대로 곱함. (0.3 이면 30% 증가)
	
		// #32220 요리에서 사용되는 스킬 상태효과 계산되기 전에 장비까지 계산된 능력치에 먼저 적용되는 공격력 변경 상태효과들.
		BLOW_200 = 200, // 200) 물리 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
		BLOW_201 = 201, // 201) 물리 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
		BLOW_202 = 202, // 202) 마법 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
		BLOW_203 = 203, // 203) 마법 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
		//////////////////////////////////////////////////////////////////////////

		// 신규 캐릭터 스킬 상태효과
		BLOW_204 = 204,	// 204) Push ??
		BLOW_205 = 205, // 205) Teleprt_Object( "오브젝트ID", 자신이 소유한 오브젝트ID 액터 위치로 이동.
		BLOW_206 = 206, // 206) ActionChangeRatio("확률;액션", 히트에 명중될때 "확률"로 "액션"으 전환
		BLOW_207 = 207, // 207) DamageTransition("비율", 지속시간동안 아군이 데미지를 입으면 데미지 * 비율 만큼 자신이 대신 데미지를 받음)
		BLOW_208 = 208, // 208) Pingpong("데미지전달횟수;범위(cm);비율", 첫 명중 후 범위내 적에게 공격이 전이됨. 전이될 대상이 없다면 "남은 전달 횟수 * 비율 * 데미지"만큼 추가 데미지를 입는다
		BLOW_209 = 209, // 209) BasisAttackDmgUp("비율", 기본 공격의 데미지 증가)
		//////////////////////////////////////////////////////////////////////////

		BLOW_210 = 210, // 210) #32729 길드전용스킬 길드전에서 상대방위치 미니맵에 표기
		BLOW_211 = 211, // 211) #33739 아크비숍네스트헬 버브,디버프 삭제용 상태효과
		BLOW_212 = 212, // 212) #34163 일정 시간 가격동안 일정 범위의 적들에게 상태효과를 적용 ----> 사용 안함.
		BLOW_213 = 213, // 213) #34770 - 계산된 공격력 + (물리 공격력 * 비율) 로 공격력 증가 시킴
		BLOW_214 = 214, // 214) #34770 - 계산된 공격력 + (마법 공격력 * 비율) 로 공격력 증가 시킴
		BLOW_215 = 215, // 215) #32426 내가 소환한 몬스터에게 특정 스킬을 사용하라고 명령하는 시그널 OrderMySummonedMonster 와 연동하는 상태효과.

		BLOW_216 = 216, // 216) 신규 캐릭터 몬스터 호출(?소환) 스킬용 상태효과
		BLOW_217 = 217, // 217) "구울모드 전용" 변신 상태효과 추가
		BLOW_218 = 218, // 218) Freezing 개선 Escape상태효과(결빙확률;얼음내구도;흔들림여부(0/1);피격액션(없을수있음))
		BLOW_219 = 219, // 219) 구울모드에서 사용되는  랜덤 성수효과
		BLOW_220 = 220, // 220) 로터스마쉬- 운석낙하지에 등장하는 몬스터가 사용할 상태효과
		BLOW_221 = 221, // 221) 물리/마법공격에 맞춰서 공격력을 추가 하는 상태효과
		BLOW_222 = 222, // 222) 화상 상태의 적에게 공격을 가할 때 크리티컬 확률이 높아지는 효과
		BLOW_223 = 223, // 223) MP 폭발 기능
		BLOW_224 = 224, // 224) 특정 액션 불가 기능
		BLOW_225 = 225, // 223) [G-드래곤]추가 스킬효과 "혼란" 구현 요청(입력 반전효과)
		BLOW_226 = 226,	// 226) 다크레어 보상 근원 아이템에서 사용하는 특정 스킬 확률로 무시. (해당 스킬의 첫번째 hit 시그널에 맞으면 그때 정해진 확률로 해당 스킬 끝까지 처리)
		BLOW_227 = 227, // 227) HP흡수. 상대방에게 준 데미지의 %만큼 자신의 HP를 회복합니다.
		BLOW_228 = 228, // 228) HitSignal 무시, 특정 확률로 해당 스킬효과가 적용된 대상이 사용하는 HitSignal을 무효화 하면서 이펙트 발생
		BLOW_229 = 229, // 229) 일정 확률로 특정 스킬 효과 발동,(해당 스킬효과가 적용된 대상에게 지속시간동안 @초에 한번씩 확률적으로 특정효과 적용
		BLOW_230 = 230, // 230) 올킬모드에 사용하는 Spectator 상태 
		BLOW_231 = 231,	// 231) 기본MP 값에 의한 지속 MP값 증가
		BLOW_232 = 232, // 232) 일반적인 변신 상태효과, 스테이지내에 사용됨
		BLOW_233 = 233, // 233) #48831 PRE_ADD_KILL_BLOW - 확실히 유저를 죽이는 상태효과.
		BLOW_234 = 234, // 234) #49070 [스킬 펫] CRITICAL_AV 스킬효과 개발(크리티컬 절대값)
		BLOW_235 = 235,	// 235) #51048 결빙, 동상, 감전, 화상 상태효과에 대해 네스트 보스처럼 효과만 적용되고 액션관련 패널티 내용이 적용되지 않도록 하는 상태효과.
		BLOW_236 = 236, // 236) #50903 데미지 분산 상태효과
		BLOW_237 = 237, // 237) #50907 무장 해제 상태효과
		BLOW_238 = 238, // 238) #50917 무기 교체 상태효과.
		BLOW_239 = 239, // 239) #50923 무적 기능, Type으로 무적 기능
		BLOW_240 = 240, // 240) #50923 무적 기능 무시, Type으로 무적 기능 무시
		BLOW_241 = 241, // 241) #51081 섬광탄
		BLOW_242 = 242, // 242) #52332 [스킬 개편]일정 HIT수 이후에 데미지 감소되는 효과 구현
		BLOW_243 = 243, // 243) #52499 칼리 신규 상태효과 마나 쉴드
		BLOW_244 = 244, // 244) #52501 [칼리] "저주" 스킬 효과 추가
		BLOW_245 = 245, // 245) #52509 [칼리] "저주 파괴" 스킬 효과 추가
		BLOW_246 = 246, // 246) #52808 [칼리] "낙인" 스킬효과 구현
		BLOW_247 = 247, // 247) #53454 [칼리] "꼭두각시" 스킬효과 구현
		BLOW_248 = 248, // 248) #52809 [칼리] "공격시 회복"스킬효과 구현
		BLOW_249 = 249, // 249) #53448 [칼리] "중첩시 추가액션"스킬효과 구현
		BLOW_250 = 250, // 250) #52906 [칼리] "대상 크리티컬 확률 추가" 스킬 효과 구현
		BLOW_251 = 251, // 251) #53492 [스킬 개편] 크리티컬 최종확률 % 수치 증가 효과
		BLOW_252 = 252, // 252) #53722 [로터스마쉬 후반]특정스킬을 면역하는 스킬효과 개발
		BLOW_253 = 253, // 253) #52905 [칼리]"중첩 추가효과"스킬 효과 구현
		BLOW_254 = 254, // 254) #60235 [데저트 드래곤] 어그로 대상에게 StateEffect 표시 기능

		//통합레벨스킬 기능 상태효과들..
		BLOW_255 = 255, // 255) //1. 사냥시 경험치 추가 n%증가
		BLOW_256 = 256, // 255)//2. 펫 경험치 n%증가
		BLOW_257 = 257, // 255)//3. 마을에서 캐릭터 이동속도 n%증가							----> 사용 안함..
		BLOW_258 = 258, // 255)//4. 탈것으 이동 속고 n%증가									----> 사용 안함
		BLOW_259 = 259, // 255)//5. 캐릭터 사망시 내구도 감소량 감소로 변경
		BLOW_260 = 260, // 255)//6. NPC에게 선물할 때 호감도 10%증가
		BLOW_261 = 261, // 255)//7. 콜로세움 메달 n개 추가 획득
		BLOW_262 = 262, // 255)//8. 농장 재배 시간 n%만큼 감소
		BLOW_263 = 263, // 255)//9. 래더 상점 아이템 1% 할인
		BLOW_264 = 264, // 255)//10. 자동 낚시 시간 n초 감소
		BLOW_265 = 265, // 255)//11. 요리 숙련 n% 증가
		BLOW_266 = 266, // 255)//12. 금화 획득량 n%증가
		//////////////////////////////////////////////////////////////////////////

		BLOW_267 = 267, // 267) 통합레벨스킬 공격용 상태효과(공격시 크리티컬이 되면 n초동안 지속되는 251번 상태효과(크리티컬 n%증가)를 공격자에게 적용, 쿨타임 n초 적용)
		BLOW_268 = 268, // 268) 통합레벨스킬 방어용 상태효과(HP가 n%미만시 데미지 감소 n%증가)
		BLOW_269 = 269, // 269) 통합레벨스킬 - 궁극기 쿨타임 감소
		BLOW_270 = 270, // 270) 귀환자 시스템 - 파티원 드랍율 증가
		BLOW_271 = 271, // 271) 케릭터 기본 상태값을 기반으로 최종 수치만 올려주는 상태효과
		BLOW_272 = 272, // 272) 상태효과가 있으면 아이템에 의해서 채우는 체력수치가 증가합니다.
		BLOW_273 = 273, // 273) 지속시간 동안 Gravity 시그널을 무시합니다.
		BLOW_274 = 274, // 274) #71659 [데저트 드래곤] 스킬효과 - 최대피해 제한 효과 개발
		BLOW_275 = 275, // 275) #71663 상태효과 적용 시 특정방식제외한 HP회복 무시
		BLOW_276 = 276, // 276) #75202 [스킬] 특정 스킬 수행 시에만 적용되는 스킬(버프)효과 구현
		BLOW_277 = 277, // 277) #75694 [70LV] ObjectVisualize 개발 요청 
		BLOW_278 = 278, // 278) #77123 [스킬] 피니시 어택 파티용 효과 제작
		BLOW_279 = 279, // 279) #77123 [스킬] 본크러쉬 파티용 효과 제작
		BLOW_280 = 280, // 280) #79363 드래곤버프 효과 제작

		// 어쎄신관련 상태효과
		BLOW_281 = 281, // 281) #84056 HP가 특정수치 이하가 되는 순간 정히진 스킬을 발동

		//RLKT BLOWS
		BLOW_298 = 298, // Dark Avenger transform+change weapon
		BLOW_306 = 306, // Reduce resistance by %% (lencea 7201 , academic)
		BLOW_345 = 345, // Mechanical Mode change skill
		BLOW_350 = 350, // Consume Bubble 
		BLOW_351 = 351, // Disable skill
		BLOW_370 = 370, // Check Bubble and cancel skill??	

		//EMPTY
		BLOW_282 = 282, //EMPTY BLOW! TEST!
		BLOW_283 = 283, //EMPTY BLOW! TEST!
		BLOW_284 = 284, //EMPTY BLOW! TEST!
		BLOW_285 = 285, //EMPTY BLOW! TEST!
		BLOW_286 = 286, //EMPTY BLOW! TEST!
		BLOW_287 = 287, //EMPTY BLOW! TEST!
		BLOW_288 = 288, //EMPTY BLOW! TEST!
		BLOW_289 = 289, //EMPTY BLOW! TEST!
		BLOW_290 = 290, //EMPTY BLOW! TEST!
		BLOW_291 = 291, //EMPTY BLOW! TEST!
		BLOW_292 = 292, //EMPTY BLOW! TEST!
		BLOW_293 = 293, //EMPTY BLOW! TEST!
		BLOW_294 = 294, //EMPTY BLOW! TEST!
		BLOW_295 = 295, //EMPTY BLOW! TEST!
		BLOW_296 = 296, //EMPTY BLOW! TEST!
		BLOW_297 = 297, //EMPTY BLOW! TEST!
		BLOW_299 = 299, //EMPTY BLOW! TEST!
		BLOW_300 = 300, //EMPTY BLOW! TEST!
		BLOW_301 = 301, //EMPTY BLOW! TEST!
		BLOW_302 = 302, //EMPTY BLOW! TEST!
		BLOW_303 = 303, //EMPTY BLOW! TEST!
		BLOW_304 = 304, //EMPTY BLOW! TEST!
		BLOW_305 = 305, //EMPTY BLOW! TEST!
		BLOW_307 = 307, //EMPTY BLOW! TEST!
		BLOW_308 = 308, //EMPTY BLOW! TEST!
		BLOW_309 = 309, //EMPTY BLOW! TEST!
		BLOW_310 = 310, //EMPTY BLOW! TEST!
		BLOW_311 = 311, //EMPTY BLOW! TEST!
		BLOW_312 = 312, //EMPTY BLOW! TEST!
		BLOW_313 = 313, //EMPTY BLOW! TEST!
		BLOW_314 = 314, //EMPTY BLOW! TEST!
		BLOW_315 = 315, //EMPTY BLOW! TEST!
		BLOW_316 = 316, //EMPTY BLOW! TEST!
		BLOW_317 = 317, //EMPTY BLOW! TEST!
		BLOW_318 = 318, //EMPTY BLOW! TEST!
		BLOW_319 = 319, //EMPTY BLOW! TEST!
		BLOW_320 = 320, //EMPTY BLOW! TEST!
		BLOW_321 = 321, //EMPTY BLOW! TEST!
		BLOW_322 = 322, //EMPTY BLOW! TEST!
		BLOW_323 = 323, //EMPTY BLOW! TEST!
		BLOW_324 = 324, //EMPTY BLOW! TEST!
		BLOW_325 = 325, //EMPTY BLOW! TEST!
		BLOW_326 = 326, //EMPTY BLOW! TEST!
		BLOW_327 = 327, //EMPTY BLOW! TEST!
		BLOW_328 = 328, //EMPTY BLOW! TEST!
		BLOW_329 = 329, //EMPTY BLOW! TEST!
		BLOW_330 = 330, //EMPTY BLOW! TEST!
		BLOW_331 = 331, //EMPTY BLOW! TEST!
		BLOW_332 = 332, //EMPTY BLOW! TEST!
		BLOW_333 = 333, //EMPTY BLOW! TEST!
		BLOW_334 = 334, //EMPTY BLOW! TEST!
		BLOW_335 = 335, //EMPTY BLOW! TEST!
		BLOW_336 = 336, //EMPTY BLOW! TEST!
		BLOW_337 = 337, //EMPTY BLOW! TEST!
		BLOW_338 = 338, //EMPTY BLOW! TEST!
		BLOW_339 = 339, //EMPTY BLOW! TEST!
		BLOW_340 = 340, //EMPTY BLOW! TEST!
		BLOW_341 = 341, //EMPTY BLOW! TEST!
		BLOW_342 = 342, //EMPTY BLOW! TEST!
		BLOW_343 = 343, //EMPTY BLOW! TEST!
		BLOW_344 = 344, //EMPTY BLOW! TEST!
		BLOW_346 = 346, //EMPTY BLOW! TEST!
		BLOW_347 = 347, //EMPTY BLOW! TEST!
		BLOW_348 = 348, //EMPTY BLOW! TEST!
		BLOW_349 = 349, //EMPTY BLOW! TEST!
		BLOW_352 = 352, //EMPTY BLOW! TEST!
		BLOW_353 = 353, //EMPTY BLOW! TEST!
		BLOW_354 = 354, //EMPTY BLOW! TEST!
		BLOW_355 = 355, //EMPTY BLOW! TEST!
		BLOW_356 = 356, //EMPTY BLOW! TEST!
		BLOW_357 = 357, //EMPTY BLOW! TEST!
		BLOW_358 = 358, //EMPTY BLOW! TEST!
		BLOW_359 = 359, //EMPTY BLOW! TEST!
		BLOW_360 = 360, //EMPTY BLOW! TEST!
		BLOW_361 = 361, //EMPTY BLOW! TEST!
		BLOW_362 = 362, //EMPTY BLOW! TEST!
		BLOW_363 = 363, //EMPTY BLOW! TEST!
		BLOW_364 = 364, //EMPTY BLOW! TEST!
		BLOW_365 = 365, //EMPTY BLOW! TEST!
		BLOW_366 = 366, //EMPTY BLOW! TEST!
		BLOW_367 = 367, //EMPTY BLOW! TEST!
		BLOW_368 = 368, //EMPTY BLOW! TEST!
		BLOW_369 = 369, //EMPTY BLOW! TEST!
		BLOW_371 = 371, //EMPTY BLOW! TEST!
		BLOW_372 = 372, //EMPTY BLOW! TEST!
		BLOW_373 = 373, //EMPTY BLOW! TEST!
		BLOW_374 = 374, //EMPTY BLOW! TEST!
		BLOW_375 = 375, //EMPTY BLOW! TEST!
		BLOW_376 = 376, //EMPTY BLOW! TEST!
		BLOW_377 = 377, //EMPTY BLOW! TEST!
		BLOW_378 = 378, //EMPTY BLOW! TEST!
		BLOW_379 = 379, //EMPTY BLOW! TEST!
		BLOW_MAX,
	};

	enum emBLOW_STATE
	{
		STATE_NONE,
		STATE_BEGIN,
		STATE_DURATION,
		STATE_END,
	};

	emBLOW_INDEX emBlowIndex;
	emBLOW_STATE emBlowState;

	std::string szValue;	// [2010/12/27 semozz] 문자열 길이 제한 없애기 위해 string으로 변경
	
	float fDurationTime;
	float fDelayTime;

	STATE_BLOW()
		: emBlowIndex(BLOW_NONE)
		, emBlowState(STATE_NONE)
		, fDurationTime(0.0f)
		, fDelayTime(0.0f)
	{

		
	}
};
