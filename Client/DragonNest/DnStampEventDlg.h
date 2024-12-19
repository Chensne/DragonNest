#pragma once

#ifdef PRE_ADD_STAMPSYSTEM

#include "DnCustomDlg.h"

struct STableStampData
{
	INT64 stampStartDay;
	int strID;
	int strDescriptID;
	CDnItem * items[2];
	int type;
	int mapID;
	int npcID;
	int wDay;
	std::vector<int> arrIndex;
	int nMissionGroupID;

	STableStampData() : stampStartDay(0), strID(0), strDescriptID(0), type(0), mapID(0), npcID(0), wDay(0), nMissionGroupID(0){
		items[0] = items[1] = NULL;
	}
	/*STableStampData( STableStampData & a ){
		strID = a.strID;
		strDescriptID = a.strDescriptID;
		items[ 0 ] = a.items[ 0 ];
		items[ 1 ] = a.items[ 1 ];
		type = a.type;
		mapID = a.mapID;
		npcID = a.npcID;
	}*/
	~STableStampData(){
		arrIndex.clear();
		for( int i=0; i<2; ++i )
			SAFE_DELETE( items[ i ] );
	}
};

class CDnStampEventDlg : public CEtUIDialog
{

protected:
	CEtUIListBoxEx *m_pListBoxEx;
#define AWEEK 7
	CEtUIStatic * m_pStaticWeekDay[AWEEK];

	struct SStampData
	{
		int weekDay;
		INT64 time;
		bool * bFlag;
		int arrSize;

		SStampData() : weekDay(-1), time(-1), bFlag(NULL), arrSize(0){}
		~SStampData(){
			if( bFlag )
				delete[] bFlag;
		}
		void Allocate( int size, bool * b ){
			arrSize = size;
			bFlag = new bool[ arrSize ];
			memcpy( bFlag, b, arrSize );
		}
		void Release(){
			if( bFlag )
				delete[] bFlag;
		}
		void Init(){
			weekDay = -1;
			time = -1;
			if( bFlag ){
				memset( bFlag, 0, sizeof(bool)*arrSize );
			}
		}
	};
	SStampData m_StampData;

public:
	CDnStampEventDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStampEventDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );


private:

	// 메일ID의 아이템반환.
	CDnItem * GetItem( int mailID );

public:

	// 도전목록 구성.
	void BuildList( INT64 startTime, int weekDay=-1, bool * bComplete=NULL );

	// 전체스탬프찍어주기.
	void SetAllStamp();

	// 스탬프찍기.
	void SetStamp( int idx, int wDay );

	// 요일변경.
	void ChangeWeekDay( int wDay );

	// 도전과제종료.
	void EndStamp();

	// 해당일의 도전과제 완료여부.
	// - 0(완료한게없음),  1(한개이상완료),  2(모두완료)
	int IsComplete( int wDay=0 );

	// 도전과제 존재여부.
	bool IsExistStamp();

	int GetToday();

};


#endif // PRE_ADD_STAMPSYSTEM