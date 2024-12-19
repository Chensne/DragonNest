#pragma once

#define MINIMAP_SIZE 256 

#define MIN_MINIMAP_ZOOM 2.0f
#define MAX_MINIMAP_ZOOM 5.0f

class CDnMinimap : public CSingleton< CDnMinimap >, public CEtCustomRender
{
public:
	enum emICON_INDEX
	{
		indexNone = -1,
		indexPlayer = 0,
		indexBoss = 1,
		indexEnemy = 2,
		indexNPC = 3,					// Ư���� ����
		indexNPC_Weapon = 4,			// �������
		indexNPC_Parts = 5,				// ������
		indexNPC_Normal = 6,			// ��ȭ����
		indexNPC_Storage = 7,			// â��
		indexNPC_Market = 8,			// ���ΰŷ���
		indexNPC_Mail = 9,				// ������
		indexNPC_Guild = 10,			// ��� ����		
		indexNPC_WarriorSkill = 11,		// �����ų
		indexNPC_ArcherSkill = 12,		// ��ó��ų
		indexNPC_SoceressSkill = 13,	// �Ҽ�������ų
		indexNPC_ClericSkill = 14,		//Ŭ������ų

		indexParty = 15,

		indexQuestRecompense = 16,	// ����Ʈ ����
		indexQuestPalying = 17,		// ����Ʈ ����
		indexQuestAvailable = 18,	// ����Ʈ ����

		indexGateVillage = 19,
		indexGateDungeon = 20,
		indexGateWorldMap = 21,
		indexGateX = 22,
		indexGateUnknown = 23,

		indexSubQuestRecompense = 24,	// ���� ����Ʈ ����
		indexSubQuestPalying = 25,		// ���� ����Ʈ ����
		indexSubQuestAvailable = 26,	// ���� ����Ʈ ����

		indexGuild = 27,	// ����
		indexNPC_Plate = 28,			// ���� ����
		indexMail_New = 29,				// �� ���� ����

		indexRadioMsgHelp = 33,
		indexRadioMsgHeal = 34,
		indexRadioMsgFollow = 35,
		indexRadioMsgBack = 36,
		indexRadioMsgAttack = 37,
		indexRadioMsgStop = 38,

		indexEnemyCaptain = 39, // ������ ���� ���� ������
		indexAllyCaptain = 40,  // ������ �Ʊ� ���� ������

		indexReputationFavorGrant = 41,   // ȣ���� ����Ʈ ����
		indexReputationFavorNow = 42,     // ȣ���� ����Ʈ ����
		indexReputationFavorComplete = 43,// ȣ���� ����Ʈ ����

		indexPartyMemberArcher = 44,
		indexPartyMemberSoceress = 45,
		indexPartyMemberWarrior = 46,

		indexRoot = 47,
		indexCereals = 48,
		indexLeaf = 49,
		indexFungus = 50,
		indexFruit = 51,

		indexNPC_AdventurerBoard = 52,          // �Խ��� ���� NPC �̴ϸ� ������
		indexAdventurerBoard_Available = 53,	// ������ �Խ��� ����
		indexAdventurerBoard_Playing = 54,		// ������ �Խ��� ����
		indexAdventurerBoard_Recompense = 55,	// ������ �Խ��� ����

#ifdef PRE_ADD_LOWLEVEL_QUEST_HIDE
		// MinimapIcon.dds �������� ���� ���� ������ ���缭 �ε����� ����Ѵ�.  
		// ������ ������ ���� ���� ���� ���ϰ� ������ ���� ū ���� ����� �Ѵ�. 
		// ������ ��� �ݴ�� �Ǿ��־ ��������Ʈ�� �̴ϸ� �ϴܿ��� �������� ���ιٲ� ��µǴ� ������ �־���. 
		indexQuestGlobal_Available = 58,	// �۷ι� ����Ʈ
		indexQuestGlobal_Playing = 57,		//
		indexQuestGlobal_Recompense = 56,	//
#else
		indexQuestGlobal_Available = 56,	// �۷ι� ����Ʈ
		indexQuestGlobal_Playing = 57,		//
		indexQuestGlobal_Recompense = 58,	//
#endif

		indexEmptyArea = 59,	// ���� �����
		indexTraceQuest = 60,	// ����Ʈ ���� ������
		indexRevengeUser = 60,
		indexPartyMemberAcademic = 61, // ��ī����
		indexNPC_Vehicle = 62,   // ���û�
		indexNPC_Ticket = 63,          // Ƽ�� ���Ǿ�
		indexNPC_AcademicSkill = 64,   // ��ī���� ��ų���Ǿ�
		indexNPC_Warp = 65,
		indexPartyMemberKali = 66, // Į��
		indexNPC_KaliSkill = 67 ,  // Į�� ��ų NPC
		indexdNPC_PcBang = 68 ,    // PCBang Npc
		indexdNPC_Notice_PcBang = 69, // PcBang ������
		indexdNPC_Repair = 70, // ���� ���Ǿ�
		indexdNPC_Notice_Repair = 71, // ���� ��ǳ��
									  
		/// OK
		indexPartyMemberAssassin = 74, //
	
		indexPartyMemberLencea = 81, // 
		indexPartyMemberMachina = 83, // 

		//
		indexNPCPresent = 75,
		indexNPCWebToon = 78,
		indexNPC_LenceaSkill = 80,
		indexNPC_MachinaSkill = 86,
		indexNPCWarp = 88,
			
		indexAmount,
		indexGateAmount = 5,

	};

	struct SNPCQuestInfo
	{
		emICON_INDEX emQuestIndex;
		emICON_INDEX nIndexIcon;
		EtVector2 vNpcPos;
		int		nNpcID;
		bool bShow;
	};

	struct SPartyMemberPosInfo
	{
		EtVector3 pos;
		int classId;
		int team;
		emICON_INDEX eIconIndex;

		SPartyMemberPosInfo() : classId(-1), eIconIndex( emICON_INDEX::indexNone ), team(0) {}
		SPartyMemberPosInfo(const EtVector3& rhsPos, int rhsClassId, int rhsTeam )  { pos = rhsPos; classId = rhsClassId; team = rhsTeam; }
	};

	struct SOutInfo
	{
		emICON_INDEX eIconType;
		float fDegree;

		SOutInfo( emICON_INDEX eIcon , float fRange )
		{
			eIconType = eIcon;
			fDegree = fRange;
		}
	};

	protected:

		std::vector< DnActorHandle > m_vechEnemy;
		std::vector< DnActorHandle > m_vechFriend;
		std::vector< DnActorHandle > m_vechBoss;
		std::vector< DnActorHandle > m_vechNpc;
		std::vector< SNPCQuestInfo > m_vecNPCQuestInfo;
		std::map<int, std::pair<EtVector3,int>> m_mHarvestInfo;
		std::map<int, std::pair<EtVector3,int>> m_mEmptyInfo;
#if defined( PRE_ADD_REVENGE )
		std::vector< DnActorHandle > m_vecRevengeUser;
#endif
		std::vector< boost::tuple< DnActorHandle, int, LOCAL_TIME> > m_RadioActors;

		std::vector< SOutInfo > m_vecOutInfo;

		LOCAL_TIME m_LocalTime;
	
		EtTextureHandle m_hRenderTarget;
		EtMaterialHandle m_hMaterial;
		std::vector< SCustomParam > m_vecCustomParam;

		EtTextureHandle m_hMinimap;
		EtTextureHandle m_hEnemy;
		EtTextureHandle m_hOpacity;

		float m_fZoom;
		std::vector< STextureDiffuseVertex > m_vecVertex;
		std::vector< WORD > m_vecIndex;
		float m_fElapsedTime;
		float m_fTotalTime;
		bool m_bShow;
		std::string m_strMinimapFileName;

		bool m_IsAllPlayerShow;
		bool m_bShowFunctionalNPC;
		bool m_bShowOtherNPC;

public:
	CDnMinimap(void);
	virtual ~CDnMinimap(void);

protected:
	emICON_INDEX ConvertNpcTypeToIconIndex( int nJobType );
	emICON_INDEX ConvertStateMarkToQuestIconIndex( int nStateMark );
	emICON_INDEX ConvertPartyMarkToPartyIconIndex( int classId );
	emICON_INDEX ConvertPvPMark( emICON_INDEX original, DnActorHandle hActor );

	void ScanActor( const EtVector3 *pPlayerPos );
	void RenderGate( const EtVector3 *pPlayerPos );
	void RenderPlayer( bool bAnimateAlpha );
	void RenderNPC( std::vector<DnActorHandle> &vechActor, const EtVector3 *pPlayerPos );	
	void RenderActor( std::vector<DnActorHandle> &vechActor, const EtVector3 *pPlayerPos, emICON_INDEX iconIndex );
	void RenderPartyMember( const EtVector3 *pPlayerPos );
	void RenderHarvest( const EtVector3 *pPlayerPos );
	void RenderEmptyArea( const EtVector3 *pPlayerPos );
	void RenderTraceQuest( const EtVector3 *pPlayerPos );
#if defined( PRE_ADD_REVENGE )
	void RenderRevengeUser( const EtVector3 *pPlayerPos );
#endif	// #if defined( PRE_ADD_REVENGE )

	void GetAvailablePartyMember( std::vector<DnActorHandle> &ResultActorList , std::vector<SPartyMemberPosInfo> &ResultPosList );
	bool CheckPvPCondition( DnActorHandle hActor );
	void GetLinePoint( const EtVector3 *pPlayerPos, EtVector2 *pLinePoint, float fDegree, float fRatio );
	bool PointInLine( const EtVector2 *pTargetPosition, const EtVector2 *pLineBegin, const EtVector2 *pLineEnd );

	void InitVertexIndex( int nCount );
	void SetVertexIndex( int nIndex, const EtVector3 *pScreenPos, emICON_INDEX indexIcon, bool bRotate, BYTE btAlpha = 255, float fScale = 1.f );

	void RotateIcon( EtVector3 *pPos, const EtVector3 *pScreenPos );

	bool IsPVPGame();

public:
	void SetTextures( const char *pMinimap );
	void Zoom( float fZoomValue );
	std::string& GetMinimapName() { return m_strMinimapFileName; }

public:
	void Initialize();
	void InitializeNPC( int nMapIndex );
	void ReInitializeNPC( int nMapIndex );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void Show( bool bShow ) { m_bShow = bShow; }

	void InsertNpc( DnActorHandle hActor );
	void RemoveNpc( DnActorHandle hActor );

	void ShowNpcQuestInfo( int nNpcID, bool bShow );

	EtTextureHandle GetTexture() { return m_hRenderTarget; }

	bool IsMinimapOut(EtVector3 vAxisPos,EtVector3 vTartGetPos,float fMaxSizeHalf,float fZoom);
	float GetDistanceDegree(EtVector3 vAxis,EtVector3 vTarget);

	void InsertHarvest( int nAreaIndex, EtVector3 etVector3, char * szHarvestIconName );
	void DeleteHarvest( int nAreaIndex );
	void InsertEmptyArea( int nAreaIndex, EtVector3 etVector3 );
	void DeleteEmptyArea( int nAreaIndex );
	void AllDeleteEmptyArea();

	void InsertOutInfo( const EtVector3 *pPlayerPos , SPartyMemberPosInfo *pMemberInfo );
	std::vector<SOutInfo> &GetMiniMapOutInfo() { return m_vecOutInfo; }

	virtual void RenderCustom( float fElapsedTime );
	void SetAllPlayerShow(bool bSet ){m_IsAllPlayerShow =  bSet;};

	void ShowFunctionalNPC( bool bShow ) {m_bShowFunctionalNPC = bShow;}
	void ShowOtherNPC( bool bShow ) { m_bShowOtherNPC = bShow;}

	void SetRadioMark( DnActorHandle hActor, int nIconIndex );
};

#define GetMiniMap()		CDnMinimap::GetInstance()