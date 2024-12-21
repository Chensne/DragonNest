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

	// ����ID�� �����۹�ȯ.
	CDnItem * GetItem( int mailID );

public:

	// ������� ����.
	void BuildList( INT64 startTime, int weekDay=-1, bool * bComplete=NULL );

	// ��ü����������ֱ�.
	void SetAllStamp();

	// ���������.
	void SetStamp( int idx, int wDay );

	// ���Ϻ���.
	void ChangeWeekDay( int wDay );

	// ������������.
	void EndStamp();

	// �ش����� �������� �ϷῩ��.
	// - 0(�Ϸ��ѰԾ���),  1(�Ѱ��̻�Ϸ�),  2(��οϷ�)
	int IsComplete( int wDay=0 );

	// �������� ���翩��.
	bool IsExistStamp();

	int GetToday();

};


#endif // PRE_ADD_STAMPSYSTEM