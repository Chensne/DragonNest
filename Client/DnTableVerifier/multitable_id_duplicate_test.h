#pragma once



// 나눠진 테이블 구조에서 중복된 테이블 ID 가 있는지 검사.
class multitable_id_duplicate_test : public ::testing::TestWithParam<string>
{
public:
	multitable_id_duplicate_test(void);
	virtual ~multitable_id_duplicate_test(void);

	// from gtest /////////////////////////////////////////////////
	// 매 테스트 마다 초기화/해제
	virtual void SetUp( void );
	virtual void TearDown( void );
	//

	// 테스트 케이스 전체에 해당하는 초기화/해제
	static void SetUpTestCase( void );
	static void TearDownTestCase( void );
	////////////////////////////////////////////////////////////////
};
