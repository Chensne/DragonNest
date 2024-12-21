#pragma once


const DWORD NUM_MAX_PLATE_HOLE = 10;
const DWORD NUM_MAX_JEWEL_COMPOUND = 10;

class CDnEmblemFactory
{
public:

	struct S_EMBLEM_COMPOUND_INFO: public TBoostMemoryPool< S_EMBLEM_COMPOUND_INFO >
	{
		//int aiHoleItemID[ NUM_MAX_PLATE_HOLE ];
		multiset<int> setHoleItemID;
		bool abShowHoleInfo[ NUM_MAX_PLATE_HOLE ];

		int iResultItemID;
		bool bAllowShowResult;

		S_EMBLEM_COMPOUND_INFO( void ) : iResultItemID( 0 ), bAllowShowResult( false )
		{
			ZeroMemory( abShowHoleInfo, sizeof(abShowHoleInfo) );
		};
	};

	struct S_PLATE_INFO:public TBoostMemoryPool< S_PLATE_INFO >
	{
		int iItemID;
		string strBGTextureFileName;
		int iSlotTypeIndex;
		int iNumSlot;
		int aiCompoundTableIndex[ NUM_MAX_JEWEL_COMPOUND ];
		bool bLostJewelWhenFailed;
		bool bLostPlateWhenFailed;
		int iCompositionFailProb;
		bool bAllowShowProb;
		bool bLostJewelWhenFailedProb;
		bool bLostPlateWhenFailedProb;
		int iCost;

		S_PLATE_INFO( void ) : iItemID( 0 ), iSlotTypeIndex( -1 ), iNumSlot( 0 ), bLostJewelWhenFailed( false ), bLostPlateWhenFailed( false ), iCompositionFailProb( 0 ),
							   bAllowShowProb( false ), bLostJewelWhenFailedProb( false ), bLostPlateWhenFailedProb( false ), iCost( 0 )
		{
			ZeroMemory( aiCompoundTableIndex, sizeof(aiCompoundTableIndex) );
		}

	};

	struct S_JEWEL_INFO:public TBoostMemoryPool< S_JEWEL_INFO >
	{
		int iItemID;
		int iGrade;
		int iColor;
		int iNeedUpgradeCount;
		int iNextGradeItemIndex;
		int iCost;

		S_JEWEL_INFO( void ) : iItemID( 0 ), iGrade( 0 ), iColor( 0 ), 
			iNeedUpgradeCount( 0 ), iNextGradeItemIndex( 0 ),
			iCost( 0 )
		{};
	};

private:
	template< typename T >
	struct FindByItemID : public unary_function<const T*, bool>
	{
		int m_iItemIDToFind;

		FindByItemID( int iItemIDToFind ) : m_iItemIDToFind( iItemIDToFind ) {};
		bool operator() ( const T* pRhs )
		{
			return (pRhs->iItemID == m_iItemIDToFind);
		}
	};

	// 조합에 필요한 모든 정보를 처음에 읽어서 다 갖고 있도록 한다..
	vector<S_EMBLEM_COMPOUND_INFO*> m_vlpCompoundInfo;
	vector<S_PLATE_INFO*> m_vlpPlateInfo;
	vector<S_JEWEL_INFO*> m_vlpJewelInfo;

protected:
	int _ValidateUpgradeJewel( S_JEWEL_INFO* pJewelInfo, int iNumJewel );
	int _ValidateCompound( S_PLATE_INFO* pPlateInfo, const multiset<int>& setJewelItemID );

public:
	CDnEmblemFactory(void);
	~CDnEmblemFactory(void);

	// 테이블 별로 초기화
	bool InitializeTable( void );

	// 정보 조회
	int GetNumEmblemCompoundInfo( void ) const { return (int)m_vlpCompoundInfo.size(); };
	const S_EMBLEM_COMPOUND_INFO* GetEmblemCompoundInfo( int iIndex ) const;
	int GetNumPlateInfo( void ) const { return (int)m_vlpPlateInfo.size(); };
	const S_PLATE_INFO* GetPlateInfoByIndex( int iIndex ) const;
	const S_PLATE_INFO* GetPlateInfoByItemID( int iItemID ) const;
	int GetNumJewelInfo( void ) const { return (int)m_vlpJewelInfo.size(); };
	const S_JEWEL_INFO* GetJewelInfoByIndex( int iIndex ) const;
	const S_JEWEL_INFO* GetJewelInfoByItemID( int iItemID ) const;

	// 업그레이드 성공시 새 아이템 아이디 리턴
	int UpgradeJewel( int iJewelItemID, int iNumJewel, /*OUT*/ int* pCost, /*OUT*/ int* pNumUse );

	// 조합 실패시 0 리턴
	int Compound( int iPlateItemID, const multiset<int>& setJewelItemID, /*OUT*/int* pCost  );
};
