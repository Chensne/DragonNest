using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Xml;

namespace SvrStructWizard
{
    public partial class Form_SvrStructWiz : Form
    {
        public SvrStruct ServerStruct;
        public ChannelInfo ChannelStruct;
        public List<ServerInfo> SIDList;
        public List<int> WorldIDList;
        public List<ServerInfo> LoginServer;
        public List<ServerInfo> MultiWorldServer;
        public List<ServerInfo> TargetIPServer;
        public List<ServerInfo> ServerNumber;
        public List<ServerIP> NetLauncherIP;
        public List<ServerIP> ChannelIP;
        public List<UIEnviornment> UIList;
        public List<ReportContext> ErrorList;
        public List<ReportContext> WaringList;
        public List<MapSet> DefaultMapSetList;

        public bool IsMakeVillage;
        public bool IsAddTabs;
        public int SelectServerIndex;
        public LoadType loadType;


        static List<string> BonusTypeList;
        static List<string> MapNameList;
        static List<string> AttributeNameList;

        public SourceGrid2.VisualModels.Common LightGrayModel;
        public SourceGrid2.VisualModels.Common WhiteModel;
        public SourceGrid2.VisualModels.Common OrangeRedModel;
        public SourceGrid2.VisualModels.Common YellowModel;
        public SourceGrid2.VisualModels.Common AntiqueWhiteModel;
        public SourceGrid2.VisualModels.Common SelectModel;

        public enum LoadType
        {
            None = 0x00,
            Server = 0x01,
            Channel = 0x02,
        }
        
        public enum TPI
        {
            Setup = 0,
            Default,
            DataBase,
            NetLauncher,
            Detail,
            Channel,
            Village,
            VillageView,
            Extra,
            Report,
            Import, // Max
        }
        
        public Form_SvrStructWiz()
        {
            
            InitializeComponent();

            ServerStruct = new SvrStruct();
            ChannelStruct = new ChannelInfo();
            SIDList = new List<ServerInfo>();
            WorldIDList = new List<int>();
            LoginServer = new List<ServerInfo>();
            MultiWorldServer = new List<ServerInfo>();
            TargetIPServer = new List<ServerInfo>();
            ServerNumber = new List<ServerInfo>();
            NetLauncherIP = new List<ServerIP>();
            ChannelIP = new List<ServerIP>();
            UIList = new List<UIEnviornment>();
            ErrorList = new List<ReportContext>();
            WaringList = new List<ReportContext>();
            DefaultMapSetList = new List<MapSet>();

            BonusTypeList = new List<string>();
            MapNameList = new List<string>();
            AttributeNameList = new List<string>();

            IsMakeVillage = false;
            IsAddTabs = false;
            SelectServerIndex = -1;
            loadType = LoadType.None;

            
            LightGrayModel = new SourceGrid2.VisualModels.Common();
            LightGrayModel.BackColor = Color.LightGray;
            
            WhiteModel = new SourceGrid2.VisualModels.Common();
            WhiteModel.BackColor = Color.White;
            

            OrangeRedModel = new SourceGrid2.VisualModels.Common();
            OrangeRedModel.BackColor = Color.OrangeRed;
            OrangeRedModel.FocusBackColor = Color.OrangeRed;

            YellowModel = new SourceGrid2.VisualModels.Common();
            YellowModel.BackColor = Color.Yellow;
            YellowModel.FocusBackColor = Color.Yellow;


            AntiqueWhiteModel = new SourceGrid2.VisualModels.Common();
            AntiqueWhiteModel.BackColor = Color.AntiqueWhite;

            SelectModel = new SourceGrid2.VisualModels.Common();
            SelectModel.BackColor = Color.LightSeaGreen;

            //////////////////////////////////////////////////////////////////////////

            HideTabPage();

            SetUIEnviornment();

            SetDefaultBousType();

            SetDefaultMapSet();

            //////////////////////////////////////////////////////////////////////////
            // grid_ServiceMng
            grid_ServiceMng.Redim(2, 2);
            grid_ServiceMng.FixedRows = 1;

            SetTextGridColumn(grid_ServiceMng, 0, "IP");
            SetTextGridColumn(grid_ServiceMng, 1, "Port");

            SetTextGridCell(grid_ServiceMng, 1, 0, ServerStruct.ServiceMng.IP);
            grid_ServiceMng[1, 0].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_ServiceMng, 1, 1, ServerStruct.ServiceMng.Port);

            grid_ServiceMng.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            SetDefServerInfo();

            SetFatigueInfo();


            //////////////////////////////////////////////////////////////////////////
            //grid_Excutefile

            grid_Excutefile.Redim(7, 2);
            grid_Excutefile.FixedColumns = 1;

            SetTextGridRowHeader(grid_Excutefile, 0, "LoginServer");
            SetTextGridCell(grid_Excutefile, 0, 1, "DNLoginserverH64.exe");

            SetTextGridRowHeader(grid_Excutefile, 1, "LogServer");
            SetTextGridCell(grid_Excutefile, 1, 1, "DNLogServer64.exe");

            SetTextGridRowHeader(grid_Excutefile, 2, "MasterServer");
            SetTextGridCell(grid_Excutefile, 2, 1, "DNMasterServer64.exe");

            SetTextGridRowHeader(grid_Excutefile, 3, "CashServer");
            SetTextGridCell(grid_Excutefile, 3, 1, "DNCashServer64.exe");

            SetTextGridRowHeader(grid_Excutefile, 4, "DBServer");
            SetTextGridCell(grid_Excutefile, 4, 1, "DNDBServer64.exe");

            SetTextGridRowHeader(grid_Excutefile, 5, "VillageServer");
            SetTextGridCell(grid_Excutefile, 5, 1, "DNVillageServerRH64.exe");

            SetTextGridRowHeader(grid_Excutefile, 6, "GameServer");
            SetTextGridCell(grid_Excutefile, 6, 1, "DNGameServerH64.exe");

            grid_Excutefile.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_memdb
            grid_memdb.Redim(1, 5);
            grid_memdb.FixedRows = 1;

            SetTextGridColumn(grid_memdb, 0, "No");
            SetTextGridColumn(grid_memdb, 1, "IP");
            SetTextGridColumn(grid_memdb, 2, "Port");
            SetTextGridColumn(grid_memdb, 3, "DBID");
            SetTextGridColumn(grid_memdb, 4, "DBName");

            grid_memdb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_worlddb
            grid_worlddb.Redim(1, 6);
            grid_worlddb.FixedRows = 1;

            SetTextGridColumn(grid_worlddb, 0, "No");
            SetTextGridColumn(grid_worlddb, 1, "WorldID");
            SetTextGridColumn(grid_worlddb, 2, "IP");
            SetTextGridColumn(grid_worlddb, 3, "Port");
            SetTextGridColumn(grid_worlddb, 4, "DBID");
            SetTextGridColumn(grid_worlddb, 5, "DBName");

            grid_worlddb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_logdb
            grid_logdb.Redim(1, 5);
            grid_logdb.FixedRows = 1;

            SetTextGridColumn(grid_logdb, 0, "No");
            SetTextGridColumn(grid_logdb, 1, "IP");
            SetTextGridColumn(grid_logdb, 2, "Port");
            SetTextGridColumn(grid_logdb, 3, "DBID");
            SetTextGridColumn(grid_logdb, 4, "DBName");

            grid_logdb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_gsmdb
            grid_gsmdb.Redim(1, 4);
            grid_gsmdb.FixedRows = 1;

            SetTextGridColumn(grid_gsmdb, 0, "No");
            SetTextGridColumn(grid_gsmdb, 1, "IP");
            SetTextGridColumn(grid_gsmdb, 2, "Port");
            SetTextGridColumn(grid_gsmdb, 3, "DBName");

            grid_gsmdb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_stagingdb
            grid_stagingdb.Redim(1, 4);
            grid_stagingdb.FixedRows = 1;

            SetTextGridColumn(grid_stagingdb, 0, "No");
            SetTextGridColumn(grid_stagingdb, 1, "IP");
            SetTextGridColumn(grid_stagingdb, 2, "Port");
            SetTextGridColumn(grid_stagingdb, 3, "DBName");

            grid_stagingdb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_timosedb
            grid_timosedb.Redim(1, 4);
            grid_timosedb.FixedRows = 1;

            SetTextGridColumn(grid_timosedb, 0, "No");
            SetTextGridColumn(grid_timosedb, 1, "IP");
            SetTextGridColumn(grid_timosedb, 2, "Port");
            SetTextGridColumn(grid_timosedb, 3, "DBName");

            grid_timosedb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_doorsdb
            grid_doorsdb.Redim(1, 4);
            grid_doorsdb.FixedRows = 1;

            SetTextGridColumn(grid_doorsdb, 0, "No");
            SetTextGridColumn(grid_doorsdb, 1, "IP");
            SetTextGridColumn(grid_doorsdb, 2, "Port");
            SetTextGridColumn(grid_doorsdb, 3, "DBName");

            grid_doorsdb.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_NetLauncher
            grid_NetLauncher.Redim(1, 15);
            grid_NetLauncher.FixedRows = 1;
            grid_NetLauncher.FixedColumns = 1;

            SetTextGridColumn(grid_NetLauncher, 0, "No");
            SetTextGridColumn(grid_NetLauncher, 1, "ID");
            SetTextGridColumn(grid_NetLauncher, 2, "IP");
            SetTextGridColumn(grid_NetLauncher, 3, "World");
            SetTextGridColumn(grid_NetLauncher, 4, "Login");
            SetTextGridColumn(grid_NetLauncher, 5, "Log");
            SetTextGridColumn(grid_NetLauncher, 6, "Master");
            SetTextGridColumn(grid_NetLauncher, 7, "Cash");
            SetTextGridColumn(grid_NetLauncher, 8, "GameDB");
            SetTextGridColumn(grid_NetLauncher, 9, "VillageDB");
            SetTextGridColumn(grid_NetLauncher, 10, "DB");
            SetTextGridColumn(grid_NetLauncher, 11, "Village");
            SetTextGridColumn(grid_NetLauncher, 12, "Game");
            SetTextGridColumn(grid_NetLauncher, 13, "WDB");
            SetTextGridColumn(grid_NetLauncher, 14, "WGame");

            grid_NetLauncher.AutoSizeAll();


            //////////////////////////////////////////////////////////////////////////
            // grid_EditSID
            grid_EditSID.Redim(1, 6);
            grid_EditSID.FixedRows = 1;
            grid_EditSID.FixedColumns = 1;

            SetTextGridColumn(grid_EditSID, 0, "No");
            SetTextGridColumn(grid_EditSID, 1, "NID");
            SetTextGridColumn(grid_EditSID, 2, "IP");
            SetTextGridColumn(grid_EditSID, 3, "World");
            SetTextGridColumn(grid_EditSID, 4, "Type");
            SetTextGridColumn(grid_EditSID, 5, "SID");

            grid_EditSID.AutoSizeAll();
            //////////////////////////////////////////////////////////////////////////
            // comboBox_SearchType
            comboBox_SearchType.DropDownStyle = ComboBoxStyle.DropDownList;

            comboBox_SearchType.Items.Add("World");
            comboBox_SearchType.Items.Add("NID");
            comboBox_SearchType.Items.Add("IP");
            comboBox_SearchType.Items.Add("Type");
            comboBox_SearchType.Items.Add("SID");
            comboBox_SearchType.SelectedIndex = 0;

            //////////////////////////////////////////////////////////////////////////
            // grid_report
            grid_report.Redim(1, 6);
            grid_report.FixedRows = 1;
            grid_report.FixedColumns = 1;
            SetTextGridColumn(grid_report, 0, "No");
            SetTextGridColumn(grid_report, 1, "Level");
            SetTextGridColumn(grid_report, 2, "Where");
            SetTextGridColumn(grid_report, 3, "Row");
            SetTextGridColumn(grid_report, 4, "Colum");
            SetTextGridColumn(grid_report, 5, "Description");

            grid_report.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_MultiWorldID
            grid_MultiWorldID.Redim(1, 4);
            grid_MultiWorldID.FixedRows = 1;
            grid_MultiWorldID.FixedColumns = 1;
            SetTextGridColumn(grid_MultiWorldID, 0, "NID");
            SetTextGridColumn(grid_MultiWorldID, 1, "IP");
            SetTextGridColumn(grid_MultiWorldID, 2, "Server");
            SetTextGridColumn(grid_MultiWorldID, 3, "World ID");

            grid_MultiWorldID.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_TargetIP
            grid_TargetIP.Redim(1, 4);
            grid_TargetIP.FixedRows = 1;
            grid_TargetIP.FixedColumns = 1;
            SetTextGridColumn(grid_TargetIP, 0, "NID");
            SetTextGridColumn(grid_TargetIP, 1, "IP");
            SetTextGridColumn(grid_TargetIP, 2, "Server");
            SetTextGridColumn(grid_TargetIP, 3, "TargetIP");

            grid_TargetIP.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_ServerNumber
            grid_ServerNumber.Redim(1, 4);
            grid_ServerNumber.FixedRows = 1;
            grid_ServerNumber.FixedColumns = 1;
            SetTextGridColumn(grid_ServerNumber, 0, "NID");
            SetTextGridColumn(grid_ServerNumber, 1, "IP");
            SetTextGridColumn(grid_ServerNumber, 2, "Server");
            SetTextGridColumn(grid_ServerNumber, 3, "Number");

            grid_ServerNumber.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_VillageInfo
            grid_VillageInfo.Redim(1, 15);
            grid_VillageInfo.FixedRows = 1;
            grid_VillageInfo.FixedColumns = 1;

            SetTextGridColumn(grid_VillageInfo, 0, "No");
            SetTextGridColumn(grid_VillageInfo, 1, "World");
            SetTextGridColumn(grid_VillageInfo, 2, "Village");
            SetTextGridColumn(grid_VillageInfo, 3, "Index");
            SetTextGridColumn(grid_VillageInfo, 4, "IP");
            SetTextGridColumn(grid_VillageInfo, 5, "Port");
            SetTextGridColumn(grid_VillageInfo, 6, "Channel");
            SetTextGridColumn(grid_VillageInfo, 7, "Map");
            SetTextGridColumn(grid_VillageInfo, 8, "Name");
            SetTextGridColumn(grid_VillageInfo, 9, "Attribute");
            SetTextGridColumn(grid_VillageInfo, 10, "MeritID");
            SetTextGridColumn(grid_VillageInfo, 11, "Visibility");
            SetTextGridColumn(grid_VillageInfo, 12, "MaxUser");
            SetTextGridColumn(grid_VillageInfo, 13, "LimitLevel");
            SetTextGridColumn(grid_VillageInfo, 14, "Dependent");

            grid_VillageInfo.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // comboBox_SearchVillageType
            comboBox_VillageType.DropDownStyle = ComboBoxStyle.DropDownList;

            comboBox_VillageType.Items.Add("World");
            comboBox_VillageType.Items.Add("IP");
            comboBox_VillageType.Items.Add("Village");
            comboBox_VillageType.SelectedIndex = 0;

            //////////////////////////////////////////////////////////////////////////
            // grid_MeritBonusTable
            grid_MeritBonusTable.Redim(1, 6);
            grid_MeritBonusTable.FixedRows = 1;

            SetTextGridColumn(grid_MeritBonusTable, 0, "No");
            SetTextGridColumn(grid_MeritBonusTable, 1, "ID");
            SetTextGridColumn(grid_MeritBonusTable, 2, "MinLevel");
            SetTextGridColumn(grid_MeritBonusTable, 3, "MaxLevel");
            SetTextGridColumn(grid_MeritBonusTable, 4, "BonusType");
            SetTextGridColumn(grid_MeritBonusTable, 5, "ExtendValue");

            grid_MeritBonusTable.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_ServerInfo
            grid_ServerInfo.Redim(1, 10);
            grid_ServerInfo.FixedRows = 1;

            SetTextGridColumn(grid_ServerInfo, 0, "No");
            SetTextGridColumn(grid_ServerInfo, 1, "World");
            SetTextGridColumn(grid_ServerInfo, 2, "MaxVillageIndex");
            SetTextGridColumn(grid_ServerInfo, 3, "IP");
            SetTextGridColumn(grid_ServerInfo, 4, "Name");
            SetTextGridColumn(grid_ServerInfo, 5, "WorldMaxUser");
            SetTextGridColumn(grid_ServerInfo, 6, "DefaultChannelCount");
            SetTextGridColumn(grid_ServerInfo, 7, "DefaultMaxUser");
            SetTextGridColumn(grid_ServerInfo, 8, "WorldSetID");
            SetTextGridColumn(grid_ServerInfo, 9, "OnTop");

            grid_ServerInfo.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_MapSet
            grid_MapSet.Redim(1, 9);
            grid_MapSet.FixedRows = 1;

            SetTextGridColumn(grid_MapSet, 0, "No");
            SetTextGridColumn(grid_MapSet, 1, "World");
            SetTextGridColumn(grid_MapSet, 2, "Name");
            SetTextGridColumn(grid_MapSet, 3, "Attribute");
            SetTextGridColumn(grid_MapSet, 4, "MeritID");
            SetTextGridColumn(grid_MapSet, 5, "LimitLevel");            
            SetTextGridColumn(grid_MapSet, 6, "Show");
            SetTextGridColumn(grid_MapSet, 7, "Hide");
            SetTextGridColumn(grid_MapSet, 8, "VillageIndex");

            grid_MapSet.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_NetLauncherIP
            grid_NetLauncherIP.Redim(1, 2);
            grid_NetLauncherIP.FixedRows = 1;
            grid_NetLauncherIP.FixedColumns = 1;

            SetTextGridColumn(grid_NetLauncherIP, 0, "Source IP");
            SetTextGridColumn(grid_NetLauncherIP, 1, "Dest IP");

            grid_NetLauncherIP.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_ChannelIP
            grid_ChannelIP.Redim(1, 2);
            grid_ChannelIP.FixedRows = 1;
            grid_ChannelIP.FixedColumns = 1;

            SetTextGridColumn(grid_ChannelIP, 0, "Source IP");
            SetTextGridColumn(grid_ChannelIP, 1, "Dest IP");

            grid_ChannelIP.AutoSizeAll();
        }

        private void SetDefaultBousType()
        {
            BonusTypeList.Add("CompleteExp");
        }

        private void AddMapSetAndAttribute(MapSet mapSet)
        {
            DefaultMapSetList.Add(mapSet);

            AddMapAttributeList(mapSet.MapAttribute);
        }

        private void SetDefaultMapSet()
        {
            AddMapAttributeList("Normal");

            // PrairieTown
            MapSet PrairieTown = new MapSet();
            PrairieTown.ID = 1;
            PrairieTown.Name = "PrairieTown";
            PrairieTown.MapAttribute = "Normal|Merit";
            PrairieTown.MaxUser = 200;
            PrairieTown.ShowCount = 4;
            PrairieTown.HideCount = 6;
            PrairieTown.IsAvailableMerit = true;
            AddMapSetAndAttribute(PrairieTown);

            // ManaRidge
            MapSet ManaRidge = new MapSet();
            ManaRidge.ID = 5;
            ManaRidge.Name = "ManaRidge";
            ManaRidge.MapAttribute = "Normal|Merit";
            ManaRidge.MaxUser = 200;
            ManaRidge.ShowCount = 4;
            ManaRidge.HideCount = 6;
            ManaRidge.IsAvailableMerit = true;
            AddMapSetAndAttribute(ManaRidge);

            // CataractGateway
            MapSet CataractGateway = new MapSet();
            CataractGateway.ID = 8;
            CataractGateway.Name = "CataractGateway";
            CataractGateway.MapAttribute = "Normal|Merit";
            CataractGateway.MaxUser = 400;
            CataractGateway.ShowCount = 4;
            CataractGateway.HideCount = 6;
            CataractGateway.IsAvailableMerit = true;
            AddMapSetAndAttribute(CataractGateway);

            // SaintHaven
            MapSet SaintHaven = new MapSet();
            SaintHaven.ID = 11;
            SaintHaven.Name = "SaintHaven";
            SaintHaven.MapAttribute = "Normal|Merit";
            SaintHaven.MaxUser = 600;
            SaintHaven.ShowCount = 4;
            SaintHaven.HideCount = 1;
            SaintHaven.IsAvailableMerit = true;
            AddMapSetAndAttribute(SaintHaven);

            // LotusMarshTown
            MapSet LotusMarshTown = new MapSet();
            LotusMarshTown.ID = 15;
            LotusMarshTown.Name = "LotusMarshTown";
            LotusMarshTown.MapAttribute = "Normal|Merit";
            LotusMarshTown.MaxUser = 100;
            LotusMarshTown.ShowCount = 10;
            LotusMarshTown.HideCount = 2;
            LotusMarshTown.IsAvailableMerit = true;
            AddMapSetAndAttribute(LotusMarshTown);

            // DarkVillage
            MapSet DarkVillage = new MapSet();
            DarkVillage.ID = 23;
            DarkVillage.Name = "DarkVillage";
            DarkVillage.MapAttribute = "Normal|DarkLair";
            DarkVillage.LimitLevel = 15;
            DarkVillage.ShowCount = 3;
            DarkVillage.HideCount = 7;
            AddMapSetAndAttribute(DarkVillage);

            // ColosseumChannel
            MapSet ColosseumChannel = new MapSet();
            ColosseumChannel.ID = 40;
            ColosseumChannel.Name = "ColosseumChannel";
            ColosseumChannel.MapAttribute = "Normal|PvP";
            ColosseumChannel.LimitLevel = 10;
            ColosseumChannel.ShowCount = 4;
            AddMapSetAndAttribute(ColosseumChannel);

            // ColosseumLobbyMap
            MapSet ColosseumLobbyMap = new MapSet();
            ColosseumLobbyMap.ID = 41;
            ColosseumLobbyMap.Name = "ColosseumLobbyMap";
            ColosseumLobbyMap.MapAttribute = "Normal|PvPLobby";
            ColosseumLobbyMap.MaxUser = 10000;
            ColosseumLobbyMap.ShowCount = 1;
            AddMapSetAndAttribute(ColosseumLobbyMap);

            // Farm_Town
            MapSet Farm_Town = new MapSet();
            Farm_Town.ID = 15107;
            Farm_Town.Name = "Farm_Town";
            Farm_Town.MapAttribute = "Normal|FarmTown";
            Farm_Town.LimitLevel = 24;
            Farm_Town.DependentMapID = 11;
            Farm_Town.ShowCount = 3;
            Farm_Town.HideCount = 3;
            AddMapSetAndAttribute(Farm_Town);

            // GM_Map
            MapSet GM_Map = new MapSet();
            GM_Map.ID = 42;
            GM_Map.Name = "GM_Map";
            GM_Map.MapAttribute = "Normal|GM";
            GM_Map.MaxUser = 200;
            AddMapSetAndAttribute(GM_Map);

            for (int i=0; i<DefaultMapSetList.Count; i++)
                MapNameList.Add(DefaultMapSetList[i].Name);

        }

        private void AddMapAttributeList(string str)
        {
            for (int i = 0; i < AttributeNameList.Count; i++)
            {
                if (AttributeNameList[i] == str)
                    return;
            }
            AttributeNameList.Add(str);
        }

        private void SetUIEnviornment()
        {
            // NONE
            UIEnviornment UI_NONE = new UIEnviornment();
            UI_NONE.nation = "";
            UIList.Add(UI_NONE);

            // KOR
            UIEnviornment UI_KOR = new UIEnviornment();
            UI_KOR.nation = "KR";
            UI_KOR.is_dolby = true;
            UIList.Add (UI_KOR);

            /*
            // KOR Stable
            UIEnviornment UI_KOR_STABLE = new UIEnviornment();
            UI_KOR_STABLE.nation = "KR Stable";
            UI_KOR_STABLE.is_dolby = true;
            UIList.Add(UI_KOR_STABLE);

            // KOR Staging
            UIEnviornment UI_KOR_STAGING = new UIEnviornment();
            UI_KOR_STAGING.nation = "KR Staging";
            UI_KOR_STAGING.is_dolby = true;
            UIList.Add(UI_KOR_STAGING);
            */

            // JPN
            UIEnviornment UI_JPN = new UIEnviornment();
            UI_JPN.nation = "JP";
            UIList.Add (UI_JPN);

            // CHN
            UIEnviornment UI_CHN = new UIEnviornment();
            UI_CHN.nation = "CH";
            UI_CHN.is_targetip = true;
            UIList.Add(UI_CHN);

            // TWN
            UIEnviornment UI_TWN = new UIEnviornment();
            UI_TWN.nation = "TW";
            UIList.Add(UI_TWN);

            // USA
            UIEnviornment UI_USA = new UIEnviornment();
            UI_USA.nation = "US";
            UI_USA.is_fatigue = true;
            UIList.Add(UI_USA);

            // SIN
            UIEnviornment UI_SIN = new UIEnviornment();
            UI_SIN.nation = "SG";
            UI_SIN.is_fatigue = true;
            UIList.Add(UI_SIN);

            // THA
            UIEnviornment UI_THA = new UIEnviornment();
            UI_THA.nation = "TH";
            UIList.Add(UI_THA);

            // IND
            UIEnviornment UI_IND = new UIEnviornment();
            UI_IND.nation = "ID";
            UIList.Add(UI_IND);

            // RUSIA
            UIEnviornment UI_RUS = new UIEnviornment();
            UI_RUS.nation = "RU";
            UIList.Add(UI_RUS);

            // EU
            UIEnviornment UI_EU = new UIEnviornment();
            UI_EU.nation = "EU";
            UIList.Add(UI_EU);


            ////////////////////////////////////////////////

            comboBox_Nation.DropDownStyle = ComboBoxStyle.DropDownList;

            for (int i = 0; i < UIList.Count; i++ )
                comboBox_Nation.Items.Add(UIList[i].nation);
            
            comboBox_Nation.SelectedIndex = 0;
        }

        private void SetFatigueInfo()
        {
            grid_FatigueInfo.Redim(3, 2);
            grid_FatigueInfo.FixedRows = 1;
            grid_FatigueInfo.FixedColumns = 1;

            SetTextGridColumn(grid_FatigueInfo, 0, "Field");
            SetTextGridColumn(grid_FatigueInfo, 1, "Value");

            SetTextGridRowHeader(grid_FatigueInfo, 1, "FTGExp");
            SetIntGridCell(grid_FatigueInfo, 1, 1, ChannelStruct.FTGExpTB.Exp.Value);

            SetTextGridRowHeader(grid_FatigueInfo, 2, "NoFTGExp");
            SetIntGridCell(grid_FatigueInfo, 2, 1, ChannelStruct.NoFTGExpTB.Exp.Value);

            grid_FatigueInfo.AutoSizeAll();
        }

        private void SetDefServerInfo()
        {
            //////////////////////////////////////////////////////////////////////////
            // grid_DefSeverInfo
            grid_DefSeverInfo.Redim(15, 2);
            grid_DefSeverInfo.FixedRows = 1;
            grid_DefSeverInfo.FixedColumns = 1;

            SetTextGridColumn(grid_DefSeverInfo, 0, "Field");
            SetTextGridColumn(grid_DefSeverInfo, 1, "Value");

            SetTextGridRowHeader(grid_DefSeverInfo, 1, "SocketMax");
            SetIntGridCell(grid_DefSeverInfo, 1, 1, ServerStruct.defSvrInfo.SocketMax);

            SetTextGridRowHeader(grid_DefSeverInfo, 2, "DBPort");
            SetIntGridCell(grid_DefSeverInfo, 2, 1, ServerStruct.defSvrInfo.DBPort);

            SetTextGridRowHeader(grid_DefSeverInfo, 3, "LoginMasterPort");
            SetIntGridCell(grid_DefSeverInfo, 3, 1, ServerStruct.defSvrInfo.LoginMasterPort);

            SetTextGridRowHeader(grid_DefSeverInfo, 4, "LoginClientPort");
            SetIntGridCell(grid_DefSeverInfo, 4, 1, ServerStruct.defSvrInfo.LoginClientPort);

            SetTextGridRowHeader(grid_DefSeverInfo, 5, "MasterVillagePort");
            SetIntGridCell(grid_DefSeverInfo, 5, 1, ServerStruct.defSvrInfo.MasterVillagePort);

            SetTextGridRowHeader(grid_DefSeverInfo, 6, "MasterGamePort");
            SetIntGridCell(grid_DefSeverInfo, 6, 1, ServerStruct.defSvrInfo.MasterGamePort);

            SetTextGridRowHeader(grid_DefSeverInfo, 7, "VillageClientPort");
            SetIntGridCell(grid_DefSeverInfo, 7, 1, ServerStruct.defSvrInfo.VillageClientPort);

            SetTextGridRowHeader(grid_DefSeverInfo, 8, "GameClientPortUDP");
            SetIntGridCell(grid_DefSeverInfo, 8, 1, ServerStruct.defSvrInfo.GameClientPortUDP);

            SetTextGridRowHeader(grid_DefSeverInfo, 9, "GameClientPortTCP");
            SetIntGridCell(grid_DefSeverInfo, 9, 1, ServerStruct.defSvrInfo.GameClientPortTCP);

            SetTextGridRowHeader(grid_DefSeverInfo, 10, "CashPort");
            SetIntGridCell(grid_DefSeverInfo, 10, 1, ServerStruct.defSvrInfo.CashPort);

            SetTextGridRowHeader(grid_DefSeverInfo, 11, "LogPort");
            SetIntGridCell(grid_DefSeverInfo, 11, 1, ServerStruct.defSvrInfo.LogPort);

            grid_DefSeverInfo.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_Dolby
            grid_Dolby.Redim(2, 5);
            grid_Dolby.FixedRows = 1;

            SetTextGridColumn(grid_Dolby, 0, "WorldID");
            SetTextGridColumn(grid_Dolby, 1, "PrivateIP");
            SetTextGridColumn(grid_Dolby, 2, "PublicIP");
            SetTextGridColumn(grid_Dolby, 3, "APort");
            SetTextGridColumn(grid_Dolby, 4, "CPort");

            SetIntGridCell(grid_Dolby, 1, 0, ServerStruct.defSvrInfo.dolbyaxon.WorldID);
            SetTextGridCell(grid_Dolby, 1, 1, ServerStruct.defSvrInfo.dolbyaxon.PrivateIP);
            SetTextGridCell(grid_Dolby, 1, 2, ServerStruct.defSvrInfo.dolbyaxon.PublicIP);

            grid_Dolby[1, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            grid_Dolby[1, 2].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetIntGridCell(grid_Dolby, 1, 3, ServerStruct.defSvrInfo.dolbyaxon.APort);
            SetIntGridCell(grid_Dolby, 1, 4, ServerStruct.defSvrInfo.dolbyaxon.CPort);

            grid_Dolby.AutoSizeAll();

            //////////////////////////////////////////////////////////////////////////
            // grid_DoorsServerInfo
            grid_DoorsServerInfo.Redim(2, 2);
            grid_DoorsServerInfo.FixedRows = 1;

            SetTextGridColumn(grid_DoorsServerInfo, 0, "IP");
            SetTextGridColumn(grid_DoorsServerInfo, 1, "Port");

            SetTextGridCell(grid_DoorsServerInfo, 1, 0, ServerStruct.defSvrInfo.doorsServerInfo.IP);
            grid_DoorsServerInfo[1, 0].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_DoorsServerInfo, 1, 1, ServerStruct.defSvrInfo.doorsServerInfo.Port);

            grid_DoorsServerInfo.AutoSizeAll();
            checkBox_DoorServer.Checked = ServerStruct.defSvrInfo.IsDoorsInclude;
            //grid_DoorsServerInfo.Enabled = ServerStruct.defSvrInfo.IsDoorsInclude;
        }

        private void SetTextGridColumn(SourceGrid2.Grid grid, int col, string str)
        {
            SourceGrid2.Cells.Real.ColumnHeader header = new SourceGrid2.Cells.Real.ColumnHeader(str);
            header.EnableSort = false;

            grid[0, col] = header;
        }

        private void SetTextGridRowHeader(SourceGrid2.Grid grid, int row, string str)
        {
            SourceGrid2.Cells.Real.RowHeader header = new SourceGrid2.Cells.Real.RowHeader(str);
            header.TextAlignment = ContentAlignment.MiddleLeft;

            grid[row, 0] = header;
        }

        private void SetTextGridCell(SourceGrid2.Grid grid, int row, int col, string str)
        {
            grid[row, col] = new SourceGrid2.Cells.Real.Cell(str, typeof(string));
            grid[row, col].DataModel.Validated += new SourceGrid2.CellEventHandler(grid_Cell_Validate);
        }

        private void SetIntGridCell(SourceGrid2.Grid grid, int row, int col, int val)
        {
            grid[row, col] = new SourceGrid2.Cells.Real.Cell(val, typeof(int));
            grid[row, col].DataModel.Validated += new SourceGrid2.CellEventHandler(grid_Cell_Validate);
        }

        private void SetComboBoxGridCell(SourceGrid2.Grid grid, int row, int col, List<string> str_list, int index)
        {
            grid[row, col] = new SourceGrid2.Cells.Real.ComboBox(str_list[index], typeof(string), str_list, true);
            grid[row, col].DataModel.Validated += new SourceGrid2.CellEventHandler(grid_Cell_Validate);
        }

        private string GetTextGridCell(SourceGrid2.Grid grid, int row, int col)
        {
            return grid[row, col].ToString();
        }

        private int GetInt32GridCell(SourceGrid2.Grid grid, int row, int col)
        {
            try
            {
                return Int32.Parse(grid[row, col].ToString());
            }
            catch( Exception )
            {
                return 0;
            }
        }

        DialogResult CheckDelete( SourceGrid2.Grid grid, int delrow )
        {
            string msg = "";

            msg += "Delete below Item\r\n";

            for (int i = 0; i < grid.ColumnsCount; i++ )
            {
                string item;
                item = GetTextGridCell(grid, 0, i);
                item += " : ";
                item += GetTextGridCell(grid, delrow, i);
                item += "\r\n";

                msg += item;
            }

            msg += "Are you Sure?";

            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

        DialogResult CheckClear(SourceGrid2.Grid grid)
        {
            string msg = "";

            msg += "Clear all items\r\n\n";
            msg += "Are you Sure?";

            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

        DialogResult CheckResult(int ErrorCount, int WaringCount)
        {
            string msg = "";

            msg += (ErrorCount) + " Errors !!\r\n\n";
            msg += (WaringCount) + " Warnings !!\r\n\n";
            msg += "Are you sure to make xml?";

            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

        DialogResult CheckRefreshServer()
        {
            string msg = "";

            msg += "Are you sure to refresh?";

            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

        DialogResult CheckTargetIP()
        {
            string msg = "";

            msg += "Do you update Master->Login Connection?\n";
            
            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

        DialogResult CheckServerNumber()
        {
            string msg = "";

            msg += "Do you update Cash Server Number?\n";

            return MessageBox.Show(msg, "Confirm", MessageBoxButtons.YesNo);
        }

       
        private bool CheckTargetIPCount()
        {
            int nTotalCount = 0;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                nTotalCount += launcher.MasterCount;
            

            if (TargetIPServer.Count == nTotalCount)
                return false;

            return true;
        }

        private bool CheckServerNumberCount()
        {
            int nTotalCount = 0;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                nTotalCount += launcher.CashCount;

            if (ServerNumber.Count == nTotalCount)
                return false;

            return true;
        }

        private bool CheckMultiWorldCount()
        {
            int nTotalMultiServer = 0;           
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.MasterCount > 1)
                    nTotalMultiServer += launcher.MasterCount;

                if (launcher.CashCount > 1)
                    nTotalMultiServer += launcher.CashCount;

            }

            int nCompareMultiServerCount = 0;
            foreach (ServerInfo server in MultiWorldServer)
                nCompareMultiServerCount++;
            
            if (nTotalMultiServer == nCompareMultiServerCount)
                return false;

            return true;
        }

        private void ResetLoginServer()
        {
            LoginServer.Clear();
        }

        private bool IsValidLoginServer(int ID)
        {
            for (int i = 0; i < LoginServer.Count; i++)
            {
                if (LoginServer[i].ID == ID)
                    return true;
            }

            return false;
        }

        private bool IsValidLoginServer(string IP)
        {
            for (int i = 0; i < LoginServer.Count; i++)
            {
                if (LoginServer[i].TargetIP == IP || IP == "000.000.000.000")
                    return true;
            }

            return false;
        }

        private void AddLoginServer(int ID, string IP)
        {
            if (!IsValidLoginServer(ID))
            {
                ServerInfo tempserver = new ServerInfo();
                tempserver.ID = ID;
                tempserver.TargetIP = IP;
                LoginServer.Add(tempserver);
            }
        }

        private void ResetWorldID()
        {
            WorldIDList.Clear();
        }

        private void AddWorldID (int nWorldID)
        {
            if (nWorldID == 0)
                return;

            if (!IsValidWorldID(nWorldID))
                WorldIDList.Add(nWorldID);
        }

        private bool IsValidWorldID(int nWorldID)
        {
            if (nWorldID < 0)
                return false;

            for (int i = 0; i < WorldIDList.Count; i++ )
            {
                if (WorldIDList[i] == nWorldID)
                    return true;
            }

            return false;
        }

        private class InputServer
        {
            public int Start;
            public int Count;
            public int ProcessCount;
            public ServerInfo.ServerType dbtype;

            public InputServer(int s, int c, int num)
            {
                Start = s;
                Count = c;
                ProcessCount = num;

                dbtype = ServerInfo.ServerType.Database;
            }
        }

        private InputServer ParseDBServer(string text)
        {
            if( text.Length <= 0 )        return null; 
            InputServer ret = new InputServer(0, 0, 0);
            try
            {
                ret.dbtype = ServerInfo.ServerType.Database;
                ret.ProcessCount = Int32.Parse(text);
                return ret;
            }
            catch (System.Exception) {  }

            text.Trim();

            int first = text.IndexOf('(');
            int last = text.IndexOf(')');

            if (text.Length-1 != last)    return null;

            string stype = text.Substring(0, first);
            string snum = text.Substring(first + 1, last - first - 1);
            try { ret.ProcessCount = Int32.Parse(snum); }
            catch (System.Exception) { return null;  }

            if (stype == "G")
            {
                ret.dbtype = ServerInfo.ServerType.GameDB;
            }
            else if (stype == "V")
            {
                ret.dbtype = ServerInfo.ServerType.VillageDB;
            }
            else
                return null;

            return ret;
        }

        private void AddmemdbGrid(MemberShipDBInfo dbinfo)
        {
            int newrow = grid_memdb.Rows.Count;
            grid_memdb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_memdb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_memdb, newrow, 1, dbinfo.basicinfo.IP);
            grid_memdb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_memdb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_memdb, newrow, 3, dbinfo.basicinfo.DBID);
            SetTextGridCell(grid_memdb, newrow, 4, dbinfo.basicinfo.DBName);

            grid_memdb.AutoSizeAll();
       }

        private void button_memadd_Click(object sender, EventArgs e)
        {
            AddMemberShipDB();
        }

        private void AddMemberShipDB()
        {
            MemberShipDBInfo dbinfo = new MemberShipDBInfo();
            ServerStruct.defDBinfo.memberDBList.Add(dbinfo);

            AddmemdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_memdel_Click(object sender, EventArgs e)
        {
            DeleteMemberShipDB();
        }

        private void DeleteMemberShipDB()
        {
            if (grid_memdb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_memdb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.memberDBList.Count < DelIndex) return;

            if (CheckDelete(grid_memdb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.memberDBList.RemoveAt(DelIndex - 1);
            grid_memdb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (MemberShipDBInfo dbinfo in ServerStruct.defDBinfo.memberDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_memdb.Redim(1, 5);
            grid_memdb.FixedRows = 1;
            foreach (MemberShipDBInfo dbinfo in ServerStruct.defDBinfo.memberDBList)
                AddmemdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void AddworlddbGrid(WorldDBInfo dbinfo)
        {
            int newrow = grid_worlddb.Rows.Count;
            grid_worlddb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_worlddb, newrow, 0, dbinfo.basicinfo.No);
            SetIntGridCell(grid_worlddb, newrow, 1, dbinfo.WorldID);
            SetTextGridCell(grid_worlddb, newrow, 2, dbinfo.basicinfo.IP);
            grid_worlddb[newrow, 2].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_worlddb, newrow, 3, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_worlddb, newrow, 4, dbinfo.basicinfo.DBID);
            SetTextGridCell(grid_worlddb, newrow, 5, dbinfo.basicinfo.DBName);

            grid_worlddb.AutoSizeAll();
        }

        private void button_worldadd_Click(object sender, EventArgs e)
        {
            AddWorldDB();
        }

        private void AddWorldDB()
        {
            WorldDBInfo dbinfo = new WorldDBInfo();
            ServerStruct.defDBinfo.worldDBList.Add(dbinfo);

            AddworlddbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_worlddel_Click(object sender, EventArgs e)
        {
            DeleteWorldDB();
        }


        private void DeleteWorldDB()
        {
            if (grid_worlddb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_worlddb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;
            if (ServerStruct.defDBinfo.worldDBList.Count < DelIndex) return;

            if (CheckDelete(grid_worlddb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.worldDBList.RemoveAt(DelIndex - 1);
            grid_worlddb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (WorldDBInfo dbinfo in ServerStruct.defDBinfo.worldDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_worlddb.Redim(1, 6);
            grid_worlddb.FixedRows = 1;
            foreach (WorldDBInfo dbinfo in ServerStruct.defDBinfo.worldDBList)
                AddworlddbGrid(dbinfo);

            RealTimeVerification();
        }
        

        private void AddlogdbGrid(LogDBInfo dbinfo)
        {
            int newrow = grid_logdb.Rows.Count;
            grid_logdb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_logdb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_logdb, newrow, 1, dbinfo.basicinfo.IP);
            grid_logdb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_logdb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_logdb, newrow, 3, dbinfo.basicinfo.DBID);
            SetTextGridCell(grid_logdb, newrow, 4, dbinfo.basicinfo.DBName);

            grid_logdb.AutoSizeAll();
        }

        private void button_logadd_Click(object sender, EventArgs e)
        {
            AddLogDB();
        }

        private void AddLogDB()
        {
            LogDBInfo dbinfo = new LogDBInfo();
            ServerStruct.defDBinfo.logDBList.Add(dbinfo);

            AddlogdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_logdel_Click(object sender, EventArgs e)
        {
            DeleteLogDB();
        }

        private void DeleteLogDB()
        {
            if (grid_logdb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_logdb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.logDBList.Count < DelIndex) return;
            if (CheckDelete(grid_logdb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.logDBList.RemoveAt(DelIndex - 1);
            grid_logdb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (LogDBInfo dbinfo in ServerStruct.defDBinfo.logDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_logdb.Redim(1, 5);
            grid_logdb.FixedRows = 1;
            foreach (LogDBInfo dbinfo in ServerStruct.defDBinfo.logDBList)
                AddlogdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void AddgsmdbGrid(GSMDBInfo dbinfo)
        {
            int newrow = grid_gsmdb.Rows.Count;
            grid_gsmdb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_gsmdb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_gsmdb, newrow, 1, dbinfo.basicinfo.IP);
            grid_gsmdb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_gsmdb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_gsmdb, newrow, 3, dbinfo.basicinfo.DBName);

            grid_gsmdb.AutoSizeAll();
        }

        private void button_gsmadd_Click(object sender, EventArgs e)
        {
            AddGsmDB();
        }

        private void AddGsmDB()
        {
            GSMDBInfo dbinfo = new GSMDBInfo();
            ServerStruct.defDBinfo.gsmDBList.Add(dbinfo);

            AddgsmdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_gsmdel_Click(object sender, EventArgs e)
        {
            DeleteGsmDB();
        }

        private void DeleteGsmDB()
        {
            if (grid_gsmdb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_gsmdb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.gsmDBList.Count < DelIndex) return;

            if (CheckDelete(grid_gsmdb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.gsmDBList.RemoveAt(DelIndex - 1);
            grid_gsmdb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (GSMDBInfo dbinfo in ServerStruct.defDBinfo.gsmDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_gsmdb.Redim(1, 4);
            grid_gsmdb.FixedRows = 1;
            foreach (GSMDBInfo dbinfo in ServerStruct.defDBinfo.gsmDBList)
                AddgsmdbGrid(dbinfo);

            RealTimeVerification();
        }
        
        private void AddstagingdbGrid(StagingDBInfo dbinfo)
        {
            int newrow = grid_stagingdb.Rows.Count;
            grid_stagingdb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_stagingdb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_stagingdb, newrow, 1, dbinfo.basicinfo.IP);
            grid_stagingdb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_stagingdb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_stagingdb, newrow, 3, dbinfo.basicinfo.DBName);

            grid_stagingdb.AutoSizeAll();
        }

        private void button_stagingadd_Click(object sender, EventArgs e)
        {
            AddStagingDB();
        }

        private void AddStagingDB()
        {
            StagingDBInfo dbinfo = new StagingDBInfo();
            ServerStruct.defDBinfo.stagingDBList.Add(dbinfo);

            AddstagingdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_stagingdel_Click(object sender, EventArgs e)
        {
            DeleteStagingDB();
        }

        private void DeleteStagingDB()
        {
            if (grid_stagingdb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_stagingdb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.stagingDBList.Count < DelIndex) return;

            if (CheckDelete(grid_stagingdb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.stagingDBList.RemoveAt(DelIndex - 1);
            grid_stagingdb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (StagingDBInfo dbinfo in ServerStruct.defDBinfo.stagingDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_stagingdb.Redim(1, 4);
            grid_stagingdb.FixedRows = 1;
            foreach (StagingDBInfo dbinfo in ServerStruct.defDBinfo.stagingDBList)
                AddstagingdbGrid(dbinfo);

            RealTimeVerification();
        }


        private void button_MakeXML_Click(object sender, EventArgs e)
        {

            // 상세 정보 확인
            CheckSpecificInfo();
                
            // 데이터 변환
            ConvertServerData();

            // 검수
            CheckVerification();

            // 리포트 
            RefreshReportGrid();

            // 빌리지 정보 구성상태 확인
            if (!CheckMakeVillage())
                return;

            // 마지막 확인처리
            if (ErrorList.Count > 0 || WaringList.Count > 0)
                if (CheckResult(ErrorList.Count, WaringList.Count) == DialogResult.No) return;

            // 돌비 적용여부
            ServerStruct.defSvrInfo.IsDolby = checkBox_DolbyAxon.Checked;

            // 파일 선택
            bool IsSaveServerStruct = false;
            SelectFile selectDialog = new SelectFile();
            DialogResult result = selectDialog.ShowDialog();
            if (result == DialogResult.Yes)
                IsSaveServerStruct = true;
            else if (result == DialogResult.No)
                IsSaveServerStruct = false;
            else
                return;

            // 파일 쓰기
            SaveFileDialog savedlg = new SaveFileDialog();

            savedlg.Filter = "XML Files(*.xml)|*.xml";
            if (savedlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    if (IsSaveServerStruct)
                        ServerStruct.ToXML(savedlg.FileName);
                    else
                        ChannelStruct.ToXML(savedlg.FileName);
                     
                    MessageBox.Show("Make XML File Success !!");
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void AllClearSeverVariation()
        {
            ClearAllGrid();
            ClearNetLauncher();
            ResetWorldID();
            ResetLoginServer();

            ServerStruct.Clear();
        }

        private void button_importxml_Click(object sender, EventArgs e)
        {
            bool IsSaveServerStruct = false;

            SelectFile selectDialog = new SelectFile();

            DialogResult result = selectDialog.ShowDialog();
            if (result == DialogResult.Yes)
                IsSaveServerStruct = true;
            else if (result == DialogResult.No)
                IsSaveServerStruct = false;
            else
                return;

            OpenFileDialog opendlg = new OpenFileDialog();
            opendlg.Filter = "XML Files(*.xml)|*.xml";

            XmlDocument doc = new XmlDocument();
            if (opendlg.ShowDialog() == DialogResult.OK)
            {   
                doc.Load(opendlg.FileName);

                if (IsSaveServerStruct)
                    LoadServerStruct(doc);
                else
                    LoadChannelInfo(doc);
            }
        }

        private void LoadServerStruct(XmlDocument doc)
        {
            AllClearSeverVariation();

            SvrStruct tmp = new SvrStruct();
            XmlNode root = doc.DocumentElement;

            int ErrorCount = 0;
            string ResultXML;

            ResultXML = tmp.ServiceMng.FromXML(XmlHelper.FindChild(root, "ServiceManager"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [ServiceManager]Error:" + ResultXML, 0, 0);
                ErrorCount++;
            }

            ResultXML = tmp.defSvrInfo.FromXML(XmlHelper.FindChild(root, "DefaultServerInfo"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [DefaultServerInfo]" + ResultXML, 0, 0);
                ErrorCount++;
            }

            checkBox_DolbyAxon.Checked = tmp.defSvrInfo.IsDolby;

            ResultXML = tmp.defDBinfo.FromXML(XmlHelper.FindChild(root, "DefaultDataBaseInfo"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [DefaultDataBaseInfo] Error:" + ResultXML, 0, 0);
                ErrorCount++;
            }

            XmlNode xmlLauncherList = XmlHelper.FindChild(root, "NetLauncherList");
            if (xmlLauncherList != null)
            {
                foreach (XmlNode n in xmlLauncherList.ChildNodes)
                {
                    if (n.NodeType != XmlNodeType.Element) continue;

                    NetLauncher launcher = new NetLauncher();
                    if (!launcher.FromXML(n))
                    {
                        AddVerification(TPI.Import, "Import Failed [NetLauncher]", 0, 0);
                        ErrorCount++;
                        continue;
                    }

                    tmp.NetLauncherList.Add(launcher);
                }
            }
            else
            {
                AddVerification(TPI.Import, "Import Failed [NetLauncherList]", 0, 0);
                ErrorCount++;
            }

            foreach (XmlNode xmlnode in root.ChildNodes)
            {
                if (xmlnode.Name != "ServerInfo") continue;

                ServerInfo info = new ServerInfo();
                if (!info.FromXML(xmlnode))
                {
                    AddVerification(TPI.Import, "Import Failed [ServerInfo]", 0, 0);
                    ErrorCount++;
                    break;
                }

                switch (info.Type)
                {
                    case ServerInfo.ServerType.GameDB: SetTextGridCell(grid_Excutefile, 4, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.VillageDB: SetTextGridCell(grid_Excutefile, 4, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Login: SetTextGridCell(grid_Excutefile, 0, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Master: SetTextGridCell(grid_Excutefile, 2, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Village: SetTextGridCell(grid_Excutefile, 5, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Game: SetTextGridCell(grid_Excutefile, 6, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Log: SetTextGridCell(grid_Excutefile, 1, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Cash: SetTextGridCell(grid_Excutefile, 3, 1, info.ExecuteFileName); break;
                    case ServerInfo.ServerType.Database: SetTextGridCell(grid_Excutefile, 4, 1, info.ExecuteFileName); break;
                }

                if (!tmp.LoadServerInfo(info))
                    AddVerification(TPI.Import, "Import Failed [ServerInfo] Cannot find NetLauncher: " + info.LauncherID, 0, 0);

            }


            grid_Excutefile.AutoSizeAll();

            if (ErrorCount > 0)
                MessageBox.Show("Fail to import ServerStruct");
            else
                MessageBox.Show("Success to import ServerStruct");
                
            
            // 정상적으로 읽혀졌으므로 정보를 저장
            ServerStruct = tmp;

            loadType |= LoadType.Server;

            if (ServerStruct.NetLauncherList.Count > 0)
            {
                ServerStruct.PID = ServerStruct.NetLauncherList[0].PID;
                textBox_PID.Text = ServerStruct.PID.ToString();
            }

            RefreshAllGrid();
        }


        private void LoadChannelInfo(XmlDocument doc)
        {
            ChannelStruct.Clear();

            ChannelInfo tmp = new ChannelInfo();
            tmp.Clear();

            XmlNode root = doc.DocumentElement;

            int ErrorCount = 0;
            string ResultXML;

            ResultXML = tmp.MeritBonusTB.FromXML(XmlHelper.FindChild(root, "MeritBonusTable"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [MeritBonusTable]Error:" + ResultXML, 0, 0);
                ErrorCount++;
            }

            if (XmlHelper.FindChild(root, "FTGExpTable") != null && XmlHelper.FindChild(root, "NoFTGExpTable") != null)
            {
                ResultXML = tmp.FTGExpTB.FromXML(XmlHelper.FindChild(root, "FTGExpTable"));
                if (ResultXML != null)
                {
                    AddVerification(TPI.Import, "Import Failed [FTGExpTable]Error:" + ResultXML, 0, 0);
                    ErrorCount++;
                }

                ResultXML = tmp.NoFTGExpTB.FromXML(XmlHelper.FindChild(root, "NoFTGExpTable"));
                if (ResultXML != null)
                {
                    AddVerification(TPI.Import, "Import Failed [NoFTGExpTable]Error:" + ResultXML, 0, 0);
                    ErrorCount++;
                }
            }
            
            ResultXML = tmp.ServerMerger.FromXML(XmlHelper.FindChild(root, "ServerMergerInfo"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [ServerMergerInfo]Error:" + ResultXML, 0, 0);
                ErrorCount++;
            }

            ResultXML = tmp.ServerInfoEx.FromXML(XmlHelper.FindChild(root, "ServerInfo"));
            if (ResultXML != null)
            {
                AddVerification(TPI.Import, "Import Failed [ServerInfo]Error:" + ResultXML, 0, 0);
                ErrorCount++;
            }

            if (ErrorCount > 0)
                MessageBox.Show("Fail to import ChannelInfo");
            else
                MessageBox.Show("Success to import ChannelInfo");

           
            // 정상적으로 읽혀졌으므로 정보를 저장
            ChannelStruct = tmp;

            loadType |= LoadType.Channel;

            RefreshAllGrid();

            IsMakeVillage = true;
        }

        private void RefreshAllGrid()
        {
            RefreshMultiWorldIDGrid();

            ConvertServerData();

            SetDefServerInfo();

            SetFatigueInfo();

            SortNetLauncherIndex();

            RefreshServiceManagerGrid();

            //RefreshMultiWorldIDGrid();

            RefreshTargetIPGrid();

            RefreshServerNumberGrid();

            RefreshDataBaseGrid();

            RefreshEditSIDGrid();

            RefreshMeritBonusGrid();

            RefreshServerInfoGrid();

            RefreshMapSet(1);

            RefreshVillageInfo();

            RefreshServerIPGrid();

            RefreshChannelIPGrid();

            int LoadServer = (int)(loadType & LoadType.Server);
            int LoadChannel = (int)(loadType & LoadType.Channel);

            if (LoadServer == 1 && LoadChannel != 2)
                CheckServer();
            else if (LoadServer != 1 && LoadChannel == 2)
                CheckChannel();
            else
                CheckVerification();

            if (!Is_Verfification())
            {
                if (LoadServer == 1 && LoadChannel != 2)
                    MessageBox.Show("Fail to verify ServerStruct");
                else if (LoadServer != 1 && LoadChannel == 2)
                    MessageBox.Show("Fail to verify ChannelInfo");
                else
                    MessageBox.Show("Verification Fail");
            }
            else
            {
                if (LoadServer == 1 && LoadChannel != 2)
                    MessageBox.Show("Success to verify ServerStruct");
                else if (LoadServer != 1 && LoadChannel == 2)
                    MessageBox.Show("Success to verify ChannelInfo, Please import ServerStruct.xml for checking in detail");
                else
                    MessageBox.Show("Verification Success");
            }

            RefreshReportGrid();
        }

        private void grid_Cell_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            if (args.Cell.Grid == grid_ServiceMng)
            {
               grid_ServiceMng_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_DoorsServerInfo)
            {
                grid_DoorsServerInfo_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_DefSeverInfo)
            {
                grid_DefSeverInfo_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_Dolby)
            {
                grid_Dolby_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_memdb)
            {
                grid_memDB_Validate(sender, args);
                RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_worlddb)
            {
                 grid_worldDB_Validate(sender, args);
                 RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_logdb)
            {
                 grid_logDB_Validate(sender, args);
                 RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_gsmdb)
            {
                 grid_gsmDB_Validate(sender, args);
                 RealTimeVerification();
               
            }
            else if (args.Cell.Grid == grid_stagingdb)
            {
                grid_stagingDB_Validate(sender, args);
                RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_timosedb)
            {
                grid_timoseDB_Validate(sender, args);
                RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_doorsdb)
            {
                grid_doorsDB_Validate(sender, args);
                RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_NetLauncher)
            {
                grid_NetLauncher_Validate(sender, args);
                RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_EditSID)
            {
                grid_EditSID_Validate(sender, args);
                if (textBox_SearchWord.Text == "")
                    RealTimeVerification();
            }
            else if (args.Cell.Grid == grid_MultiWorldID)
            {
                grid_MultiWorldID_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_TargetIP)
            {
                grid_TargetIP_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_ServerNumber)
            {
                grid_ServerNo_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_VillageInfo)
            {
                grid_VillageInfo_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_MeritBonusTable)
            {
                grid_MeritBonusTable_Validate(sender, args);
                RealTimeVerification();
                IsMakeVillage = true;
            }
            else if (args.Cell.Grid == grid_ServerInfo)
            {
                grid_ServerInfo_Validate(sender, args);
                RealTimeVerification();
                IsMakeVillage = true;
            }
            else if (args.Cell.Grid == grid_MapSet)
            {
                grid_MapSet_Validate(sender, args);
                RealTimeVerification();
                IsMakeVillage = true;
            }
            else if (args.Cell.Grid == grid_NetLauncherIP)
            {
                grid_NetLauncherIP_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_ChannelIP)
            {
                grid_ChannelIP_Validate(sender, args);
            }
            else if (args.Cell.Grid == grid_FatigueInfo)
            {
                grid_FatigueInfo_Validate(sender, args);
                RealTimeVerification();
            }
        }

        private void grid_ServiceMng_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ServerStruct.ServiceMng.IP = GetTextGridCell(grid_ServiceMng, 1, 0);
            ServerStruct.ServiceMng.Port = GetInt32GridCell(grid_ServiceMng, 1, 1);

            grid_ServiceMng.AutoSizeAll();
        }

        private void grid_DoorsServerInfo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ServerStruct.defSvrInfo.doorsServerInfo.IP = GetTextGridCell(grid_DoorsServerInfo, 1, 0);
            ServerStruct.defSvrInfo.doorsServerInfo.Port = GetInt32GridCell(grid_DoorsServerInfo, 1, 1);

            grid_DoorsServerInfo.AutoSizeAll();
        }
        
        private void grid_FatigueInfo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ChannelStruct.FTGExpTB.Exp.Value = GetInt32GridCell(grid_FatigueInfo, 1, 1);
            ChannelStruct.NoFTGExpTB.Exp.Value = GetInt32GridCell(grid_FatigueInfo, 2, 1);

            grid_FatigueInfo.AutoSizeAll();
        }
        

        private void grid_DefSeverInfo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ServerStruct.defSvrInfo.SocketMax = GetInt32GridCell(grid_DefSeverInfo, 1, 1);
            ServerStruct.defSvrInfo.DBPort = GetInt32GridCell(grid_DefSeverInfo, 2, 1);
            ServerStruct.defSvrInfo.LoginMasterPort = GetInt32GridCell(grid_DefSeverInfo, 3, 1);
            ServerStruct.defSvrInfo.LoginClientPort = GetInt32GridCell(grid_DefSeverInfo, 4, 1);
            ServerStruct.defSvrInfo.MasterVillagePort = GetInt32GridCell(grid_DefSeverInfo, 5, 1);
            ServerStruct.defSvrInfo.MasterGamePort = GetInt32GridCell(grid_DefSeverInfo, 6, 1);
            ServerStruct.defSvrInfo.VillageClientPort = GetInt32GridCell(grid_DefSeverInfo, 7, 1);
            ServerStruct.defSvrInfo.GameClientPortUDP = GetInt32GridCell(grid_DefSeverInfo, 8, 1);
            ServerStruct.defSvrInfo.GameClientPortTCP = GetInt32GridCell(grid_DefSeverInfo, 9, 1);
            ServerStruct.defSvrInfo.CashPort = GetInt32GridCell(grid_DefSeverInfo, 10, 1);
            ServerStruct.defSvrInfo.LogPort = GetInt32GridCell(grid_DefSeverInfo, 11, 1);

            grid_DefSeverInfo.AutoSizeAll();
        }

        private void grid_Dolby_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ServerStruct.defSvrInfo.dolbyaxon.WorldID = GetInt32GridCell(grid_Dolby, 1, 0);
            ServerStruct.defSvrInfo.dolbyaxon.PrivateIP = GetTextGridCell(grid_Dolby, 1, 1);
            ServerStruct.defSvrInfo.dolbyaxon.PublicIP = GetTextGridCell(grid_Dolby, 1, 2);
            ServerStruct.defSvrInfo.dolbyaxon.APort = GetInt32GridCell(grid_Dolby, 1, 3);
            ServerStruct.defSvrInfo.dolbyaxon.CPort = GetInt32GridCell(grid_Dolby, 1, 4);

            grid_Dolby.AutoSizeAll();
        }

        private void grid_memDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_memdb.Rows.Count; i++)
            {
                MemberShipDBInfo dbinfo = ServerStruct.defDBinfo.memberDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_memdb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_memdb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_memdb, i, 2);
                dbinfo.basicinfo.DBID = GetTextGridCell(grid_memdb, i, 3);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_memdb, i, 4);
            }
            grid_memdb.AutoSizeAll();
        }

        private void grid_worldDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_worlddb.Rows.Count; i++)
            {
                WorldDBInfo dbinfo = ServerStruct.defDBinfo.worldDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_worlddb, i, 0);
                dbinfo.WorldID = GetInt32GridCell(grid_worlddb, i, 1);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_worlddb, i, 2);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_worlddb, i, 3);
                dbinfo.basicinfo.DBID = GetTextGridCell(grid_worlddb, i, 4);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_worlddb, i, 5);
            }
            grid_worlddb.AutoSizeAll();
        }

        private void grid_logDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_logdb.Rows.Count; i++)
            {
                LogDBInfo dbinfo = ServerStruct.defDBinfo.logDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_logdb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_logdb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_logdb, i, 2);
                dbinfo.basicinfo.DBID = GetTextGridCell(grid_logdb, i, 3);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_logdb, i, 4);
            }
            grid_logdb.AutoSizeAll();
        }

        private void grid_gsmDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_gsmdb.Rows.Count; i++)
            {
                GSMDBInfo dbinfo = ServerStruct.defDBinfo.gsmDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_gsmdb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_gsmdb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_gsmdb, i, 2);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_gsmdb, i, 3);
            }
            grid_gsmdb.AutoSizeAll();
        }

        private void grid_stagingDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_stagingdb.Rows.Count; i++)
            {
                StagingDBInfo dbinfo = ServerStruct.defDBinfo.stagingDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_stagingdb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_stagingdb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_stagingdb, i, 2);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_stagingdb, i, 3);
            }
            grid_stagingdb.AutoSizeAll();
        }

        private void grid_timoseDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_timosedb.Rows.Count; i++)
            {
                TimoseDBInfo dbinfo = ServerStruct.defDBinfo.timoseDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_timosedb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_timosedb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_timosedb, i, 2);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_timosedb, i, 3);
            }
            grid_timosedb.AutoSizeAll();
        }

        private void grid_doorsDB_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_doorsdb.Rows.Count; i++)
            {
                DoorsDBInfo dbinfo = ServerStruct.defDBinfo.doorsDBList[i - 1];
                dbinfo.basicinfo.No = GetInt32GridCell(grid_doorsdb, i, 0);
                dbinfo.basicinfo.IP = GetTextGridCell(grid_doorsdb, i, 1);
                dbinfo.basicinfo.Port = GetInt32GridCell(grid_doorsdb, i, 2);
                dbinfo.basicinfo.DBName = GetTextGridCell(grid_doorsdb, i, 3);
            }
            grid_doorsdb.AutoSizeAll();
        }

        private void grid_NetLauncher_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_NetLauncher.Rows.Count; i++)
            {
                NetLauncher launcher = ServerStruct.NetLauncherList[i - 1];
                launcher.No = GetInt32GridCell(grid_NetLauncher, i, 0);
                launcher.ID = GetInt32GridCell(grid_NetLauncher, i, 1);
                launcher.IP = GetTextGridCell(grid_NetLauncher, i, 2);
                launcher.WID = GetInt32GridCell(grid_NetLauncher, i, 3);
                launcher.LoginCount = GetInt32GridCell(grid_NetLauncher, i, 4);
                launcher.LogCount = GetInt32GridCell(grid_NetLauncher, i, 5);
                launcher.MasterCount = GetInt32GridCell(grid_NetLauncher, i, 6);
                launcher.CashCount = GetInt32GridCell(grid_NetLauncher, i, 7);
                launcher.GameDBCount = GetInt32GridCell(grid_NetLauncher, i, 8);
                launcher.VillageDBCount = GetInt32GridCell(grid_NetLauncher, i, 9);
                launcher.DBCount = GetInt32GridCell(grid_NetLauncher, i, 10);
                launcher.VillageCount = GetInt32GridCell(grid_NetLauncher, i, 11);
                launcher.GameCount = GetInt32GridCell(grid_NetLauncher, i, 12);
                launcher.WDBCount = GetInt32GridCell(grid_NetLauncher, i, 13);
                launcher.WGameCount = GetInt32GridCell(grid_NetLauncher, i, 14);

            }
            grid_NetLauncher.AutoSizeAll();
        }

        private void grid_MeritBonusTable_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_MeritBonusTable.Rows.Count; i++)
            {
                MeritBonus Bonus = ChannelStruct.MeritBonusTB.MeritBonusList[i - 1];
                Bonus.No = GetInt32GridCell(grid_MeritBonusTable, i, 0);
                Bonus.ID = GetInt32GridCell(grid_MeritBonusTable, i, 1);
                Bonus.MinLevel = GetInt32GridCell(grid_MeritBonusTable, i, 2);
                Bonus.MaxLevel = GetInt32GridCell(grid_MeritBonusTable, i, 3);
                Bonus.BonusType = GetTextGridCell(grid_MeritBonusTable, i, 4);
                Bonus.ExtendValue = GetInt32GridCell(grid_MeritBonusTable, i, 5);
            }
            grid_NetLauncher.AutoSizeAll();
        }

        private void grid_ServerInfo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_ServerInfo.Rows.Count; i++)
            {
                ServerEx server = ChannelStruct.ServerInfoEx.RealServerList[i - 1];
                server.No = GetInt32GridCell(grid_ServerInfo, i, 0);
                server.ID = GetInt32GridCell(grid_ServerInfo, i, 1);
                server.MaxVillageCount = GetInt32GridCell(grid_ServerInfo, i, 2);
                server.IP = GetTextGridCell(grid_ServerInfo, i, 3);
                server.Name = GetTextGridCell(grid_ServerInfo, i, 4);
                server.WorldMaxUser = GetInt32GridCell(grid_ServerInfo, i, 5);
                server.DefaultChannelCount = GetInt32GridCell(grid_ServerInfo, i, 6);
                server.DefaultMaxUser = GetInt32GridCell(grid_ServerInfo, i, 7);
                server.SetID = GetInt32GridCell(grid_ServerInfo, i, 8);
                server.OnTop = GetInt32GridCell(grid_ServerInfo, i, 9);
            }
            grid_NetLauncher.AutoSizeAll();
        }

        private void grid_NetLauncherIP_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
           string SouceIP;
           string DestIP;
           for (int i = 1; i < grid_NetLauncherIP.Rows.Count; i++)
           {
               SouceIP = GetTextGridCell(grid_NetLauncherIP, i, 0);
               DestIP = GetTextGridCell(grid_NetLauncherIP, i, 1);

               SetServerIP(NetLauncherIP, SouceIP, DestIP);
           }

           grid_NetLauncherIP.AutoSizeAll();
        }

        private void grid_ChannelIP_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            string SouceIP;
            string DestIP;
            for (int i = 1; i < grid_ChannelIP.Rows.Count; i++)
            {
                SouceIP = GetTextGridCell(grid_ChannelIP, i, 0);
                DestIP = GetTextGridCell(grid_ChannelIP, i, 1);

                SetServerIP(ChannelIP, SouceIP, DestIP);
            }

            grid_ChannelIP.AutoSizeAll();
        }

        private void grid_MapSet_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            ServerEx serverInfo = ChannelStruct.ServerInfoEx.RealServerList[SelectServerIndex];
            List<MapSet> MapSetList = serverInfo.MapSetList;

            string MapName;
            bool IsRefresh = false;
            int index = 0;

            for (int i = 1; i < grid_MapSet.Rows.Count; i++)
            {
                MapSet map = MapSetList[i - 1];
                map.No = GetInt32GridCell(grid_MapSet, i, 0);
                map.WorldID = GetInt32GridCell(grid_MapSet, i, 1);
                MapName = GetTextGridCell(grid_MapSet, i, 2);
                map.MapAttribute = GetTextGridCell(grid_MapSet, i, 3);
                map.MeritBonusID = GetInt32GridCell(grid_MapSet, i, 4);
                map.LimitLevel = GetInt32GridCell(grid_MapSet, i, 5);
                map.ShowCount = GetInt32GridCell(grid_MapSet, i, 6);
                map.HideCount = GetInt32GridCell(grid_MapSet, i, 7);
                map.VillageIndex= GetInt32GridCell(grid_MapSet, i, 8);

                if (map.Name != MapName) // 맵이름 바꿀때만 처리한다.
                {
                    IsRefresh = true;
                    index = GetIndexFromString(MapName, MapNameList);
                    map.SetData(DefaultMapSetList[index]);
                }
            }
            grid_MapSet.AutoSizeAll();

            if (IsRefresh)
                RefreshMapSet(SelectServerIndex+1);
        }

        private void grid_VillageInfo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_VillageInfo.Rows.Count; i++)
            {
                InputVillageInfo(i);
            }
            grid_VillageInfo.AutoSizeAll();
        }

        private void InputVillageInfo(int row)
        {
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                    {
                        if (villageInfo.No == row)
                        {
                            villageInfo.No = GetInt32GridCell(grid_VillageInfo, row, 0);
                            server.ID = GetInt32GridCell(grid_VillageInfo, row, 1);
                            villageServer.ID = GetInt32GridCell(grid_VillageInfo, row, 2);
                            villageServer.Index = GetInt32GridCell(grid_VillageInfo, row, 3);
                            villageServer.IP = GetTextGridCell(grid_VillageInfo, row, 4);
                            villageServer.Port = GetInt32GridCell(grid_VillageInfo, row, 5);
                            villageInfo.CID = GetInt32GridCell(grid_VillageInfo, row, 6);
                            villageInfo.ID = GetInt32GridCell(grid_VillageInfo, row, 7);
                            villageInfo.Name = GetTextGridCell(grid_VillageInfo, row, 8);
                            villageInfo.MapAttribute = GetTextGridCell(grid_VillageInfo, row, 9);
                            villageInfo.MeritBonusID = GetInt32GridCell(grid_VillageInfo, row, 10);
                            villageInfo.Visibility = GetTextGridCell(grid_VillageInfo, row, 11);
                            villageInfo.MaxUser = GetInt32GridCell(grid_VillageInfo, row, 12);
                            villageInfo.LimitLevel = GetInt32GridCell(grid_VillageInfo, row, 13);
                            villageInfo.DependentMapID = GetInt32GridCell(grid_VillageInfo, row, 14);

                            return;
                        }
                    }
                }
            }
        }

        private void grid_EditSID_Validate(object sender, SourceGrid2.CellEventArgs args)
        {

            for (int i = 1; i < grid_EditSID.Rows.Count; i++)
            {
                int No = GetInt32GridCell(grid_EditSID, i, 0);
                int SID = GetInt32GridCell(grid_EditSID, i, 5);

                foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                {
                    if (launcher.RealServer.ChangeSIDByInput(No, SID))
                        break;
                }
            }

            grid_EditSID.AutoSizeAll();
            
        }

        private void grid_MultiWorldID_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_MultiWorldID.Rows.Count; i++)
            {
                ServerInfo server = MultiWorldServer[i - 1];
                server.WorldID = GetInt32GridCell(grid_MultiWorldID, i, 3);
            }
        }

        private void grid_TargetIP_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_TargetIP.Rows.Count; i++)
            {
                ServerInfo server = TargetIPServer[i - 1];
                server.TargetIP = GetTextGridCell(grid_TargetIP, i, 3);
            }
        }

        private void grid_ServerNo_Validate(object sender, SourceGrid2.CellEventArgs args)
        {
            for (int i = 1; i < grid_ServerNumber.Rows.Count; i++)
            {
                ServerInfo server = ServerNumber[i - 1];
                server.ServerNo = GetInt32GridCell(grid_ServerNumber, i, 3);
            }
        }

        private string SetIpRage(string ip)
        {
            string[] vals = ip.Split('.');
            if (vals.Length < 4)
            {
                return "000.000.000";
            }

            int nIndex = 0;
            foreach (string val in vals)
            {
                if (Int32.Parse(val) < 0 || Int32.Parse(val) > 255)
                    return "000.000.000";

                nIndex++;
            }

            if (nIndex >= 3)
                return vals[0] + "." + vals[1] + "." + vals[2];

            return "000.000.000";
        }

        private bool IsMaxVillageIndex(ServerEx server)
        {
            if (server.SetID != -1)
                return true;

            List<int> checkList = new List<int>();
            for (int i = 0; i < server.MapSetList.Count; i++ )
            {
                bool bCheck = false;
                for (int j=0; j<checkList.Count; j++)
                {
                    if (checkList[j] == server.MapSetList[i].VillageIndex)
                        bCheck = true;
                }

                if (!bCheck)
                    checkList.Add(server.MapSetList[i].VillageIndex);
            }

            if (checkList.Count == server.MaxVillageCount)
                return true;

            return false;
        }

       
        private bool IsIPValid(string ip)
        {
            string[] vals = ip.Split('.');
            if (vals.Length != 4)
                return false;

            foreach (string val in vals)
            {
                try
                {
                    if (Int32.Parse(val) < 0 || Int32.Parse(val) > 255)
                        return false;

                    if (vals[0] == "000" || vals[1] == "000"|| vals[2] == "000" || vals[3] == "000")
                        return false;
                }
                catch (Exception)
                {
                    return false;
                }
            }

            return true;
        }

        private bool IsIpRangeValid(string ip)
        {
            string[] vals = ip.Split('.');
            if (vals.Length != 3)
                return false;

            foreach (string val in vals)
            {
                try
                {
                    if (Int32.Parse(val) < 0 || Int32.Parse(val) > 255)
                        return false;

                    if (vals[0] == "000" || vals[1] == "000" || vals[2] == "000")
                        return false;
                }
                catch (Exception)
                {
                    return false;
                }
            }

            return true;
        }

        private void grid_IP_Validating(Object sender, SourceGrid2.ValidatingCellEventArgs args)
        {
            if (IsIPValid(args.NewValue.ToString()) == false)
                args.Cancel = true;   
        }

        private void TrueFalse_Validating(Object sender, SourceGrid2.ValidatingCellEventArgs args)
        {
            if (args.NewValue.ToString() == "True" || args.NewValue.ToString() == "False")
            {

            }
            else
                args.Cancel = true;
        }


        private void grid_DBServer_Validating(Object sender, SourceGrid2.ValidatingCellEventArgs args)
        {
            if (args.NewValue.ToString().Length <= 0)
            {
                args.Cancel = false;
                return;
            }

            InputServer tmp = ParseDBServer(args.NewValue.ToString());
            if (tmp == null)
                args.Cancel = true;
        }

        private void Add_Launcher_Click(object sender, EventArgs e)
        {
            AddNetLauncherMenu();
        }

        private void AddNetLauncherMenu()
        {
            string LauncherIP = "000.000.000.000";
            AddNetLauncher(LauncherIP);

            RealTimeVerification();
        }

        private void AddNetLauncher (string LauncherIP)
        {
            NetLauncher launcher = new NetLauncher();
            launcher.No = SvrStruct.NextLauncherNo();
            launcher.ID = GetLastLauncherID();
            launcher.IP = LauncherIP;
            launcher.WID = 0;
            ServerStruct.NetLauncherList.Add(launcher);


            /////////////////////////////////////////////////////////////////////////////////////
            int newrow = grid_NetLauncher.Rows.Count;
            grid_NetLauncher.Rows.Insert(newrow);

            SetIntGridCell(grid_NetLauncher, newrow, 0, launcher.No);
            SetIntGridCell(grid_NetLauncher, newrow, 1, launcher.ID);

            SetTextGridCell(grid_NetLauncher, newrow, 2, launcher.IP);
            grid_NetLauncher[newrow, 2].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetIntGridCell(grid_NetLauncher, newrow, 3, launcher.WID);
            SetIntGridCell(grid_NetLauncher, newrow, 4, launcher.LoginCount);
            SetIntGridCell(grid_NetLauncher, newrow, 5, launcher.LogCount);
            SetIntGridCell(grid_NetLauncher, newrow, 6, launcher.MasterCount);
            SetIntGridCell(grid_NetLauncher, newrow, 7, launcher.CashCount);
            SetIntGridCell(grid_NetLauncher, newrow, 8, launcher.GameDBCount);
            SetIntGridCell(grid_NetLauncher, newrow, 9, launcher.VillageDBCount);
            SetIntGridCell(grid_NetLauncher, newrow, 10, launcher.DBCount);
            SetIntGridCell(grid_NetLauncher, newrow, 11, launcher.VillageCount);
            SetIntGridCell(grid_NetLauncher, newrow, 12, launcher.GameCount);
            SetIntGridCell(grid_NetLauncher, newrow, 13, launcher.WDBCount);
            SetIntGridCell(grid_NetLauncher, newrow, 14, launcher.WGameCount);

            grid_NetLauncher[newrow, 0].DataModel.EnableEdit = false;
            
            PaintBackgroundColor(grid_NetLauncher);

        }

        private int GetLastLauncherID()
        {
            int NID = 0;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.ID > NID)
                    NID = launcher.ID;
            }

            return ++NID;
        }

        private void SortNetLauncherIndex ()
        {
            SvrStruct.ResetLauncherID();
            foreach (NetLauncher I in ServerStruct.NetLauncherList)
            {
                I.No = SvrStruct.NextLauncherNo();
            }

            RefreshNetLauncherGrid();

            PaintNetLauncherGrid();
        }

        private void AddMultiWorldServer (NetLauncher launcher, ServerInfo.ServerType Type, int nIndex, int WorldID)
        {
            string ServerName;
            if (Type == ServerInfo.ServerType.Master)
            {
                ServerName = "Master " + nIndex;
            }
            else if (Type == ServerInfo.ServerType.Cash)
            {
                ServerName = "Cash " + nIndex;
            }
            else
            {
                MessageBox.Show("AddMultiWorldServer Type Error");
                return;
            }

            int newrow = grid_MultiWorldID.Rows.Count;
            grid_MultiWorldID.Rows.Insert(newrow);

            SetIntGridCell(grid_MultiWorldID, newrow, 0, launcher.ID);
            SetTextGridCell(grid_MultiWorldID, newrow, 1, launcher.IP);
            grid_MultiWorldID[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetTextGridCell(grid_MultiWorldID, newrow, 2, ServerName);
            SetIntGridCell(grid_MultiWorldID, newrow, 3, WorldID);

            grid_MultiWorldID[newrow, 0].DataModel.EnableEdit = false;
            grid_MultiWorldID[newrow, 1].DataModel.EnableEdit = false;
            grid_MultiWorldID[newrow, 2].DataModel.EnableEdit = false;

            ServerInfo tempserver = new ServerInfo();
            tempserver.LauncherID = launcher.ID;
            tempserver.Type = Type;
            tempserver.WorldID = WorldID;

            MultiWorldServer.Add(tempserver);
        }

        private void ClearMultiWorldServer()
        {
            grid_MultiWorldID.Redim(1, 4);
            grid_MultiWorldID.FixedRows = 1;
            grid_MultiWorldID.FixedColumns = 1;

            MultiWorldServer.Clear();
        }

        private void RefreshDataBaseGrid()
        {
            grid_memdb.Redim(1, 5);
            grid_memdb.FixedRows = 1;
            foreach (MemberShipDBInfo dbinfo in ServerStruct.defDBinfo.memberDBList)
                AddmemdbGrid(dbinfo);

            grid_worlddb.Redim(1, 6);
            grid_worlddb.FixedRows = 1;
            foreach (WorldDBInfo dbinfo in ServerStruct.defDBinfo.worldDBList)
                AddworlddbGrid(dbinfo);

            grid_logdb.Redim(1, 5);
            grid_logdb.FixedRows = 1;
            foreach (LogDBInfo dbinfo in ServerStruct.defDBinfo.logDBList)
                AddlogdbGrid(dbinfo);

            grid_gsmdb.Redim(1, 4);
            grid_gsmdb.FixedRows = 1;
            foreach (GSMDBInfo dbinfo in ServerStruct.defDBinfo.gsmDBList)
                AddgsmdbGrid(dbinfo);

            grid_stagingdb.Redim(1, 4);
            grid_stagingdb.FixedRows = 1;
            foreach (StagingDBInfo dbinfo in ServerStruct.defDBinfo.stagingDBList)
                AddstagingdbGrid(dbinfo);

            grid_timosedb.Redim(1, 4);
            grid_timosedb.FixedRows = 1;
            foreach (TimoseDBInfo dbinfo in ServerStruct.defDBinfo.timoseDBList)
                AddtimosedbGrid(dbinfo);

            grid_doorsdb.Redim(1, 4);
            grid_doorsdb.FixedRows = 1;
            foreach (DoorsDBInfo dbinfo in ServerStruct.defDBinfo.doorsDBList)
                AddDoorsdbGrid(dbinfo);
        }

        private void RefreshServiceManagerGrid()
        {            
            SetTextGridCell(grid_ServiceMng, 1, 0, ServerStruct.ServiceMng.IP);
            grid_ServiceMng[1, 0].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_ServiceMng, 1, 1, ServerStruct.ServiceMng.Port);
            grid_ServiceMng.AutoSizeAll();
        }

        private void RefreshMultiWorldIDGrid()
        {
            ClearMultiWorldServer();
           
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.RealServer.Master.Count > 1)
                {
                    for (int i = 0; i < launcher.RealServer.Master.Count; i++)
                        AddMultiWorldServer(launcher, ServerInfo.ServerType.Master, i, launcher.RealServer.Master[i].WorldID);
                }

                if (launcher.RealServer.Cash.Count > 1)
                {
                    for (int i = 0; i < launcher.RealServer.Cash.Count; i++)
                        AddMultiWorldServer(launcher, ServerInfo.ServerType.Cash, i, launcher.RealServer.Cash[i].WorldID);
                }
            }

            
            PaintBackgroundColor(grid_MultiWorldID);

        }

        private void UpdateMultiWorldIDGrid()
        {
            ClearMultiWorldServer();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.MasterCount > 1)
                {
                    for (int i = 0; i < launcher.MasterCount; i++)
                    {
                        if (i < launcher.RealServer.Master.Count)
                            AddMultiWorldServer(launcher, ServerInfo.ServerType.Master, i, launcher.RealServer.Master[i].WorldID);
                        else
                            AddMultiWorldServer(launcher, ServerInfo.ServerType.Master, i, -1);
                    }
                }

                if (launcher.CashCount > 1)
                {
                    for (int i = 0; i < launcher.CashCount; i++)
                    {
                        if (i < launcher.RealServer.Cash.Count)
                            AddMultiWorldServer(launcher, ServerInfo.ServerType.Cash, i, launcher.RealServer.Cash[i].WorldID);
                        else
                            AddMultiWorldServer(launcher, ServerInfo.ServerType.Cash, i, -1);
                    }
                }
            }

            
            PaintBackgroundColor(grid_MultiWorldID);
        }

        private void ClearTargetIPServer()
        {
            grid_TargetIP.Redim(1, 4);
            grid_TargetIP.FixedRows = 1;
            grid_TargetIP.FixedColumns = 1;

            TargetIPServer.Clear();
        }

        private void UpdateTargetIPGrid()
        {
            ClearTargetIPServer();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                for (int i = 0; i < launcher.MasterCount; i++)
                {
                    if (i < launcher.RealServer.Master.Count)
                        AddTargetIPServer(launcher, ServerInfo.ServerType.Master, i, launcher.RealServer.Master[i].TargetIP);
                    else
                        AddTargetIPServer(launcher, ServerInfo.ServerType.Master, i, "000.000.000.000");
                }
            }

           
            PaintBackgroundColor(grid_TargetIP);
        }

       
        private void RefreshTargetIPGrid()
        {
            ClearTargetIPServer();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                for (int i = 0; i < launcher.RealServer.Master.Count; i++)
                {
                    bool bAdd = false;
                    for (int j = 0; j < LoginServer.Count; j++)
                    {
                        if (launcher.RealServer.Master[i].TargetID == LoginServer[j].ID)
                        {
                            AddTargetIPServer(launcher, ServerInfo.ServerType.Master, i, LoginServer[j].TargetIP);
                            launcher.RealServer.Master[i].TargetIP = LoginServer[j].TargetIP;
                            bAdd = true;
                        }
                    }

                    if (!bAdd)
                    {
                        AddTargetIPServer(launcher, ServerInfo.ServerType.Master, i, "000.000.000.000");
                    }
                }
            }

            
            PaintBackgroundColor(grid_TargetIP);
        }

        private void AddTargetIPServer (NetLauncher launcher, ServerInfo.ServerType Type, int nIndex, string TargetIP)
        {
            string ServerName;
            if (Type == ServerInfo.ServerType.Master)
            {
                ServerName = "Master " + nIndex;
            }
            else
            {
                MessageBox.Show("AddTargetIPServer Type Error");
                return;
            }

            int newrow = grid_TargetIP.Rows.Count;
            grid_TargetIP.Rows.Insert(newrow);

            SetIntGridCell(grid_TargetIP, newrow, 0, launcher.ID);
            SetTextGridCell(grid_TargetIP, newrow, 1, launcher.IP);
            grid_TargetIP[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetTextGridCell(grid_TargetIP, newrow, 2, ServerName);
            SetTextGridCell(grid_TargetIP, newrow, 3, TargetIP);
            //grid_TargetIP[newrow, 3].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            grid_TargetIP[newrow, 0].DataModel.EnableEdit = false;
            grid_TargetIP[newrow, 1].DataModel.EnableEdit = false;
            grid_TargetIP[newrow, 2].DataModel.EnableEdit = false;
            
            ServerInfo tempserver = new ServerInfo();
            tempserver.LauncherID = launcher.ID;
            tempserver.Type = Type;
            tempserver.TargetIP = TargetIP;

            TargetIPServer.Add(tempserver);
        }

        private void ClearServerNumber()
        {
            grid_ServerNumber.Redim(1, 4);
            grid_ServerNumber.FixedRows = 1;
            grid_ServerNumber.FixedColumns = 1;

            ServerNumber.Clear();
        }

        private void UpdateServerNumberGrid()
        {
            ClearServerNumber();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                for (int i = 0; i < launcher.CashCount; i++)
                {
                    if (i < launcher.RealServer.Cash.Count)
                        AddServerNumber(launcher, ServerInfo.ServerType.Cash, i, launcher.RealServer.Cash[i].ServerNo);
                    else
                        AddServerNumber(launcher, ServerInfo.ServerType.Cash, i, -1);
                }
            }

            
            PaintBackgroundColor(grid_ServerNumber);
        }

        private void RefreshServerNumberGrid()
        {
            ClearServerNumber();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                for (int i = 0; i < launcher.RealServer.Cash.Count; i++)
                    AddServerNumber(launcher, ServerInfo.ServerType.Cash, i, launcher.RealServer.Cash[i].ServerNo);
            }

            
            PaintBackgroundColor(grid_ServerNumber);
        }

        private void PaintBackgroundColor(SourceGrid2.Grid Grid)
        {
            for (int i = 1; i < Grid.Rows.Count; i++)
            {
                if (i % 2 == 1)
                {
                    for (int j = 0; j < Grid.ColumnsCount; j++)
                        Grid[i, j].VisualModel = LightGrayModel;
                }
                else
                {
                    for (int j = 0; j < Grid.ColumnsCount; j++)
                        Grid[i, j].VisualModel = WhiteModel;
                }
            }

            Grid.AutoSizeAll();
        }

        private void PaintSelectRows (SourceGrid2.Grid Grid, int Row)
        {
            if (Grid.Rows.Count < 2)
                return;

            for (int i=0; i<Grid.ColumnsCount; i++)
                Grid[Row, i].VisualModel = SelectModel;

            Grid.AutoSizeAll();
        }

        private void PaintBackgroundWhite(SourceGrid2.Grid Grid)
        {
            for (int i = 1; i < Grid.Rows.Count; i++)
            {
                for (int j = 0; j < Grid.ColumnsCount; j++)
                    Grid[i, j].VisualModel = WhiteModel;
                
            }

            Grid.AutoSizeAll();
        }

        private void AddServerNumber (NetLauncher launcher, ServerInfo.ServerType Type, int nIndex, int nServerNo)
        {
            string ServerName;
            if (Type == ServerInfo.ServerType.Cash)
            {
                ServerName = "Cash " + nIndex; 
            }
            else
            {
                MessageBox.Show("AddServerNumber Type Error");
                return;
            }

            int newrow = grid_ServerNumber.Rows.Count;
            grid_ServerNumber.Rows.Insert(newrow);

            SetIntGridCell(grid_ServerNumber, newrow, 0, launcher.ID);
            SetTextGridCell(grid_ServerNumber, newrow, 1, launcher.IP);
            grid_ServerNumber[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetTextGridCell(grid_ServerNumber, newrow, 2, ServerName);
            SetIntGridCell(grid_ServerNumber, newrow, 3, nServerNo);

            ServerInfo tempserver = new ServerInfo();
            tempserver.LauncherID = launcher.ID;
            tempserver.Type = Type;
            tempserver.ServerNo = nServerNo;

            ServerNumber.Add(tempserver);

        }

        private void PaintNetLauncherGrid()
        {
            for (int i = 1; i < grid_NetLauncher.Rows.Count; i++)
            {
                NetLauncher launcher = ServerStruct.NetLauncherList[i - 1];

                if (i % 2 == 1)
                {
                    for (int j = 0; j < grid_NetLauncher.ColumnsCount; j++)
                        grid_NetLauncher[i, j].VisualModel = LightGrayModel;
                }
                else
                {
                    for (int j = 0; j < grid_NetLauncher.ColumnsCount; j++)
                        grid_NetLauncher[i, j].VisualModel = WhiteModel;
                }

                if (launcher.ID < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher ID [No:" + launcher.No + "]", launcher.No, 1);

                if (launcher.WID < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher WorldID [No:" + launcher.No + "]", launcher.No, 3);

                if (launcher.LoginCount > 0)
                    grid_NetLauncher[i, 4].VisualModel = AntiqueWhiteModel;
                else if (launcher.LoginCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher LoginCount [No:" + launcher.No + "]", launcher.No, 4);

                if (launcher.LogCount > 0)
                    grid_NetLauncher[i, 5].VisualModel = AntiqueWhiteModel;
                else if (launcher.LogCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher LogCount [No:" + launcher.No + "]", launcher.No, 5);

                if (launcher.MasterCount > 0)
                    grid_NetLauncher[i, 6].VisualModel = AntiqueWhiteModel;
                else if (launcher.MasterCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher MasterCount [No:" + launcher.No + "]", launcher.No, 6);

                if (launcher.CashCount > 0)
                    grid_NetLauncher[i, 7].VisualModel = AntiqueWhiteModel;
                else if (launcher.CashCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher CashCount [No:" + launcher.No + "]", launcher.No, 7);

                if (launcher.GameDBCount > 0)
                    grid_NetLauncher[i, 8].VisualModel = AntiqueWhiteModel;
                else if (launcher.GameDBCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher GameDBCount [No:" + launcher.No + "]", launcher.No, 8);

                if (launcher.VillageDBCount > 0)
                    grid_NetLauncher[i, 9].VisualModel = AntiqueWhiteModel;
                else if (launcher.VillageDBCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher VillageDBCount [No:" + launcher.No + "]", launcher.No, 9);

                if (launcher.DBCount > 0)
                    grid_NetLauncher[i, 10].VisualModel = AntiqueWhiteModel;
                else if (launcher.DBCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher DBCount [No:" + launcher.No + "]", launcher.No, 10);

                if (launcher.VillageCount > 0)
                    grid_NetLauncher[i, 11].VisualModel = AntiqueWhiteModel;
                else if (launcher.VillageCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher VillageCount [No:" + launcher.No + "]", launcher.No, 11);

                if (launcher.GameCount > 0)
                    grid_NetLauncher[i, 12].VisualModel = AntiqueWhiteModel;
                else if (launcher.GameCount < 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher GameCount [No:" + launcher.No + "]", launcher.No, 11);
            }

            grid_NetLauncher.AutoSizeAll();
        }


        private void RefreshNetLauncherGrid()
        {
            grid_NetLauncher.Redim(1, 15);
            grid_NetLauncher.FixedRows = 1;
            grid_NetLauncher.FixedColumns = 1;

            ResetWorldID();
            ResetLoginServer();
            
            int newrow;

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                newrow = grid_NetLauncher.Rows.Count;
                grid_NetLauncher.Rows.Insert(newrow);
                
                AddWorldID(launcher.WID);

                if (launcher.LoginCount > 0)
                    AddLoginServer(launcher.ID, launcher.IP);

                SetIntGridCell(grid_NetLauncher, newrow, 0, launcher.No);
                SetIntGridCell(grid_NetLauncher, newrow, 1, launcher.ID);

                SetTextGridCell(grid_NetLauncher, newrow, 2, launcher.IP);
                grid_NetLauncher[newrow, 2].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

                SetIntGridCell(grid_NetLauncher, newrow, 3, launcher.WID);
                SetIntGridCell(grid_NetLauncher, newrow, 4, launcher.LoginCount);
                SetIntGridCell(grid_NetLauncher, newrow, 5, launcher.LogCount);
                SetIntGridCell(grid_NetLauncher, newrow, 6, launcher.MasterCount);
                SetIntGridCell(grid_NetLauncher, newrow, 7, launcher.CashCount);
                SetIntGridCell(grid_NetLauncher, newrow, 8, launcher.GameDBCount);
                SetIntGridCell(grid_NetLauncher, newrow, 9, launcher.VillageDBCount);
                SetIntGridCell(grid_NetLauncher, newrow, 10, launcher.DBCount);
                SetIntGridCell(grid_NetLauncher, newrow, 11, launcher.VillageCount);
                SetIntGridCell(grid_NetLauncher, newrow, 12, launcher.GameCount);
                SetIntGridCell(grid_NetLauncher, newrow, 13, launcher.WDBCount);
                SetIntGridCell(grid_NetLauncher, newrow, 14, launcher.WGameCount);

                grid_NetLauncher[newrow, 0].DataModel.EnableEdit = false;
            }

            grid_NetLauncher.AutoSizeAll();

            textBox_PID.Text = ServerStruct.PID.ToString();
        }

    
        private void RefreshEditSIDGrid()
        {
            grid_EditSID.Redim(1, 6);
            grid_EditSID.FixedRows = 1;
            grid_EditSID.FixedColumns = 1;

            SvrStruct.ResetServerNo();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                foreach (ServerInfo info in launcher.RealServer.Login)
                    AddEditSIDGrid (info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Log)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Master)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Cash)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.DataBase)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.VillageDB)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.GameDB)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Village)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Game)
                    AddEditSIDGrid(info, launcher);
                
            }

            
            PaintBackgroundColor(grid_EditSID);

        }

        private void AddEditSIDGrid(ServerInfo info, NetLauncher launcher)
        {
            int newrow = grid_EditSID.Rows.Count;
            grid_EditSID.Rows.Insert(newrow);

            info.No = SvrStruct.NextServerNo(); 

            SetIntGridCell(grid_EditSID, newrow, 0, info.No);
            SetIntGridCell(grid_EditSID, newrow, 1, launcher.ID);

            SetTextGridCell(grid_EditSID, newrow, 2, launcher.IP);
            grid_EditSID[newrow, 2].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);

            SetIntGridCell(grid_EditSID, newrow, 3, launcher.WID);
            SetTextGridCell(grid_EditSID, newrow, 4, info.Type.ToString());
            SetIntGridCell(grid_EditSID, newrow, 5, info.ID);

            grid_EditSID[newrow, 0].DataModel.EnableEdit = false;
            grid_EditSID[newrow, 1].DataModel.EnableEdit = false;
            grid_EditSID[newrow, 2].DataModel.EnableEdit = false;
            grid_EditSID[newrow, 3].DataModel.EnableEdit = false;
            grid_EditSID[newrow, 4].DataModel.EnableEdit = false;
        }

        private void AddMapSetGrid(MapSet map)
        {
            int newrow = grid_MapSet.Rows.Count;
            grid_MapSet.Rows.Insert(newrow);

            map.No = newrow;

            SetIntGridCell(grid_MapSet, newrow, 0, map.No);
            SetIntGridCell(grid_MapSet, newrow, 1, map.WorldID);
            int index = GetIndexFromString(map.Name, MapNameList);
            SetComboBoxGridCell(grid_MapSet, newrow, 2, MapNameList, index);

            index = GetIndexFromString(map.MapAttribute, AttributeNameList);
            SetComboBoxGridCell(grid_MapSet, newrow, 3, AttributeNameList, index);

            SetIntGridCell(grid_MapSet, newrow, 4, map.MeritBonusID);
            SetIntGridCell(grid_MapSet, newrow, 5, map.LimitLevel);
            SetIntGridCell(grid_MapSet, newrow, 6, map.ShowCount);
            SetIntGridCell(grid_MapSet, newrow, 7, map.HideCount);
            SetIntGridCell(grid_MapSet, newrow, 8, map.VillageIndex);

            grid_MapSet[newrow, 0].DataModel.EnableEdit = false;

            grid_MapSet.AutoSizeAll();
        }

        private void ConvertMultiServerID (NetLauncher launcher)
        {
            // MultiServer
            foreach (ServerInfo server in MultiWorldServer)
            {
                // Master
                if (server.LauncherID == launcher.ID && !server.Allowance && server.Type == ServerInfo.ServerType.Master)
                {
                    for (int i = 0; i < launcher.RealServer.Master.Count; i++)
                    {
                        if (launcher.RealServer.Master[i].WorldID <= 0)
                        {
                            launcher.RealServer.Master[i].WorldID = server.WorldID;
                            server.Allowance = true;
                            //AddWorldID(server.WorldID);
                            break;
                        }
                    }
                }

                // Cash
                if (server.LauncherID == launcher.ID && !server.Allowance && server.Type == ServerInfo.ServerType.Cash)
                {
                    for (int i = 0; i < launcher.RealServer.Cash.Count; i++)
                    {
                        if (launcher.RealServer.Cash[i].WorldID <= 0)
                        {
                            launcher.RealServer.Cash[i].WorldID = server.WorldID;
                            server.Allowance = true;
                            //AddWorldID(server.WorldID);
                            break;
                        }
                    }
                }
            }
        }

        private void SetTargetID(NetLauncher launcher)
        {
            foreach (ServerInfo server in TargetIPServer)
            {
                for (int i = 0; i < launcher.RealServer.Login.Count; i++ )
                {
                    if (server.TargetIP == launcher.IP)
                    {
                        server.TargetID = launcher.RealServer.Login[i].ID;
                    }
                }
            }
        }

        private void ConvertTargetIPServer ()
        {
            foreach (ServerInfo server in TargetIPServer)
                server.Allowance = false;

            
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)    
            {
                foreach (ServerInfo server in TargetIPServer)    
                {
                    for (int i = 0; i < launcher.RealServer.Master.Count; i++)
                    {
                        if (!server.Allowance && launcher.ID == server.LauncherID && launcher.RealServer.Master[i].TargetID <= 0)
                        {
                            launcher.RealServer.Master[i].TargetID = server.TargetID;
                            launcher.RealServer.Master[i].TargetIP = server.TargetIP;
                            server.Allowance = true;
                        }
                    }
                }
            }
        }

        private void ConvertServerNumber ()
        {
            foreach (ServerInfo server in ServerNumber)
                server.Allowance = false;

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                foreach (ServerInfo server in ServerNumber)
                {
                    for (int i = 0; i < launcher.RealServer.Cash.Count; i++)
                    {
                        if (!server.Allowance && launcher.ID == server.LauncherID && launcher.RealServer.Cash[i].ServerNo <= 0)
                        {
                            launcher.RealServer.Cash[i].ServerNo = server.ServerNo;
                            server.Allowance = true;
                        }
                    }
                }
            }
        }

        private void ConvertWorldIDList()
        {
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                foreach (ServerInfo server in launcher.RealServer.WDB)    
                    server.WorldIDList = WorldIDList;

                foreach (ServerInfo server in launcher.RealServer.WGame)
                    server.WorldIDList = WorldIDList;
            }
        }

        private void ConvertServerData()
        {
            ResetWorldID();
            ResetLoginServer();
            SvrStruct.ResetServerNo();
            SvrStruct.ResetVillageID();

            foreach (ServerInfo server in MultiWorldServer)
                server.Allowance = false;

            ServerInfo.ServerType ServerType;
            
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                AddWorldID(launcher.WID);

                if (launcher.LoginCount > 0)
                    AddLoginServer(launcher.ID, launcher.IP);

                launcher.ConvertServerInfo();

                ServerType = ServerInfo.ServerType.Login;
                for (int i = 0; i < launcher.LoginCount; i++)
                {
                    if (i < launcher.TempServer.Login.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Login[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Log;
                for (int i = 0; i < launcher.LogCount; i++)
                {
                    if (i < launcher.TempServer.Log.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Log[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Master;
                for (int i = 0; i < launcher.MasterCount; i++)
                {
                    if (i < launcher.TempServer.Master.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Master[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Cash;
                for (int i = 0; i < launcher.CashCount; i++)
                {
                    if (i < launcher.TempServer.Cash.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Cash[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Village;
                for (int i = 0; i < launcher.VillageCount; i++)
                {
                    if (i < launcher.TempServer.Village.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Village[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Game;
                for (int i = 0; i < launcher.GameCount; i++)
                {
                    if (i < launcher.TempServer.Game.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.Game[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.VillageDB;
                for (int i = 0; i < launcher.VillageDBCount; i++)
                {
                    if (i < launcher.TempServer.VillageDB.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.VillageDB[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.GameDB;
                for (int i = 0; i < launcher.GameDBCount; i++)
                {
                    if (i < launcher.TempServer.GameDB.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.GameDB[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.Database;
                for (int i = 0; i < launcher.DBCount; i++)
                {
                    if (i < launcher.TempServer.DataBase.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.DataBase[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.WDB;
                for (int i = 0; i < launcher.WDBCount; i++)
                {
                    if (i < launcher.TempServer.WDB.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.WDB[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ServerType = ServerInfo.ServerType.WGame;
                for (int i = 0; i < launcher.WGameCount; i++)
                {
                    if (i < launcher.TempServer.WGame.Count)
                        launcher.AddServerInfo(ServerType, launcher.TempServer.WGame[i].ID);
                    else
                        launcher.AddServerInfo(ServerType, SvrStruct.NextServerID());
                }

                ConvertMultiServerID(launcher);
                SetTargetID(launcher);
            }

            ConvertTargetIPServer();
            ConvertServerNumber();
            ConvertWorldIDList();
            ConvertExcuteName();
        }

        private void ConvertExcuteName()
        {
            // 실행명 폼에서 가져오기
            ExcuteFileName.LoginName = GetTextGridCell(grid_Excutefile, 0, 1);
            ExcuteFileName.LogName = GetTextGridCell(grid_Excutefile, 1, 1);
            ExcuteFileName.MasterName = GetTextGridCell(grid_Excutefile, 2, 1);
            ExcuteFileName.CashName = GetTextGridCell(grid_Excutefile, 3, 1);
            ExcuteFileName.DBName = GetTextGridCell(grid_Excutefile, 4, 1);
            ExcuteFileName.VillageName = GetTextGridCell(grid_Excutefile, 5, 1);
            ExcuteFileName.GameName = GetTextGridCell(grid_Excutefile, 6, 1);
        }

        private void Delete_Launcher_Click(object sender, EventArgs e)
        {
            DeleteNetLauncher();
        }

        private void DeleteNetLauncher()
        {
            if (grid_NetLauncher.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_NetLauncher.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.NetLauncherList.Count < DelIndex) return;
            if (CheckDelete(grid_NetLauncher, DelIndex) == DialogResult.No) return;

            ServerStruct.NetLauncherList.RemoveAt(DelIndex - 1);
            grid_NetLauncher.Rows.Remove(DelIndex);

            SortNetLauncherIndex();
        }

        private void AllClear_Click(object sender, EventArgs e)
        {
            ClearNetLauncherMenu();
        }

        private void ClearNetLauncherMenu()
        {
            if (CheckClear(grid_NetLauncher) == DialogResult.No) return;

            ClearNetLauncher();
        }

        private void ClearNetLauncher ()
        {
            ServerStruct.NetLauncherList.Clear();
            
            SvrStruct.ResetLauncherID();
            ServerStruct.NetLauncherList.Clear();

            int nCount = grid_NetLauncher.Rows.Count;

            for (int i = 1; i < nCount; i++)
            {
                grid_NetLauncher.Rows.Remove(1);
            }
        }

        private void ClearAllGrid()
        {           
            for (int i = 1; i < grid_NetLauncher.Rows.Count; i++)
                grid_NetLauncher.Rows.Remove(1);

            for (int i = 1; i < grid_MultiWorldID.Rows.Count; i++)
                grid_MultiWorldID.Rows.Remove(1);

            for (int i = 1; i < grid_TargetIP.Rows.Count; i++)
                grid_TargetIP.Rows.Remove(1);

            for (int i = 1; i < grid_ServerNumber.Rows.Count; i++)
                grid_ServerNumber.Rows.Remove(1);
        }

        private void button_NationApply_Click(object sender, EventArgs e)
        {
            string nation = textBox_Nation.Text;
            if (nation == "")
            {
                MessageBox.Show("Please, Input NationCode");
                return;
            }

            SetNationFileName(nation);
        }

        private void SetNationFileName (string nation)
        {
            ExcuteFileName.SetNation(nation);

            SetTextGridCell(grid_Excutefile, 0, 1, ExcuteFileName.LoginName);
            SetTextGridCell(grid_Excutefile, 1, 1, ExcuteFileName.LogName);
            SetTextGridCell(grid_Excutefile, 2, 1, ExcuteFileName.MasterName);
            SetTextGridCell(grid_Excutefile, 3, 1, ExcuteFileName.CashName);
            SetTextGridCell(grid_Excutefile, 4, 1, ExcuteFileName.DBName);
            SetTextGridCell(grid_Excutefile, 5, 1, ExcuteFileName.VillageName);
            SetTextGridCell(grid_Excutefile, 6, 1, ExcuteFileName.GameName);


            grid_Excutefile.AutoSizeAll();
        }

        private void CheckIPAddress()
        {
            
            if (!IsIPValid(ServerStruct.ServiceMng.IP))
                AddVerificationEx(TPI.Default, grid_ServiceMng, "Invalid ServiceManger IP", 1, 0);

            if (ServerStruct.ServiceMng.Port <= 0)
                AddVerificationEx(TPI.Default, grid_ServiceMng, "Invalid ServiceManger Port", 1, 1);

            if (ServerStruct.defSvrInfo.IsDoorsInclude)
            {
                if (!IsIPValid(ServerStruct.defSvrInfo.doorsServerInfo.IP))
                    AddVerificationEx(TPI.Default, grid_DoorsServerInfo, "Invalid DoorsServer IP", 1, 0);

                if (ServerStruct.defSvrInfo.doorsServerInfo.Port <= 0)
                    AddVerificationEx(TPI.Default, grid_DoorsServerInfo, "Invalid DoorsServer Port", 1, 1);
            }

            if (checkBox_DolbyAxon.Checked)
            {
                if (!IsIPValid(ServerStruct.defSvrInfo.dolbyaxon.PrivateIP))
                    AddVerification(TPI.Default, "Invalid Dolby PrivateIP", 0, 0);

                if (!IsIPValid(ServerStruct.defSvrInfo.dolbyaxon.PublicIP))
                    AddVerification(TPI.Default, "Invalid Dolby PublicIP", 0, 0);
            }

            foreach (MemberShipDBInfo member in ServerStruct.defDBinfo.memberDBList)
            {
                if (!IsIPValid(member.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_memdb, "Invalid MemeberShipDB IP", member.basicinfo.No, 1);
                

                if (member.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_memdb, "Invalid MemeberShipDB Port", member.basicinfo.No, 2);
            }

            foreach (WorldDBInfo world in ServerStruct.defDBinfo.worldDBList)
            {
                if (!IsIPValid(world.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_worlddb, "Invalid WorldDB IP", world.basicinfo.No, 2);
                
                if (world.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_worlddb, "Invalid WorldDB Port", world.basicinfo.No, 3);
                
            }

            foreach (LogDBInfo log in ServerStruct.defDBinfo.logDBList)
            {
                if (!IsIPValid(log.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_logdb, "Invalid LogDB IP", log.basicinfo.No, 1);
  

                if (log.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_logdb, "Invalid LogDB Port", log.basicinfo.No, 2);
                
            }

            foreach (StagingDBInfo stage in ServerStruct.defDBinfo.stagingDBList)
            {
                if (!IsIPValid(stage.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_stagingdb, "Invalid StagingDB IP", stage.basicinfo.No, 1);

                if (stage.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_stagingdb, "Invalid StagingDB Port", stage.basicinfo.No, 2);
            }

            foreach (GSMDBInfo gsm in ServerStruct.defDBinfo.gsmDBList)
            {
                if (!IsIPValid(gsm.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_gsmdb, "Invalid GSMDB IP", gsm.basicinfo.No, 1);

                if (gsm.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_gsmdb, "Invalid GSMDB Port", gsm.basicinfo.No, 2);
            }

            foreach (TimoseDBInfo timose in ServerStruct.defDBinfo.timoseDBList)
            {
                if (!IsIPValid(timose.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_timosedb, "Invalid TIMOSEDB IP", timose.basicinfo.No, 1);

                if (timose.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_timosedb, "Invalid TIMOSEDB Port", timose.basicinfo.No, 2);
            }

            foreach (DoorsDBInfo doors in ServerStruct.defDBinfo.doorsDBList)
            {
                if (!IsIPValid(doors.basicinfo.IP))
                    AddVerificationEx(TPI.DataBase, grid_doorsdb, "Invalid DOORSDB IP", doors.basicinfo.No, 1);

                if (doors.basicinfo.Port <= 0)
                    AddVerificationEx(TPI.DataBase, grid_doorsdb, "Invalid DOORSDB Port", doors.basicinfo.No, 2);
            }
        }

        private void CheckNetLauncher()
        {
            for (int i = 0; i < ServerStruct.NetLauncherList.Count; i++ )
            {
                if (!IsIPValid(ServerStruct.NetLauncherList[i].IP))
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Invalid NetLauncher IP [" + ServerStruct.NetLauncherList[i].ID + "]", ServerStruct.NetLauncherList[i].No, 2);
                
                for (int j = i+1; j < ServerStruct.NetLauncherList.Count; j++ )
                {
                    if (ServerStruct.NetLauncherList[i].ID == ServerStruct.NetLauncherList[j].ID)
                    {
                        AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Conflict NetLauncher ID No1:[" + ServerStruct.NetLauncherList[i].No + "]", ServerStruct.NetLauncherList[i].No, 1);
                        AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Conflict NetLauncher ID No2:[" + ServerStruct.NetLauncherList[j].No + "]", ServerStruct.NetLauncherList[j].No, 1);
                    }

                    if (ServerStruct.NetLauncherList[i].IP == ServerStruct.NetLauncherList[j].IP)
                    {
                        AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Conflict NetLauncher IP No1:[" + ServerStruct.NetLauncherList[i].No + "]", ServerStruct.NetLauncherList[i].No, 2);
                        AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Conflict NetLauncher IP No2:[" + ServerStruct.NetLauncherList[j].No + "]", ServerStruct.NetLauncherList[j].No, 2);
                    }
                }
            }

            if (ServerStruct.PID < 0)
                AddVerification(TPI.NetLauncher, "PartitionID does not existed", 0, 0);

        }

        private void CheckDataBase()
        {
            if (ServerStruct.defDBinfo.memberDBList.Count <= 0)
                AddVerification(TPI.DataBase, "Memebership DB does not existed", 0, 0);
            
            if (ServerStruct.defDBinfo.worldDBList.Count <= 0)
                AddVerification(TPI.DataBase, "World DB does not existed", 0, 0);
            
            if (ServerStruct.defDBinfo.logDBList.Count <= 0)
                AddVerification(TPI.DataBase, "Log DB does not existed", 0, 0);
            
            if (ServerStruct.defDBinfo.stagingDBList.Count <= 0)
                AddVerification(TPI.DataBase, "Staging DB does not existed", 0, 0);
            
            if (ServerStruct.defDBinfo.gsmDBList.Count <= 0)
                AddVerification(TPI.DataBase, "GSM DB does not existed", 0, 0);

            if (ServerStruct.NetLauncherList.Count <= 0)
                AddVerification(TPI.DataBase, "NetLauncher does not existed", 0, 0);


            List<int> WorldDBIDList = new List<int>();

            bool IsValidWorldID = true;

            foreach (WorldDBInfo info in ServerStruct.defDBinfo.worldDBList)
            {
                for (int i=0; i<WorldDBIDList.Count; i++)
                {
                    if (info.WorldID == WorldDBIDList[i])
                    {
                        AddVerificationEx(TPI.DataBase, grid_worlddb, "Duplicated WorldID[" + info.WorldID + "]", info.basicinfo.No, 1);
                        IsValidWorldID = false;
                        break;
                    }
                }

                if (IsValidWorldID)
                    WorldDBIDList.Add(info.WorldID);

                IsValidWorldID = true;
            }
        }

        private void CheckNetLauncherAndWorldDB()
        {
            // WorldDB -> NetLancher(WID) 여부 확인
            List<int> NoWorldDBList = new List<int>();

            bool ValidWorldDB;
            foreach (WorldDBInfo dbinfo in ServerStruct.defDBinfo.worldDBList)
            {
                ValidWorldDB = false;
                foreach (NetLauncher Iauncher in ServerStruct.NetLauncherList)
                {
                    if (dbinfo.WorldID == Iauncher.WID || Iauncher.WID == 0)
                    {
                        ValidWorldDB = true;
                        break;
                    }
                }

                if (!ValidWorldDB)
                {
                    for (int i = 0; i < NoWorldDBList.Count; i++)
                    {
                        if (NoWorldDBList[i] == dbinfo.WorldID)
                        {
                            ValidWorldDB = true;
                            break;
                        }
                    }
                }

                if (!ValidWorldDB && dbinfo.WorldID > 0)
                    NoWorldDBList.Add(dbinfo.WorldID);

            }

            for (int i = 0; i < NoWorldDBList.Count; i++)
                AddVerification(TPI.DataBase, "WorldDBID does not existed[" + NoWorldDBList[i] + "]", 0, 0);

           
            SIDList.Clear();
            
            // NetLauncher(WID) -> WorldDB
            List<int> NoWorldIDList = new List<int>();

            bool ValidWorldID;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                ValidWorldID = false;
                foreach (WorldDBInfo dbinfo in ServerStruct.defDBinfo.worldDBList)
                {
                    if (dbinfo.WorldID == launcher.WID || launcher.WID == 0)
                    {
                        ValidWorldID = true;
                        break;
                    }
                }

                if (!ValidWorldID)
                {
                    for (int i = 0; i < NoWorldIDList.Count; i++)
                    {
                        if (NoWorldIDList[i] == launcher.WID)
                        {
                            ValidWorldID = true;
                            break;
                        }
                    }
                }

                if (!ValidWorldID && launcher.WID > 0)
                    NoWorldIDList.Add(launcher.WID);

                CheckSID(launcher);
            }

            for (int i = 0; i < NoWorldIDList.Count; i++)
                AddVerification(TPI.DataBase, "World DataBase[WID:" + NoWorldIDList[i] + "] does not existed", 0, 0);


        }

        private void CheckMultiWorldID()
        {
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.RealServer.Master.Count > 1)
                {
                    for (int i=0; i<launcher.RealServer.Master.Count; i++)
                    {
                        if (!IsValidWorldID(launcher.RealServer.Master[i].WorldID))
                        {
                            AddVerification(TPI.Detail, "Invalid MultiWorldID on Master [ NID:" + launcher.ID + "  WorldID:" + launcher.RealServer.Master[i].WorldID + " ]", 0, 0);
                        }
                    }
                }

                if (launcher.RealServer.Cash.Count > 1)
                {
                    for (int i = 0; i < launcher.RealServer.Cash.Count; i++)
                    {
                        if (!IsValidWorldID(launcher.RealServer.Cash[i].WorldID))
                        {
                            AddVerification(TPI.Detail, "Invalid MultiWorldID on Cash [ NID:" + launcher.ID + "  WorldID:" + launcher.RealServer.Cash[i].WorldID + " ]", 0, 0);
                        }
                    }
                }
            }
        }

        private void CheckTargetIPServer()
        {
            foreach (ServerInfo server in TargetIPServer)
            {
                if (!IsValidLoginServer(server.TargetIP))
                {
                    AddVerification(TPI.Detail, "LoginServer IP does not existed : " + server.TargetIP + "   NID : " + server.ID, 0, 0);
                }
            }
        }

        private void CheckWorldStructure()
        {
            for (int i=0; i<WorldIDList.Count; i++)
            {
                if (WorldIDList[i] <= 0)
                    continue;

               
                // 월드당 서버갯수를 체크한다.
                int MasterCount = 0;
                int VillageCount = 0;
                int GameCount = 0;
                int VillageDBCount = 0;
                int GameDBCount = 0;
                int DBCount = 0;
                int CashCount = 0;
                foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                {
                   if (launcher.WID == WorldIDList[i])
                   {
                       MasterCount += launcher.MasterCount;
                       VillageCount += launcher.VillageCount;
                       GameCount += launcher.GameCount;
                       CashCount += launcher.CashCount;

                       VillageDBCount += launcher.VillageDBCount;
                       GameDBCount += launcher.GameDBCount;
                       DBCount += launcher.DBCount;
                   }
                }

                foreach (ServerInfo server in MultiWorldServer)
                {
                    if (server.WorldID == WorldIDList[i] && server.Type == ServerInfo.ServerType.Master)
                        MasterCount++;

                    if (server.WorldID == WorldIDList[i] && server.Type == ServerInfo.ServerType.Cash)
                        CashCount++;
                }


                if (MasterCount <= 0) 
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] Master Server does not existed", 0, 0);

                if (CashCount <= 0) 
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] Cash Server does not existed", 0, 0);
                
                if (VillageCount <= 0) AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] Village Server does not existed", 0, 0);
                if (GameCount <= 0) AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] Game Server does not existed", 0, 0);

                if (VillageDBCount <= 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] VillageDB Server does not existed", 0, 0);   
                

                if (GameDBCount <= 0)
                    AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "WorldID[" + WorldIDList[i] + "] GameDB Server does not existed", 0, 0);
            }


            int LoginCount = 0;
            int LogCount = 0;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                LoginCount += launcher.LoginCount;
                LogCount += launcher.LogCount;
            }

            if (LoginCount <= 0) AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Login Server does not existed", 0, 0);
            if (LogCount <= 0) AddVerificationEx(TPI.NetLauncher, grid_NetLauncher, "Log Server does not existed", 0, 0);
        }

        private void CheckMeritBonusTable()
        {
            if (ChannelStruct.MeritBonusTB.MeritBonusList.Count <= 0)
                AddVerification(TPI.Channel, "MeritBonusTable does not existed", 0, 0);

            foreach (MeritBonus Bonus in ChannelStruct.MeritBonusTB.MeritBonusList)
            {
                if (Bonus.ID < 0)
                    AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Invalid ID [No:" + Bonus.No + "]", Bonus.No, 1);

                if (Bonus.MinLevel < 0)
                    AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Invalid MinLevel [No:" + Bonus.No + "]", Bonus.No, 2);

                if (Bonus.MaxLevel < 0)
                    AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Invalid MaxLevel [No:" + Bonus.No + "]", Bonus.No, 3);

                if (Bonus.MinLevel >= Bonus.MaxLevel)
                {
                    AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Invalid MinLevel [No:" + Bonus.No + "]", Bonus.No, 2);
                    AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Invalid MaxLevel [No:" + Bonus.No + "]", Bonus.No, 3);
                }

                foreach (MeritBonus SearchBonus in ChannelStruct.MeritBonusTB.MeritBonusList)
                {
                    if (Bonus.ID == SearchBonus.ID && Bonus.No != SearchBonus.No)
                    {
                        AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Duplicated ID [No:" + Bonus.No + "]", Bonus.No, 1);
                        AddVerificationEx(TPI.Channel, grid_MeritBonusTable, "Duplicated ID [No:" + SearchBonus.No + "]", SearchBonus.No, 1);
                    }
                }
            }

        }

        private void CheckServerInfo()
        {
            if (ChannelStruct.ServerInfoEx.RealServerList.Count <= 0)
                AddVerification(TPI.Channel, "ServerInfo does not existed", 0, 0);

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (!IsMaxVillageIndex(server))
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "MaxVillageCount does not match to VillageIndex in MapSet", server.No, 2);

                if (!IsIPValid(server.IP))
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid IP", server.No, 3);

                if (server.Name == "")
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid Name", server.No, 4);

                if (server.WorldMaxUser <= 0)
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid WorldMaxUser", server.No, 5);

                if (server.DefaultChannelCount <= 0)
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid DefaultChannelCount", server.No, 6);

                if (server.DefaultMaxUser <= 0)
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid DefaultMaxUser", server.No, 7);

                if (!IsValidSetID(server))
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "ServerInfo Invalid SetID", server.No, 8);                
            }
        }

        private bool IsValidSetID(ServerEx checkServer)
        {
            if (checkServer.SetID == -1)
                return true;

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (server.No == checkServer.No)
                    continue;

                if (server.SetID != -1 && server.ID == checkServer.SetID)
                    return false;

                if (server.ID == checkServer.SetID)
                    return true;
            }

            return false;
        }

        private bool IsValidMeritBonus(MapSet mapSet)
        {
            if (mapSet.MeritBonusID <= 0)
            {
                if (mapSet.MapAttribute.IndexOf("PvP") > 0 || 
                    mapSet.MapAttribute.IndexOf("DarkLair") > 0 || 
                    mapSet.MapAttribute.IndexOf("FarmTown") > 0 ||
                    mapSet.MapAttribute.IndexOf("GM") > 0)
                    return true;
            }

            foreach (MeritBonus Bonus in ChannelStruct.MeritBonusTB.MeritBonusList)
            {
                if (Bonus.ID == mapSet.MeritBonusID)
                    return true;
            }

            return false;
        }

        private bool IsValidDependant(MapSet mapSet)
        {
            if (mapSet.DependentMapID > 0)
            {
                if (mapSet.MapAttribute.IndexOf("FarmTown") > 0)
                {
                    foreach (MeritBonus Bonus in ChannelStruct.MeritBonusTB.MeritBonusList)
                    {
                        if (Bonus.ID == mapSet.DependentMapID)
                            return true;
                    }
                }
                else
                    return false;
            }

            if (mapSet.DependentMapID == 0)
                return false;

            return true;
        }

        private bool IsValidMaxUser(MapSet mapSet)
        {
            if (mapSet.MaxUser == 0)
                return false;

            if (mapSet.MaxUser < 0)
            {
                if (mapSet.Name == "DarkVillage" || mapSet.Name == "ColosseumChannel" || mapSet.Name == "Farm_Town")
                    return true;
                else
                    return false;
            }

            return true;
        }

        private bool IsValidVillageIndex(MapSet mapSet, int MaxVillageIndex)
        {
            if (mapSet.VillageIndex < 0)
                return false;

            if (mapSet.VillageIndex >= MaxVillageIndex)
                return false;
           
            return true;
        }

       
        private void CheckMapSet()
        {
            if (SelectServerIndex >= ChannelStruct.ServerInfoEx.RealServerList.Count)
                return;

            if (SelectServerIndex < 0)
                return;

            ServerEx currentServer = ChannelStruct.ServerInfoEx.RealServerList[SelectServerIndex];

            foreach (ServerEx serverInfo in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (serverInfo.SetID != -1)
                    continue;

                bool IsValidMapSet = true;
                bool IsWaringMapSet = true;
                bool IsPvPLobby = false;
                foreach (MapSet mapSet in serverInfo.MapSetList)
                {
                    if (currentServer == serverInfo)
                    {
                        if (mapSet.Name.IndexOf("LobbyMap") > 0)
                        {
                            if (IsPvPLobby == false)
                                IsPvPLobby = true;
                            else
                            {
                                AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Duplicated ColosseumLobbyMap", mapSet.No, 2);
                                IsValidMapSet = false;
                            }
                        }

                        if (mapSet.IsAvailableMerit == true)
                        {
                            if (mapSet.MapAttribute.IndexOf("Merit") > 0 || mapSet.MapAttribute == "Normal")
                            {

                            }
                            else
                            {
                                AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid Attribute", mapSet.No, 3);
                                IsValidMapSet = false;
                            }
                        }
                        else
                        {
                            if (GetMapAttributeByName(mapSet.Name) != mapSet.MapAttribute)
                            {
                                AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid Attribute", mapSet.No, 3);
                                IsValidMapSet = false;
                            }
                        }

                        if (!IsValidMeritBonus(mapSet))
                        {
                            AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid MeritBonusID", mapSet.No, 4);
                            IsValidMapSet = false;
                        }

                        if (mapSet.Name.IndexOf("PrairieTown") == 0 || mapSet.Name.IndexOf("ManaRidge") == 0)
                        {
                            if (mapSet.LimitLevel != -1)
                            {
                                AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet LimitLevel must be -1", mapSet.No, 5);
                                IsValidMapSet = false;
                            }
                        }

                        if (mapSet.ShowCount <= 0 && mapSet.HideCount <= 0)
                        {
                            AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid Show Count", mapSet.No, 6);
                            AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid Hide Count", mapSet.No, 7);
                            IsValidMapSet = false;
                        }

                        if (!IsValidVillageIndex(mapSet, serverInfo.MaxVillageCount))
                        {
                            AddVerificationEx(TPI.Channel, grid_MapSet, "MapSet Invalid VillageIndex", mapSet.No, 8);
                            IsValidMapSet = false;
                        }

                        if (mapSet.ID != 40)
                        {
                            int MaxUserCount = 0;
                            foreach (MapSet TargetMapSet in serverInfo.MapSetList)
                            {
                                if (mapSet.ID == TargetMapSet.ID)
                                {
                                    if (mapSet.ShowCount > 0 && TargetMapSet.ShowCount > 0 && mapSet.VillageIndex != TargetMapSet.VillageIndex)
                                    {
                                        AddWaring(TPI.Channel, grid_MapSet, "MapSet Strange Show Count", mapSet.No, 6);
                                        AddWaring(TPI.Channel, grid_MapSet, "MapSet Strange Show Count", TargetMapSet.No, 6);
                                        IsWaringMapSet = false;
                                    }

                                    if (mapSet.ID == 41)
                                        mapSet.MaxUser = serverInfo.WorldMaxUser;
                                    else
                                    {
                                        MaxUserCount += TargetMapSet.MaxUser * (TargetMapSet.ShowCount + TargetMapSet.HideCount);
                                        if (MaxUserCount > serverInfo.WorldMaxUser)
                                        {
                                            AddWaring(TPI.Channel, grid_MapSet, "TotalUserCount in "+ TargetMapSet.Name +" is over than WorldMaxUSer", TargetMapSet.No, 2);
                                            IsWaringMapSet = false;
                                        }
                                    }                              
                                }
                            }
                        }
                    }
                    else
                    {
                        if (mapSet.Name.IndexOf("LobbyMap") > 0)
                        {
                            if (IsPvPLobby == false)
                                IsPvPLobby = true;
                            else
                                IsValidMapSet = false;
                        }

                        if (mapSet.IsAvailableMerit == true)
                        {
                            if (mapSet.MapAttribute.IndexOf("Normal|Merit") == 0 || mapSet.MapAttribute.IndexOf("Normal") == 0)
                            {

                            }
                            else
                                IsValidMapSet = false;
                        }
                        else
                        {
                            if (GetMapAttributeByName(mapSet.Name) != mapSet.MapAttribute)
                                IsValidMapSet = false;
                        }

                        if (!IsValidMeritBonus(mapSet))
                            IsValidMapSet = false;

                        if (mapSet.ShowCount <= 0 && mapSet.HideCount <= 0)
                            IsValidMapSet = false;
                        
                        if (!IsValidVillageIndex(mapSet, serverInfo.MaxVillageCount))
                            IsValidMapSet = false;

                        if (mapSet.ID != 40)
                        {
                            int MaxUserCount = 0;
                            foreach (MapSet TargetMapSet in serverInfo.MapSetList)
                            {
                                if (mapSet.ID == TargetMapSet.ID)
                                {
                                    if (mapSet.ShowCount > 0 && TargetMapSet.ShowCount > 0 && mapSet.VillageIndex != TargetMapSet.VillageIndex)
                                        IsWaringMapSet = false;

                                    if (mapSet.ID == 41)
                                        mapSet.MaxUser = serverInfo.WorldMaxUser;
                                    else
                                    {
                                        MaxUserCount += TargetMapSet.MaxUser * (TargetMapSet.ShowCount + TargetMapSet.HideCount);
                                        if (MaxUserCount > serverInfo.WorldMaxUser)
                                            IsWaringMapSet = false;
                                    }
                                }
                            }
                        }
                    }

                }


                if (!IsWaringMapSet)
                    AddWaring(TPI.Channel, grid_ServerInfo, "MapSet Warning Data", serverInfo.No, 0);

                if (!IsValidMapSet)
                    AddVerificationEx(TPI.Channel, grid_ServerInfo, "MapSet Invalid Data", serverInfo.No, 0);
            }
        }

        private void CheckFatigue()
        {
            if (ChannelStruct.IsFTGExp)
            {
                if (ChannelStruct.FTGExpTB.Exp.Value <= 0)
                    AddVerificationEx(TPI.Extra, grid_FatigueInfo, "Invalid FTGExp", 1, 1);

                if (ChannelStruct.NoFTGExpTB.Exp.Value <= 0)
                    AddVerificationEx(TPI.Extra, grid_FatigueInfo, "Invalid NoFTGExp", 2, 1);
            }
        }

        private string GetMapAttributeByName(string str)
        {
            for(int i=0; i<DefaultMapSetList.Count; i++)
            {
                if (DefaultMapSetList[i].Name == str)
                    return DefaultMapSetList[i].MapAttribute;
            }

            return "";
        }

        private void CheckErrorList()
        {
            foreach (ReportContext Error in ErrorList)
            {
                if (Error.Row == 0)
                    continue;

                if (Error.Grid != null)
                    Error.Grid[Error.Row, Error.Colum].VisualModel = OrangeRedModel;
            }

            foreach (ReportContext Waring in WaringList)
            {
                if (Waring.Row == 0)
                    continue;

                if (Waring.Grid != null)
                    Waring.Grid[Waring.Row, Waring.Colum].VisualModel = YellowModel;
            }

            
        }

        private bool IsTapPageValid(int TabPageIndex)
        {
            foreach (ReportContext Error in ErrorList)
            {
                if (Error.TabPageIndex == TabPageIndex)
                    return false;
            }

            return true;
        }

        private bool IsTapPageWaring(int TabPageIndex)
        {
            foreach (ReportContext Waring in WaringList)
            {
                if (Waring.TabPageIndex == TabPageIndex)
                    return false;
            }

            return true;
        }

        private void CheckSID(NetLauncher launcher)
        {
            foreach (ServerInfo info in launcher.RealServer.Login)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Login Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.Log)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Log Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.Master)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Master Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.Cash)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Cash Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.DataBase)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "DB Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.VillageDB)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "VillageDB Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.GameDB)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "GameDB Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.Village)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Village Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }

            foreach (ServerInfo info in launcher.RealServer.Game)
            {
                if (IsExistedSID(info))
                {
                    AddVerificationEx(TPI.Detail, grid_EditSID, "Game Duplicated SID[" + info.ID + "] No[" + info.No + "]", info.No, 5);
                }
                else
                    SIDList.Add(info);
            }
        }


        private bool IsExistedSID(ServerInfo info)
        {
            if (info.ID < 0)
                return true;

            foreach (ServerInfo serverInfo in SIDList)
            {
                if (info.ID == serverInfo.ID)
                    return true;
            }

            return false;

        }

        private bool CheckSpecificInfo()
        {
            if (CheckMultiWorldCount())
            {
                // NetLauncherList -> MultiWorldID Grid
                UpdateMultiWorldIDGrid();
            }

            if (CheckTargetIPCount())
            {
                //if (CheckTargetIP() == DialogResult.Yes)
                    UpdateTargetIPGrid();
            }

            if (CheckServerNumberCount())
            {
                //if (CheckServerNumber() == DialogResult.Yes)
                    UpdateServerNumberGrid();
            }

            return true;
        }

        private void button_Verify_Click(object sender, EventArgs e)
        {
            // 상세 정보 확인
            CheckSpecificInfo();

            ConvertServerData();

            CheckVerification();

            if (!Is_Verfification())
                MessageBox.Show("Verification Fail");
            else
                MessageBox.Show("Verification Success");

            RefreshReportGrid();
        }

        private void ClearErrorReport()
        {
            grid_report.Redim(1, 6);
            grid_report.FixedRows = 1;
            grid_report.FixedColumns = 1;

            ErrorList.Clear();
            WaringList.Clear();
        }

        private void RealTimeVerification()
        {
            // 데이터 변환
            ConvertServerData();

            CheckVerification();
        }


        private void PaintAllGrid()
        {
            // NetLauncher
            PaintNetLauncherGrid();

            // Extra
            PaintBackgroundColor(grid_EditSID);
            PaintBackgroundColor(grid_VillageInfo);
            PaintBackgroundColor(grid_ServiceMng);
            PaintBackgroundColor(grid_ServerInfo);
            PaintBackgroundColor(grid_MeritBonusTable);
            PaintBackgroundColor(grid_FatigueInfo);
            PaintBackgroundColor(grid_DoorsServerInfo);
           
            // DataBase
            PaintBackgroundColor(grid_logdb);
            PaintBackgroundColor(grid_gsmdb);
            PaintBackgroundColor(grid_stagingdb);
            PaintBackgroundColor(grid_memdb);
            PaintBackgroundColor(grid_worlddb);
            PaintBackgroundColor(grid_timosedb);
            PaintBackgroundColor(grid_doorsdb);

            PaintSelectRows(grid_ServerInfo, SelectServerIndex+1);
            PaintBackgroundColor(grid_MapSet);            
        }

        private void CheckServer()
        {
            ClearErrorReport();

            CheckIPAddress();

            CheckNetLauncher();

            CheckDataBase();

            CheckNetLauncherAndWorldDB();

            CheckMultiWorldID();

            CheckTargetIPServer();

            CheckWorldStructure();

            PaintAllGrid();

            CheckErrorList();

            CheckNationUI();
        }

        private void CheckChannel()
        {
            ClearErrorReport();

            CheckMeritBonusTable();

            CheckMapSet();

            CheckFatigue();

            PaintAllGrid();

            CheckErrorList();
        }

        private bool CheckMakeVillage()
        {
            if (IsMakeVillage)
            {
                if (DialogResult.No == MessageBox.Show("Are you sure to continue, even if village information have changed?", "Confirm", MessageBoxButtons.YesNo))
                    return false;
            }

            return true;
        }

        private void CheckVerification()
        {
            ClearErrorReport();

            CheckIPAddress();

            CheckNetLauncher();

            CheckDataBase();

            CheckNetLauncherAndWorldDB();

            CheckMultiWorldID();

            CheckTargetIPServer();

            CheckWorldStructure();

            CheckMeritBonusTable();

            CheckServerInfo();

            CheckMapSet();

            CheckFatigue();

            PaintAllGrid();

            CheckErrorList();
             
            CheckNationUI();
        }

        private int AddReportGrid(TPI TabPage, string LogText, int Row, int Colum, string Level)
        {
            int newrow = grid_report.Rows.Count;
            grid_report.Rows.Insert(newrow);

            SetTextGridCell(grid_report, newrow, 0, newrow.ToString());
            SetTextGridCell(grid_report, newrow, 1, Level.ToString());
            SetTextGridCell(grid_report, newrow, 2, TabPage.ToString());

            if (Row == 0) SetTextGridCell(grid_report, newrow, 3, "");
            else SetTextGridCell(grid_report, newrow, 3, Row.ToString());

            if (Colum == 0) SetTextGridCell(grid_report, newrow, 4, "");
            else SetTextGridCell(grid_report, newrow, 4, Colum.ToString());

            SetTextGridCell(grid_report, newrow, 5, LogText);

            grid_report[newrow, 0].DataModel.EnableEdit = false;
            grid_report.AutoSizeAll();

            PaintBackgroundColor(grid_report);

            return newrow;
        }

        private void AddVerification(TPI TabPage, string LogText, int Row, int Colum)
        {
            ReportContext Error = new ReportContext();
            Error.TabPageIndex = (int)TabPage;
            Error.LogText = LogText;
            Error.Row = Row;
            Error.Colum = Colum;
            Error.Grid = null;
            ErrorList.Add(Error);
        }

        private void AddVerificationEx(TPI TabPage, SourceGrid2.Grid Grid, string LogText, int Row, int Colum)
        {
            for (int i = 0; i < ErrorList.Count; i++)
            {
                if (ErrorList[i].TabPageIndex == (int)TabPage && ErrorList[i].LogText == LogText && ErrorList[i].Row == Row && ErrorList[i].Colum == Colum)
                    return;
            }

            ReportContext Error = new ReportContext();
            Error.TabPageIndex = (int)TabPage;
            Error.LogText = LogText;
            Error.Row = Row;
            Error.Colum = Colum;
            Error.Grid = Grid;
            ErrorList.Add(Error);
        }

        private void AddWaring(TPI TabPage, SourceGrid2.Grid Grid, string LogText, int Row, int Colum)
        {
            for (int i=0; i<WaringList.Count; i++)
            {
                if (WaringList[i].TabPageIndex == (int)TabPage && WaringList[i].LogText == LogText && WaringList[i].Row == Row && WaringList[i].Colum == Colum)
                    return;
            }

            ReportContext Waring = new ReportContext();
            Waring.TabPageIndex = (int)TabPage;
            Waring.LogText = LogText;
            Waring.Row = Row;
            Waring.Colum = Colum;
            Waring.Grid = Grid;
            WaringList.Add(Waring);
        }


        private bool Is_Verfification ()
        {
            if (ErrorList.Count > 0)
                return false;

            if (WaringList.Count > 0)
                return false;

            return true;
        }

        private void button_SortID_Click(object sender, EventArgs e)
        {
            SortNetLauncherIndex();
        }

        private void Nation_Apply_Click(object sender, EventArgs e)
        {
            if (SetNationUI(comboBox_Nation.SelectedIndex))
                MessageBox.Show("Success to change Nation Code");
        }

        private bool SetNationUI (int index)
        {
            comboBox_Nation.SelectedIndex = index;

            if (index >= UIList.Count || index == 0)
            {
                MessageBox.Show("Invalid Nation Code");
                return false;
            }
            
            // UI
            ShowDolbyUI(UIList[index].is_dolby);
            ShowServerNumUI(UIList[index].is_servernum);
            ShowTargetIpUI(UIList[index].is_targetip);
            ShowFatigueUI(UIList[index].is_fatigue);
            SetExcuteFileUI(UIList[index].nation);

            ChannelStruct.IsFTGExp = UIList[index].is_fatigue;

            if (UIList[index].is_dolby)
            {
                groupBox5.Location = new Point(403,460);
                groupBox_DoorsServerInfo.Location = new Point(403,333);
            }
            else
            {
                groupBox_DoorsServerInfo.Location = new Point(403,376);
            }

            if (!IsAddTabs)
            {
                ShowTabPage();

                IsAddTabs = true;
            }
            
            return true;
        }

        private void ShowTabPage()
        {
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Default);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_DataBase);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_NetLauncher);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Detail);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Channel);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Village);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_VillageView);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Extra);
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Verification);
        }

        private void HideTabPage()
        {
            this.tabCtrl_SvrStruct.Controls.Clear();
            this.tabCtrl_SvrStruct.Controls.Add(this.tabPage_Setup);
            
        }

        private void ShowDolbyUI(bool is_visible)
        {
            groupBox5.Visible = is_visible;
            grid_Dolby.Visible = is_visible;
            checkBox_DolbyAxon.Visible = false;
            checkBox_DolbyAxon.Checked = is_visible;
        }

        private void ShowServerNumUI (bool is_visible)
        {
            groupBox10.Visible = is_visible;
            grid_ServerNumber.Visible = is_visible;
        }

        private void ShowTargetIpUI (bool is_visible)
        {
            groupBox4.Visible = is_visible;
            grid_TargetIP.Visible = is_visible;
        }

        private void ShowFatigueUI(bool is_visible)
        {
            groupBox21.Visible = is_visible;
            grid_FatigueInfo.Visible = is_visible;
        }

        private void SetExcuteFileUI (string nation)
        {
            textBox_Nation.Text = nation;
            SetNationFileName(nation);
        }

        private void CheckNationUI ()
        {
            string ExcuteName = GetTextGridCell(grid_Excutefile, 0, 1);

            int index = ExcuteName.IndexOf("_");
            index++;

            string NationCode = ExcuteName.Substring(index, 2);

            for (int i=0; i<comboBox_Nation.Items.Count; i++)
            {
                if (comboBox_Nation.Items[i].ToString() == NationCode)
                {
                    SetNationUI(i);
                    return;
                }
            }
        }

        private void button_RefreshSIDList_Click(object sender, EventArgs e)
        {
            RefreshEditSIDMenu();
        }

        private void RefreshEditSIDMenu()
        {
            textBox_SearchWord.Text = "";

            RefreshEditSIDGrid();

            RealTimeVerification();
        }

        private void button_SortSID_Click(object sender, EventArgs e)
        {
            SortSIDMenu();
        }

        private void SortSIDMenu()
        {
            SvrStruct.ResetServerID();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                launcher.RealServer.ClearServerInfo();

            ConvertServerData();

            RefreshEditSIDGrid();
        }

        private void button_SearchSID_Click(object sender, EventArgs e)
        {
            int nIndex = comboBox_SearchType.SelectedIndex;

            string SearchWord = textBox_SearchWord.Text;

            if (SearchWord.Length <= 0)
                return;

            int nCount = grid_EditSID.Rows.Count;
            for (int i = 1; i < nCount; i++)
                grid_EditSID.Rows.Remove(1);

            if (nIndex == 3)
                RefreshEditSIDGridByType(SearchWord);
            else if (nIndex == 4)
                RefreshEditSIDGridBySID (Int32.Parse(SearchWord));
            else
                RefreshEditSIDGridByCondition(nIndex, SearchWord);

            grid_EditSID.AutoSizeAll();
            PaintBackgroundColor(grid_EditSID);
        }

        private void RefreshEditSIDGridByCondition(int nIndex, string SearchWord)
        {
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                try
                {
                    if (nIndex == 0 && launcher.WID != Int32.Parse(SearchWord))
                        continue;

                    if (nIndex == 1 && launcher.ID != Int32.Parse(SearchWord))
                        continue;

                    if (nIndex == 2 && launcher.IP != SearchWord)
                        continue;
                }
                catch
                {
                    MessageBox.Show("Invalid SeachWord!");
                    return;
                }

                foreach (ServerInfo info in launcher.RealServer.Login)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Log)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Master)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Cash)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.DataBase)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.VillageDB)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.GameDB)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Village)
                    AddEditSIDGrid(info, launcher);

                foreach (ServerInfo info in launcher.RealServer.Game)
                    AddEditSIDGrid(info, launcher);

            }
        }

        private void RefreshEditSIDGridByType(string Type)
        {
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                switch (Type)
                {
                    case "Login":
                        {
                            foreach (ServerInfo info in launcher.RealServer.Login)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "Log":
                        {
                            foreach (ServerInfo info in launcher.RealServer.Log)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "Cash":
                        {
                            foreach (ServerInfo info in launcher.RealServer.Cash)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "Database":
                        {
                            foreach (ServerInfo info in launcher.RealServer.DataBase)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "VillageDB":
                        {
                            foreach (ServerInfo info in launcher.RealServer.VillageDB)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "GameDB":
                        {
                            foreach (ServerInfo info in launcher.RealServer.GameDB)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "Village":
                        {
                            foreach (ServerInfo info in launcher.RealServer.Village)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;

                    case "Game":
                        {
                            foreach (ServerInfo info in launcher.RealServer.Game)
                                AddEditSIDGrid(info, launcher);
                        }
                        break;
                }
            }
        }

        private void RefreshEditSIDGridBySID(int SID)
        {
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                foreach (ServerInfo info in launcher.RealServer.Login)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.Log)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.Master)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.Cash)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.DataBase)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.VillageDB)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.GameDB)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.Village)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }

                foreach (ServerInfo info in launcher.RealServer.Game)
                {
                    if (info.ID == SID)
                        AddEditSIDGrid(info, launcher);
                }
                       
                
            }
        }

        private void RefreshVillageInfo()
        {
            grid_VillageInfo.Redim(1, 15);
            grid_VillageInfo.FixedRows = 1;

            int newrow;
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (server.SetID != -1)
                    continue;

                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                    {
                        newrow = grid_VillageInfo.Rows.Count;
                        grid_VillageInfo.Rows.Insert(newrow);

                        SetVillageInfoGrid(server, villageServer, villageInfo, newrow);
                    }
                }
            }

            grid_VillageInfo.AutoSizeAll();
            PaintBackgroundColor(grid_VillageInfo);
        }

        private void SetVillageInfoGrid(ServerEx server, VillageServerInfo villageServer, VillageInfo villageInfo, int newrow)
        {
            villageInfo.No = newrow;

            SetIntGridCell(grid_VillageInfo, newrow, 0, villageInfo.No);
            SetIntGridCell(grid_VillageInfo, newrow, 1, server.ID);
            SetIntGridCell(grid_VillageInfo, newrow, 2, villageServer.ID);
            SetIntGridCell(grid_VillageInfo, newrow, 3, villageServer.Index);
            SetTextGridCell(grid_VillageInfo, newrow, 4, villageServer.IP);
            SetIntGridCell(grid_VillageInfo, newrow, 5, villageServer.Port);
            SetIntGridCell(grid_VillageInfo, newrow, 6, villageInfo.CID);
            SetIntGridCell(grid_VillageInfo, newrow, 7, villageInfo.ID);
            SetTextGridCell(grid_VillageInfo, newrow, 8, villageInfo.Name);
            SetTextGridCell(grid_VillageInfo, newrow, 9, villageInfo.MapAttribute);
            SetIntGridCell(grid_VillageInfo, newrow, 10, villageInfo.MeritBonusID);
            SetTextGridCell(grid_VillageInfo, newrow, 11, villageInfo.Visibility);
            SetIntGridCell(grid_VillageInfo, newrow, 12, villageInfo.MaxUser);
            SetIntGridCell(grid_VillageInfo, newrow, 13, villageInfo.LimitLevel);
            SetIntGridCell(grid_VillageInfo, newrow, 14, villageInfo.DependentMapID);

            grid_VillageInfo[newrow, 0].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 1].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 2].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 3].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 4].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 5].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 6].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 7].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 8].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 9].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 10].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 11].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 12].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 13].DataModel.EnableEdit = false;
            grid_VillageInfo[newrow, 14].DataModel.EnableEdit = false;
        }

        private void RefreshVillageInfoByCondition(int Index, string SearchWord)
        {
            grid_VillageInfo.Redim(1, 15);
            grid_VillageInfo.FixedRows = 1;

            int newrow;
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (Index == 0)
                {
                    try
                    {
                        if (server.ID != Int32.Parse(SearchWord))
                            continue;
                    }
                    catch
                    {
                        MessageBox.Show("Please, input number");
                        return;
                    }
                    
                }
                else if (Index == 1 || Index == 2)
                {
                }
                else
                    return;

                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    if (Index == 1)
                    {
                        if (villageServer.IP != SearchWord)
                            continue;
                    }
                    else if (Index == 2)
                    {
                        try
                        {
                            if (villageServer.ID != Int32.Parse(SearchWord))
                                continue;
                        }
                        catch
                        {
                            MessageBox.Show("Invalid SeachWord!");
                            return;
                        }
                    }

                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                    {
                        newrow = grid_VillageInfo.Rows.Count;
                        grid_VillageInfo.Rows.Insert(newrow);

                        SetVillageInfoGrid(server, villageServer, villageInfo, newrow);
                    }
                }
            }

            grid_VillageInfo.AutoSizeAll();
            PaintBackgroundColor(grid_VillageInfo);

        }

        
        private void grid_report_CellGotFocus(object sender, SourceGrid2.PositionCancelEventArgs e)
        {
            int index = e.Position.Row;

            for (int i=0; i<ErrorList.Count; i++)
            {
                if (index == ErrorList[i].No)
                {
                    if (ErrorList[i].TabPageIndex == (int)TPI.Import)
                        MessageBox.Show("Please, check xml file");
                    else
                        tabCtrl_SvrStruct.SelectTab(ErrorList[i].TabPageIndex);                   
                    return;
                }
            }

            for (int i = 0; i < WaringList.Count; i++)
            {
                if (index == WaringList[i].No)
                {
                    tabCtrl_SvrStruct.SelectTab(WaringList[i].TabPageIndex);
                    return;
                }
            }
        }

        private void grid_ServerInfo_CellGotFocus(object sender, SourceGrid2.PositionCancelEventArgs e)
        {
            int SelectIndex = e.Position.Row;

            RefreshMapSet(SelectIndex);
        }

        

        private void button_VillageSearch_Click(object sender, EventArgs e)
        {
            int nIndex = comboBox_VillageType.SelectedIndex;

            string SearchWord = textBox_SearchVillage.Text;

            if (SearchWord.Length <= 0)
                return;

            RefreshVillageInfoByCondition(nIndex, SearchWord);

        }

        private void button_VillageRefresh_Click(object sender, EventArgs e)
        {
            RefreshVillageInfoMenu();
        }

        private void RefreshVillageInfoMenu()
        {
            RefreshVillageInfo();

            textBox_SearchVillage.Text = "";
        }


        private void button_AddMeritBonus_Click(object sender, EventArgs e)
        {
            AddMeritBonusMenu();
        }

        private void AddMeritBonusMenu()
        {
            MeritBonus Bonus = new MeritBonus();
            ChannelStruct.MeritBonusTB.MeritBonusList.Add(Bonus);

            AddMeritBonusGrid(Bonus);

            RealTimeVerification();
        }

        private void RefreshMeritBonusGrid()
        {
            grid_MeritBonusTable.Redim(1, 6);
            grid_MeritBonusTable.FixedRows = 1;
            foreach (MeritBonus Bonus in ChannelStruct.MeritBonusTB.MeritBonusList)
                AddMeritBonusGrid(Bonus);
        }

        private void AddMeritBonusGrid(MeritBonus Bonus)
        {
            int newrow = grid_MeritBonusTable.Rows.Count;
            grid_MeritBonusTable.Rows.Insert(newrow);

            Bonus.No = newrow;
            Bonus.ID = newrow;

           
            SetIntGridCell(grid_MeritBonusTable, newrow, 0, Bonus.No);
            SetIntGridCell(grid_MeritBonusTable, newrow, 1, Bonus.ID);
            SetIntGridCell(grid_MeritBonusTable, newrow, 2, Bonus.MinLevel);
            SetIntGridCell(grid_MeritBonusTable, newrow, 3, Bonus.MaxLevel);

            int index = GetIndexFromString(Bonus.BonusType, BonusTypeList);
            SetComboBoxGridCell(grid_MeritBonusTable, newrow, 4, BonusTypeList, index);
            
            SetIntGridCell(grid_MeritBonusTable, newrow, 5, Bonus.ExtendValue);

            grid_MeritBonusTable[newrow, 0].DataModel.EnableEdit = false;

            grid_MeritBonusTable.AutoSizeAll();

            
        }
        
        private int GetIndexFromString(string search_str, List<string> str_list)
        {
            for (int i = 0; i < str_list.Count; i++)
            {
                if (str_list[i] == search_str)
                    return i;
            }

            return -1;
        }  
        

        private void button_DelMeritBonus_Click(object sender, EventArgs e)
        {
            DeleteMeritBonusMenu();
        }

        private void DeleteMeritBonusMenu()
        {
            if (grid_MeritBonusTable.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_MeritBonusTable.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ChannelStruct.MeritBonusTB.MeritBonusList.Count < DelIndex) return;
            if (CheckDelete(grid_MeritBonusTable, DelIndex) == DialogResult.No) return;

            ChannelStruct.MeritBonusTB.MeritBonusList.RemoveAt(DelIndex - 1);
            grid_MeritBonusTable.Rows.Remove(DelIndex);

            RefreshMeritBonusGrid();

            RealTimeVerification();
        }

        private void CreateServerEx(int WorldID)
        {
            // Channel Info
            ServerEx server = new ServerEx();
            ChannelStruct.ServerInfoEx.RealServerList.Add(server);
            server.ID = WorldID;

            int VillageCount = 0;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.WID != server.ID)
                    continue;

                for (int i = 0; i < launcher.RealServer.Village.Count; i++)
                {
                    VillageServerInfo VillageServer = new VillageServerInfo();

                    VillageServer.WID = launcher.WID;
                    VillageServer.ID = launcher.RealServer.Village[i].VillageID;
                    VillageServer.IP = launcher.IP;
                    VillageServer.Port = ServerStruct.defSvrInfo.VillageClientPort + i;
                    VillageServer.Index = VillageCount++;

                    server.VillageServerList.Add(VillageServer);
                }
            }
            server.MaxVillageCount = VillageCount;

            List<MapSet> MapSetList = server.MapSetList;

            for (int i = 0; i < MapSetList.Count; i++ )
            {
                MapSetList[i].WorldID = server.ID;
                MapSetList[i].No = i + 1;
            }

            
            AddServerInfoGrid(server);
        }

        private void UpdateServerEx(ServerEx server)
        {

            int Index = 0;
            int MaxVillageCount = 0;

            List<VillageServerInfo> TempVillageServerList = new List<VillageServerInfo>();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (launcher.WID != server.ID)
                    continue;

                for (int i = 0; i < launcher.RealServer.Village.Count; i++)
                {
                    VillageServerInfo VillageServer = new VillageServerInfo();

                    VillageServer.WID = launcher.WID;
                    VillageServer.ID = launcher.RealServer.Village[i].VillageID;
                    VillageServer.IP = launcher.IP;
                    VillageServer.Port = ServerStruct.defSvrInfo.VillageClientPort + i;
                    VillageServer.Index = Index++;
                    TempVillageServerList.Add(VillageServer);
                }

                MaxVillageCount += launcher.RealServer.Village.Count;
            }

            if (MaxVillageCount <= 0)
                return;

            server.VillageServerList = TempVillageServerList;


            Index = 0;
            foreach (VillageServerInfo villageServer in server.VillageServerList)
            {
                villageServer.Index = Index++ % MaxVillageCount;
            }

            server.MaxVillageCount = MaxVillageCount;
 
               
            AddServerInfoGrid(server);
        }

        private void RefreshServerInfoGrid()
        {
            grid_ServerInfo.Redim(1, 10);
            grid_ServerInfo.FixedRows = 1;

            RefreshServerMerger();

            ChannelStruct.ServerInfoEx.RealServerList.Clear();

            foreach (WorldDBInfo World in ServerStruct.defDBinfo.worldDBList)
            {
                
                bool IsExistedServer = false;
                foreach (ServerEx server in ChannelStruct.ServerInfoEx.TempServerList)
                {
                    if (!IsIntegrateServer(server.ID))
                    {
                        if (server.ID == World.WorldID && server.SetID == -1)
                        {
                            UpdateServerEx(server);
                            ChannelStruct.ServerInfoEx.RealServerList.Add(server);
                            IsExistedServer = true;
                            break;
                        }
                    }
                }

                if (!IsExistedServer && !IsIntegrateServer(World.WorldID))
                    CreateServerEx(World.WorldID);
            }

            IntegrateServerEx();

            FinalChannel();
        }

        private void RefreshServerMerger()
        {
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                UpdateServerMerger(server.SetID, server.ID);
            }
        }

        private void FinalChannel()
        {
            SvrStruct.ResetVillageID();
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                foreach (VillageServerInfo villageInfo in server.VillageServerList)
                    villageInfo.ID = SvrStruct.NextVillageID();
            }
        }

        private void IntegrateServerEx()
        {
            foreach (Merger merger in ChannelStruct.ServerMerger.MergerList)
            {
                foreach (int i in merger.ServerIDs)
                {
                    foreach (ServerEx server in ChannelStruct.ServerInfoEx.TempServerList)
                    {
                        if (server.ID == i && server.ID != merger.SetID)
                        {
                            server.SetID = merger.SetID;   
                            ChannelStruct.ServerInfoEx.RealServerList.Add(server);
                            AddServerInfoGrid(server);
                            break;
                        }
                    }
                }
                
            }
            
        }
        

        private void button_RefreshServerInfo_Click(object sender, EventArgs e)
        {
            RefreshChannelMenu();
        }

        private void RefreshChannelMenu()
        {
            if (CheckRefreshServer() == DialogResult.No) 
                return;

            ConvertServerData();
           
            RefreshServerInfoGrid();

            RefreshMapSet(1);

            CheckVerification();
        }

        private void AddServerInfoGrid(ServerEx server)
        {
            int newrow = grid_ServerInfo.Rows.Count;
            grid_ServerInfo.Rows.Insert(newrow);

            server.No = newrow;
            
            SetIntGridCell(grid_ServerInfo, newrow, 0, server.No);
            SetIntGridCell(grid_ServerInfo, newrow, 1, server.ID);
            SetIntGridCell(grid_ServerInfo, newrow, 2, server.MaxVillageCount);
            SetTextGridCell(grid_ServerInfo, newrow, 3, server.IP);
            grid_ServerInfo[newrow, 3].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetTextGridCell(grid_ServerInfo, newrow, 4, server.Name);
            SetIntGridCell(grid_ServerInfo, newrow, 5, server.WorldMaxUser);
            SetIntGridCell(grid_ServerInfo, newrow, 6, server.DefaultChannelCount);
            SetIntGridCell(grid_ServerInfo, newrow, 7, server.DefaultMaxUser);
            SetIntGridCell(grid_ServerInfo, newrow, 8, server.SetID);
            SetIntGridCell(grid_ServerInfo, newrow, 9, server.OnTop);

            grid_ServerInfo[newrow, 0].DataModel.EnableEdit = false;
            grid_ServerInfo[newrow, 1].DataModel.EnableEdit = false;
            grid_ServerInfo[newrow, 2].DataModel.EnableEdit = false;

            grid_ServerInfo.AutoSizeAll();
        }

        private void button_AddMapSet_Click(object sender, EventArgs e)
        {
            AddMapSetMenu();
        }

        private void AddMapSetMenu()
        {
            if (SelectServerIndex < 0)
                return;

            if (ChannelStruct.ServerInfoEx.RealServerList.Count <= SelectServerIndex)
                return;

            ServerEx serverInfo = ChannelStruct.ServerInfoEx.RealServerList[SelectServerIndex];

            if (serverInfo.SetID != -1)
                return;

            List<MapSet> MapSetList = serverInfo.MapSetList;

            MapSet map = new MapSet();
            map.WorldID = serverInfo.ID;
            map.SetData(DefaultMapSetList[0]);  // 기본값

            MapSetList.Add(map);
            AddMapSetGrid(map);

            RealTimeVerification();
        }

        private void button_DelMapSet_Click(object sender, EventArgs e)
        {
            DeleteMapSetMenu();
        }

        private void DeleteMapSetMenu()
        {
            if (grid_MapSet.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            ServerEx serverInfo = ChannelStruct.ServerInfoEx.RealServerList[SelectServerIndex];
            List<MapSet> MapSetList = serverInfo.MapSetList;

            int DelIndex = grid_MapSet.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (MapSetList.Count < DelIndex) return;

            if (CheckDelete(grid_MapSet, DelIndex) == DialogResult.No) return;

            MapSetList.RemoveAt(DelIndex - 1);
            grid_MapSet.Rows.Remove(DelIndex);

            grid_MapSet.Redim(1, 9);
            grid_MapSet.FixedRows = 1;
            foreach (MapSet map in MapSetList)
                AddMapSetGrid(map);

            RealTimeVerification();
        }

        private void FinalVillageInfo()
        {
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                int CID = 1;
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    // CID 정렬
                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                        villageInfo.CID = CID++;
                }

                IntegrateVillageInfo(server);
            }
        }

        private void IntegrateVillageInfo(ServerEx ServerInte)
        {
            if (ServerInte.SetID == -1)
                return;


            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (server.ID == ServerInte.SetID)
                {
                    ServerInte.IP = server.IP;
                    ServerInte.MaxVillageCount = server.MaxVillageCount;

                    UpdateServerMerger(server.ID, server.ID);       // SetID도 ServerID에 편입시킨다
                    UpdateServerMerger(server.ID, ServerInte.ID);
                    break;
                }

            }

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.TempServerList)
            {
                if (ServerInte.ID == server.ID)
                {
                    ServerInte.Name = server.Name;
                    break;
                }
            }

        }

        private bool IsIntegrateServer(int ID)
        {
            foreach (Merger merger in ChannelStruct.ServerMerger.MergerList)
            {
                foreach (int i in merger.ServerIDs)
                {
                    if (ID == i && merger.SetID != ID)
                        return true;
                }
            }

            return false;
        }


        private void UpdateServerMerger(int SetID, int ServerID)
        {
            if (SetID == -1)
                return;

            // SetID 찾기
            foreach (Merger merger in ChannelStruct.ServerMerger.MergerList)
            {
                if (merger.SetID == SetID)
                {
                    // 업데이트
                    foreach (int i in merger.ServerIDs)
                    {
                        if (ServerID == i)
                            return;     // 이미 있음
                    }

                    merger.ServerIDs.Add(ServerID);
                    return;
                }
            }

            // 못찾음
            Merger newmerger = new Merger();
            newmerger.SetID = SetID;
            newmerger.ServerIDs.Add(ServerID);
            ChannelStruct.ServerMerger.MergerList.Add(newmerger);

        }


        private void DistributeVillageInfo()
        {
            int EachShowCount = 0;
            int LeftShowCount = 0;
            int EachHideCount = 0;
            int LeftHideCount = 0;
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                foreach (MapSet mapSet in server.MapSetList)
                {
                    if (mapSet.VillageIndex == -1)  // 분산
                    {
                        if (server.VillageServerList.Count <= 0)
                            continue;

                        if (mapSet.ShowCount > 0)
                        {
                            EachShowCount = mapSet.ShowCount / server.VillageServerList.Count;
                            LeftShowCount = mapSet.ShowCount % server.VillageServerList.Count;
                        }
                        else
                        {
                            EachShowCount = 0;
                            LeftShowCount = 0;
                        }

                        if (mapSet.HideCount > 0)
                        {
                            EachHideCount = mapSet.HideCount / server.VillageServerList.Count;
                            LeftHideCount = mapSet.HideCount % server.VillageServerList.Count;
                        }
                        else
                        {
                            EachHideCount = 0;
                            LeftHideCount = 0;
                        }

                        

                        for (int i = 0; i < LeftShowCount; i++)
                        {
                            VillageServerInfo villageServer = server.VillageServerList[i];

                            VillageInfo villageInfo = new VillageInfo();
                            villageInfo.ID = mapSet.ID;
                            villageInfo.Name = mapSet.Name;
                            villageInfo.MapAttribute = mapSet.MapAttribute;
                            villageInfo.MeritBonusID = mapSet.MeritBonusID;
                            villageInfo.Visibility = "True";
                            villageInfo.MaxUser = mapSet.MaxUser;
                            villageInfo.LimitLevel = mapSet.LimitLevel;
                            villageInfo.DependentMapID = mapSet.DependentMapID;

                            villageServer.VillageInfoList.Add(villageInfo);

                        }

                       
                        for (int i = 0; i < LeftHideCount; i++)
                        {
                            VillageServerInfo villageServer = server.VillageServerList[i];

                            VillageInfo villageInfo = new VillageInfo();
                            villageInfo.ID = mapSet.ID;
                            villageInfo.Name = mapSet.Name;
                            villageInfo.MapAttribute = mapSet.MapAttribute;
                            villageInfo.MeritBonusID = mapSet.MeritBonusID;
                            villageInfo.Visibility = "False";
                            villageInfo.MaxUser = mapSet.MaxUser;
                            villageInfo.LimitLevel = mapSet.LimitLevel;
                            villageInfo.DependentMapID = mapSet.DependentMapID;

                            villageServer.VillageInfoList.Add(villageInfo);

                        }

                        foreach (VillageServerInfo villageServer in server.VillageServerList)
                        {
                            for (int i = 0; i < EachShowCount; i++)
                            {
                                VillageInfo villageInfo = new VillageInfo();
                                villageInfo.ID = mapSet.ID;
                                villageInfo.Name = mapSet.Name;
                                villageInfo.MapAttribute = mapSet.MapAttribute;
                                villageInfo.MeritBonusID = mapSet.MeritBonusID;
                                villageInfo.Visibility = "True";
                                villageInfo.MaxUser = mapSet.MaxUser;
                                villageInfo.LimitLevel = mapSet.LimitLevel;
                                villageInfo.DependentMapID = mapSet.DependentMapID;

                                villageServer.VillageInfoList.Add(villageInfo);
                            }

                            for (int i = 0; i < EachHideCount; i++)
                            {
                                VillageInfo villageInfo = new VillageInfo();
                                villageInfo.ID = mapSet.ID;
                                villageInfo.Name = mapSet.Name;
                                villageInfo.MapAttribute = mapSet.MapAttribute;
                                villageInfo.MeritBonusID = mapSet.MeritBonusID;
                                villageInfo.Visibility = "False";
                                villageInfo.MaxUser = mapSet.MaxUser;
                                villageInfo.LimitLevel = mapSet.LimitLevel;
                                villageInfo.DependentMapID = mapSet.DependentMapID;

                                villageServer.VillageInfoList.Add(villageInfo);
                            }                     
                        }
                    }
                }
            }
        }

        private void GatherVillageInfo()
        {
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    foreach (MapSet mapSet in server.MapSetList)
                    {
                        if (villageServer.Index == mapSet.VillageIndex)
                        {
                            for (int i = 0; i < mapSet.ShowCount; i++)
                            {
                                VillageInfo villageInfo = new VillageInfo();
                                villageInfo.ID = mapSet.ID;
                                villageInfo.Name = mapSet.Name;
                                villageInfo.MapAttribute = mapSet.MapAttribute;
                                villageInfo.MeritBonusID = mapSet.MeritBonusID;
                                villageInfo.Visibility = "True";
                                villageInfo.MaxUser = mapSet.MaxUser;
                                villageInfo.LimitLevel = mapSet.LimitLevel;
                                villageInfo.DependentMapID = mapSet.DependentMapID;

                                villageServer.VillageInfoList.Add(villageInfo);
                            }

                            for (int i = 0; i < mapSet.HideCount; i++)
                            {
                                VillageInfo villageInfo = new VillageInfo();
                                villageInfo.ID = mapSet.ID;
                                villageInfo.Name = mapSet.Name;
                                villageInfo.MapAttribute = mapSet.MapAttribute;
                                villageInfo.MeritBonusID = mapSet.MeritBonusID;
                                villageInfo.Visibility = "False";
                                villageInfo.MaxUser = mapSet.MaxUser;
                                villageInfo.LimitLevel = mapSet.LimitLevel;
                                villageInfo.DependentMapID = mapSet.DependentMapID;

                                villageServer.VillageInfoList.Add(villageInfo);
                            }
                        }
                    }
                }
            }
        }

        private void ResetVillageInfo()
        {
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                    villageServer.Clear();
            }
        }

        private void ConstructVillageInfo()
        {
            ResetVillageInfo();

            DistributeVillageInfo();

            GatherVillageInfo();

            FinalVillageInfo();
        }

        private void button_MakeVillage_Click(object sender, EventArgs e)
        {
            CheckVerification();

            if (!IsTapPageValid((int)TPI.Channel))
            {
                MessageBox.Show("Do not make village Infomation, please modify wrong data in Channel");
                return;
            }

            if (!IsTapPageWaring((int)TPI.Channel))
            {
                if (DialogResult.No == MessageBox.Show("Are you sure to continue, even if there are warning ?", "Confirm", MessageBoxButtons.YesNo))
                    return;
            }

            ConstructVillageInfo();

            RefreshVillageInfo();

            MessageBox.Show("Success to make village information");

            tabCtrl_SvrStruct.SelectTab((int)TPI.Village);

            IsMakeVillage = false;
        }

        private void button_RefreshMapSet_Click(object sender, EventArgs e)
        {
            if (grid_ServerInfo.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int SelectIndex = grid_ServerInfo.Selection.SelectedRows[0].Index;
            if (SelectIndex == 0) return;  // Header!!

            RefreshMapSet(SelectIndex);
        }

        private void RefreshMapSet(int SelectIndex)
        {
            grid_MapSet.Redim(1, 9);
            grid_MapSet.FixedRows = 1;

            SelectServerIndex = SelectIndex - 1;

            if (SelectServerIndex < 0)
                return;

            if (SelectServerIndex >= ChannelStruct.ServerInfoEx.RealServerList.Count)
                return;

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (server.SetID != -1 && server.No == SelectIndex)
                {
                    PaintAllGrid();
                    return;
                }

            }

            ServerEx serverInfo = ChannelStruct.ServerInfoEx.RealServerList[SelectServerIndex];
            List<MapSet> MapSetList = serverInfo.MapSetList;

            foreach (MapSet map in MapSetList)
            {
                map.WorldID = serverInfo.ID;
                AddMapSetGrid(map);
            }

            CheckVerification();
        }

        private void button_RefreshReport_Click(object sender, EventArgs e)
        {
            RefreshReportGrid();
        }

        private void RefreshReportGrid()
        {
            grid_report.Redim(1, 6);
            grid_report.FixedRows = 1;
            grid_report.FixedColumns = 1;

            foreach (ReportContext Error in ErrorList)
            {
                Error.No = AddReportGrid((TPI)Error.TabPageIndex, Error.LogText, Error.Row, Error.Colum, "Error");
            }

            foreach (ReportContext Waring in WaringList)
            {
                Waring.No = AddReportGrid((TPI)Waring.TabPageIndex, Waring.LogText, Waring.Row, Waring.Colum, "Warning");
            }
        }

        private void MemberShipMenu_Click(object sender, EventArgs e)
        {
            AddMemberShipDB();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DeleteMemberShipDB();
        }

        private void addToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AddGsmDB();
        }

        private void deleteToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            DeleteGsmDB();
        }

        private void addToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            AddWorldDB();
        }

        private void deleteToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            DeleteWorldDB();
        }

        private void addToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            AddStagingDB();
        }

        private void deleteToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            DeleteStagingDB();
        }

        private void addToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            AddLogDB();
        }

        private void deleteToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            DeleteLogDB();
        }

        private void addToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            AddNetLauncherMenu();
        }

        private void deleteToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            DeleteNetLauncher();
        }

        private void sortNoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SortNetLauncherIndex();
        }

        private void clearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearNetLauncherMenu();
        }

        private void refreshToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RefreshEditSIDMenu();
        }

        private void sortSIDToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SortSIDMenu();
        }

        private void addToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            AddMeritBonusMenu();
        }

        private void deleteToolStripMenuItem6_Click(object sender, EventArgs e)
        {
            DeleteMeritBonusMenu();
        }

        private void refreshToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            RefreshChannelMenu();
        }

        private void addToolStripMenuItem6_Click(object sender, EventArgs e)
        {
            AddMapSetMenu();
        }

        private void deleteToolStripMenuItem7_Click(object sender, EventArgs e)
        {
            DeleteMapSetMenu();
        }

        private void refreshToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            RefreshChannelMenu();
        }

        private void refreshToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            RefreshVillageInfoMenu();
        }

        private void refreshToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            RefreshReportGrid();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            UpdateMultiWorldIDGrid();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            UpdateTargetIPGrid();
        }

        private void button_RefreshServerNum_Click(object sender, EventArgs e)
        {
            UpdateServerNumberGrid();
        }

        private void refreshToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            UpdateMultiWorldIDGrid();
        }

        private void refreshToolStripMenuItem6_Click(object sender, EventArgs e)
        {
            UpdateTargetIPGrid();
        }

        private void refreshToolStripMenuItem7_Click(object sender, EventArgs e)
        {
            UpdateServerNumberGrid();
        }

        private void button_RefreshVillageTree_Click(object sender, EventArgs e)
        {
            RefreshVillageInfoTree();
        }

        private void RefreshVillageInfoTree()
        {
            if (ChannelStruct.ServerInfoEx.RealServerList.Count <= 0)
                return;

            treeView_Village.Nodes.Clear();

            TreeNode serverInfoNode =  treeView_Village.Nodes.Add("ServerInfo");

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.RealServerList)
            {
                if (server.SetID != -1)
                    continue;

                string serverText = "Server ID=" + server.ID.ToString() + "  IP=" + server.IP + "  Name=" + server.Name + "  WorldMaxUser=" 
                                    + server.WorldMaxUser.ToString() + "  DefaultChannelCount=" + server.DefaultChannelCount.ToString() 
                                    + "  DefaultMaxUser=" + server.DefaultMaxUser.ToString();

                TreeNode serverNode = serverInfoNode.Nodes.Add(serverText);
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    string villageText = "VillageServerInfo ID=" + villageServer.ID.ToString() + "  IP=" + villageServer.IP + "  Port="+villageServer.Port.ToString();
                    TreeNode villageServerNode = serverNode.Nodes.Add(villageText);

                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                    {
                        string infoText = "VillageInfo CID=" + villageInfo.CID + "  Name=" + villageInfo.Name + "  Attribute=" + villageInfo.MapAttribute + "  Visibility=" + villageInfo.Visibility;

                        if (villageInfo.MeritBonusID > 0)
                            infoText += "  MeritBonusID=" + villageInfo.MeritBonusID.ToString();

                        if (villageInfo.LimitLevel > 0)
                            infoText += "  LimitLevel=" + villageInfo.LimitLevel.ToString();

                        if (villageInfo.DependentMapID > 0)
                            infoText += "  DependentMapID=" + villageInfo.DependentMapID.ToString();

                        if (villageInfo.MaxUser > 0)
                            infoText += "  MaxUser=" + villageInfo.MaxUser.ToString();


                        TreeNode villageInfoNode = villageServerNode.Nodes.Add(infoText);
                    }
                }
            }

        }

        private void button_RefreshServerIP_Click(object sender, EventArgs e)
        {
            RefreshServerIPGrid();
        }

        private void RefreshServerIPGrid()
        {
            grid_NetLauncherIP.Redim(1, 2);
            grid_NetLauncherIP.FixedRows = 1;

            NetLauncherIP.Clear();

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                if (GetServerIP(NetLauncherIP, launcher.IP) == "No IP")
                {
                    int newrow = grid_NetLauncherIP.Rows.Count;
                    grid_NetLauncherIP.Rows.Insert(newrow);

                    SetTextGridCell(grid_NetLauncherIP, newrow, 0, launcher.IP);

                    string DestIP = SetServerIP(NetLauncherIP, launcher.IP, "");
                    SetTextGridCell(grid_NetLauncherIP, newrow, 1, DestIP);

                    grid_NetLauncherIP[newrow, 0].DataModel.EnableEdit = false;
                    grid_NetLauncherIP[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
                }
            }

            grid_NetLauncherIP.AutoSizeAll();
            PaintBackgroundColor(grid_NetLauncherIP);
        }

        private string SetServerIP(List<ServerIP> IPList, string SourceIP, string DestIP)
        {
            foreach (ServerIP serverIp in IPList)
            {
                if (serverIp.SourceIP == SourceIP)
                {
                    if (DestIP != "")
                        serverIp.DestIP = DestIP;

                    return serverIp.DestIP;
                }
            }

            ServerIP newIp = new ServerIP();
            newIp.SourceIP = SourceIP;
            IPList.Add(newIp);

            return newIp.DestIP;
        }

        private string GetServerIP(List<ServerIP> IPList, string SourceIP)
        {
            foreach (ServerIP serverIp in IPList)
            {
                if (serverIp.SourceIP == SourceIP)
                    return serverIp.DestIP;
                
            }
            return "No IP";
        }

        private void button_ConvertServerIP_Click(object sender, EventArgs e)
        {
            if ((int)(loadType & LoadType.Server) != 1)
            {
                MessageBox.Show("Please, import ServerStruct.xml before converting");
                return;
            }

            if ((int)(loadType & LoadType.Channel) != 2)
            {
                MessageBox.Show("Please, import ChannelInfo.xml before converting");
                return;
            }

            ConvertNetLauncherIP();

            RefreshNetLauncherGrid();

            PaintNetLauncherGrid();

            RefreshEditSIDGrid();

            RefreshServerIPGrid();

            MessageBox.Show("Success to convert NetLauncherIP");
        }

        private void ConvertNetLauncherIP()
        {
            string DestIP;
            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
            {
                DestIP = GetServerIP(NetLauncherIP, launcher.IP);
                if (DestIP == "No IP" || DestIP == "")
                {
                }
                else
                    launcher.IP = DestIP;
            }
        }

        private void button_ConvertChannelIP_Click(object sender, EventArgs e)
        {
            if ((int)(loadType & LoadType.Server) != 1)
            {
                MessageBox.Show("Please, import ServerStruct.xml before converting");
                return;
            }

            if ((int)(loadType & LoadType.Channel) != 2)
            {
                MessageBox.Show("Please, import ChannelInfo.xml before converting");
                return;
            }

            ConvertChannelIP();

            RefreshServerInfoGrid();

            RefreshVillageInfo();

            RefreshChannelIPGrid();

            MessageBox.Show("Success to convert ChannelIP");
        }

        private void button_RefreshChannelIP_Click(object sender, EventArgs e)
        {
            RefreshChannelIPGrid();
        }

        private void ConvertChannelIP()
        {
            string DestIP;
            foreach (ServerEx server in ChannelStruct.ServerInfoEx.TempServerList)
            {
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    DestIP = GetServerIP(ChannelIP, villageServer.IP);
                    if (DestIP == "No IP" || DestIP == "")
                    {
                    }
                    else
                        villageServer.IP = DestIP;
                }

                DestIP = GetServerIP(ChannelIP, server.IP);
                if (DestIP == "No IP" || DestIP == "")
                {
                }
                else
                    server.IP = DestIP;
            }
        }

        private void RefreshChannelIPGrid()
        {
            grid_ChannelIP.Redim(1, 2);
            grid_ChannelIP.FixedRows = 1;

            ChannelIP.Clear();

            foreach (ServerEx server in ChannelStruct.ServerInfoEx.TempServerList)
            {
                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    if (GetServerIP(ChannelIP, villageServer.IP) == "No IP")
                    {
                        int newrow = grid_ChannelIP.Rows.Count;
                        grid_ChannelIP.Rows.Insert(newrow);

                        SetTextGridCell(grid_ChannelIP, newrow, 0, villageServer.IP);

                        string DestIP = SetServerIP(ChannelIP, villageServer.IP, "");
                        SetTextGridCell(grid_ChannelIP, newrow, 1, DestIP);

                        grid_ChannelIP[newrow, 0].DataModel.EnableEdit = false;
                        grid_ChannelIP[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
                    }
                }
            }

            grid_ChannelIP.AutoSizeAll();
            PaintBackgroundColor(grid_ChannelIP);
        }

        private void button_RefreshMultiWorld_Click(object sender, EventArgs e)
        {
            if (CheckMultiWorldCount())
            {
                // NetLauncherList -> MultiWorldID Grid
                UpdateMultiWorldIDGrid();
            }
        }

        private void button_RefreshTargetIP_Click(object sender, EventArgs e)
        {
            if (CheckTargetIPCount())
                UpdateTargetIPGrid();
        }

        private void button_RefreshServerNum_Click_1(object sender, EventArgs e)
        {
            if (CheckServerNumberCount())
                UpdateServerNumberGrid();
        }

        private void button_ApplyPID_Click(object sender, EventArgs e)
        {
            ApplyPID();
        }

        private void ApplyPID()
        {
            if (textBox_PID.Text == "")
                return;

            int PID = Int32.Parse(textBox_PID.Text);

            if (PID < 0)
            {
                textBox_PID.Text = "";
                MessageBox.Show("Invalid PartitionID");
                return;
            }

            foreach (NetLauncher launcher in ServerStruct.NetLauncherList)
                launcher.PID = PID;


            MessageBox.Show("Success to apply PID");
        }

        private void button_SpreadTree_Click(object sender, EventArgs e)
        {
            foreach (TreeNode Node in treeView_Village.Nodes)
                Node.ExpandAll();   
        }

        private void AddtimosedbGrid(TimoseDBInfo dbinfo)
        {
            int newrow = grid_timosedb.Rows.Count;
            grid_timosedb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_timosedb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_timosedb, newrow, 1, dbinfo.basicinfo.IP);
            grid_timosedb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_timosedb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_timosedb, newrow, 3, dbinfo.basicinfo.DBName);

            grid_timosedb.AutoSizeAll();
        }

        private void AddTimoseDB()
        {
            TimoseDBInfo dbinfo = new TimoseDBInfo();
            ServerStruct.defDBinfo.timoseDBList.Add(dbinfo);

            AddtimosedbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_timoseadd_Click(object sender, EventArgs e)
        {
            AddTimoseDB();
        }

        private void DeleteTimoseDB()
        {
            if (grid_timosedb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_timosedb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.timoseDBList.Count < DelIndex) return;
            if (CheckDelete(grid_timosedb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.timoseDBList.RemoveAt(DelIndex - 1);
            grid_timosedb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (TimoseDBInfo dbinfo in ServerStruct.defDBinfo.timoseDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_timosedb.Redim(1, 4);
            grid_timosedb.FixedRows = 1;
            foreach (TimoseDBInfo dbinfo in ServerStruct.defDBinfo.timoseDBList)
                AddtimosedbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_timosedel_Click(object sender, EventArgs e)
        {
            DeleteTimoseDB();
        }
        
        private void button_doorsadd_Click(object sender, EventArgs e)
        {
            AddDoorsDB();
        }

        private void AddDoorsDB()
        {
            DoorsDBInfo dbInfo = new DoorsDBInfo();
            ServerStruct.defDBinfo.doorsDBList.Add(dbInfo);

            AddDoorsdbGrid(dbInfo);

            RealTimeVerification();

        }
        private void AddDoorsdbGrid(DoorsDBInfo dbinfo)
        {
            int newrow = grid_doorsdb.Rows.Count;
            grid_doorsdb.Rows.Insert(newrow);

            dbinfo.basicinfo.No = newrow;
            SetIntGridCell(grid_doorsdb, newrow, 0, dbinfo.basicinfo.No);
            SetTextGridCell(grid_doorsdb, newrow, 1, dbinfo.basicinfo.IP);
            grid_doorsdb[newrow, 1].DataModel.Validating += new SourceGrid2.ValidatingCellEventHandler(grid_IP_Validating);
            SetIntGridCell(grid_doorsdb, newrow, 2, dbinfo.basicinfo.Port);
            SetTextGridCell(grid_doorsdb, newrow, 3, dbinfo.basicinfo.DBName);

            grid_doorsdb.AutoSizeAll();
        }

        private void DeleteDoorsDB()
        {
            if (grid_doorsdb.Selection.SelectedRows.Length != 1)
            {
                MessageBox.Show("Please, Select Only One");
                return;
            }

            int DelIndex = grid_doorsdb.Selection.SelectedRows[0].Index;
            if (DelIndex == 0) return;  // Header!!
            if (ServerStruct.defDBinfo.doorsDBList.Count < DelIndex) return;
            if (CheckDelete(grid_doorsdb, DelIndex) == DialogResult.No) return;

            ServerStruct.defDBinfo.doorsDBList.RemoveAt(DelIndex - 1);
            grid_doorsdb.Rows.Remove(DelIndex);

            int nIndex = 1;
            foreach (DoorsDBInfo dbinfo in ServerStruct.defDBinfo.doorsDBList)
            {
                dbinfo.basicinfo.No = nIndex++;
            }

            grid_doorsdb.Redim(1, 4);
            grid_doorsdb.FixedRows = 1;
            foreach (DoorsDBInfo dbinfo in ServerStruct.defDBinfo.doorsDBList)
                AddDoorsdbGrid(dbinfo);

            RealTimeVerification();
        }

        private void button_doorsdel_Click(object sender, EventArgs e)
        {
            DeleteDoorsDB();
        }

        private void addToolStripMenuItem_Timose_Click(object sender, EventArgs e)
        {
            AddTimoseDB();
        }

        private void delToolStripMenuItem_Timose_Click(object sender, EventArgs e)
        {
            DeleteTimoseDB();
        }

        private void addToolStripMenuItem_Doors_Click(object sender, EventArgs e)
        {
            AddDoorsDB();
        }

        private void delToolStripMenuItem_Doors_Click(object sender, EventArgs e)
        {
            DeleteDoorsDB();
        }

        private void checkBox_DoorServer_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_DoorServer.Checked)
            {
                ServerStruct.defSvrInfo.IsDoorsInclude = true;
            }
            else
            {
                ServerStruct.defSvrInfo.IsDoorsInclude = false;
            }
        }
    }
}