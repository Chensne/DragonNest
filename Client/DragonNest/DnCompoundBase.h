#pragma once

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

class CDnCompoundBase : public CEtUIDialog
{
public:
	CDnCompoundBase( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnCompoundBase();

	enum eCompoundTabType
	{
		eSetItem = 0,
		eSuffix,
		eArtfice,
		eChandlery,

		eMaxCompoundTypeCount,

	};

	enum 
	{
		COMPOUND_SETITEM_DIALOG ,
		COMPOUND_SUFFIX_DIALOG,
		COMPOUND_ARTFICE_DIALOG,
		COMPOUND_CHANDLERY_DIALOG,
		MAX_COMPOUND_DIALOG_COUNT,
	};

	enum{
		COMPOUND_MESSAGE_DIALOG = 18,
		COMPOUND_ITEMLIST_DIALOG = 19,
	};

	enum
	{
		WARRIOR = 0,
		ARCHER,
		SOCERESS,
		CLERIC,
		ACADEMIC,
		Kali,
        ASSASSIN,
		LENCEA,
		MACHINA,

		PLAYER_MAX
	};

	enum 
	{
		eRARE = 2,
		eEPIC = 3,
		eUNIQUE = 4,
		eLEGEND = 5,

		eMAXRANK = 5, 
	};

	enum 
	{
		MAXLEVEL = 8,
		MAXSUFFIXSIZE = 14,
		MAXPVPSUFFIXSIZE = 6,
	};

	struct ItemGroupStruct
	{
		int nGroupTableID;				// 탭 분류 
		int nCompoundGroupID;			
		int nCompoundMainCategory;		// 현재 메인탭 분류 ( 세트, 접미사, 기교, 잡화 ) 
		int nCompoundGroupType;			// 이걸로 장비 파츠의 분류 ( 주무기1, 주무기2, 상의, 하의... ) 를 한다.
		int	nPartsID;					// 고유 파츠의 ID ( 대검, 도끼..etc )<< UIString 의 키값 
		int nItemID;
		int nLevel;
		int nRank;
		int nJob;
		int nActivateLevel;
		bool bIsPreview;
		int nPreviewDescUIStringID;

		int nCompoundSetItemID;			// 세트 아이템의 고유 식별 ID << UIString의 키 값 

		int	nCompoundSuffixID;			// 접미사 ID 
		int nSuffixGrade;				// 접미사 단계 

		int nCompooudSkillID;			// 스킬 ID 

		std::vector<int> nVecCompoundList;
		std::vector<int> nVecResultCompoundList;
	};

	struct SetItemStruct
	{
		int nSetIndex;
		std::vector< CDnCompoundBase::ItemGroupStruct* > m_SetItemList;	

		SetItemStruct( int nIndex ) : nSetIndex( nIndex ) {}
		~SetItemStruct(){}

		void Add( CDnCompoundBase::ItemGroupStruct* pGroupData )
		{	
			m_SetItemList.push_back( pGroupData );
		}

	};

	struct RankStruct
	{
		int nRare;
		int nEpic;
		int nUnique;
		int nLegend;
	};

	struct PartsItemStruct
	{
		int nJob;
		std::multimap<int, int> mutimapEquipList;

		PartsItemStruct( int Job ) : nJob( Job ){}

		void Reset()
		{
			nJob = -1;
			mutimapEquipList.clear();
		}
	};

	struct FilterTypeStruct
	{
		int nID;
		int nUniqueID;
		int nLevel;
		int nRank;
	};

public:

	virtual void SetCompoundTable( int nType ) = 0;
	virtual void UpdateGroupList( std::vector< CDnCompoundBase::ItemGroupStruct* >& pVecResultList ) = 0;
	virtual void SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData ) = 0;
	virtual void Reset() = 0;
};

#endif 