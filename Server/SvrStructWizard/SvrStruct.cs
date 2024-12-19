using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;

namespace SvrStructWizard
{
    public class XmlHelper
    {
        static public XmlNode FindChild(XmlNode node, string childname)
        {
            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.Name == childname)
                {
                    return n;
                }
            }

            return null;
        }

        static public XmlAttribute FindAttribute(XmlNode node, string attname)
        {
            foreach (XmlAttribute att in node.Attributes)
            {
                if (att.Name == attname)
                {
                    return att;
                }
            }

            return null;
        }
    }

    public class ServiceManager
    {
        public string IP;
        public int Port;
        
        public ServiceManager()
        {
            Clear();
        }

        public void Clear()
        {
            IP = "000.000.000.000";
            Port = 8610;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("ServiceManager");
            XmlAttribute att;

            att = doc.CreateAttribute("IP"); att.Value = IP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Port"); att.Value = Port.ToString(); retElement.Attributes.Append(att);

            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            if (node == null) return "No Node";

            if (XmlHelper.FindAttribute(node, "IP") == null) return "No IP";
            if (XmlHelper.FindAttribute(node, "Port") == null) return "No Port";

            IP = XmlHelper.FindAttribute(node, "IP").Value;
            Port = Int32.Parse(XmlHelper.FindAttribute(node, "Port").Value);

            return null ;
        }
    }

    public class DoorsServerInfo
    {
        public string IP;
        public int Port;

        public DoorsServerInfo()
        {
            Clear();
        }

        public void Clear()
        {
            IP = "000.000.000.000";
            Port = 11111;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("DoorsInfo");
            XmlAttribute att;

            att = doc.CreateAttribute("IP"); att.Value = IP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Port"); att.Value = Port.ToString(); retElement.Attributes.Append(att);
            
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "IP") == null) return false;
            if (XmlHelper.FindAttribute(node, "Port") == null) return false;

            IP = XmlHelper.FindAttribute(node, "IP").Value;
            Port = Int32.Parse(XmlHelper.FindAttribute(node, "Port").Value);

            return true;
        }
    }
   
    public class DolbyAxon
    {
        public int WorldID;
        public string PrivateIP;
        public string PublicIP;
        public int APort;
        public int CPort;
        
        public DolbyAxon()
        {
            Clear();
        }

        public void Clear()
        {
            WorldID = 0;
            PrivateIP = "000.000.000.000";
            PublicIP = "000.000.000.000";
            APort = 30000;
            CPort = 33000;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("DolbyAxon");
            XmlAttribute att;

            att = doc.CreateAttribute("WorldID"); att.Value = WorldID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("PrivateIP"); att.Value = PrivateIP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("PublicIP"); att.Value = PublicIP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("APort");   att.Value = APort.ToString();   retElement.Attributes.Append(att);
            att = doc.CreateAttribute("CPort");   att.Value = CPort.ToString();   retElement.Attributes.Append(att);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "WorldID") == null) return false;
            if (XmlHelper.FindAttribute(node, "PrivateIP") == null) return false;
            if (XmlHelper.FindAttribute(node, "PublicIP") == null) return false;
            if (XmlHelper.FindAttribute(node, "APort") == null) return false;
            if (XmlHelper.FindAttribute(node, "CPort") == null) return false;

            WorldID = Int32.Parse(XmlHelper.FindAttribute(node, "WorldID").Value);
            PrivateIP = XmlHelper.FindAttribute(node, "PrivateIP").Value;
            PublicIP = XmlHelper.FindAttribute(node, "PublicIP").Value;
            APort = Int32.Parse(XmlHelper.FindAttribute(node, "APort").Value);
            CPort = Int32.Parse(XmlHelper.FindAttribute(node, "CPort").Value);

            return true;
        }
    }

    public class DefaultServerInfo
    {
        public int SocketMax;
        public int DBPort;
        public int LoginMasterPort;
        public int LoginClientPort;
        public int MasterVillagePort;
        public int MasterGamePort;
        public int VillageClientPort;
        public int GameClientPortUDP;
        public int GameClientPortTCP;
        public int CashPort;
        public int LogPort;
        public bool IsDolby;
        public DolbyAxon dolbyaxon;
        public bool IsDoorsInclude;
        public DoorsServerInfo doorsServerInfo;

        public DefaultServerInfo()
        {
            Clear();
            dolbyaxon = new DolbyAxon();
            doorsServerInfo = new DoorsServerInfo();
        }

        public void Clear()
        {
            SocketMax = 1000;
            DBPort = 26000;
            LoginMasterPort = 17000;
            LoginClientPort = 14300;
            MasterVillagePort = 18000;
            MasterGamePort = 19000;
            VillageClientPort = 14400;
            GameClientPortUDP = 15100;
            GameClientPortTCP = 14500;
            CashPort = 12600;
            LogPort = 30000;
            IsDolby = false;
            IsDoorsInclude = false;

            if (doorsServerInfo != null)
                doorsServerInfo.Clear();

            if (dolbyaxon != null)
                dolbyaxon.Clear();

        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("DefaultServerInfo");
            XmlElement element;
            XmlAttribute att;

            element = doc.CreateElement("SocketMax");
            att = doc.CreateAttribute("Socket"); att.Value = SocketMax.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("DBPort");
            att = doc.CreateAttribute("Port"); att.Value = DBPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("LoginMasterPort");
            att = doc.CreateAttribute("Port"); att.Value = LoginMasterPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("LoginClientPort");
            att = doc.CreateAttribute("Port"); att.Value = LoginClientPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("MasterVillagePort");
            att = doc.CreateAttribute("Port"); att.Value = MasterVillagePort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("MasterGamePort");
            att = doc.CreateAttribute("Port"); att.Value = MasterGamePort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("VillageClientPort");
            att = doc.CreateAttribute("Port"); att.Value = VillageClientPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("GameClientPortUDP");
            att = doc.CreateAttribute("Port"); att.Value = GameClientPortUDP.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("GameClientPortTCP");
            att = doc.CreateAttribute("Port"); att.Value = GameClientPortTCP.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("CashPort");
            att = doc.CreateAttribute("Port"); att.Value = CashPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            element = doc.CreateElement("LogPort");
            att = doc.CreateAttribute("Port"); att.Value = LogPort.ToString(); element.Attributes.Append(att);
            retElement.AppendChild(element);

            if (IsDolby)
                retElement.AppendChild(dolbyaxon.ToXML(doc));

            if (IsDoorsInclude)
                retElement.AppendChild(doorsServerInfo.ToXML(doc));

            return retElement;
        }

        public string FromXML(XmlNode node)
        {   
            if (node == null) return "No Node";

            if (XmlHelper.FindChild(node, "SocketMax") == null) return "SocketMax";
            if (XmlHelper.FindChild(node, "DBPort") == null) return "DBPort";
            if (XmlHelper.FindChild(node, "LoginMasterPort") == null) return "LoginMasterPort";
            if (XmlHelper.FindChild(node, "LoginClientPort") == null) return "LoginClientPort";
            if (XmlHelper.FindChild(node, "MasterVillagePort") == null) return "MasterVillagePort";
            if (XmlHelper.FindChild(node, "MasterGamePort") == null) return "MasterGamePort";
            if (XmlHelper.FindChild(node, "VillageClientPort") == null) return "VillageClientPort";
            if (XmlHelper.FindChild(node, "GameClientPortUDP") == null) return "GameClientPortUDP";
            if (XmlHelper.FindChild(node, "GameClientPortTCP") == null) return "GameClientPortTCP";
            if (XmlHelper.FindChild(node, "CashPort") == null) return "CashPort";
            if (XmlHelper.FindChild(node, "LogPort") == null) return "LogPort";
            if (XmlHelper.FindChild(node, "DolbyAxon") != null)
            {
                dolbyaxon.FromXML(XmlHelper.FindChild(node, "DolbyAxon"));
                IsDolby = true;
            }
            if (XmlHelper.FindChild(node, "DoorsInfo") != null)
            {
                doorsServerInfo.FromXML(XmlHelper.FindChild(node, "DoorsInfo"));
                IsDoorsInclude = true;
            }
            
            try
            {
                SocketMax = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "SocketMax"), "Socket").Value);
                DBPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "DBPort"), "Port").Value);
                LoginMasterPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "LoginMasterPort"), "Port").Value);
                LoginClientPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "LoginClientPort"), "Port").Value);
                MasterVillagePort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "MasterVillagePort"), "Port").Value);
                MasterGamePort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "MasterGamePort"), "Port").Value);
                VillageClientPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "VillageClientPort"), "Port").Value);
                GameClientPortUDP = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "GameClientPortUDP"), "Port").Value);
                GameClientPortTCP = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "GameClientPortTCP"), "Port").Value);
                CashPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "CashPort"), "Port").Value);
                LogPort = Int32.Parse(XmlHelper.FindAttribute(XmlHelper.FindChild(node, "LogPort"), "Port").Value);
                
            }
            catch (System.Exception)
            {
                return "Parsing Error";
            }
            return null ;
        }
    }

    public class BasicDBSvrInfo
    {
        public int No;
        public string IP;
        public int Port;
        public string DBID;
        public string DBName;

        public BasicDBSvrInfo()
        {
            IP = "000.000.000.000";
            Port = 43000;
            DBID = "";
            DBName = "";
        }

        public void SetBasicInfo(XmlDocument doc, XmlNode node)
        {
            XmlAttribute att;

            att = doc.CreateAttribute("IP"); att.Value = IP; node.Attributes.Append(att);
            att = doc.CreateAttribute("Port"); att.Value = Port.ToString(); node.Attributes.Append(att);
            if (DBID.Length > 0) { att = doc.CreateAttribute("DBID"); att.Value = DBID; node.Attributes.Append(att); }
            att = doc.CreateAttribute("DBName"); att.Value = DBName; node.Attributes.Append(att);
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "IP") == null) return false;
            if (XmlHelper.FindAttribute(node, "Port") == null) return false;
            if (XmlHelper.FindAttribute(node, "DBName") == null) return false;

            IP = XmlHelper.FindAttribute(node, "IP").Value;
            Port = Int32.Parse(XmlHelper.FindAttribute(node, "Port").Value);
            if (XmlHelper.FindAttribute(node, "DBID") != null) { DBID = XmlHelper.FindAttribute(node, "DBID").Value; }
            DBName = XmlHelper.FindAttribute(node, "DBName").Value;

            return true;
        }
    }

    public class MemberShipDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public MemberShipDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "DragonNest";
            basicinfo.DBName = "DNMembership";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("MemberShip");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class WorldDBInfo
    {
        public int WorldID;
        public BasicDBSvrInfo basicinfo;
        public WorldDBInfo()
        {
            WorldID = 1;
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "DragonNest";
            basicinfo.DBName = "DNWorld";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("World");
            XmlAttribute att;

            att = doc.CreateAttribute("WorldID"); att.Value = WorldID.ToString(); retElement.Attributes.Append(att);
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "WorldID") == null) return false;
            WorldID = Int32.Parse(XmlHelper.FindAttribute(node, "WorldID").Value);

            return basicinfo.FromXML(node);
        }
    }

    public class LogDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public LogDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "DragonNest";
            basicinfo.DBName = "DNServerLog";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Log");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class GSMDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public GSMDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "";
            basicinfo.DBName = "DNGSM";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("GSM");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class StagingDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public StagingDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "";
            basicinfo.DBName = "DNStaging";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Staging");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class TimoseDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public TimoseDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "";
            basicinfo.DBName = "TIMOSE";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Timose");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class DoorsDBInfo
    {
        public BasicDBSvrInfo basicinfo;
        public DoorsDBInfo()
        {
            basicinfo = new BasicDBSvrInfo();
            basicinfo.IP = "000.000.000.000";
            basicinfo.Port = 43000;
            basicinfo.DBID = "";
            basicinfo.DBName = "DOORS";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Doors");
            basicinfo.SetBasicInfo(doc, retElement);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            return basicinfo.FromXML(node);
        }
    }

    public class DefaultDatabaseInfo
    {
        public List<MemberShipDBInfo> memberDBList;
        public List<WorldDBInfo> worldDBList;
        public List<LogDBInfo> logDBList;
        public List<GSMDBInfo> gsmDBList;
        public List<StagingDBInfo> stagingDBList;
        public List<TimoseDBInfo> timoseDBList;
        public List<DoorsDBInfo> doorsDBList;

        public DefaultDatabaseInfo()
        {
            memberDBList = new List<MemberShipDBInfo>();
            worldDBList = new List<WorldDBInfo>();
            logDBList = new List<LogDBInfo>();
            gsmDBList = new List<GSMDBInfo>();
            stagingDBList = new List<StagingDBInfo>();
            timoseDBList = new List<TimoseDBInfo>();
            doorsDBList = new List<DoorsDBInfo>();
        }

        public void Clear()
        {
            memberDBList.Clear();
            worldDBList.Clear();
            logDBList.Clear();
            gsmDBList.Clear();
            stagingDBList.Clear();
            timoseDBList.Clear();
            doorsDBList.Clear();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("DefaultDataBaseInfo");

            foreach (MemberShipDBInfo member in memberDBList)
            {
                retElement.AppendChild(member.ToXML(doc));
            }
            foreach (WorldDBInfo world in worldDBList)
            {
                retElement.AppendChild(world.ToXML(doc));
            }
            foreach (LogDBInfo log in logDBList)
            {
                retElement.AppendChild(log.ToXML(doc));
            }
            foreach (GSMDBInfo log in gsmDBList)
            {
                retElement.AppendChild(log.ToXML(doc));
            }
            foreach (StagingDBInfo log in stagingDBList)
            {
                retElement.AppendChild(log.ToXML(doc));
            }
            foreach (TimoseDBInfo log in timoseDBList)
            {
                retElement.AppendChild(log.ToXML(doc));
            }
            foreach (DoorsDBInfo log in doorsDBList)
            {
                retElement.AppendChild(log.ToXML(doc));
            }
            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            if (node == null) return "No Node";

            foreach(XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element)      continue;
                if (n.Name == "MemberShip")
                {
                    MemberShipDBInfo dbinfo = new MemberShipDBInfo();
                    if (dbinfo.FromXML(n) == false)
                        return n.Name;

                    memberDBList.Add(dbinfo);
                }
                else if (n.Name == "World")
                {
                    WorldDBInfo dbinfo = new WorldDBInfo();
                    if (dbinfo.FromXML(n) == false)
                        return n.Name;

                    worldDBList.Add(dbinfo);
                }
                else if (n.Name == "Log")
                {
                    LogDBInfo dbinfo = new LogDBInfo();
                    if (dbinfo.FromXML(n) == false)
                        return n.Name;

                    logDBList.Add(dbinfo);
                }
                else if (n.Name == "GSM")
                {
                    GSMDBInfo dbinfo = new GSMDBInfo();
                    if (dbinfo.FromXML(n) == false)
                        return n.Name;

                    gsmDBList.Add(dbinfo);
                }
                else if (n.Name == "staging" || n.Name == "Staging")
                {
                    StagingDBInfo dbinfo = new StagingDBInfo();
                    if (dbinfo.FromXML(n) == false)
                        return n.Name;

                    stagingDBList.Add(dbinfo);
                }
                else if (n.Name == "timose" || n.Name == "Timose")
                {
                    TimoseDBInfo dbInfo = new TimoseDBInfo();
                    if (dbInfo.FromXML(n) == false)
                        return n.Name;

                    timoseDBList.Add(dbInfo);
                }
                else if (n.Name == "doors" || n.Name == "Doors")
                {
                    DoorsDBInfo dbInfo = new DoorsDBInfo();
                    if( dbInfo.FromXML(n) == false )
                        return n.Name;

                    doorsDBList.Add(dbInfo);
                }
                else
                    return n.Name;

                
            }
            return null;
        }
    }

    public class ExcuteFileName
    {
        public static string LoginName;
        public static string LogName;
        public static string MasterName;
        public static string CashName;
        public static string DBName;
        public static string VillageName;
        public static string GameName;

        public static void SetNation (string nation)
        {
            if (nation == "KR" || nation == "US")
                LoginName = "DNLoginServerH_" + nation + ".exe";
            else
                LoginName = "DNLoginServerH64_" + nation + ".exe";

            LogName = "DNLogServer64_" + nation + ".exe";
            MasterName = "DNMasterServer64_" + nation + ".exe";
            CashName = "DNCashServer64_" + nation + ".exe";
            DBName = "DNDBServer64_" + nation + ".exe";
            VillageName = "DNVillageServerRH64_" + nation + ".exe";
            GameName = "DNGameServerH64_" + nation + ".exe";
        }

        public static string GetExcuteFileName(ServerInfo.ServerType type)
        {
            switch(type)
            {
                case ServerInfo.ServerType.GameDB: return DBName;
                case ServerInfo.ServerType.VillageDB: return DBName;
                case ServerInfo.ServerType.Login: return LoginName;
                case ServerInfo.ServerType.Master: return MasterName;
                case ServerInfo.ServerType.Village: return VillageName;
                case ServerInfo.ServerType.Game: return GameName;
                case ServerInfo.ServerType.Log: return LogName;
                case ServerInfo.ServerType.Cash: return CashName;
                case ServerInfo.ServerType.Database: return DBName;
                case ServerInfo.ServerType.WDB: return DBName;
                case ServerInfo.ServerType.WGame: return GameName;
            }

            return "";
        }
    }

    public class ServerInfo
    {
        public enum ServerType
        {
            NotDefine,
            Login,
            Log,
            Master,
            Cash,
            GameDB,
            VillageDB,
            Village,
            Game,
            Database,   // For Attribute 가 없는 경우를 처리한다.
            WDB,
            WGame,
            Max,
        };

        public int No;
        public int ID;
        public ServerType Type;
        public int LauncherID;
        public int VillageID;
        public int WorldID;
        public int TargetID;
        public string TargetIP;
        public bool Allowance;
        public int ServerNo;
        public List<int> WorldIDList;
        
        
        public string ExecuteFileName;

        public ServerInfo()
        {
            No = 0;
            ID = 0;
            Type = ServerType.NotDefine;
            LauncherID = 0;
            ExecuteFileName = "";
            WorldID = -1;
            VillageID = 0;
            TargetID = -1;
            TargetIP = "000.000.000.000";
            Allowance = false;
            ServerNo = -1;
            WorldIDList = new List<int>();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("ServerInfo");
            XmlAttribute att;

            att = doc.CreateAttribute("SID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Type");
            switch (Type)
            {
                case ServerType.GameDB: att.Value = "DB"; break;
                case ServerType.VillageDB: att.Value = "DB"; break;
                case ServerType.Login: att.Value = "Login"; break;
                case ServerType.Master: att.Value = "Master"; break;
                case ServerType.Village: att.Value = "Village"; break;
                case ServerType.Game: att.Value = "Game"; break;
                case ServerType.Log: att.Value = "Log"; break;
                case ServerType.Cash: att.Value = "Cash"; break;
                case ServerType.Database: att.Value = "DB"; break;
                case ServerType.WGame: att.Value = "Game"; break;
                case ServerType.WDB: att.Value = "DB"; break;

                default:
                    att = null;
                    break;
            }
            if (att != null) retElement.Attributes.Append(att);
            att = doc.CreateAttribute("AssignedLauncherID"); att.Value = LauncherID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("ExcuteFileName"); att.Value = ExcuteFileName.GetExcuteFileName(Type); if (att != null) retElement.Attributes.Append(att);

            if (ServerNo > 0)
            {
                att = doc.CreateAttribute("ServerNum");
                att.Value = ServerNo.ToString();
                retElement.Attributes.Append(att);
            }
            
            att = doc.CreateAttribute("For");
            switch (Type)
            {
                case ServerType.VillageDB: att.Value = "Village"; break;
                case ServerType.GameDB: att.Value = "Game"; break;
                default:
                    att = null;
                    break;
            }
            if (att != null) retElement.Attributes.Append(att);

            switch (Type)
            {
                case ServerType.WDB:
                case ServerType.WGame:
                    {
                        for (int i=0; i<WorldIDList.Count; i++)
                        {
                            XmlElement node = doc.CreateElement("WorldID");
                            XmlAttribute node_att = doc.CreateAttribute("ID");

                            node_att.Value = WorldIDList[i].ToString();
                            node.Attributes.Append(node_att);
                            retElement.AppendChild(node);
                        }
                    }
                    break;
                case ServerType.Village:
                    {
                        XmlElement node = doc.CreateElement("VillageID");
                        XmlAttribute node_att = doc.CreateAttribute("ID");
                        node_att.Value = VillageID.ToString();
                        node.Attributes.Append(node_att);
                        retElement.AppendChild(node);
                    }
                    break;

                case ServerType.Master:
                    {
                        if (WorldID > 0)
                        {
                            XmlElement node = doc.CreateElement("WorldID");
                            XmlAttribute node_att = doc.CreateAttribute("ID");
                            node_att.Value = WorldID.ToString();
                            node.Attributes.Append(node_att);
                            retElement.AppendChild(node);
                        }

                        if (TargetID > 0)
                        {
                            XmlElement node = doc.CreateElement("SIDConnection");
                            XmlAttribute node_att;

                            node_att = doc.CreateAttribute("Type");
                            node_att.Value = "Login";
                            node.Attributes.Append(node_att);

                            node_att = doc.CreateAttribute("SID");
                            node_att.Value = TargetID.ToString();
                            node.Attributes.Append(node_att);

                            retElement.AppendChild(node);
                        }
                    }
                    break;

                case ServerType.Cash:
                    {
                        if (WorldID > 0)
                        {
                            XmlElement node = doc.CreateElement("WorldID");
                            XmlAttribute node_att = doc.CreateAttribute("ID");
                            node_att.Value = WorldID.ToString();
                            node.Attributes.Append(node_att);
                            retElement.AppendChild(node);
                        }
                    }
                    break;
            }
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "SID") == null) return false;
            if (XmlHelper.FindAttribute(node, "Type") == null) return false;
            if (XmlHelper.FindAttribute(node, "AssignedLauncherID") == null) return false;
            if (XmlHelper.FindAttribute(node, "ExcuteFileName") == null) return false;

            string xmlType = XmlHelper.FindAttribute(node, "Type").Value;
            if (xmlType == "DB")
            {
                Type = ServerType.Database;
                if (XmlHelper.FindAttribute(node, "For") != null)
                {
                    if (XmlHelper.FindAttribute(node, "For").Value == "Game") Type = ServerType.GameDB;
                    else if (XmlHelper.FindAttribute(node, "For").Value == "Village") Type = ServerType.VillageDB;
                }
            }
            else if (xmlType == "Login") { Type = ServerType.Login; }
            else if (xmlType == "Master") { Type = ServerType.Master; }
            else if (xmlType == "Village") { Type = ServerType.Village; }
            else if (xmlType == "Game") { Type = ServerType.Game; }
            else if (xmlType == "Log") { Type = ServerType.Log; }
            else if (xmlType == "Cash") { Type = ServerType.Cash; }
            else
                return false;

            No = 0;
            ID = Int32.Parse(XmlHelper.FindAttribute(node, "SID").Value);
            LauncherID = Int32.Parse(XmlHelper.FindAttribute(node, "AssignedLauncherID").Value);
            ExecuteFileName = XmlHelper.FindAttribute(node, "ExcuteFileName").Value;

            SvrStruct.SetServerID(ID);

            if (XmlHelper.FindAttribute(node, "ServerNum") != null)
                ServerNo = Int32.Parse(XmlHelper.FindAttribute(node, "ServerNum").Value);
            else
                ServerNo = -1;

            switch (Type)
            {
                case ServerType.Database:
                    {
                        foreach (XmlNode n in node.ChildNodes)
                        {
                            if (n.Name == "WorldID")
                            {
                                XmlAttribute attWorldID = XmlHelper.FindAttribute(n, "ID");
                                if (attWorldID == null)
                                    return false;

                                WorldIDList.Add(Int32.Parse(attWorldID.Value));
                            }
                        }

                        if (WorldIDList.Count > 0)
                            Type = ServerType.WDB;
                    }
                    break;

                case ServerType.Game:
                    {
                        foreach (XmlNode n in node.ChildNodes)
                        {
                            if (n.Name == "WorldID")
                            {
                                XmlAttribute attWorldID = XmlHelper.FindAttribute(n, "ID");
                                if (attWorldID == null)
                                    return false;

                                WorldIDList.Add(Int32.Parse(attWorldID.Value));
                            }
                        }

                        if (WorldIDList.Count > 0)
                            Type = ServerType.WGame;
                    }
                    break;

                case ServerType.Village:
                    {
                        XmlNode xmlVillageID = XmlHelper.FindChild(node, "VillageID");
                        if (xmlVillageID == null) return false;

                        XmlAttribute attVillageID = XmlHelper.FindAttribute(xmlVillageID, "ID");
                        if (attVillageID == null) return false;

                        VillageID = Int32.Parse(attVillageID.Value);
                    }
                    break;

                case ServerType.Master:
                    {
                        XmlNode xmlWorldID = XmlHelper.FindChild(node, "WorldID");
                        if (xmlWorldID != null) 
                        {
                            XmlAttribute attWorldID = XmlHelper.FindAttribute(xmlWorldID, "ID");
                            if (attWorldID == null) 
                                return false;

                            WorldID = Int32.Parse(attWorldID.Value);
                        }


                        ////////////////////////////////////////////////////////////////////////////////////
                        XmlNode xmlSID = XmlHelper.FindChild(node, "SIDConnection");
                        if (xmlSID != null)
                        {
                            if (XmlHelper.FindAttribute(xmlSID, "Type") == null) 
                                return false;

                            xmlType = XmlHelper.FindAttribute(xmlSID, "Type").Value;
                            if (xmlType != "Login") 
                                return false;

                            XmlAttribute attSID = XmlHelper.FindAttribute(xmlSID, "SID");
                            if (attSID == null) 
                                return false;

                            TargetID = Int32.Parse(attSID.Value);
                        }
                    }
                    break;
                case ServerType.Cash:
                    {
                        XmlNode xmlWorldID = XmlHelper.FindChild(node, "WorldID");
                        if (xmlWorldID == null) return true;

                        XmlAttribute attWorldID = XmlHelper.FindAttribute(xmlWorldID, "ID");
                        if (attWorldID == null) return false;

                        WorldID = Int32.Parse(attWorldID.Value);
                    }
                    break;
            }
 

            return true;
        }
    }

    public class ServerGroup
    {
        public List<ServerInfo> Login;
        public List<ServerInfo> Log;
        public List<ServerInfo> Master;
        public List<ServerInfo> Cash;
        public List<ServerInfo> DataBase;
        public List<ServerInfo> VillageDB;
        public List<ServerInfo> GameDB;
        public List<ServerInfo> Village;
        public List<ServerInfo> Game;
        public List<ServerInfo> WDB;
        public List<ServerInfo> WGame;

        public ServerGroup()
        {
            Login = new List<ServerInfo>();
            Log = new List<ServerInfo>();
            Master = new List<ServerInfo>();
            Cash = new List<ServerInfo>();
            VillageDB = new List<ServerInfo>();
            GameDB = new List<ServerInfo>();
            DataBase = new List<ServerInfo>();
            Village = new List<ServerInfo>();
            Game = new List<ServerInfo>();
            WDB = new List<ServerInfo>();
            WGame = new List<ServerInfo>();
        }

        public void ClearServerInfo()
        {
            Login.Clear();
            Log.Clear();
            Master.Clear();
            Cash.Clear();
            Village.Clear();
            Game.Clear();
            VillageDB.Clear();
            GameDB.Clear();
            DataBase.Clear();
            WDB.Clear();
            WGame.Clear();
        }

        public void SwapServerInfo (ServerGroup TempServer)
        {
            TempServer.ClearServerInfo();

            for (int i = 0; i < Login.Count; i++)
                TempServer.Login.Add(Login[i]);

            for (int i = 0; i < Log.Count; i++)
                TempServer.Log.Add(Log[i]);

            for (int i = 0; i < Cash.Count; i++)
                TempServer.Cash.Add(Cash[i]);

            for (int i = 0; i < Master.Count; i++)
                TempServer.Master.Add(Master[i]);

            for (int i = 0; i < Village.Count; i++)
                TempServer.Village.Add(Village[i]);

            for (int i = 0; i < Game.Count; i++)
                TempServer.Game.Add(Game[i]);

            for (int i = 0; i < DataBase.Count; i++)
                TempServer.DataBase.Add(DataBase[i]);

            for (int i = 0; i < VillageDB.Count; i++)
                TempServer.VillageDB.Add(VillageDB[i]);

            for (int i = 0; i < GameDB.Count; i++)
                TempServer.GameDB.Add(GameDB[i]);

            for (int i = 0; i < WDB.Count; i++)
                TempServer.WDB.Add(WDB[i]);

            for (int i = 0; i < WGame.Count; i++)
                TempServer.WGame.Add(WGame[i]);

            ClearServerInfo();
        }

        public bool AddServer(ServerInfo info)
        {
            switch (info.Type)
            {
                case ServerInfo.ServerType.Login:   Login.Add(info); break;
                case ServerInfo.ServerType.Log:     Log.Add(info); break;
                case ServerInfo.ServerType.Master:  Master.Add(info); break;
                case ServerInfo.ServerType.Cash:    Cash.Add(info); break;
                case ServerInfo.ServerType.GameDB:  GameDB.Add(info); break;
                case ServerInfo.ServerType.VillageDB:   VillageDB.Add(info); break;
                case ServerInfo.ServerType.Database:    DataBase.Add(info); break;
                case ServerInfo.ServerType.Game:    Game.Add(info); break;
                case ServerInfo.ServerType.WGame:   WGame.Add(info); break;
                case ServerInfo.ServerType.WDB:     WDB.Add(info); break;
                case ServerInfo.ServerType.Village:
                    {
                        info.VillageID = SvrStruct.NextVillageID();
                        Village.Add(info); break;
                    }
                
                default: return false;
            }
            return true;
        }

        public void WriteServerInfo(XmlDocument doc, XmlNode node)
        {
            foreach (ServerInfo info in Login) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in Log) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in Master) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in Cash) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in DataBase) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in GameDB) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in VillageDB) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in Village) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in Game) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in WDB) { node.AppendChild(info.ToXML(doc)); }
            foreach (ServerInfo info in WGame) { node.AppendChild(info.ToXML(doc)); }
        }

        public bool ChangeSIDByInput(int No, int SID)
        {
            foreach (ServerInfo serverinfo in Login)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in Log)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in Master)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in Cash)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in Village)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in Game)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in VillageDB)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in GameDB)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in DataBase)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in WGame)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }

            foreach (ServerInfo serverinfo in WDB)
            {
                if (serverinfo.No == No)
                {
                    serverinfo.ID = SID;
                    return true;
                }
            }


            return false;
        }
    };

    public class NetLauncher
    {
        public int No;
        public int ID;
        public string IP;
        public int WID;
        public int PID;

        public ServerGroup TempServer;
        public ServerGroup RealServer;

      
        public int LoginCount;
        public int LogCount;
        public int CashCount;
        public int MasterCount;
        public int VillageDBCount;
        public int GameDBCount;
        public int DBCount;
        public int VillageCount;
        public int GameCount;
        public int WDBCount;
        public int WGameCount;

        public NetLauncher()
        {
            No = 0;
            ID = 0;
            IP = "000.000.000.000";
            WID = 0;
            PID = 0;

            
            LoginCount = 0;
            LogCount = 0;
            CashCount = 0;
            MasterCount = 0;
            VillageDBCount = 0;
            GameDBCount = 0;
            DBCount = 0;
            VillageCount = 0;
            GameCount = 0;
            WDBCount = 0;
            WGameCount = 0;

            TempServer = new ServerGroup();
            RealServer = new ServerGroup();
        }

        public void ConvertServerInfo()
        {
            RealServer.SwapServerInfo(TempServer);  
        }

        public void ResetData()
        {
            ID = 0;
            IP = "000.000.000.000";
            WID = 0;
            PID = 0;

            LoginCount = 0;
            LogCount = 0;
            CashCount = 0;
            MasterCount = 0;
            VillageDBCount = 0;
            GameDBCount = 0;
            DBCount = 0;
            VillageCount = 0;
            GameCount = 0;
        }

        public bool LoadServerInfo(ServerInfo info)
        {
            switch (info.Type)
            {
                case ServerInfo.ServerType.Login:   LoginCount++;  break;
                case ServerInfo.ServerType.Log:     LogCount++; break;
                case ServerInfo.ServerType.Master:  MasterCount++; break;
                case ServerInfo.ServerType.Cash:    CashCount++; break;
                case ServerInfo.ServerType.GameDB:  GameDBCount++; break;
                case ServerInfo.ServerType.VillageDB: VillageDBCount++; break;
                case ServerInfo.ServerType.Database: DBCount++; break;
                case ServerInfo.ServerType.Village: VillageCount++; break;
                case ServerInfo.ServerType.Game: GameCount++; break;
                case ServerInfo.ServerType.WDB: WDBCount++; break;
                case ServerInfo.ServerType.WGame: WGameCount++; break;
                default: return false;
            }

            RealServer.AddServer(info);

            return true;
        }

        public void AddServerInfo(ServerInfo.ServerType Type, int SID)
        {
            ServerInfo newserver = new ServerInfo();
            newserver.No = SvrStruct.NextServerNo();
            newserver.ID = SID;
            newserver.LauncherID = ID;
            newserver.Type = Type;

            RealServer.AddServer(newserver);
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Launcher");
            XmlAttribute att;

            att = doc.CreateAttribute("ID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("IP"); att.Value = IP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("WID"); att.Value = WID.ToString(); retElement.Attributes.Append(att);
            if (PID > 0)
            {
                att = doc.CreateAttribute("PID"); att.Value = PID.ToString(); retElement.Attributes.Append(att);
            }
            
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "ID") == null) return false;
            if (XmlHelper.FindAttribute(node, "IP") == null) return false;
            if (XmlHelper.FindAttribute(node, "WID") == null) return false;

            ID = Int32.Parse(XmlHelper.FindAttribute(node, "ID").Value);
            IP = XmlHelper.FindAttribute(node, "IP").Value;
            WID = Int32.Parse(XmlHelper.FindAttribute(node, "WID").Value);

            if (XmlHelper.FindAttribute(node, "PID") != null)
                PID = Int32.Parse(XmlHelper.FindAttribute(node, "PID").Value);

            return true;
        }
        
        public void WriteServerInfo(XmlDocument doc, XmlNode node)
        {
            RealServer.WriteServerInfo(doc, node);
        }
    }



    public class SvrStruct
    {
        public ServiceManager ServiceMng;
        public DefaultServerInfo defSvrInfo;
        public DefaultDatabaseInfo defDBinfo;

        public List<NetLauncher> NetLauncherList;

        private static int sServerNo;
        private static int sServerID;
        private static int sLauncherNo;
        private static int sVillageID;

        public int PID;     // 파티션 아이디
        
        public void Clear()
        {
            ServiceMng.Clear();
            defSvrInfo.Clear();
            defDBinfo.Clear();
            NetLauncherList.Clear();
            PID = 0;
        }

        public static int NextServerNo()
        {
            sServerNo++;
            return sServerNo;
        }

        public static void ResetServerNo()
        {
            sServerNo = 0;
        }

        public static void SetServerNo (int nServerNo)
        {
            sServerNo = nServerNo;
        }

        public static int NextServerID()
        {
            sServerID++;
            return sServerID;
        }

        public static void ResetServerID()
        {
            sServerID = 0;
        }

        public static void SetServerID(int nServerID)
        {
            if (nServerID > sServerID)
                sServerID = nServerID;
        }

        public static int NextLauncherNo()
        {
            sLauncherNo++;
            return sLauncherNo;
        }

        public static int GetLauncherID()
        {
            return sLauncherNo;
        }

        public static void ResetLauncherID()
        {
            sLauncherNo = 0;
        }

        public static int NextVillageID()
        {
            sVillageID++;
            return sVillageID;
        }

        public static void ResetVillageID()
        {
            sVillageID = 0;
        }

        public SvrStruct()
        {
            ServiceMng = new ServiceManager();
            defSvrInfo = new DefaultServerInfo();
            defDBinfo = new DefaultDatabaseInfo();
            NetLauncherList = new List<NetLauncher>();
        }

        public bool LoadServerInfo(ServerInfo info)
        {
            NetLauncher launcher = null;
            foreach (NetLauncher l in NetLauncherList)
            {
                if (l.ID == info.LauncherID)
                    launcher = l;
            }

            if (launcher == null) return false;

            return launcher.LoadServerInfo(info);
        }

        public void ToXML(string filename)
        {
            XmlDocument doc = new XmlDocument();

            XmlDeclaration xmldecl;
            xmldecl = doc.CreateXmlDeclaration("1.0", "utf-8", "yes");

            XmlElement root;

            XmlElement svrstruct = doc.CreateElement("ServerStruct");
            doc.AppendChild(svrstruct);

            XmlElement version = doc.CreateElement("Struct");
            XmlAttribute ver_att = doc.CreateAttribute("Version");
            ver_att.Value = "1";
            version.Attributes.Append(ver_att);
            svrstruct.AppendChild(doc.CreateComment("Version"));
            svrstruct.AppendChild(version);

            svrstruct.AppendChild(doc.CreateComment("Service Manager"));
            svrstruct.AppendChild(ServiceMng.ToXML(doc));
            

            root = doc.DocumentElement;
            doc.InsertBefore(xmldecl, root);

            XmlElement eleNetLauncherList = doc.CreateElement("NetLauncherList");
            foreach(NetLauncher launcher in NetLauncherList)
            {
                eleNetLauncherList.AppendChild(launcher.ToXML(doc));
            }
            svrstruct.AppendChild(doc.CreateComment("NetLauncher List"));
            svrstruct.AppendChild(eleNetLauncherList);

            svrstruct.AppendChild(doc.CreateComment("Default Server Info"));
            svrstruct.AppendChild(defSvrInfo.ToXML(doc));
            svrstruct.AppendChild(doc.CreateComment("Default Database Server List"));
            svrstruct.AppendChild(defDBinfo.ToXML(doc));

            svrstruct.AppendChild(doc.CreateComment("Start ServerInfo"));
            foreach (NetLauncher launcher in NetLauncherList)
            {
                launcher.WriteServerInfo(doc, svrstruct);
            }

            doc.Save(filename);
        }
    }

   
    public class MeritBonusTable
    {
        public List<MeritBonus> MeritBonusList;

        public MeritBonusTable()
        {
            MeritBonusList = new List<MeritBonus>();

            SetDefaultMeritBounsTable();
        }

        public void Clear()
        {
            MeritBonusList.Clear();
        }

        private void SetDefaultMeritBounsTable()
        {
            MeritBonus Bonus1 = new MeritBonus();
            Bonus1.ID = 1;
            Bonus1.MinLevel = 1;
            Bonus1.MaxLevel = 5;
            MeritBonusList.Add(Bonus1);

            MeritBonus Bonus2 = new MeritBonus();
            Bonus2.ID = 2;
            Bonus2.MinLevel = 5;
            Bonus2.MaxLevel = 9;
            MeritBonusList.Add(Bonus2);

            MeritBonus Bonus3 = new MeritBonus();
            Bonus3.ID = 3;
            Bonus3.MinLevel = 9;
            Bonus3.MaxLevel = 100;
            MeritBonusList.Add(Bonus3);

            MeritBonus Bonus4 = new MeritBonus();
            Bonus4.ID = 4;
            Bonus4.MinLevel = 9;
            Bonus4.MaxLevel = 16;
            MeritBonusList.Add(Bonus4);

            MeritBonus Bonus5 = new MeritBonus();
            Bonus5.ID = 5;
            Bonus5.MinLevel = 16;
            Bonus5.MaxLevel = 24;
            MeritBonusList.Add(Bonus5);

            MeritBonus Bonus6 = new MeritBonus();
            Bonus6.ID = 6;
            Bonus6.MinLevel = 24;
            Bonus6.MaxLevel = 100;
            MeritBonusList.Add(Bonus6);

            MeritBonus Bonus7 = new MeritBonus();
            Bonus7.ID = 7;
            Bonus7.MinLevel = 24;
            Bonus7.MaxLevel = 32;
            MeritBonusList.Add(Bonus7);

            MeritBonus Bonus8 = new MeritBonus();
            Bonus8.ID = 8;
            Bonus8.MinLevel = 32;
            Bonus8.MaxLevel = 40;
            MeritBonusList.Add(Bonus8);

            MeritBonus Bonus9 = new MeritBonus();
            Bonus9.ID = 9;
            Bonus9.MinLevel = 40;
            Bonus9.MaxLevel = 50;
            MeritBonusList.Add(Bonus9);

            MeritBonus Bonus10 = new MeritBonus();
            Bonus10.ID = 10;
            Bonus10.MinLevel = 1;
            Bonus10.MaxLevel = 9;
            MeritBonusList.Add(Bonus10);

            MeritBonus Bonus11 = new MeritBonus();
            Bonus11.ID = 11;
            Bonus11.MinLevel = 24;
            Bonus11.MaxLevel = 28;
            MeritBonusList.Add(Bonus11);

            MeritBonus Bonus12 = new MeritBonus();
            Bonus12.ID = 12;
            Bonus12.MinLevel = 28;
            Bonus12.MaxLevel = 32;
            MeritBonusList.Add(Bonus12);
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("MeritBonusTable");

            foreach (MeritBonus bonus in MeritBonusList)
            {
                retElement.AppendChild(bonus.ToXML(doc));
            }

            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            if (node == null) return "No Node";

            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element) continue;

                if (n.Name == "Bonus")
                {
                    MeritBonus Bonus = new MeritBonus();
                    if (Bonus.FromXML(n) == false)
                        return n.Name;

                    MeritBonusList.Add(Bonus);
                }
                else
                    return n.Name;
            }

            return null;
        }
    };

    public class MeritBonus
    {
        public int No;
        public int ID;
        public int MinLevel;
        public int MaxLevel;
        public string BonusType;
        public int ExtendValue;

        public MeritBonus()
        {
            No = 0;
            ID = 1;
            MinLevel = 0;
            MaxLevel = 50;
            BonusType = "CompleteExp";
            ExtendValue = 5;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Bonus");
            XmlAttribute att;

            att = doc.CreateAttribute("ID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("MinLevel"); att.Value = MinLevel.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("MaxLevel"); att.Value = MaxLevel.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("BonusType"); att.Value = BonusType; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("ExtendValue"); att.Value = ExtendValue.ToString(); retElement.Attributes.Append(att);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "ID") == null) return false;
            if (XmlHelper.FindAttribute(node, "MinLevel") == null) return false;
            if (XmlHelper.FindAttribute(node, "MaxLevel") == null) return false;
            if (XmlHelper.FindAttribute(node, "BonusType") == null) return false;
            if (XmlHelper.FindAttribute(node, "ExtendValue") == null) return false;

            ID = Int32.Parse(XmlHelper.FindAttribute(node, "ID").Value);
            MinLevel = Int32.Parse(XmlHelper.FindAttribute(node, "MinLevel").Value);
            MaxLevel = Int32.Parse(XmlHelper.FindAttribute(node, "MaxLevel").Value);
            BonusType = XmlHelper.FindAttribute(node, "BonusType").Value;
            ExtendValue = Int32.Parse(XmlHelper.FindAttribute(node, "ExtendValue").Value);

            return true;
        }
    };

    public class FTGExpTable
    {
        public FTGExp Exp;

        public FTGExpTable()
        {
            Exp = new FTGExp();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("FTGExpTable");

            retElement.AppendChild(Exp.ToXML(doc));
            
            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            Exp.FromXML(XmlHelper.FindChild(node, "FTGExp"));
            return null;
        }

    };

    public class FTGExp
    {
        public int Value;

        public FTGExp()
        {
            Value = 0;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("FTGExp");
            XmlAttribute att;

            att = doc.CreateAttribute("Value"); att.Value = Value.ToString(); retElement.Attributes.Append(att);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "Value") == null) return false;

            Value = Int32.Parse(XmlHelper.FindAttribute(node, "Value").Value);

            return true;
        }
    };

    public class NoFTGExpTable
    {
        public NoFTGExp Exp;

        public NoFTGExpTable()
        {
            Exp = new NoFTGExp();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("NoFTGExpTable");

            retElement.AppendChild(Exp.ToXML(doc));

            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            Exp.FromXML(XmlHelper.FindChild(node, "NoFTGExp"));

            return null;
        }

    };

    public class NoFTGExp
    {
        public int Value;

        public NoFTGExp()
        {
            Value = 0;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("NoFTGExp");
            XmlAttribute att;

            att = doc.CreateAttribute("Value"); att.Value = Value.ToString(); retElement.Attributes.Append(att);

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "Value") == null) return false;

            Value = Int32.Parse(XmlHelper.FindAttribute(node, "Value").Value);

            return true;
        }
    };

    public class MapSet
    {
        public int No;
        public int WorldID;
        public int ID;
        public string Name;
        public string MapAttribute;
        public int MeritBonusID;
        public int MaxUser;
        public int LimitLevel;
        public int DependentMapID;
        public int ShowCount;
        public int HideCount;
        public int VillageIndex;
        public bool IsAvailableMerit;

        public MapSet()
        {
            No = 0;
            WorldID = 0;
            ID = 0;
            Name = "";
            MapAttribute = "";
            MeritBonusID = -1;
            MaxUser = -1;
            LimitLevel = -1;
            DependentMapID = -1;
            ShowCount = 0;
            HideCount = 0;
            VillageIndex = -1;
            IsAvailableMerit = false;
        }

        public void SetData(MapSet mapSet)
        {
            ID = mapSet.ID;
            Name = mapSet.Name;
            MapAttribute = mapSet.MapAttribute;
            MeritBonusID = mapSet.MeritBonusID;
            MaxUser = mapSet.MaxUser;
            LimitLevel = mapSet.LimitLevel;
            DependentMapID = mapSet.DependentMapID;
            IsAvailableMerit = GetAvailableMerit(MapAttribute);
        }


        public bool GetAvailableMerit(string str)
        {
            if (str.IndexOf("Merit") >= 0 || str == "Normal")
                return true;

            return false;
        }
    };

    public class VillageInfo
    {
        public int No;
        public int CID;
        public int ID;
        public string Name;
        public string MapAttribute;
        public int MeritBonusID;
        public string Visibility;
        public int MaxUser;
        public int LimitLevel;
        public int DependentMapID;
        public int Index;

        public VillageInfo()
        {
            No = 0;
            CID = 0;
            ID = 0;
            Name = "";
            MapAttribute = "";
            MeritBonusID = -1;
            Visibility = "";
            MaxUser = -1;
            LimitLevel = -1;
            DependentMapID = -1;
            Index = -1;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("VillageInfo");
            XmlAttribute att;

            att = doc.CreateAttribute("CID"); att.Value = CID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("ID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Name"); att.Value = Name; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Attribute"); att.Value = MapAttribute; retElement.Attributes.Append(att);

            if (MeritBonusID > 0)
            {
                att = doc.CreateAttribute("MeritBonusID"); att.Value = MeritBonusID.ToString(); retElement.Attributes.Append(att);
            }

            att = doc.CreateAttribute("Visibility"); att.Value = Visibility; retElement.Attributes.Append(att);

            if (MaxUser > 0)
            {
                att = doc.CreateAttribute("MaxUser"); att.Value = MaxUser.ToString(); retElement.Attributes.Append(att);
            }

            if (LimitLevel > 0)
            {
                att = doc.CreateAttribute("LimitLevel"); att.Value = LimitLevel.ToString(); retElement.Attributes.Append(att);
            }

            if (DependentMapID > 0)
            {
                att = doc.CreateAttribute("DependentMapID"); att.Value = DependentMapID.ToString(); retElement.Attributes.Append(att);
            }
            
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "CID") == null) return false;
            if (XmlHelper.FindAttribute(node, "ID") == null) return false;
            if (XmlHelper.FindAttribute(node, "Name") == null) return false;
            if (XmlHelper.FindAttribute(node, "Attribute") == null) return false;
            if (XmlHelper.FindAttribute(node, "Visibility") == null) return false;
            
            
            CID = Int32.Parse(XmlHelper.FindAttribute(node, "CID").Value);
            ID = Int32.Parse(XmlHelper.FindAttribute(node, "ID").Value);
            Name = XmlHelper.FindAttribute(node, "Name").Value;
            MapAttribute = XmlHelper.FindAttribute(node, "Attribute").Value;

            if (XmlHelper.FindAttribute(node, "MeritBonusID") != null)
                MeritBonusID = Int32.Parse(XmlHelper.FindAttribute(node, "MeritBonusID").Value);

            Visibility = XmlHelper.FindAttribute(node, "Visibility").Value;

            if (XmlHelper.FindAttribute(node, "MaxUser") != null)
                MaxUser = Int32.Parse(XmlHelper.FindAttribute(node, "MaxUser").Value);

            if (XmlHelper.FindAttribute(node, "LimitLevel") != null)
                LimitLevel = Int32.Parse(XmlHelper.FindAttribute(node, "LimitLevel").Value);

            if (XmlHelper.FindAttribute(node, "DependentMapID") != null)
                DependentMapID = Int32.Parse(XmlHelper.FindAttribute(node, "DependentMapID").Value);

            return true;
        }

    }

    public class VillageServerInfo
    {
        public int No;
        public int WID;
        public int ID;
        public string IP;
        public int Port;
        public int Index;

        public List<VillageInfo> VillageInfoList;

        public VillageServerInfo()
        {
            No = 0;
            WID = 0;
            ID = 0;
            IP = "000.000.000.000";
            Port = 0;
            Index = 0;

            VillageInfoList = new List<VillageInfo>();
        }

        public void Clear()
        {
            VillageInfoList.Clear();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("VillageServerInfo");
            XmlAttribute att;

            att = doc.CreateAttribute("ID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("IP"); att.Value = IP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Port"); att.Value = Port.ToString(); retElement.Attributes.Append(att);
            
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "ID") == null) return false;
            if (XmlHelper.FindAttribute(node, "IP") == null) return false;
            if (XmlHelper.FindAttribute(node, "Port") == null) return false;
            
            ID = Int32.Parse(XmlHelper.FindAttribute(node, "ID").Value);
            IP = XmlHelper.FindAttribute(node, "IP").Value;
            Port = Int32.Parse(XmlHelper.FindAttribute(node, "Port").Value);

            return true;
        }

        public string LoadVillageInfo(XmlNode node, int Index)
        {
            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element) continue;

                if (n.Name == "VillageInfo")
                {
                    VillageInfo village = new VillageInfo();
                    village.Index = Index;
                    if (village.FromXML(n) == false)
                        return n.Name;

                    VillageInfoList.Add(village);
                }
                else
                    return n.Name;
            }

            return null;
        }
    };

    public class ServerEx
    {
        public int No;
        public int ID;      // WorldID
        public int MaxVillageCount;

        public string IP;
        public string Name;
        public int WorldMaxUser;
        public int DefaultChannelCount;
        public int DefaultMaxUser;
        public int SetID;
        public int OnTop;

        public List<MapSet> MapSetList;
        public List<VillageServerInfo> VillageServerList;
        

        public ServerEx()
        {
            No = 0;
            ID = 1;
            MaxVillageCount = 0;
            IP = "000.000.000.000";
            Name = "";
            WorldMaxUser = 8000;
            DefaultChannelCount = 1;
            DefaultMaxUser = 200;
            SetID = -1;
            OnTop = 0;

            VillageServerList = new List<VillageServerInfo>();
            MapSetList = new List<MapSet>();

        }

        public void Clear()
        {
            foreach (VillageServerInfo server in VillageServerList)
                server.Clear();
            
            VillageServerList.Clear();
            MapSetList.Clear();
        }

        public void LoadMapSet()
        {
            MapSetList.Clear();

            foreach (VillageServerInfo villageServer in VillageServerList)
            {
                foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                {
                    MapSet existedMapSet = GetMapSet(villageInfo);
                    if (existedMapSet == null)
                    {
                        MapSet mapSet = new MapSet();
                        mapSet.ID = villageInfo.ID;
                        mapSet.Name = villageInfo.Name;
                        mapSet.MapAttribute = villageInfo.MapAttribute;
                        mapSet.MeritBonusID = villageInfo.MeritBonusID;
                        mapSet.MaxUser = villageInfo.MaxUser;
                        mapSet.LimitLevel = villageInfo.LimitLevel;
                        mapSet.DependentMapID = villageInfo.DependentMapID;
                        mapSet.VillageIndex = villageInfo.Index;
                        mapSet.IsAvailableMerit = mapSet.GetAvailableMerit(mapSet.MapAttribute);

                        if (villageInfo.Visibility == "True")
                            mapSet.ShowCount = 1;
                        else
                            mapSet.HideCount = 1;

                        MapSetList.Add(mapSet);
                    }
                    else
                    {
                        if (villageInfo.Visibility == "True")
                            existedMapSet.ShowCount++;
                        else
                            existedMapSet.HideCount++;
                    }
                }
            }
        }

        private MapSet GetMapSet(VillageInfo villageInfo)
        {
            foreach (MapSet mapSet in MapSetList)
            {
                if (mapSet.ID != villageInfo.ID)
                    continue;

                if (mapSet.VillageIndex != villageInfo.Index)
                    continue;

                if (mapSet.MapAttribute != villageInfo.MapAttribute)
                    continue;

                if (mapSet.MeritBonusID != villageInfo.MeritBonusID)
                    continue;

                if (mapSet.MaxUser != villageInfo.MaxUser)
                    continue;

                return mapSet;
            }

            return null;
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Server");
            XmlAttribute att;

            att = doc.CreateAttribute("ID"); att.Value = ID.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("IP"); att.Value = IP; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("Name"); att.Value = Name; retElement.Attributes.Append(att);
            att = doc.CreateAttribute("WorldMaxUser"); att.Value = WorldMaxUser.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("DefaultChannelCount"); att.Value = DefaultChannelCount.ToString(); retElement.Attributes.Append(att);
            att = doc.CreateAttribute("DefaultMaxUser"); att.Value = DefaultMaxUser.ToString(); retElement.Attributes.Append(att);

            if (OnTop == 1)
            {
                att = doc.CreateAttribute("OnTop"); att.Value = "true"; retElement.Attributes.Append(att);
            }
            

            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return false;

            if (XmlHelper.FindAttribute(node, "ID") == null) return false;
            if (XmlHelper.FindAttribute(node, "IP") == null) return false;
            if (XmlHelper.FindAttribute(node, "Name") == null) return false;
            if (XmlHelper.FindAttribute(node, "WorldMaxUser") == null) return false;
            if (XmlHelper.FindAttribute(node, "DefaultChannelCount") != null)
            if (XmlHelper.FindAttribute(node, "DefaultMaxUser") == null) return false;

            ID = Int32.Parse(XmlHelper.FindAttribute(node, "ID").Value);

            IP = XmlHelper.FindAttribute(node, "IP").Value;
            Name = XmlHelper.FindAttribute(node, "Name").Value;

            WorldMaxUser = Int32.Parse(XmlHelper.FindAttribute(node, "WorldMaxUser").Value);

            if (XmlHelper.FindAttribute(node, "DefaultChannelCount") != null)
                DefaultChannelCount = Int32.Parse(XmlHelper.FindAttribute(node, "DefaultChannelCount").Value);

            DefaultMaxUser = Int32.Parse(XmlHelper.FindAttribute(node, "DefaultMaxUser").Value);

            if (XmlHelper.FindAttribute(node, "OnTop") != null)
            {
                string szTop = XmlHelper.FindAttribute(node, "OnTop").Value;
                if (szTop == "true" || szTop == "True")
                    OnTop = 1;
            }
            
            return true;
        }

        public string LoadVillageServerInfo(XmlNode node)
        {
            int nIndex = 0;
            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element) continue;

                if (n.Name == "VillageServerInfo")
                {
                    VillageServerInfo villageServer = new VillageServerInfo();
                    villageServer.Index = nIndex++;

                    if (villageServer.FromXML(n) == false)
                        return n.Name;

                    /////////////////////////////////
                    // VillageInfo
                    /////////////////////////////////
                    string result = villageServer.LoadVillageInfo(n, villageServer.Index);
                    if (result != null)
                        return result;

                    VillageServerList.Add(villageServer);

                }
                else
                    return n.Name;
            }

            return null;
        }
    };

    public class ServerInfoEx
    {
        public List<ServerEx> TempServerList;
        public List<ServerEx> RealServerList;

        public ServerInfoEx()
        {
            TempServerList = new List<ServerEx>();
            RealServerList = new List<ServerEx>();
        }

        public void Clear()
        {
            TempServerList.Clear();
            RealServerList.Clear();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("ServerInfo");

            foreach (ServerEx bonus in RealServerList)
            {
                retElement.AppendChild(bonus.ToXML(doc));
            }

            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            if (node == null) return "No Node";

            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element) continue;

                if (n.Name == "Server")
                {
                    ServerEx server = new ServerEx();
                    if (server.FromXML(n) == false)
                        return n.Name;

                    /////////////////////////////
                    //VillageServerInfo -> VillageInfo
                    ////////////////////////////
                    string result = server.LoadVillageServerInfo(n);
                    if (result != null)
                        return result;

                    /////////////////////////////
                    // Load MapSet
                    /////////////////////////////
                    server.LoadMapSet();

                    TempServerList.Add(server);
                }
                else
                    return n.Name;
            }

            return null;
        }
    };

    public class Merger
    {
        public int SetID;
        public List<int> ServerIDs;
        string TextServerID;

        public Merger()
        {
            ServerIDs = new List<int>();

            Clear();
        }

        public void Clear()
        {
            SetID = -1;
            ServerIDs.Clear();
            TextServerID = "";
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("Merger");
            XmlAttribute att;
            att = doc.CreateAttribute("SetID"); att.Value = SetID.ToString(); retElement.Attributes.Append(att);

            int nIndex = 0;
            TextServerID = "";
            foreach (int i in ServerIDs)
            {
                nIndex++;
                if (nIndex == ServerIDs.Count)
                    TextServerID += i;
                else
                    TextServerID += i + ", ";
            }

            att = doc.CreateAttribute("ServerID"); att.Value = TextServerID; retElement.Attributes.Append(att);
            
            return retElement;
        }

        public bool FromXML(XmlNode node)
        {
            if (node == null) return true;

            if (XmlHelper.FindAttribute(node, "SetID") == null) return false;
            if (XmlHelper.FindAttribute(node, "ServerID") == null) return false;

            SetID = Int32.Parse(XmlHelper.FindAttribute(node, "SetID").Value);
            TextServerID = XmlHelper.FindAttribute(node, "ServerID").Value;

            string[] vals = TextServerID.Split(',');
            foreach (string val in vals)
                ServerIDs.Add(Int32.Parse(val));
            

            return true;
        }

    };

    public class ServerMergerInfo
    {
        public List<Merger> MergerList;

        public ServerMergerInfo()
        {
            MergerList = new List<Merger>();
            Clear();
        }

        public void Clear()
        {
            foreach (Merger merger in MergerList)
                merger.Clear();

            MergerList.Clear();
        }

        public XmlNode ToXML(XmlDocument doc)
        {
            XmlElement retElement = doc.CreateElement("ServerMergerInfo");

            foreach (Merger merger in MergerList)
            {
                retElement.AppendChild(merger.ToXML(doc));
            }

            return retElement;
        }

        public string FromXML(XmlNode node)
        {
            if (node == null) return null;

            foreach (XmlNode n in node.ChildNodes)
            {
                if (n.NodeType != XmlNodeType.Element) continue;

                if (n.Name == "Merger")
                {
                    Merger merger = new Merger();
                    if (merger.FromXML(n) == false)
                        return n.Name;

                    MergerList.Add(merger);
                }
                else
                    return n.Name;
            }

            return null;
        }
    };

    
    public class ChannelInfo
    {
        public MeritBonusTable MeritBonusTB;
        public ServerMergerInfo ServerMerger;
        public ServerInfoEx ServerInfoEx;
        public List<MapSet> MapSetList;

        public bool IsFTGExp;
        public FTGExpTable FTGExpTB;
        public NoFTGExpTable NoFTGExpTB;

        public ChannelInfo()
        {
            MeritBonusTB = new MeritBonusTable();
            ServerInfoEx = new ServerInfoEx();
            ServerMerger = new ServerMergerInfo();
            FTGExpTB = new FTGExpTable();
            NoFTGExpTB = new NoFTGExpTable();
        }

        public void Clear()
        {
            MeritBonusTB.Clear();
            ServerInfoEx.Clear();
            ServerMerger.Clear();
            IsFTGExp = false;
        }

        public void ToXML (string filename)
        {
            XmlDocument doc = new XmlDocument();

            XmlDeclaration xmldecl;
            xmldecl = doc.CreateXmlDeclaration("1.0", "utf-8", "yes");

            XmlElement root;

            XmlElement channelInfo = doc.CreateElement("ChannelInfo");
            doc.AppendChild(channelInfo);

            channelInfo.AppendChild(MeritBonusTB.ToXML(doc));
            if (IsFTGExp)
            {
                channelInfo.AppendChild(FTGExpTB.ToXML(doc));
                channelInfo.AppendChild(NoFTGExpTB.ToXML(doc));
            }
            
            if (ServerMerger.MergerList.Count > 0)
                channelInfo.AppendChild(ServerMerger.ToXML(doc));

            root = doc.DocumentElement;
            doc.InsertBefore(xmldecl, root);

            XmlElement eleServerInfo = doc.CreateElement("ServerInfo");
            channelInfo.AppendChild(eleServerInfo);

            foreach (ServerEx server in ServerInfoEx.RealServerList)
            {

                XmlNode eleServer = server.ToXML(doc);
                eleServerInfo.AppendChild(eleServer);

                if (server.SetID != -1)
                {
                    XmlComment xmlComment = doc.CreateComment("This Server is integrated to WorldSetID:" +server.SetID);
                    eleServer.AppendChild(xmlComment);
                    continue;
                }

                foreach (VillageServerInfo villageServer in server.VillageServerList)
                {
                    XmlNode eleVillage = villageServer.ToXML(doc);
                    eleServer.AppendChild(eleVillage);

                    foreach (VillageInfo villageInfo in villageServer.VillageInfoList)
                    {
                        eleVillage.AppendChild(villageInfo.ToXML(doc));
                    }
                }
            }
           

            doc.Save(filename);
        }
    };

    public class ServerIP
    {
        public string SourceIP;
        public string DestIP;

        public ServerIP()
        {
            SourceIP = "";
            DestIP = "";
        }

    }

    public class UIEnviornment
    {
        public string nation;
        public bool is_dolby;
        public bool is_servernum;
        public bool is_targetip;
        public bool is_fatigue;

        public UIEnviornment()
        {
            Clear();
        }

        public void Clear()
        {
            nation = "";
            is_dolby = false;
            is_servernum = false;
            is_targetip = false;
            is_fatigue = false;
        }
    }

    public class ReportContext
    {
        public int No;
        public int TabPageIndex;
        public string LogText;
        public int Row;
        public int Colum;
        public SourceGrid2.Grid Grid;

        public ReportContext()
        {
            No = 0;
            TabPageIndex = 0;
            LogText = "";
            Row = 0;
            Colum = 0;
            Grid = null;
        }
    };
}
