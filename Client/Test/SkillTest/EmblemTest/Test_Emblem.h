#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class CDnItemCompounder;

class CTest_Emblem : public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE( CTest_Emblem );
	CPPUNIT_TEST( test_composition_Jewel_Plate );
	CPPUNIT_TEST( test_item_decomposition );
	CPPUNIT_TEST_SUITE_END();

private:
	CDnItemCompounder* m_pItemCompounder;

public:
	CTest_Emblem(void);
	~CTest_Emblem(void);

	//void test_composition_Jewel_Jewel( void );
	void test_composition_Jewel_Plate( void );
	void test_item_decomposition( void );

};
