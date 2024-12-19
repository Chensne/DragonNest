#pragma once

class CEtActionBase;

class SpecialBox_Test : public ::testing::Test
{
public:
	static DNTableFileFormat* s_pMailTable;
	static DNTableFileFormat* s_pKeepBoxProvideItemTable;

public:
	SpecialBox_Test(void);
	virtual ~SpecialBox_Test(void);

	virtual void SetUp( void );
	virtual void TearDown( void );

	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
};

namespace SpecialBox
{
	struct Common
	{
		enum eCommon
		{
			ListMax = 40,				// 리스트 최대값
			RewardAllMax = 10,			// 보상 최대값
			RewardSelectMax = 50,		// 선택보상 최대값
		};

		enum eRequestDelay
		{
			DelayTime = 5,
		};
	};

	struct TargetTypeCode
	{
		enum eTargetTypeCode
		{
			AccountAll = 1,			// 1=계정전체
			AccountSelect = 2,		// 2=계정지정
			CharacterAll = 3,		// 3=캐릭터전체
			CharacterSelect = 4,	// 4=캐릭터지정
			ConditionSelect = 5,	// 5=조건지정
		};
	};

	struct ReceiveTypeCode
	{
		enum eReceiveTypeCode
		{
			All = 1,
			Select = 2,
		};
	};
};