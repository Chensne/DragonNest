
#include "stdafx.h"
#include <stdio.h>

#include <vector>
#include <map>

#include <algorithm>

#define PRE_ADD_COMPARE_TABLE // bintitle.

#include "DNTableFile.h"
#include "MemPool.h"
#include "DnItemCompounder.h"
#include "SundriesFunc.h"


using namespace std;


struct SCell
{
	int mainID; // 메인테이블에서의 ID.
	int enchantID;
	int enchatLevel;

	int id;			   // ItemID.
	string name;       // 컬럼명.
	DNTableCell * pData; // 데이타.
};

//typedef vector< vector< SCell > * > vNT;
//vNT g_newTable;
typedef std::map< int, vector< SCell > * > vNT;
vNT g_newTable;


enum E_TABLE
{
	ENCHANTTABLE = 0,
	ENCHANTMAINTABLE = 1,
	ENCHANTMAXSTATETABLE = 2,
	ENCHANTNEEDITEMTABLE = 3,

	MAX
};

int GetFiledNameIndex( vector< string > & vVector, const char * name )
{
	int resIndex = -1;
	int size = vVector.size();	
	for( int i=0; i<size; ++i )
	{
		if( vVector[i].compare( name ) == 0 )
		{
			resIndex = i;
			break;
		}
	}
	return resIndex;
}

void main()
{
	//// .ext 파일들.
	//std::vector< std::string > vFileNames;
	//vFileNames.push_back("enchanttable");
	//vFileNames.push_back("enchantmaintable");
	//vFileNames.push_back("enchantmaxstatetable");
	//vFileNames.push_back("enchantneeditemtable");

	//// Tables.
	//std::vector< DNTableFileFormat * > vTables;
	//DNTableFileFormat * pTable = NULL;

	//// 테이블 생성.
	//for( int i=E_TABLE::ENCHANTTABLE; i<E_TABLE::MAX; ++i )
	//{
	//	pTable = new DNTableFileFormat;
	//	pTable->Load( ( vFileNames[ i ] + ".ext" ).c_str() );
	//	pTable->SetLoadName( vFileNames[ i ].c_str() );
	//	vTables.push_back( pTable );
	//}



	// Before.
	printf("** Load [enchant]\n");
	DNTableFileFormat * pTableBefore = new DNTableFileFormat;
	pTableBefore->Load( "out/enchanttable.dnt" );
	pTableBefore->SetLoadName( "enchanttable.dnt" );




	// Main Table.
	printf("** Load [enchantMain.dnt]\n");
	DNTableFileFormat * pTableMain = new DNTableFileFormat;
	pTableMain->Load( "out/enchantmaintable.dnt" );
	pTableMain->SetLoadName( "enchantmaintable.dnt" );
	int sizeMain = pTableMain->GetItemCount();
	int fieldCntMain = pTableMain->GetFieldCount();

	// State.
	printf("** Load [enchantMaxState.dnt]\n");
	DNTableFileFormat * pTableState = new DNTableFileFormat;
	pTableState->Load( "out/enchantmaxstatetable.dnt" );
	pTableState->SetLoadName( "enchantmaxstatetable.dnt" );

	// NeedItem.
	printf("** Load [enchantNeedItem.dnt]\n");
	DNTableFileFormat * pTableNeedItem = new DNTableFileFormat;
	pTableNeedItem->Load( "out/enchantneeditemtable.dnt" );
	pTableNeedItem->SetLoadName( "enchantneeditemtable.dnt" );


	//// Main Table Data 컨테이너 설정.	
	//g_newTable.reserve( sizeMain );
	//for( int i=0; i<sizeMain; ++i )
	//{
	//	vector< SCell > * pVec = new vector< SCell >;
	//	pVec->reserve( pTableBefore->GetFieldCount() );
	//	g_newTable.push_back( pVec );
	//}


	//----------------------------------------------------------
	// 나뉜 테이블을 하나로 취합.
	//
	DNTableCell * pTempCell = NULL;
	printf("\n** 데이타통합 [enchantMaxState--enchantMain--enchantNeedItem]\n");
	for( int i=0; i<sizeMain; ++i )
	{
		int mainItemID = pTableMain->GetItemID( i );

		// 1행 데이터 벡터.
		//vector< SCell > * pVecRow = g_newTable[ i ];
		vector< SCell > * pVecRow = new vector< SCell >;
		
		// 1행 채우기.
		for( int fidx=0; fidx<fieldCntMain; ++fidx )
		{
			const char * fieldNameRef = NULL;
			const char * fieldNameMain = pTableMain->GetFieldLabel( fidx );

			SCell aCell;			
			DNTableCell * pTableCell = pTableMain->GetFieldFromLablePtr( mainItemID, fieldNameMain );
			DNTableCell * pTableCellEnchantID = pTableMain->GetFieldFromLablePtr( mainItemID, "_EnchantID" ); // EnchantID.
			DNTableCell * pTableCellEnchantLevel = pTableMain->GetFieldFromLablePtr( mainItemID, "_EnchantLevel" ); // EnchantLevel.

			//pTableMain->getfield

			aCell.mainID = mainItemID;
			aCell.enchantID = pTableCellEnchantID->GetInteger();
			aCell.enchatLevel = pTableCellEnchantLevel->GetInteger();


			//if( _stricmp( fieldNameMain, "_EnchantID" ) == 0 || 
			//	_stricmp( fieldNameMain, "_EnchantLevel" ) == 0 ||
			//	_stricmp( fieldNameMain, "_NeedCoin" ) == 0 ||
			//	_stricmp( fieldNameMain, "_DisjointDrop" ) == 0 ) 



			// NeedItem Table.
			if( _stricmp( fieldNameMain, "_NeedItemTableID" ) == 0 )
			{						
				int itemID_Need = pTableCell->GetInteger();
				for( int idxneedfield=0; idxneedfield<pTableNeedItem->GetFieldCount(); ++idxneedfield )
				{					
					const char * pNeedFieldName = pTableNeedItem->GetFieldLabel( idxneedfield );

					// NeedItemTable 에만 존재하는"_UpStateRatio" 컬럼은 제외. 
					if( _stricmp( pNeedFieldName, "_UpStateRatio" ) != 0 )
					{						
						pTempCell = pTableNeedItem->GetFieldFromLablePtr( itemID_Need, pTableNeedItem->GetFieldLabel(idxneedfield) );

						aCell.id = itemID_Need;								 
						aCell.name.assign( pNeedFieldName );
						aCell.pData = pTempCell;

						// 행에 한셀 데이터 추가.
						pVecRow->push_back( aCell );

					}					
				}				
			}

			// State Table.
			else if( _stricmp( fieldNameMain, "_StateID" ) == 0 )
			{				
				int itemID_State = pTableCell->GetInteger();
				for( int idxstatefield=0; idxstatefield<pTableState->GetFieldCount(); ++idxstatefield )
				{		
					const char * pStateFieldName = pTableState->GetFieldLabel( idxstatefield );					
					pTempCell = pTableState->GetFieldFromLablePtr( itemID_State, pStateFieldName );

					aCell.id = itemID_State;
					aCell.name.assign( pStateFieldName );
					aCell.pData = pTableState->GetFieldFromLablePtr( itemID_State, pStateFieldName );

					// 행에 한셀 데이터 추가.
					pVecRow->push_back( aCell );


				}				
			}

			// 우선 [enchantMain] 과 [enchant] 의 비교는 제외.  needcoin 과  disjointdrop 값이 서로 많이 다름.
			// Main Table.
			else
			{		
				aCell.id = mainItemID;
				aCell.name.assign( fieldNameMain );
				aCell.pData = pTableCell;

				// 행에 한셀 데이터 추가.
				pVecRow->push_back( aCell );
			}

		}

		g_newTable.insert( vNT::value_type( mainItemID, pVecRow ) );
	}


	// 필드명 등록. 데이타 타입구분용.
	vector< string > vFieldNames;
	vector< string >::iterator vFieldNameIt;	
	int fieldCnt_Before = pTableBefore->GetFieldCount();
	for( int i=0; i<fieldCnt_Before; ++i )
	{
		vFieldNames.push_back( string( pTableBefore->GetFieldLabel( i ) ) );
	}


	//----------------------------------------------------------
	// 기존테이블 데이타와 비교.
	//
	printf("\n** 데이타비교 [enchant] VS [enchat NeedItem--Main--MaxState]\n");
	FILE * fpOut = fopen("out\\different.txt", "w");

	fprintf( fpOut, "enchantmain:%d,  enchant:%d\n\n", g_newTable.size(), pTableBefore->GetItemCount() );

	int nDiffCnt = 0;
	for( int i=0; i<g_newTable.size(); ++i )
	{	
		int itemID = pTableBefore->GetItemID( i );   // Before Table.

		//vector< SCell > * pVecRow = g_newTable[ i ]; // newTableData vector.

		vNT::iterator it = g_newTable.find( itemID );
		if( it == g_newTable.end() )
			continue;
		vector< SCell > * pVecRow = (*it).second;
		
		DNTableCell * pCellBefore = NULL;

		for( int k=0; k<pVecRow->size(); ++k )
		{
			SCell & aCell = pVecRow->at( k );

			// Field Data Type.
			int idxField = GetFiledNameIndex( vFieldNames, aCell.name.c_str() );
			if( idxField == -1 )
				continue;
			CDNTableFile::FieldType::eCode cFldType = pTableBefore->m_nVecFieldType[ idxField ];											


			// 기존데이타.
			pCellBefore = pTableBefore->GetFieldFromLablePtr( itemID, aCell.name.c_str() );


			// 둘다 NULL 인경우 무시.
			if( aCell.pData == NULL && pCellBefore == NULL )
				continue;

			// NULL Data.
			if( aCell.pData == NULL && pCellBefore != NULL )
			{
				//++nDiffCnt;

				switch( cFldType ) {
					case CDNTableFile::FieldType::eCode::STRING:	

						// [NULL]-[] 무시.
						if( _stricmp( pCellBefore->GetString(), "0" ) != 0 )
						{
							++nDiffCnt;
							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %s ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), "NULL", pCellBefore->GetString());							
						}
						break;

					case CDNTableFile::FieldType::eCode::BOOL:
					case CDNTableFile::FieldType::eCode::NA:							
						++nDiffCnt;
						fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %d ]\n", 
							aCell.mainID, aCell.enchantID, aCell.enchatLevel,
							aCell.id, i, aCell.name.c_str(), "NULL", pCellBefore->GetInteger());							
						break;


					case CDNTableFile::FieldType::eCode::INT:
						
						// NULL / 0 허용.
						if( pCellBefore->GetInteger() != 0  )
						{
							++nDiffCnt;

							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %d ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), "NULL", pCellBefore->GetInteger());							
							
						}
						break;

					case CDNTableFile::FieldType::eCode::PER:  
						++nDiffCnt;

						fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %.6f ]\n", 
							aCell.mainID, aCell.enchantID, aCell.enchatLevel,
							aCell.id, i, aCell.name.c_str(), "NULL", pCellBefore->GetFloat());							
						break;	

					case CDNTableFile::FieldType::eCode::FLOAT:

						// NULL / 0 허용.
						if( pCellBefore->GetFloat() != 0.0f  )
						{
							++nDiffCnt;

							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %.6f ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), "NULL", pCellBefore->GetFloat());							
						}
						break;						
				}
			}

			else
			{
				switch( cFldType ) {
					case CDNTableFile::FieldType::eCode::STRING:							
						if( _stricmp( aCell.pData->GetString(), pCellBefore->GetString() ) != 0 )
						{
							// []-[0] 무시.
							if( (strlen( aCell.pData->GetString() ) < 1) && 
								(_stricmp( pCellBefore->GetString(), "0" ) == 0) )
								continue;

							++nDiffCnt;
							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %s ]-[ %s ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), aCell.pData->GetString(), pCellBefore->GetString());						
						}
						break;

					case CDNTableFile::FieldType::eCode::BOOL:
					case CDNTableFile::FieldType::eCode::NA:													
						if( aCell.pData->GetInteger() != pCellBefore->GetInteger() )
						{
							++nDiffCnt;
							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %d ]-[ %d ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), aCell.pData->GetInteger(), pCellBefore->GetInteger());
						}						
						break;

					case CDNTableFile::FieldType::eCode::INT:
						{
							// 1의 오차 허용.
							int nValue = abs( aCell.pData->GetInteger() - pCellBefore->GetInteger() );
							if( nValue > 1 )
								//if( aCell.pData->GetInteger() != pCellBefore->GetInteger() )
							{
								++nDiffCnt;
								fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %d ]-[ %d ]\n", 
									aCell.mainID, aCell.enchantID, aCell.enchatLevel,
									aCell.id, i, aCell.name.c_str(), aCell.pData->GetInteger(), pCellBefore->GetInteger());
							}
						}
						break;

					case CDNTableFile::FieldType::eCode::PER:
					case CDNTableFile::FieldType::eCode::FLOAT:

						int fValue = fabs( aCell.pData->GetFloat() - pCellBefore->GetFloat() );
						
						// 1의 오차 허용.
						//if( aCell.pData->GetFloat() != pCellBefore->GetFloat() )
						if( fValue > 1.0f )
						{
							++nDiffCnt;
							fprintf( fpOut, "MainID[%7d], EnchantID[%7d], Level[%2d], RefID[%7d] : Row:%6d, {%15s} [ %f ]-[ %.6f ]\n", 
								aCell.mainID, aCell.enchantID, aCell.enchatLevel,
								aCell.id, i, aCell.name.c_str(), aCell.pData->GetFloat(), pCellBefore->GetFloat());
						}
						break;						
				}
			}
		}

	}
	fprintf( fpOut, "\n***  Different Count : %d", nDiffCnt);
	fclose( fpOut );
	//-------------


	printf("** 총 %d 개 데이타 다름\n", nDiffCnt);




	//----------------------------------------------------------
	// 출력용.

	//char * str[32];
	//FILE * fp = fopen("out\\data.txt", "w");
	//fprintf( fp, "Size : %d ", g_newTable.size() );


	//// 컬럼명.
	//vector< SCell > * pVecRowTemp = g_newTable[ 0 ];
	//for( int x=0; x<pVecRowTemp->size(); ++x )
	//{
	//	fprintf( fp, "%s ", pVecRowTemp->at(x).name.c_str() );
	//}

	//for( int i=0; i<g_newTable.size(); ++i )
	//{
	//	memset(str,0,32);
	//	vector< SCell > * pVecRow = g_newTable[ i ];
	//	fprintf( fp, "%d : ", i);

	//	for( int k=0; k<pVecRow->size(); ++k )
	//	{
	//		SCell & aCell = pVecRow->at( k );
	//		if( aCell.pData == NULL )
	//		{
	//			fprintf( fp, "NULL ");
	//			continue;
	//		}

	//		int idxField = GetFiledNameIndex( vFieldNames, aCell.name.c_str() );
	//		if( idxField == -1 )
	//			continue;

	//		char cFldType = (char)pTableBefore->m_nVecFieldType[ idxField ];											
	//				
	//		switch( cFldType ) {
	//				case CSOXFile::FT_STRING:
	//					fprintf( fp, "%s ", aCell.pData->GetString() );
	//					break;

	//				case CSOXFile::FT_BOOL:
	//				case CSOXFile::FT_NA:							
	//				case CSOXFile::FT_INT:
	//					fprintf( fp, "%d ", aCell.pData->GetInteger() );
	//					break;

	//				case CSOXFile::FT_PER:
	//				case CSOXFile::FT_FLOAT:
	//					fprintf( fp, "%f ", aCell.pData->GetFloat() );
	//					break;						
	//		}
	//	}

	//	fprintf( fp, "\n");
	//}
	//fclose( fp );



	//------------------------------------------------------------------------------------------------------------------------




	//------------------------------------------------------------------------------------------------------------------------






	// g_newTable 제거.
	/*for( int i=0; i<g_newTable.size(); ++i )
	{
		g_newTable[ i ]->clear();
		delete g_newTable[ i ];
	}*/
	vNT::iterator it = g_newTable.begin();
	for( ; it != g_newTable.end(); it++ )
	{		
		(*it).second->clear();
		delete (*it).second;
	}
	g_newTable.clear();


	//// Table 제거.
	//for( int i=0; i<vTables.size(); ++i )
	//	delete vTables[ i ];
	//vTables.clear();


}