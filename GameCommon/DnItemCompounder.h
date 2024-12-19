#pragma once

//------------------------------------------------------------------------------
/**
    Note 한기:

	테이블에서 데이터를 로드해서 쥐고 있다가 아이템 조합/문장보옥 생성 요청이 오면
	적절한 결과를 리턴해주는 클래스입니다.

	무언가를 새로 생성하거나 데이터를 읽어놓고 변경을 가하지 않기 때문에
	멀티스레드 고려가 되어있지 않습니다.
*/
//------------------------------------------------------------------------------

const DWORD NUM_MAX_NEED_ITEM = 10;
const DWORD NUM_MAX_PLATE_COMPOUND_VARI = 5;
const DWORD NUM_MAX_RESULT_ITEM = 3;


class CDnItemCompounder
{
public:
	enum ERROR_CODE
	{
		E_NONE = 0,					// 에러 아님
		E_NOT_ENOUGH_MONEY,			// 돈이 모자름
		E_NOT_ENOUGH_ITEM,			// 재료 아이템 부족
		E_NOT_MATCH_SLOT,				// 슬롯에 맞지 않는 아이템임
		E_NOT_ENOUGH_ITEM_COUNT,		// 슬롯엔 맞았으나 갯수가 모자람
		E_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND,		// 플레이트가 수행하는 아이템 조합 리스트에 입력받은 아이템 조합 인덱스 없음.

		E_INVALID_CALL,				// 잘못된 호출
	};

	enum RESULT_CODE
	{
		R_SUCCESS,					// 조합 성공
		R_FAIL,						// 조합 실패
		R_POSSIBILITY_FAIL,			// 확률 실패.
		R_ERROR,					// 요청 에러
	};

	// 조합 요청
	struct S_ITEM_SETTING_INFO
	{
		int iCompoundTableID;
#ifndef _UNIT_TEST		// 유닛테스트 프로젝트에선 4바이트씩 구조체가 밀리는듯..
		INT64 iHasMoney;
#else
		int iHasMoney;
#endif
		int aiItemID[ NUM_MAX_NEED_ITEM ];			// 재료 아이템 슬롯에 박아놓은 아이템 ID
		int aiItemCount[ NUM_MAX_NEED_ITEM ];		// 재료 아이템 슬롯에 박아놓은 아이템 갯수.
		INT64 iDiscountedCost;						// 원격 제작 아이템등을 써서 할인된 수수료인 경우 이 값으로 대체.
	
		S_ITEM_SETTING_INFO( void )
		{
			ZeroMemory( this, sizeof(S_ITEM_SETTING_INFO) );
		}
	};

	// 조합 결과물 
	struct S_OUTPUT
	{
		vector<int> vlItemID;
		vector<int> vlItemCount;
		char cItemOptionIndex;
#ifndef _UNIT_TEST		// 유닛테스트 프로젝트에선 4바이트씩 구조체가 밀리는듯..
		INT64 iCost;
#else
		int iCost;
#endif
		RESULT_CODE eResultCode;				// 결과 코드
		ERROR_CODE eErrorCode;					// 에러 발생 시에 에러코드를 알려줌
		float fTimeRequired;

		S_OUTPUT( void ) : iCost( 0 ), eResultCode( R_SUCCESS ), 
			eErrorCode( E_NONE ), fTimeRequired( 0.0f )
			, cItemOptionIndex(0)
		{

		}
	};

#ifdef _UNIT_TEST
	struct S_PLATE_INFO
#else
	struct S_PLATE_INFO : public TBoostMemoryPool< S_PLATE_INFO >
#endif
	{
		int iPlateItemID;
		//string strBGTextureFileName;
		int aiCompoundTableID[ NUM_MAX_PLATE_COMPOUND_VARI ];
		int aiCompoundTableNum[ NUM_MAX_PLATE_COMPOUND_VARI ];

		S_PLATE_INFO( void )
		{
			ZeroMemory( this, sizeof(S_PLATE_INFO) );
		}

	};


private:
#ifdef _UNIT_TEST
	struct S_ITEM_COMPOUND_INFO
#else
	struct S_ITEM_COMPOUND_INFO : public TBoostMemoryPool< S_ITEM_COMPOUND_INFO >
#endif
	{
		int iCompoundTableID;
		int iNameStringID;
		int iDescStringID;
		float fSuccessPossibility;
#ifndef _UNIT_TEST		// 유닛테스트 프로젝트에선 4바이트씩 구조체가 밀리는듯..
		INT64 iCost;
#else
		int iCost;
#endif
		char cSuccessItemOptionIndex;

		float fTimeRequired;									// 조합 시 소요 시간

		int aiSuccessTypeOrCount[ NUM_MAX_RESULT_ITEM ];		// 0이면 랜덤. 드랍 테이블의 인덱스가 됨
		int aiSuccessItemID[ NUM_MAX_RESULT_ITEM ];				// SuccessType 이 0 보다크면 결과물 아이템 갯수
		int aiFailTypeOrCount[ NUM_MAX_RESULT_ITEM ];			// SuccessType 과 마찬가지임
		int aiFailItemID[ NUM_MAX_RESULT_ITEM ];				// SuccessItemID 와 마찬가지..

		int aiSlotItemID[ NUM_MAX_NEED_ITEM ];					// 재료 아이템 슬롯하나에 필요한 아이템 ID
		int aiSlotItemCount[ NUM_MAX_NEED_ITEM ];				// 재료 아이템 슬롯에 필요한 아이템 갯수
		bool bResultItemIsNeedItem;
		bool abResultItemIsNeedItem[ NUM_MAX_NEED_ITEM ];

		int iCompoundPreliminaryID;	//장비재료의 옵션번호를 제한하는 필드

		S_ITEM_COMPOUND_INFO( void )
		{
			ZeroMemory( this, sizeof(S_ITEM_COMPOUND_INFO) );
		}
	};

	//template< typename T >
	//struct FindByItemID : public unary_function<const T*, bool>
	//{
	//	int m_iItemIDToFind;

	//	FindByItemID( int iItemIDToFind ) : m_iItemIDToFind( iItemIDToFind ) {};
	//	bool operator() ( const T* pRhs )
	//	{
	//		return (pRhs->iItemID == m_iItemIDToFind);
	//	}
	//};
public:
	// 외부에서 필요한 조합 정보
	struct S_COMPOUND_INFO_EXTERN
	{
		int iCompoundTableID;
		int aiSuccessTypeOrCount[ NUM_MAX_RESULT_ITEM ];		// 0이면 랜덤. 드랍 테이블의 인덱스가 됨
		int aiSuccessItemID[ NUM_MAX_RESULT_ITEM ];				// SuccessType 이 0 보다크면 결과물 아이템 갯수
		int aiItemID[ NUM_MAX_NEED_ITEM ];			// 재료 아이템 슬롯에 박아놓은 아이템 ID
		int aiItemCount[ NUM_MAX_NEED_ITEM ];		// 재료 아이템 슬롯에 박아놓은 아이템 갯수.
#ifndef _UNIT_TEST		// 유닛테스트 프로젝트에선 4바이트씩 구조체가 밀리는듯..
		INT64 iCost;								// 비용
#else
		int iCost;
#endif
		float fSuccessPossibility;					// 성공확률
		float fTimeRequired;						// 소요시간
		bool bResultItemIsNeedItem;
		bool abResultItemIsNeedItem[ NUM_MAX_NEED_ITEM ];

		int iCompoundPreliminaryID;	//장비재료의 옵션번호를 제한하는 필드

		S_COMPOUND_INFO_EXTERN( void )
		{
			ZeroMemory( this, sizeof(S_COMPOUND_INFO_EXTERN) );
		};
	};

private:
	// 조합에 필요한 모든 정보를 처음에 읽어서 다 갖고 있도록 한다..
	//vector<S_ITEM_COMPOUND_INFO*> m_vlpCompoundInfo;
	map<int, S_ITEM_COMPOUND_INFO*> m_mapCompoundInfo;
	vector<S_PLATE_INFO*> m_vlpPlateInfo;
	map<int, S_PLATE_INFO*> m_mapPlateInfoByItemID;
	typedef std::map<int, S_ITEM_COMPOUND_INFO*> TMapItemCompoundInfo;
#ifdef _CLIENT
	#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
	std::set<int> m_SuccessIDsAsCompoundResultItem;	// 파츠 아이템 툴팁에서 아이템 제작 가능 여부를 판별하기 위한 캐시
	#endif
#endif

	int m_iLastError;
	bool m_bCheckPossibility;

protected:
	void _ResetOutputInfo( S_OUTPUT* pOutput );
#if defined( _GAMESERVER )
	void _Compound( CMultiRoom* pRoom, S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult );
#else
	void _Compound( S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult );
#endif // #if defined( _GAMESERVER )
	//int _ValidateCompound( S_PLATE_INFO* pPlateInfo, const multiset<int>& setJewelItemID );

public:
	CDnItemCompounder(void);
	~CDnItemCompounder(void);

	// 테이블 별로 초기화
#if defined (_WORK) && defined (_SERVER)
	bool InitializeTable( bool bReLoad );
#else		//#if defined (_WORK) && defined (_SERVER)
	bool InitializeTable( void );
#endif		//#if defined (_WORK) && defined (_SERVER)

	// 업그레이드 성공시 새 아이템 아이디 리턴
	//int UpgradeJewel( int iJewelItemID, int iNumJewel, /*OUT*/ int* pCost, /*OUT*/ int* pNumUse );

	// UI 에 표시할 때 필요한 정보들.
	void GetCompoundInfo( int iCompoundTableID, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo );
	int GetEmblemCompoundInfoCountByPlate( int iPlateTableID );
	void GetEmblemCompoundInfo( int iPlateTableID, int iCompoundTableID, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo );
	void GetEmblemCompoundInfoByIndex( int iPlateTableID, int iIndex, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo );
	const CDnItemCompounder::S_PLATE_INFO* GetPlateInfoByItemID( int iPlateTableID );

	// 조합 전에 검증 하는 함수
	void ValidateEmblemCompound( int iPlateTableID, S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult );
	void ValidateCompound( S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult );

	// 조합 함수들.. 조합 실패시 0 리턴
#if defined( _GAMESERVER )
	void EmblemCompound( CMultiRoom* pRoom, int iPlateItemID, S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult );
	void Compound( CMultiRoom* pRoom, /*IN*/ S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult );
#else
	void EmblemCompound( int iPlateItemID, S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult );
	void Compound( /*IN*/ S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult );
#endif // #if defined( _GAMESERVER )

	int GetLastError( void ) { return m_iLastError; };

	void CheckPossibility( bool bCheckPossibility ) { m_bCheckPossibility = bCheckPossibility; };

#ifdef _CLIENT
	#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
	bool IsEnableCompoundItem(int itemId) const;
	#endif
#endif
};
