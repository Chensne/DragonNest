#include "StdAfx.h"
#include "multitable_id_duplicate_test.h"


multitable_id_duplicate_test::multitable_id_duplicate_test(void)
{
}

multitable_id_duplicate_test::~multitable_id_duplicate_test(void)
{
}

// �� �׽�Ʈ ���� �ʱ�ȭ/����
void multitable_id_duplicate_test::SetUp( void )
{

}


void multitable_id_duplicate_test::TearDown( void )
{

}

// �׽�Ʈ ���̽� ��ü�� �ش��ϴ� �ʱ�ȭ/����
void multitable_id_duplicate_test::SetUpTestCase( void )
{

}


void multitable_id_duplicate_test::TearDownTestCase( void )
{

}


TEST_F( multitable_id_duplicate_test, TABLE_ID_DUPLICATE_TEST )
{
	string strTableFileName;
	vector<string> vlExtFilesToVerify;

	vector<std::string> tokens;
	string delimiters = ";";

	TokenizeA( g_strTableName, tokens, delimiters );

	std::vector<std::string>::iterator iter = tokens.begin();
	for ( ; iter != tokens.end(); ++iter)
	{
		vlExtFilesToVerify.push_back( *iter );
	}

	int iNumExtToVerify = (int)vlExtFilesToVerify.size();
	for( int iIndex = 0; iIndex < iNumExtToVerify; ++iIndex )
	{
		strTableFileName = vlExtFilesToVerify.at( iIndex );
		strTableFileName.append( ".dnt" );

		vector<string> vlFilePathsToVerify;
		string strTableFullPath;

		GatherConcernedExt( strTableFileName.c_str(), vlFilePathsToVerify, strTableFullPath );

		// ����� Ǯ �н��� �ϳ��� ���ٸ� �� ���̺��� ������ ���� �ƴϹǷ� ������ �ʿ䰡 ����.
		if( vlFilePathsToVerify.empty() )
			continue;

		// vlFilePathsToVerify �� �ε���. ���̵� ��ø�Ǿ��� ��� �ش� ���̵� ��� ���̺� ������� �˾Ƴ��� ����.
		// -1 �̸� _ �� ���� ���� ���̺� �̸�.
		vector<string> vlFilePaths = vlFilePathsToVerify;
		int iTableFilePathIndex = -1;

		// ó���� ���� �ε��ؼ� ���̵� �ߺ� üũ�ϰ� �Ŀ� ��ø�ؼ� �ε��� �� ���� �ݺ�.
		DNTableFileFormat* pSox = new DNTableFileFormat;
		pSox->Load( strTableFileName.c_str(), false );

		// ���� ���� ���̺� �̸����� ������ �������� �ʴ´ٸ� �ø��� �� �� �� ������ �������� ó���Ѵ�.
		if( 0 == pSox->GetFieldCount() )
		{
			delete pSox;
			pSox = new DNTableFileFormat;
			pSox->Load( vlFilePathsToVerify.back().c_str(), false );

			// �񱳴���� �Ǵ� �� �� ������ �����Ѵ�.
			vlFilePathsToVerify.pop_back();
			iTableFilePathIndex = (int)vlFilePaths.size()-1;
		}

		multimap<int, int> mmapIDToTableNameIndex;
		set<int> setItemIDs;
		for( int i = 0; i < pSox->GetItemCount(); ++i )
		{
			setItemIDs.insert( pSox->GetItemID( i ) );
			mmapIDToTableNameIndex.insert( make_pair( pSox->GetItemID( i ), iTableFilePathIndex ) );
		}

		TCHAR atcTableSeriesFileName[ MAX_PATH ] = { 0 };
		TCHAR atcTableFileName[ MAX_PATH ] = { 0 };
		TCHAR atcFileName[ MAX_PATH ] = { 0 };
		TCHAR atcFileExt[ MAX_PATH ] = { 0 };
		TCHAR atcBuf[ 256 ] = { 0 };
		for( DWORD i=0; i< vlFilePathsToVerify.size(); i++ ) 
		{
			DNTableFileFormat* pAddition = new DNTableFileFormat;

			pAddition->Load( vlFilePathsToVerify[ i ].c_str(), false );

			if( pSox->GetFieldCount() != pAddition->GetFieldCount() )
			{
				MultiByteToWideChar( CP_ACP, 0, strTableFileName.c_str(), -1, atcTableSeriesFileName, MAX_PATH );
				MultiByteToWideChar( CP_ACP, 0, vlFilePathsToVerify[ i ].c_str(), -1, atcTableFileName, MAX_PATH );
				_stprintf_s( atcBuf, _T("{TableSeriesName:%s, TableFilePath:%s, Note:���̺� ���̺� ������ �ٸ��ϴ�.}"), atcTableSeriesFileName, atcTableFileName );
				SCOPED_TRACE( atcBuf );
				EXPECT_TRUE( pSox->GetFieldCount() == pAddition->GetFieldCount() );
				continue;
			}

			set<int> setAdditionItemIDs;
			for( int k = 0; k < pAddition->GetItemCount(); ++k )
				setAdditionItemIDs.insert( pAddition->GetItemID( k ) );

			vector<int> vlIntersection;
			set_intersection( setItemIDs.begin(), setItemIDs.end(), 
							  setAdditionItemIDs.begin(), setAdditionItemIDs.end(), 
							  back_inserter( vlIntersection ) );

			for( int k = 0; k < (int)vlIntersection.size(); ++k )
			{
				// ��� ���̺� ���� ����ΰ�?
				pair<multimap<int, int>::iterator, multimap<int, int>::iterator> iter_pair = mmapIDToTableNameIndex.equal_range( vlIntersection.at(k) );
				multimap<int, int>::iterator iter = iter_pair.first;
				string strExistingIDFilePath;
				if( -1 < iter->second )
					strExistingIDFilePath = vlFilePaths.at( iter->second );
				else
					strExistingIDFilePath = strTableFileName;

				MultiByteToWideChar( CP_ACP, 0, strExistingIDFilePath.c_str(), -1, atcTableSeriesFileName, MAX_PATH );
				MultiByteToWideChar( CP_ACP, 0, vlFilePathsToVerify[ i ].c_str(), -1, atcTableFileName, MAX_PATH );
				//_stprintf_s( atcBuf, _T("{ExistingFilePath:%s, TableFilePath:%s, ID:%d, Note:���̺� ���̵� �ߺ��Ǿ����ϴ�.}"), atcTableSeriesFileName, 
				//			 atcTableFileName, vlIntersection.at(k) );

				_tsplitpath_s( atcTableFileName, NULL, 0, NULL, 0, atcFileName, _countof(atcFileName), atcFileExt, _countof(atcFileName) );
				tstring strFileName( atcFileName );
				strFileName += atcFileExt;
				_stprintf_s( atcBuf, _T("{TablenName:%s, Index:%d, Note:���̺� ���̵� �ߺ��Ǿ����ϴ�.}"), strFileName.c_str(), vlIntersection.at(k) );
				SCOPED_TRACE( atcBuf );
				EXPECT_TRUE( vlIntersection.empty() );
			}

			for( int k = 0; k < pAddition->GetItemCount(); ++k )
			{
				setItemIDs.insert( pAddition->GetItemID( k ) );
				mmapIDToTableNameIndex.insert( make_pair( pAddition->GetItemID( k ), i ) );
			}

			delete pAddition;
		}

		delete pSox;
	}
}


//INSTANTIATE_TEST_CASE_P( SELECTED_TABLE, multitable_id_duplicate_test, ValuesIn() );