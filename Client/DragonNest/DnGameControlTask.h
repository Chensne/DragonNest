#pragma once

#include "Task.h"
#include "DnTableDB.h"

class CDnGameControlTask : public CTask, public CSingleton<CDnGameControlTask>
{
public:
	struct SKeyData
	{
		int nItemID;
		int nControlType;
		int nCategoryType;
		int nLowCategorySection;
		int nLowCategoryType;
		int nLowCategoryStringID;
		int nDefaultKey_first;
		int nDefaultKey_second;
		bool bConversion;
		bool bShow;

		SKeyData()
			: nItemID( 0 )
			, nControlType( 0 )
			, nCategoryType( 0 )
			, nLowCategorySection( 0 )
			, nLowCategoryType( 0 )
			, nLowCategoryStringID( 0 )
			, nDefaultKey_first( 0 )
			, nDefaultKey_second( 0 )
			, bConversion( false )
			, bShow( false )
		{}
	};

	struct SChangeValue
	{
		int nLowCategorySection;
		int nLowCategoryType;
		BYTE cFirstKey;
		BYTE cSecondKey;

		SChangeValue()
			: nLowCategorySection( 0 )
			, nLowCategoryType( 0 )
			, cFirstKey( 0 )
			, cSecondKey( 0 )
		{}

		void Clear()
		{
			nLowCategorySection = 0;
			nLowCategoryType = 0;
			cFirstKey = 0;
			cSecondKey = 0;
		}
	};

	enum eControlType
	{
		eControlType_Keyboard = 0,
		eControlType_XBox,
		eControlType_Hangame,
		eControlType_GAMMAC,
		eControlType_CustomPad,

		eControlType_Max
	};

	enum eCategoryType
	{
		eCategoryType_Motion = 0,
		eCategoryType_QuickSlot,
		eCategoryType_Interface,

		eCategoryType_Max
	};

	enum eLowCategorySection
	{
		eLowCategorySection_Motion = 0,
		eLowCategorySection_Interface,

		eLowCategorySection_Max
	};

	BYTE m_WrappingData[WrappingKeyIndex_Amount];
	BYTE m_WrappingDataAssist[WrappingKeyIndex_Amount];
	BYTE m_UI_WrappingData[UIWrappingKeyIndex_Amount];
	BYTE m_UI_WrappingDataAssist[UIWrappingKeyIndex_Amount];

	BYTE m_WrappingData_Pad[WrappingKeyIndex_Amount];
	BYTE m_WrappingDataAssist_Pad[WrappingKeyIndex_Amount];
	BYTE m_UI_WrappingData_Pad[UIWrappingKeyIndex_Amount];
	BYTE m_UI_WrappingDataAssist_Pad[UIWrappingKeyIndex_Amount];

	SChangeValue m_ChangeValue;

public :
	CDnGameControlTask();
	virtual ~CDnGameControlTask();

	bool Initialize();
	DNTableFileFormat * GetControlTable();

	void GetDataByType( const int eControlIndex, const int eCategoryIndex, std::vector<SKeyData *> & vKeyData );
	void GetControlTypeData( std::map<int, int> & mControlType );

	void SetWrappingDataByControlType( const int eControlIndex );
	std::pair<int, int> GetWrappingData( const int nControlType, const int nCategorySection, const int nCategoryType );

	bool RequestChangeKey( const SChangeValue & sChangeValue, std::wstring & wszControlNameString );
	void ChangeKey();
	bool RequestChangePad( const SChangeValue & sChangeValue, std::wstring & wszControlNameString );
	void ChangePad();

	void ChangeValue( const int nControlType );
	bool IsChangeValue( const int nControlType );
	void DefaultValue( const int nControlType );
	bool IsDefaultValue( const int nControlType );

	void ChangePadType( const int nControlType );

protected:
	void InitializeControlTypeString();
	void InitializeFixedControl();
	void ResetWrappingData();

protected:
	std::vector<SKeyData *> m_vKeyData;
	std::map<int, int>	m_mControlType;

	int m_ControlTypeStrings[eControlType_Max];
};

#define GetGameControlTask()		CDnGameControlTask::GetInstance()