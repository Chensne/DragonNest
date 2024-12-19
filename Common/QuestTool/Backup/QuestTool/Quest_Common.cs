using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;
using System.Security;

namespace QuestTool
{
    public enum eCOUNTRY
    {
        DEFAULT,
        KOR,
        CHN,
        JPN,
        USA,
        TWN,
        SIN,
        THA,
        IDN,
        RUS,
        EU,
    }

    public class Quest_Common
    {
        // 퀘스트 기본 폴더
        private string _DefaultQuestPath;
        private string _KoreaQuestPath;
        private string _JapanQuestPath;
        private string _ChinaQuestPath;
        private string _USAQuestPath;
        private string _TaiwanQuestPath;
        private string _SingaporeNMalaysiaQuestPath;
        private string _ThailandQuestPath;
        private string _IndonesiaQuestPath;
        private string _RusiaQuestPath;
        private string _EuropeQuestPath;

        // NPC 기본 폴더
        private string _DefalutNPCPath;
        private string _KoreaNPCPath;
        private string _JapanNPCPath;
        private string _ChinaNPCPath;
        private string _USANPCPath;
        private string _TaiwanNPCPath;
        private string _SingaporeNMalaysiaNPCPath;
        private string _ThailandNPCPath;
        private string _IndonesiaNPCPath;
        private string _RusiaNPCPath;
        private string _EuropeNPCPath;

        // 레지스트리 경로
        private static string regSubkey = "Software\\DNQuestTool";
        
        public Quest_Common()
        {
            RegistryKey rk = Registry.LocalMachine.OpenSubKey(regSubkey, true);

            // 없으면 서브키를 만든다.
            if (rk == null)
                rk = Registry.LocalMachine.CreateSubKey(regSubkey);

            if (rk.GetValue("_DefaultQuestPath") == null)
                rk.SetValue("_DefaultQuestPath",@"R:\GameRes\Resource\Script\Talk_Quest");
            if (rk.GetValue("_KoreaQuestPath") == null)
                rk.SetValue("_KoreaQuestPath",  @"R:\GameRes\Resource_KOR\Script\Talk_Quest");
            if (rk.GetValue("_JapanQuestPath") == null)
                rk.SetValue("_JapanQuestPath",  @"R:\GameRes\Resource_JPN\Script\Talk_Quest");
            if (rk.GetValue("_ChinaQuestPath") == null)
                rk.SetValue("_ChinaQuestPath",  @"R:\GameRes\Resource_CHN\Script\Talk_Quest");
            if (rk.GetValue("_USAQuestPath") == null)
                rk.SetValue("_USAQuestPath",    @"R:\GameRes\Resource_USA\Script\Talk_Quest");
            if (rk.GetValue("_TaiwanQuestPath") == null)
                rk.SetValue("_TaiwanQuestPath", @"R:\GameRes\Resource_TWN\Script\Talk_Quest");
            if (rk.GetValue("_SingaporeNMalaysiaQuestPath") == null)
                rk.SetValue("_SingaporeNMalaysiaQuestPath", @"R:\GameRes\Resource_SIN\Script\Talk_Quest");
          
            if (rk.GetValue("_ThailandQuestPath") == null)
                rk.SetValue("_ThailandQuestPath", @"R:\GameRes\Resource_THA\Script\Talk_Quest");
            if (rk.GetValue("_IndonesiaQuestPath") == null)
                rk.SetValue("_IndonesiaQuestPath", @"R:\GameRes\Resource_IDN\Script\Talk_Quest");
            if (rk.GetValue("_RusiaQuestPath") == null)
                rk.SetValue("_RusiaQuestPath", @"R:\GameRes\Resource_RUS\Script\Talk_Quest");
            if (rk.GetValue("_EuropeQuestPath") == null)
                rk.SetValue("_EuropeQuestPath", @"R:\GameRes\Resource_EU\Script\Talk_Quest");



            if (rk.GetValue("_DefalutNPCPath") == null)
                rk.SetValue("_DefalutNPCPath",  @"R:\GameRes\Resource\Script\Talk_Npc");
            if (rk.GetValue("_KoreaNPCPath") == null)
                rk.SetValue("_KoreaNPCPath",    @"R:\GameRes\Resource_KOR\Script\Talk_Npc");
            if (rk.GetValue("_JapanNPCPath") == null)
                rk.SetValue("_JapanNPCPath",    @"R:\GameRes\Resource_JPN\Script\Talk_Npc");
            if (rk.GetValue("_ChinaNPCPath") == null)
                rk.SetValue("_ChinaNPCPath",    @"R:\GameRes\Resource_CHN\Script\Talk_Npc");
            if (rk.GetValue("_USANPCPath") == null)
                rk.SetValue("_USANPCPath",      @"R:\GameRes\Resource_USA\Script\Talk_Npc");
            if (rk.GetValue("_TaiwanNPCPath") == null)
                rk.SetValue("_TaiwanNPCPath",   @"R:\GameRes\Resource_TWN\Script\Talk_Npc");
            if (rk.GetValue("_SingaporeNMalaysiaNPCPath") == null)
                rk.SetValue("_SingaporeNMalaysiaNPCPath", @"R:\GameRes\Resource_SIN\Script\Talk_Npc");

            if (rk.GetValue("_ThailandNpcPath") == null)
                rk.SetValue("_ThailandNpcPath", @"R:\GameRes\Resource_THA\Script\Talk_Npc");
            if (rk.GetValue("_IndonesiaNpcPath") == null)
                rk.SetValue("_IndonesiaNpcPath", @"R:\GameRes\Resource_IDN\Script\Talk_Npc");
            if (rk.GetValue("_RusiaNpcPath") == null)
                rk.SetValue("_RusiaNpcPath", @"R:\GameRes\Resource_RUS\Script\Talk_Npc");
            if (rk.GetValue("_EuropeNpcPath") == null)
                rk.SetValue("_EuropeNpcPath", @"R:\GameRes\Resource_EU\Script\Talk_Npc");



            _DefaultQuestPath = rk.GetValue("_DefaultQuestPath").ToString();
            _KoreaQuestPath = rk.GetValue("_KoreaQuestPath").ToString();
            _JapanQuestPath = rk.GetValue("_JapanQuestPath").ToString();
            _ChinaQuestPath = rk.GetValue("_ChinaQuestPath").ToString();
            _USAQuestPath = rk.GetValue("_USAQuestPath").ToString();
            _TaiwanQuestPath = rk.GetValue("_TaiwanQuestPath").ToString();
            _SingaporeNMalaysiaQuestPath = rk.GetValue("_SingaporeNMalaysiaQuestPath").ToString();

            _ThailandQuestPath = rk.GetValue("_ThailandQuestPath").ToString();
            _IndonesiaQuestPath = rk.GetValue("_IndonesiaQuestPath").ToString();
            _RusiaQuestPath = rk.GetValue("_RusiaQuestPath").ToString();
            _EuropeQuestPath = rk.GetValue("_EuropeQuestPath").ToString();

            _DefalutNPCPath = rk.GetValue("_DefalutNPCPath").ToString();
            _KoreaNPCPath = rk.GetValue("_KoreaNPCPath").ToString();
            _JapanNPCPath = rk.GetValue("_JapanNPCPath").ToString();
            _ChinaNPCPath = rk.GetValue("_ChinaNPCPath").ToString();
            _USANPCPath = rk.GetValue("_USANPCPath").ToString();
            _TaiwanNPCPath = rk.GetValue("_TaiwanNPCPath").ToString();
            _SingaporeNMalaysiaNPCPath = rk.GetValue("_SingaporeNMalaysiaNPCPath").ToString();

            _ThailandNPCPath = rk.GetValue("_ThailandNPCPath").ToString();
            _IndonesiaNPCPath = rk.GetValue("_IndonesiaNPCPath").ToString();
            _RusiaNPCPath = rk.GetValue("_RusiaNPCPath").ToString();
            _EuropeNPCPath = rk.GetValue("_EuropeNPCPath").ToString();
        }
       
        // 기본 퀘스트 경로 프로퍼티
        public string DefaultQuestPath
        {
            set
            {
                _DefaultQuestPath = value;
            }

            get
            {
                return _DefaultQuestPath;
            }
        }

        // 기본 NPC 경로 프로퍼티
        public string DefaultNPCPath
        {
            set
            {
                _DefalutNPCPath = value;
            }

            get
            {
                return _DefalutNPCPath;
            }
        }

        // 한국어 퀘스트 경로 프로퍼티
        public string KoreaQuestPath
        {
            set
            {
                _KoreaQuestPath = value;
            }

            get
            {
                return _KoreaQuestPath;
            }
        }

        // 한국어 NPC 경로 프로퍼티
        public string KoreaNPCPath
        {
            set
            {
                _KoreaNPCPath = value;
            }

            get
            {
                return _KoreaNPCPath;
            }
        }

        // 일본어 퀘스트 경로 프로퍼티
        public string JapanQuestPath
        {
            set
            {
                _JapanQuestPath = value;
            }

            get
            {
                return _JapanQuestPath;
            }
        }

        // 일본어 NPC 경로 프로퍼티
        public string JapanNPCPath
        {
            set
            {
                _JapanNPCPath = value;
            }

            get
            {
                return _JapanNPCPath;
            }
        }

        // 중국어 퀘스트 경로 프로퍼티
        public string ChinaQuestPath
        {
            set
            {
                _ChinaQuestPath = value;
            }

            get
            {
                return _ChinaQuestPath;
            }
        }

        // 중국어 NPC 경로 프로퍼티
        public string ChinaNPCPath
        {
            set
            {
                _ChinaNPCPath = value;
            }

            get
            {
                return _ChinaNPCPath;
            }
        }

        // 영어 퀘스트 경로 프로퍼티
        public string USAQuestPath
        {
            set
            {
                _USAQuestPath = value;
            }

            get
            {
                return _USAQuestPath;
            }
        }

        // 영어 NPC 경로 프로퍼티
        public string USANPCPath
        {
            set
            {
                _USANPCPath = value;
            }

            get
            {
                return _USANPCPath;
            }
        }
             

        /// <summary>
        /// 대만 Quest 경로 프로퍼티
        /// </summary>
        public string TaiwanQuestPath
        {
            set
            {
                _TaiwanQuestPath = value;
            }
            get
            {
                return _TaiwanQuestPath;
            }
        }

        /// <summary>
        /// 대만 NPC 경로 프로퍼티
        /// </summary>
        public string TaiwanNPCPath
        {
            set
            {
                _TaiwanNPCPath = value;
            }
            get
            {
                return _TaiwanNPCPath;
            }
        }

        /// <summary>
        /// 싱가포르 & 말레이시아 Quest 경로 프로퍼티
        /// </summary>
        public string SingaporeNMalaysiaQuestPath
        {
            set
            {
                _SingaporeNMalaysiaQuestPath = value;
            }
            get
            {
                return _SingaporeNMalaysiaQuestPath;
            }
        }

        /// <summary>
        /// 싱가포르 & 말레이시아 NPC 경로 프로퍼티
        /// </summary>
        public string SingaporeNMalaysiaNPCPath
        {
            set
            {
                _SingaporeNMalaysiaNPCPath = value;
            }
            get
            {
                return _SingaporeNMalaysiaNPCPath;
            }
        }


        // 태국 Quest 경로 프로퍼티
        public string ThailandQuestPath
        {
            set
            {
                _ThailandQuestPath = value;
            }

            get
            {
                return _ThailandQuestPath;
            }
        }

        // 인도네시아 Quest 경로 프로퍼티
        public string IndonesiaQuestPath
        {
            set
            {
                _IndonesiaQuestPath = value;
            }

            get
            {
                return _IndonesiaQuestPath;
            }
        }

        // 러시아 Quest 경로 프로퍼티
        public string RusiaQuestPath
        {
            set
            {
                _RusiaQuestPath = value;
            }

            get
            {
                return _RusiaQuestPath;
            }
        }

        // 유럽 Quest 경로 프로퍼티
        public string EuropeQuestPath
        {
            set
            {
                _EuropeQuestPath = value;
            }

            get
            {
                return _EuropeQuestPath;
            }
        }

        /// <summary>
        /// 태국 NPC 경로 프로퍼티
        /// </summary>
        public string ThailandNPCPath
        {
            set
            {
                _ThailandNPCPath = value;
            }
            get
            {
                return _ThailandNPCPath;
            }
        }


        /// <summary>
        /// 인도네시아 NPC 경로 프로퍼티
        /// </summary>
        public string IndonesiaNPCPath
        {
            set
            {
                _IndonesiaNPCPath = value;
            }
            get
            {
                return _IndonesiaNPCPath;
            }
        }


        /// <summary>
        /// 러시아 NPC 경로 프로퍼티
        /// </summary>
        public string RusiaNPCPath
        {
            set
            {
                _RusiaNPCPath = value;
            }
            get
            {
                return _RusiaNPCPath;
            }
        }


        /// <summary>
        /// 유럽 NPC 경로 프로퍼티
        /// </summary>
        public string EuropeNPCPath
        {
            set
            {
                _EuropeNPCPath = value;
            }
            get
            {
                return _EuropeNPCPath;
            }
        }

        public void ReloadPath()
        {
            RegistryKey rk = Registry.LocalMachine.OpenSubKey(regSubkey, true);

            if (rk != null)
            {
                _DefaultQuestPath = rk.GetValue("_DefaultQuestPath").ToString();
                _KoreaQuestPath = rk.GetValue("_KoreaQuestPath").ToString();
                _JapanQuestPath = rk.GetValue("_JapanQuestPath").ToString();
                _ChinaQuestPath = rk.GetValue("_ChinaQuestPath").ToString();
                _USAQuestPath = rk.GetValue("_USAQuestPath").ToString();
                _TaiwanQuestPath = rk.GetValue("_TaiwanQuestPath").ToString();
                _SingaporeNMalaysiaQuestPath = rk.GetValue("_SingaporeNMalaysiaQuestPath").ToString();

                _ThailandQuestPath = rk.GetValue("_ThailandQuestPath").ToString();
                _IndonesiaQuestPath = rk.GetValue("_IndonesiaQuestPath").ToString();
                _RusiaQuestPath = rk.GetValue("_RusiaQuestPath").ToString();
                _EuropeQuestPath = rk.GetValue("_EuropeQuestPath").ToString();


                _DefalutNPCPath = rk.GetValue("_DefalutNPCPath").ToString();
                _KoreaNPCPath = rk.GetValue("_KoreaNPCPath").ToString();
                _JapanNPCPath = rk.GetValue("_JapanNPCPath").ToString();
                _ChinaNPCPath = rk.GetValue("_ChinaNPCPath").ToString();
                _USANPCPath = rk.GetValue("_USANPCPath").ToString();
                _TaiwanNPCPath = rk.GetValue("_TaiwanNPCPath").ToString();
                _SingaporeNMalaysiaNPCPath = rk.GetValue("_SingaporeNMalaysiaNPCPath").ToString();

                _ThailandNPCPath = rk.GetValue("_ThailandNPCPath").ToString();
                _IndonesiaNPCPath = rk.GetValue("_IndonesiaNPCPath").ToString();
                _RusiaNPCPath = rk.GetValue("_RusiaNPCPath").ToString();
                _EuropeNPCPath = rk.GetValue("_EuropeNPCPath").ToString();
            }
        }

        public string getQuestPathByCountry(eCOUNTRY ecountry)
        {
            switch (ecountry)
            {
                case eCOUNTRY.KOR:
                    return _KoreaQuestPath;
                case eCOUNTRY.CHN:
                    return _ChinaQuestPath;
                case eCOUNTRY.JPN:
                    return _JapanQuestPath;
                case eCOUNTRY.USA:
                    return _USAQuestPath;
                case eCOUNTRY.TWN:
                    return _TaiwanQuestPath;
                case eCOUNTRY.SIN:
                    return _SingaporeNMalaysiaQuestPath;
              
                case eCOUNTRY.THA:
                    return _ThailandQuestPath;
                case eCOUNTRY.IDN:
                    return _IndonesiaQuestPath;
                case eCOUNTRY.RUS:
                    return _RusiaQuestPath;
                case eCOUNTRY.EU:
                    return _EuropeQuestPath;

                default:
                    return _DefaultQuestPath;
            }
        }

        public string getNPCPathByCountry(eCOUNTRY ecountry)
        {
            switch (ecountry)
            {
                case eCOUNTRY.KOR:
                    return _KoreaNPCPath;
                case eCOUNTRY.CHN:
                    return _ChinaNPCPath;
                case eCOUNTRY.JPN:
                    return _JapanNPCPath;
                case eCOUNTRY.USA:
                    return _USANPCPath;
                case eCOUNTRY.TWN:
                    return _TaiwanNPCPath;
                case eCOUNTRY.SIN:
                    return _SingaporeNMalaysiaNPCPath;

                case eCOUNTRY.THA:
                    return _ThailandNPCPath;
                case eCOUNTRY.IDN:
                    return _IndonesiaNPCPath;
                case eCOUNTRY.RUS:
                    return _RusiaNPCPath;
                case eCOUNTRY.EU:
                    return _EuropeNPCPath;
                default:
                    return _DefalutNPCPath;
            }
        }
        
    }
}