using System;
using System.Data;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Windows.Forms;

namespace DNQuest
{
    public class SaveLua
    {
        private int m_nHuntCount = 0;
        private string m_strRemoteStartData = "";
        private DataSet m_savingDataSet = null;
        private string m_strFilename = "";
        private string m_strQuestType = ""; // 서버에서 사용하지 않는 값
        private int m_nCompleteType = 0;

        public SaveLua()
        {
        }

        public void Clear()
        {
            m_nHuntCount = 0;
            m_strRemoteStartData = "";
            m_savingDataSet = null;
            m_strFilename = "";
            m_nCompleteType = 0;
            m_strQuestType = "";
        }

        public void Execute(DataSet savingDataSet, string strFilename, int nQuestType, int nCompleteType)
        {
            m_nHuntCount = 0;
            m_strRemoteStartData = "";
            m_savingDataSet = savingDataSet;
            m_strFilename = strFilename;
            m_nCompleteType = nCompleteType;
            if (nQuestType == 0)   //main Quest
                m_strQuestType = "2";
            else                                //Sub Quest, 모험자 퀘스트, 일일 Quest
                m_strQuestType = "1";

            string qename = m_savingDataSet.Tables["quest_desc"].Rows[0]["qename"].ToString(); // 퀘스트 영문이름
            string qid = m_savingDataSet.Tables["quest_desc"].Rows[0]["qid"].ToString(); // 퀘스트 아이디
            string next_qename = m_savingDataSet.Tables["quest_desc"].Rows[0]["nextqname"].ToString(); // 다음 퀘스트 이름
            string next_qid = m_savingDataSet.Tables["quest_desc"].Rows[0]["nextqid"].ToString(); // 다음 퀘스트 아이디
            string next_qnpc = m_savingDataSet.Tables["quest_desc"].Rows[0]["nextqnpc"].ToString(); // 다음 퀘스트 NPC 이름
            string str = "";
            string str2 = "";
            string data = "";
            string oncount = "\r\nfunction " + qename + "_OnCounting( userObjID, CountingType, CountIndex, Count, TargetCount, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n";
            string oncomplete = "\r\nfunction " + qename + "_OnCompleteCounting( userObjID, CountingType, CountIndex, Count, TargetCount, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n";
            string onall = "\r\nfunction " + qename + "_OnCompleteAllCounting( userObjID, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n\tlocal questID=" + qid + ";\r\n";

            data += "function " + qename + "_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)";
            data += "\r\n-----------------------------------------------------------------------------------------------";
            data += "\r\n\r\n\tlocal npcID = api_npc_GetNpcIndex(userObjID, npcObjID);\r\n";

            DataRowCollection rows_npc = m_savingDataSet.Tables["qnpc"].Rows;

            foreach (DataRow row_npc in rows_npc)
            {
                data += "\r\n\tif npcID == " + row_npc["npcid"].ToString() + " then";
                data += "\r\n\t\t" + qename + "_OnTalk_" + row_npc["npcename"].ToString() + "(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID);";
                data += "\r\n\t\treturn;";
                data += "\r\n\tend\r\n";
            }

            data += "\r\nend\r\n------------------------------------------------------------------------------------------------\r\n\r\n";

            foreach (DataRow row_npc in rows_npc)
            {
                str += "--" + row_npc["npcename"].ToString() + "--------------------------------------------------------------------------------";
                str += "\r\nfunction " + qename + "_OnTalk_" + row_npc["npcename"].ToString() + "(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)";
                str += "\r\n------------------------------------------------------------------------------------------";
                str += "\r\n\r\n\tif npc_talk_index == \"!quit\" then";
                str += "\r\n\t\treturn;";
                str += "\r\n\tend\r\n\tlocal queststep = api_quest_GetQuestStep(userObjID, questID);";
                str += "\r\n\r\n\tif npc_talk_index == \"q_enter\" then\r\n";
                str2 = "\r\n\t---------------------------------\r\n\telse\r\n\r\n";

                #region 퀘스트 없을 단계

                DataRow[] rows_noquest = m_savingDataSet.Tables["talk_paragraph"].Select("index = '" + row_npc["npcename"].ToString() + "-noquest'");

                if (rows_noquest.Length > 0)
                {
                    DataRow row_noquest = rows_noquest[0];
                    str += "\r\n\t\t if api_quest_UserHasQuest(userObjID, questID) <= 0 then";
                    str += "\r\n\t\t\t npc_talk_index = \"" + row_noquest["index"].ToString() + "\";";
                    str += "\r\n\t\t\t if  api_quest_IsPlayingQuestMaximum(userObjID) == 1 then ";
                    str += "\r\n\t\t\t\t npc_talk_index = \"_full_quest\";";
                    str += "\r\n\t\t\t end ";
                    str += "\r\n\t\t\t\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
                    str += "\r\n\t\t end";
                }

                #endregion

                #region 퀘스트 받는 단계

                DataRow[] rows_accept = m_savingDataSet.Tables["talk_paragraph"].Select("index = '" + row_npc["npcename"].ToString() + "-accepting'");

                if (rows_accept.Length > 0)
                {
                    int talk_Count = 0;

                    DataRow row_accept = rows_accept[0];

                    string[] index = row_accept["index"].ToString().Split('-');

                    str += "\r\n\t\t if api_quest_UserHasQuest(userObjID, questID) <= 0 then";

                    DataRow[] rows_exelink = m_savingDataSet.Tables["exelink"].Select("qstep = 'accepting' and npc = '" + index[0] + "'");

                    string str_sub = "";

                    foreach (DataRow row_exelink in rows_exelink)
                    {
                        if (row_exelink["sub"].ToString() == "")
                        {
                            str += "\r\n\t\t" + _writeExeBlock(row_exelink, "accepting", "exelink_Text", ref talk_Count, next_qid, next_qnpc, next_qename, false);
                        }
                        else
                        {
                            if (str_sub.IndexOf(row_exelink["sub"].ToString() + ":" + row_exelink["exelink_Text"].ToString()) < 0)
                            {
                                string sub = row_exelink["sub"].ToString().Replace("accepting", "");

                                if (sub == "")
                                {
                                    str2 += "\tif npc_talk_index == \"" + index[0] + "-accepting\" then\r\n";
                                    str2 += _writeExeBlock(row_exelink, "accepting", "exelink_Text", ref talk_Count, next_qid, next_qnpc, next_qename, false) + "\r\n";
                                    str2 += "\tend\r\n";
                                }
                                else
                                {
                                    str2 += "\tif npc_talk_index == \"" + index[0] + "-accepting-" + row_exelink["sub"].ToString() + "\" then\r\n";
                                    str2 += _writeExeBlock(row_exelink, "accepting", "exelink_Text", ref talk_Count, next_qid, next_qnpc, next_qename, false) + "\r\n";
                                    str2 += "\tend\r\n";
                                }

                                str_sub += row_exelink["sub"].ToString() + ":" + row_exelink["exelink_Text"].ToString() + ";";
                            }
                        }
                    }

                    str += "\r\n\t\t\t if  api_quest_IsPlayingQuestMaximum(userObjID) == 1 then ";
                    str += "\r\n\t\t\t\t npc_talk_index = \"_full_quest\";";
                    str += "\r\n\t\t\t\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
                    str += "\r\n\t\t\t\t return; ";
                    str += "\r\n\t\t\t end ";
                    if (rows_exelink.Length > 0)
                    {
                        str += "\r\n\t\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + index[0] + "-accepting\", npc_talk_target);";
                    }

                    str += "\r\n\t\tend";
                }

                #endregion

                #region 퀘스트 일반 단계

                str += "\r\n\t\twhile queststep > -1 do \r\n\t\t---------------------------------\r\n";

                DataRow[] rows_general = m_savingDataSet.Tables["talk_paragraph"].Select("index like '" + row_npc["npcename"].ToString() + "-%' and index not like'%noquest%' and index not like '%accepting%' and index <> '!quit' and index <> 'q_enter' and index <> '_no_quest' and index <> '_full_quest' and index <> '_full_inventory'");

                string flag = "";

                foreach (DataRow row_general in rows_general)
                {
                    string[] index = row_general["index"].ToString().Split('-');

                    if (index.Length == 2)
                    {
                        str += "\r\n\t\t\tif api_quest_UserHasQuest(userObjID, questID) > 0 and queststep == " + index[1] + " then\r\n";
                        str += "\t\t\t\tnpc_talk_index = \"" + row_general["index"].ToString() + "\";\r\n";

                        DataRow[] rows_exelink = m_savingDataSet.Tables["exelink"].Select("qstep = '" + index[1] + "' and npc = '" + index[0] + "'");

                        int talk_count = 0;

                        foreach (DataRow row_exelink in rows_exelink)
                        {
                            if (row_exelink["sub"].ToString() == "")
                            {
                                str += _writeExeBlock(row_exelink, index[1], "exelink_Text", ref talk_count, next_qid, next_qnpc, next_qename, false);
                            }
                            else
                            {
                                if (row_exelink["sub"].ToString() != "!next")
                                {
                                    int i;
                                    if (Int32.TryParse(row_exelink["sub"].ToString(), out i))
                                        str2 += "\tif npc_talk_index == \"" + index[0] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";
                                    else
                                        str2 += "\tif npc_talk_index == \"" + index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";

                                    str2 += _writeExeBlock(row_exelink, index[1], "exelink_Text", ref talk_count, next_qid, next_qnpc, next_qename, false);
                                    str2 += "\tend\r\n";
                                }
                                else
                                {
                                    if (flag != index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString())
                                    {
                                        flag = index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString();
                                        str2 += "\tif npc_talk_index == \"" + index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";
                                        str2 += "\t\tlocal cqresult = 1\r\n";
                                        str2 += _writeExeBlock(row_exelink, index[1], "exelink_Text", ref talk_count, next_qid, next_qnpc, next_qename, false);
                                        str2 += "\r\n\t\tif cqresult == 1 then\r\n";
                                        str2 += "\t\t\tapi_npc_NextScript(userObjID, npcObjID, \"" + next_qnpc + "-1\", \"" + next_qename + ".xml\");\r\n";
                                        str2 += "\t\t\treturn;\r\n";
                                        str2 += "\t\tend\r\n";
                                        str2 += "\tend\r\n";
                                    }
                                }
                            }

                            if (talk_count == 1)
                                str2 += "\treturn;\r\n";
                        }


                        str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
                        str += "\r\n\t\t\t\tbreak;\r\n";
                        str += "\t\t\tend\r\n";
                    }
                    else
                    {
                        DataRow[] rows_answer = m_savingDataSet.Tables["talk_answer"].Select("talk_paragraph_Id = '" + row_general["talk_paragraph_Id"] + "'");

                        foreach (DataRow row_answer in rows_answer)
                        {
                            if (row_answer["link_index"].ToString().IndexOf("!next") > -1)
                            {
                                if (flag != row_answer["link_index"].ToString())
                                {
                                    flag = row_answer["link_index"].ToString();

                                    str2 += "\tif npc_talk_index == \"" + row_answer["link_index"].ToString() + "\" then \r\n";

                                    DataRow[] rows_exelink = m_savingDataSet.Tables["exelink"].Select("qstep = '" + index[1] + "' and npc = '" + index[0] + "'");

                                    int talk_count = 0;

                                    foreach (DataRow row_exelink in rows_exelink)
                                    {
                                        if (row_exelink["sub"].ToString() == "!next")
                                        {
                                            str2 += "\r\n\t\tlocal cqresult = 1";
                                            str2 += _writeExeBlock(row_exelink, index[1], "exelink_Text", ref talk_count, next_qid, next_qnpc, next_qename, false);
                                            str2 += "\r\n\t\tif cqresult == 1 then\r\n";
                                            str2 += "\t\t\tapi_npc_NextScript(userObjID, npcObjID, \"" + next_qnpc + "-1\", \"" + next_qename + ".xml\");\r\n";
                                            str2 += "\t\t\treturn;\r\n";
                                            str2 += "\t\tend\r\n";
                                        }
                                        else
                                        {
                                            //str2 += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename);
                                        }
                                    }

                                    if (talk_count == 1)
                                        str2 += "\treturn;\r\n";

                                    str2 += "\tend\r\n";
                                }
                            }
                        }
                    }
                }

                str += "\r\n\t\t\tqueststep = queststep -1;\r\n\t\tend\r\n\r\n";
                str2 += "\r\n\t---------------------------------\r\n";
                str2 += "\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);\r\n";
                str2 += "\t end\r\n";
                #endregion

                str += str2;
                str += "------------------------------------------------------------------------------------------------\r\nend\r\n";
            }

            data += str;

            #region 사냥 처리 루틴
            DataRow[] rows_exe = m_savingDataSet.Tables["exe"].Select("type='huntmon' or type='huntitem' or type='huntsymbol' or type='huntall' or type='checkStageClearRank' ");

            int talk_count_blank = 0;

            foreach (DataRow row_exe in rows_exe)
            {
                DataRow[] executions = m_savingDataSet.Tables["executions"].Select("executions_Id = '" + row_exe["executions_Id"].ToString() + "'");
                DataRow[] qstep_change = m_savingDataSet.Tables["qstep_change"].Select("qstep_change_Id = '" + executions[0]["qstep_change_Id"].ToString() + "'");
                switch (row_exe["type"].ToString())
                {
                    case "checkStageClearRank":
                        {
                            oncount += "\t local qstep= " + qstep_change[0]["qstep"].ToString() + "; \r\n";
                            oncount += "\t if qstep == " + qstep_change[0]["qstep"].ToString() + " then \r\n";
                            oncount += "\t\t if api_user_GetLastStageClearRank( userObjID ) " + row_exe["true"].ToString().Replace("[", "<").Replace("]", ">") + " " + row_exe["rand"].ToString() + " then \r\n";
                            oncount += _writeExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "onexe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
                            oncount += "\t\t end \r\n";
                            oncount += "\t end \r\n";
                            break;
                        }
                    case "huntmon":
                    case "huntitem":
                    case "huntsymbol":
                        {
                            string[] string_monster = row_exe["exe_Text"].ToString().Split(',');
                            int monsterCount = row_exe["num"] != null ? Convert.ToInt32(row_exe["num"].ToString()) : 0;

                            foreach (string monster in string_monster)
                            {
                                int temp = 0;

                                if (Int32.TryParse(qstep_change[0]["qstep"].ToString(), out temp))
                                {
                                    oncount += "\tif qstep == " + (temp + 1).ToString() + " and CountIndex == " + monster;
                                    oncount += " then\r\n";
                                }
                                else
                                {
                                    if (qstep_change[0]["qstep"].ToString() == "accepting")
                                        oncount += "\tif qstep == 1 and CountIndex == " + monster + " then\r\n";
                                    else
                                        oncount += "\tif qstep == " + qstep_change[0]["qstep"].ToString() + " and CountIndex == " + monster + " then\r\n";
                                }

                                if (row_exe["onexe"].ToString() != "")
                                {
                                    oncount += _writeExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "onexe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
                                }

                                oncount += "\r\n\tend\r\n";

                                if (Int32.TryParse(qstep_change[0]["qstep"].ToString(), out temp))
                                {
                                    oncomplete += "\tif qstep == " + (temp + 1).ToString() + " and CountIndex == " + monster;
                                    if (row_exe["num"].ToString().Trim().Length > 0)
                                        oncomplete += " and Count >= TargetCount ";

                                    oncomplete += " then\r\n";
                                }
                                else
                                {
                                    if (qstep_change[0]["qstep"].ToString() == "accepting")
                                    {
                                        oncomplete += "\tif qstep == 1 and CountIndex == " + monster;
                                        if (row_exe["num"].ToString().Trim().Length > 0)
                                            oncomplete += " and Count >= TargetCount ";

                                        oncomplete += " then\r\n";
                                    }
                                    else
                                    {
                                        oncomplete += "\tif qstep == " + qstep_change[0]["qstep"].ToString() + " and CountIndex == " + monster;
                                        if (row_exe["num"].ToString().Trim().Length > 0)
                                            oncomplete += " and Count >= TargetCount ";

                                        oncomplete += " then\r\n";
                                    }
                                }

                                if (row_exe["exe"].ToString() != "")
                                    oncomplete += _writeExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "exe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);

                                oncomplete += "\r\n\tend\r\n";
                            }
                        }
                        break;
                    case "huntall":
                        onall += "\tif qstep == " + (Convert.ToInt16(qstep_change[0]["qstep"].ToString()) + 1).ToString() + " then\r\n";
                        onall += _writeExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "all", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
                        onall += "\tend\r\n";
                        break;
                }
            }
            #endregion

            oncount = oncount + "end\r\n";
            oncomplete = oncomplete + "end\r\n";
            onall = onall + "end\r\n";

            data += oncount + oncomplete + onall;

            #region 원격 시작 루틴
            string onremotestart = "\r\nfunction " + qename + "_OnRemoteStart( userObjID, questID )\r\n";
            onremotestart += m_strRemoteStartData;
            onremotestart += "end\r\n";
            data += onremotestart;
            #endregion

            DataRow[] rows_remotecomplete = m_savingDataSet.Tables["remotecomplete"].Select();
            if (0 < rows_remotecomplete.Length)
            {
                int qstep = Convert.ToInt32(rows_remotecomplete[0]["qstep"].ToString());

                #region 원격 완료 루틴
                string onremotecomplete = "\r\nfunction " + qename + "_OnRemoteComplete( userObjID, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, questID);\r\n";
                onremotecomplete += "\tif qstep == " + qstep.ToString() + " then\r\n";
                onremotecomplete += _writeExeBlock(rows_remotecomplete[0], qstep.ToString(), "rc_Text", ref talk_count_blank, next_qid, next_qnpc, next_qename, false);
                onremotecomplete += "\tend\r\nend\r\n";
                data += onremotecomplete;
                #endregion

                #region 원격 완료 체크 루틴
                string canremotecompletestep = "\r\nfunction " + qename + "_CanRemoteCompleteStep( userObjID, questID, questStep )\r\n" + "\tif questStep == " + qstep.ToString() + " then\r\n\t\treturn true;\r\n\tend\r\n\r\n\treturn false;\r\nend\r\n";
                data += canremotecompletestep;
                #endregion
            }
            else
            {
                // 원격으로 시작 하지만 완료는 하지 않는 퀘스트를 위해서 추가한다.

                #region 원격 완료 체크 루틴
                string canremotecompletestep = "\r\nfunction " + qename + "_CanRemoteCompleteStep( userObjID, questID, questStep )\r\n\treturn false;\r\nend\r\n";
                data += canremotecompletestep;
                #endregion
            }

            using (FileStream fs = new FileStream(m_strFilename, FileMode.Create))
            {
                byte[] data_byte = Encoding.UTF8.GetBytes(_replaceScript(ref data));
                fs.Write(data_byte, 0, data_byte.Length);
            }
        }

        private string _replaceScript(ref string str)
        {
            const string VillageServerTag_Start = "<VillageServer>";
            const string VillageServerTag_End = "</VillageServer>";
        
            const string GameServerTag_Start = "<GameServer>";
            const string GameServerTag_End = "</GameServer>";

            //VillageServerTag 추가
            StringBuilder sbVillageServer = new StringBuilder();
            sbVillageServer.AppendLine(VillageServerTag_Start);
            sbVillageServer.AppendLine("");
            sbVillageServer.Append(str);
            sbVillageServer.AppendLine("");
            sbVillageServer.AppendLine(VillageServerTag_End);

            /**
             * function, api 마다 "pRoom," 추가
             * function pattern (function)\s\w+\s?\(
             * api_ pattern (api_)\w+\s?\(
             * 함수 명에 onTalk 포함 .*_OnTalk_.*\s?\
             */
            Regex pattern = new Regex(@"(function)\s\w+\s?\(|(api_)\w+\s?\(|.*_OnTalk_.*\s?\(", RegexOptions.IgnoreCase);
            Match m;
            string gameServerParameter = " pRoom, ";

            List<string> regiList = new List<string>();

            for (m = pattern.Match(str); m.Success; m = m.NextMatch())
            {
                if (!regiList.Contains(m.Groups[0].Value))
                {
                    regiList.Add(m.Groups[0].Value);
                    str = str.Replace(m.Groups[0].Value, m.Groups[0].Value + gameServerParameter);
                }
            }

            //Game Server Tag 추가
            sbVillageServer.AppendLine("");
            sbVillageServer.Append(GameServerTag_Start);
            sbVillageServer.AppendLine("");
            sbVillageServer.Append(str);
            sbVillageServer.AppendLine("");
            sbVillageServer.Append(GameServerTag_End);

            return sbVillageServer.ToString();
        }

        private string _writeExeBlock(DataRow p_exe, string p_step, string p_type, ref int p_talk, string p_next_qid, string p_next_npc, string p_next_qename, bool b_condition)
        {
            if (p_exe == null)
                return "";

            string qstep = p_step;
            string linkdata = "";
            bool remotecomplete = false;

            switch (p_type)
            {
                case "onexe":
                    linkdata = p_exe["onexe"].ToString();
                    break;
                case "exe":
                    linkdata = p_exe["exe"].ToString();
                    break;
                case "all":
                    linkdata = p_exe["exe_Text"].ToString();
                    break;
                case "rc_Text":
                    {
                        linkdata = p_exe["rc_Text"].ToString();
                        remotecomplete = true;
                    }
                    break;
                case "exelink_Text":
                    {
                        if (p_exe["qstep"].ToString() == "complete")
                            return "";

                        linkdata = p_exe["exelink_Text"].ToString();
                        qstep = p_exe["qstep"].ToString();
                    }
                    break;
                default:
                    linkdata = p_exe["exe_Text"].ToString();
                    break;
            }

            DataRow[] rows_qstep = m_savingDataSet.Tables["qstep_change"].Select("qstep = '" + qstep + "'");
            DataRow row_qstep = rows_qstep[0];
            DataRow[] rows_executions = m_savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_qstep["qstep_change_Id"].ToString() + "'");
            DataRow row_executions = rows_executions[0];
            DataRow[] rows_exe = m_savingDataSet.Tables["exe"].Select("executions_Id = '" + row_executions["executions_Id"].ToString() + "'");

            return _writeExeBlock_Detail(rows_exe, p_step, linkdata, ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, remotecomplete);
        }

        private string _writeExeBlock_Detail(DataRow[] p_exelink, string p_step, string p_count, ref int p_talk, string p_next_qid, string p_next_npc, string p_next_qename, bool b_condition, bool b_remotecomplete)
        {
            string ret_str = "";

            if (p_exelink == null)
            {
                return "";
            }

            string[] arr_index = p_count.Split(',');

            foreach (string index in arr_index)
            {
                if (index == "")
                    continue;

                if (Convert.ToInt16(index) >= p_exelink.Length)
                {
                    continue;
                }

                DataRow row_exe = p_exelink[Convert.ToInt16(index)];

                int rcstep = 0;
                DataRow[] rows_remotecomplete = m_savingDataSet.Tables["remotecomplete"].Select();
                if (0 < rows_remotecomplete.Length)
                {
                    rcstep = Convert.ToInt32(rows_remotecomplete[0]["qstep"].ToString());
                }

                switch (row_exe["type"].ToString())
                {
                    case "setstep":
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString() && false == b_condition)
                        {
                            m_strRemoteStartData += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";
                        }
                        break;
                    case "setjornal":
                        ret_str += "\t\t\t\tapi_quest_SetJournalStep(userObjID, questID, " + row_exe["exe_Text"].ToString() + ");\r\n";
                        break;
                    case "delitem":
                        if (row_exe["num"].ToString() == "-1")
                        {
                            ret_str += "\r\n";
                            ret_str += "\t\t\t\tif api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1) > 0 then \r\n";
                            ret_str += "\t\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1, questID));\r\n";
                            ret_str += "\t\t\t\tend\r\n";
                        }
                        else
                        {
                            ret_str += "\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                        }
                        break;
                    case "delsymbol":
                        if (row_exe["num"].ToString() == "-1")
                        {
                            if( m_nCompleteType == 0)
                            {
                                ret_str += "\r\n";
                                ret_str += "\t\t\t\tif api_quest_HasQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1) > 0 then \r\n";
                                ret_str += "\t\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", api_quest_HasQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1));\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                            else
                            {
                                ret_str += "\r\n";
                                ret_str += "\t\t\t\tif api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1) > 0 then \r\n";
                                ret_str += "\t\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1));\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        else
                        {
                            if (m_nCompleteType == 0)
                            {
                                ret_str += "\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                            }
                        }
                        break;
                    case "delquestitem":
                        if (row_exe["num"].ToString() == "-1")
                        {
                            ret_str += "\r\n";
                            ret_str += "\t\t\t\tif api_quest_HasQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1) > 0 then \r\n";
                            ret_str += "\t\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", api_quest_HasQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1));\r\n";
                            ret_str += "\t\t\t\tend\r\n";
                        }
                        else
                        {
                            ret_str += "\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        }
                        break;
                    case "additem":
                        {
                            string sExe_Text = row_exe["exe_Text"].ToString();
                            string sNum = row_exe["num"].ToString();
                            char splitChar = ',';
                            if (sExe_Text.IndexOf(splitChar) > 0)
                            {
                                string[] sSplitItem = sExe_Text.Split(splitChar);
                                string[] sSplitNum = sNum.Split(splitChar);
                                if (sSplitItem.Length != sSplitNum.Length)
                                {
                                    MessageBox.Show("아이템 인덱스 개수와 아이템 개수의 수가 일치하지 않습니다.\n 콤마(,)를 확인해 주세요.");
                                    return "";
                                }

                                ret_str += "\t\t\t\t local TableItem = { ";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {

                                    ret_str += "\t\t\t\t { " + sSplitItem[i] + ", " + sSplitNum[i] + " } ";
                                    ret_str += (sSplitItem.Length - 1 > i) ? " , \r\n" : " \r\n";
                                }

                                ret_str += " \t\t\t\t} \r\n";

                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItemList(userObjID, TableItem) == 1 then\r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\tapi_user_AddItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
                                }
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ") == 1 then\r\n";
                                ret_str += "\t\t\t\t\tapi_user_AddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        break;
                    case "addsymbol":
                        if (m_nCompleteType == 0)
                        {
                            ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ") == 1 then \r\n";
                            ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                            ret_str += "\t\t\t\telse\r\n";
                            ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                            ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                            ret_str += "\t\t\t\t\treturn;\r\n";
                            ret_str += "\t\t\t\tend\r\n";
                        }
                        else
                        {
                            ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ") == 1 then \r\n";
                            ret_str += "\t\t\t\tapi_user_AddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                            ret_str += "\t\t\t\telse\r\n";
                            ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                            ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                            ret_str += "\t\t\t\t\treturn;\r\n";
                            ret_str += "\t\t\t\tend\r\n";
                        }
                        break;
                    case "addquestitem":
                        {
                            string sExe_Text = row_exe["exe_Text"].ToString();
                            string sNum = row_exe["num"].ToString();
                            char splitChar = ',';
                            if (sExe_Text.IndexOf(splitChar) > 0)
                            {
                                string[] sSplitItem = sExe_Text.Split(splitChar);
                                string[] sSplitNum = sNum.Split(splitChar);
                                if (sSplitItem.Length != sSplitNum.Length)
                                {
                                    MessageBox.Show("아이템 인덱스 개수와 아이템 개수의 수가 일치하지 않습니다.\n 콤마(,)를 확인해 주세요.");
                                    return "";
                                }

                                ret_str += "\t\t\t\t local TableItem = { \r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t { " + sSplitItem[i].Trim() + ", " + sSplitNum[i].Trim() + " } ";
                                    ret_str += (sSplitItem.Length - 1 > i) ? " , \r\n" : " \r\n";
                                }

                                ret_str += " \t\t\t\t} \r\n";

                                ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem) == 1 then\r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
                                }
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ") == 1 then \r\n";
                                ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        break;
                    case "checkStageClearRank":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStartData += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                        }

                        ++m_nHuntCount;
                        break;
                    case "huntmon":
                        string[] array_monster = row_exe["exe_Text"].ToString().Split(',');

                        foreach (string monster in array_monster)
                        {
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";

                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                            {
                                m_strRemoteStartData += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                            }

                            ++m_nHuntCount;
                        }
                        break;
                    case "huntitem":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStartData += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        }

                        ++m_nHuntCount;
                        break;
                    case "huntsymbol":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (m_nHuntCount % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        ++m_nHuntCount;
                        break;
                    case "huntall":
                        break;
                    case "setDailyQuest":
                        ret_str += "\t\t\t\t if api_quest_AddDailyQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + m_strQuestType + ") == 1 then \r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["true"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false);
                        ret_str += "\t\t\t\t else \r\n";
                        if (row_exe["false"] == null || string.IsNullOrEmpty(row_exe["false"].ToString()))
                            ret_str += "\t\t\t\tnpc_talk_index = \"_daily_quest_fail\";";
                        else
                            ret_str += "\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["false"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false);

                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "setquest":
                        ret_str += "\t\t\t\tapi_quest_AddQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + m_strQuestType + ");\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        break;
                    case "msg":
                        ret_str += "\n\t\t\t\tlocal " + "TP_" + p_step + "_" + p_count.Replace(',', '_') + " = {};\n\t\t\t\tapi_user_UserMessage(userObjID," + row_exe["msg_type"].ToString() + ", " + row_exe["exe_Text"].ToString() + ", " + "TP_" + p_step + "_" + p_count.Replace(',', '_') + ");\r\n";
                        break;
                    case "talk":
                        int p_temp = 0;

                        if (Int32.TryParse(row_exe["exe_Text"].ToString(), out p_temp))
                        {
                            ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            p_talk = 1;
                        }
                        else
                        {
                            ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            p_talk = 1;
                        }

                        ++p_talk;
                        break;
                    case "random_coice":

                        string[] temp = row_exe["rand"].ToString().Split(',');
                        string[] exe = row_exe["exe_Text"].ToString().Split(',');
                        int sum = 0;

                        foreach (string value in temp)
                        {
                            sum += Convert.ToInt16(value);
                        }

                        ret_str += "\t\t\t\tlocal rcval_" + p_step + "_" + index + " = math.random(1," + sum.ToString() + ");\r\n";

                        int value_prev = 1;

                        for (int i = 0; i < temp.Length; ++i)
                        {
                            int value = 0;

                            if (i > 0)
                            {
                                for (int j = 0; j <= i; ++j)
                                {
                                    value += Convert.ToInt32(temp[j]);
                                }
                            }
                            else
                            {
                                value = Convert.ToInt32(temp[i]);
                            }

                            ret_str += "\t\t\t\tif rcval_" + p_step + "_" + index + " >= " + value_prev.ToString() + " and rcval_" + p_step + "_" + index + " < " + value.ToString() + " then\r\n";

                            DataRow[] row_tempqstepchange = m_savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");
                            DataRow[] row_tempexecution = m_savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_tempqstepchange[0]["qstep_change_Id"].ToString() + "'");
                            DataRow[] row_tempexe = m_savingDataSet.Tables["exe"].Select("executions_Id = '" + row_tempexecution[0]["executions_Id"].ToString() + "'");

                            ret_str += "\t" + _writeExeBlock_Detail(row_tempexe, p_step, exe[i], ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false);

                            ret_str += "\t\t\t\tend\r\n";

                            value_prev = value;
                        }

                        break;
                    case "conexe":
                        ret_str += "\t\t\t\tif " + _writeChkBlock(p_step, row_exe["exe_Text"].ToString()) + "then\r\n";

                        if (row_exe["true"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["true"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false) + "\r\n";

                        ret_str += "\t\t\t\telse" + "\r\n";

                        if (row_exe["false"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["false"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false) + "\r\n";

                        ret_str += "\t\t\t\tend\r\n";
                        break;
                    case "exp":
                        ret_str += "\t\t\t\tapi_user_AddExp(userObjID, questID, " + row_exe["exe_Text"].ToString() + ");\r\n";
                        break;
                    case "money":
                        ret_str += "\t\t\t\tapi_user_AddCoin(userObjID, " + row_exe["exe_Text"].ToString() + ", questID );\r\n";
                        break;
                    case "moneyDelete":
                        ret_str += "\t\t\t\t if api_user_GetCoin( userObjID ) > " + row_exe["num"].ToString() + " then \r\n";
                        ret_str += "\t\t\t\t\t api_user_DelCoin( userObjID, " + row_exe["num"].ToString() + ", questID ); \r\n";
                        ret_str += "\t\t\t\t\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["onexe"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false) + "\r\n";
                        ret_str += "\t\t\t\t else \r\n";
                        ret_str += "\t\t\t\t\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["exe_Text"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false) + "\r\n";
                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "complete":
                        // 반복성일 경우 - true, 일회성일 경우 - false
                        // comboBox_CompleteType[0] - 1회성, comboBox_CompleteType[1] - 반복성
                        if (m_nCompleteType == 0)
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, false);";
                        else
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, true);";

                        ret_str += "\r\n\t\t\t\tif cqresult == 1  then";

                        if (p_next_qid != "")
                        {
                            ret_str += "\r\n\t\t\t\t\tapi_quest_AddQuest(userObjID, " + p_next_qid + ", " + m_strQuestType + ");";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetQuestStep(userObjID, " + p_next_qid + ", 1);";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetJournalStep(userObjID, " + p_next_qid + ", 1);\r\n";
                        }

                        if ("" != row_exe["true"].ToString())
                        {
                            if (row_exe["type"].ToString() != "complete" || row_exe["true"].ToString() != index) // 무한루프 제거
                            {
                                ret_str += "\r\n\t\t\t\t\t" + _writeExeBlock_Detail(p_exelink, p_step, row_exe["true"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, b_condition, false) + "\r\n";
                            }                            
                        }

                        ret_str += "\r\n\t\t\t\telse";
                        ret_str += "\r\n\t\t\t\t\tnpc_talk_index = \"_full_inventory\";\r\n";
                        ret_str += "\r\n\t\t\t\tend\r\n";
                        break;
                    case "movie":
                        ret_str += "\t\t\t\tapi_user_PlayCutScene(userObjID, " + row_exe["exe_Text"].ToString() + ");\r\n";
                        break;
                    case "delcount":
                        ret_str += "\t\t\t\tapi_quest_ClearCountingInfo(userObjID, questID);\r\n";
                        break;
                    case "custom":
                        //커스텀 코드로 api_quest_RewardQuestUser를 넣었고 questID가 없을 경우 questID를 넣어준다.
                        if (row_exe["exe_Text"].ToString().IndexOf("api_quest_RewardQuestUser") >= 0 && row_exe["exe_Text"].ToString().IndexOf("questID") < 0)
                        {
                            string temp_str = row_exe["exe_Text"].ToString();
                            temp_str = temp_str.Insert(temp_str.LastIndexOf(')'), ", questID");
                            ret_str += "\t\t\t\t" + temp_str + "\r\n";
                        }
                        else
                        {
                            ret_str += "\t\t\t\t" + row_exe["exe_Text"].ToString() + "\r\n";
                        }
                        break;
                    case "return":
                        ret_str += "\t\t\t\treturn;\r\n";
                        break;
                    case "fullinven":
                        ret_str += "\t\t\t\tnpc_talk_index = \"_full_inventory\";\r\n";
                        break;
                    case "fullquest":
                        ret_str += "\t\t\t\tnpc_talk_index = \"_full_quest\";\r\n";
                        break;
                    case "recompense_view":
                        {
                            //Class 별 보상창 출력 구현 20091030
                            string exe_Text = row_exe["exe_Text"].ToString();
                            if (exe_Text.IndexOf(',') > -1)
                            {
                                string[] classesReward = exe_Text.Split(',');
                                for (int i = 0; i < classesReward.Length; i++)
                                {
                                    if (classesReward[i].Trim().Length < 1)
                                        classesReward[i] = classesReward[0];
                                }

                                ret_str += "\t\t\t if api_user_GetUserClassID(userObjID) == 1 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[1] + ", false);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 2 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[2] + ", false);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 3 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[3] + ", false);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 4 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[4] + ", false);\r\n";
                                if (classesReward.Length > 5)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 5 then \r\n";
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[5] + ", false);\r\n";
                                }
                                if (classesReward.Length > 6)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 6 then \r\n";
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[6] + ", false);\r\n";
                                }
                                if (classesReward.Length > 7)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 7 then \r\n";
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[7] + ", false);\r\n";
                                }
                                ret_str += "\t\t\t end \r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tapi_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", false);\r\n";
                            }
                        }
                        break;
                    case "recompense":
                        {
                            string exe_Text = row_exe["exe_Text"].ToString();
                            if (exe_Text.IndexOf(',') > -1)
                            {
                                string[] classesReward = exe_Text.Split(',');
                                for (int i = 0; i < classesReward.Length; i++)
                                {
                                    if (classesReward[i].Trim().Length < 1)
                                    {
                                        classesReward[i] = classesReward[0];
                                    }
                                }

                                ret_str += "\t\t\t if api_user_GetUserClassID(userObjID) == 1 then \r\n";
                                if (false == b_remotecomplete)
                                {
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[1] + ", true);\r\n";
                                }
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[1] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 2 then \r\n";
                                if (false == b_remotecomplete)
                                {
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[2] + ", true);\r\n";
                                }
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[2] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 3 then \r\n";
                                if (false == b_remotecomplete)
                                {
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[3] + ", true);\r\n";
                                }
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[3] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 4 then \r\n";
                                if (false == b_remotecomplete)
                                {
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[4] + ", true);\r\n";
                                }
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[4] + ", questID, 1);\r\n";
                                if (classesReward.Length > 5)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 5 then \r\n";
                                    if (false == b_remotecomplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[5] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[5] + ", questID, 1);\r\n";
                                }
                                if (classesReward.Length > 6)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 6 then \r\n";
                                    if (false == b_remotecomplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[6] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[6] + ", questID, 1);\r\n";
                                }
                                if (classesReward.Length > 7)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 7 then \r\n";
                                    if (false == b_remotecomplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[7] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[7] + ", questID, 1);\r\n";
                                }

                                ret_str += "\t\t\t end \r\n";
                            }
                            else
                            {
                                if (false == b_remotecomplete)
                                {
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", true);\r\n";
                                }
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + row_exe["exe_Text"].ToString() + ", questID, 1);\r\n";
                            }
                        }
                        break;
                    case "addItemAndQuestItem":
                        {
                            string sExe_Text = row_exe["exe_Text"].ToString();
                            string sNum = row_exe["num"].ToString();
                            char charSplitForQuestItemORItem = '|';
                            char splitChar = ',';

                            string[] sQuestItemsOrItems = null;
                            string[] sQuestItemsNumOrItemsNum = null;

                            if (sExe_Text.IndexOf(charSplitForQuestItemORItem) > 0 && sNum.IndexOf(charSplitForQuestItemORItem) > 0)
                            {
                                sQuestItemsOrItems = sExe_Text.Split(charSplitForQuestItemORItem);
                                sQuestItemsNumOrItemsNum = sNum.Split(charSplitForQuestItemORItem);
                            }
                            else
                            {
                                MessageBox.Show("Quest Item과 일반 아이템이 꼭 존재해야 합니다. "
                                    + "\n값이 존재하는지 확인하여주십시오."
                                    + "\nXML내에 문자(|)을 확인하여 주십시오.");
                                return "";
                            }

                            if (sExe_Text.IndexOf(splitChar) > 0)
                            {
                                string[] sSplitItem = sQuestItemsOrItems[0].Split(splitChar);
                                string[] sSplitNum = sQuestItemsNumOrItemsNum[0].Split(splitChar);
                                string[] sSplitQuestItem = sQuestItemsOrItems[1].Split(splitChar);
                                string[] sSplitQuestNum = sQuestItemsNumOrItemsNum[1].Split(splitChar);

                                if (sSplitItem.Length != sSplitNum.Length || sSplitQuestItem.Length != sSplitQuestNum.Length)
                                {
                                    MessageBox.Show("아이템 인덱스 개수와 아이템 개수의 수가 일치하지 않습니다.\n 콤마(,)를 확인해 주세요.");
                                    return "";
                                }

                                ret_str += "\t\t\t\t local TableItem = { ";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t { " + sSplitItem[i] + ", " + sSplitNum[i] + " } ";
                                    ret_str += (sSplitItem.Length - 1 > i) ? " , \r\n" : " \r\n";
                                }
                                ret_str += " \t\t\t\t} \r\n";

                                ret_str += "\t\t\t\t local TableItem2 = { ";
                                for (int i = 0; i < sSplitQuestItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t { " + sSplitQuestItem[i] + ", " + sSplitQuestNum[i] + " } ";
                                    ret_str += (sSplitQuestItem.Length - 1 > i) ? " , \r\n" : " \r\n";
                                }
                                ret_str += " \t\t\t\t} \r\n";

                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItemList(userObjID, TableItem) == 1 and api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem2) == 1 then\r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\tapi_user_AddItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
                                }
                                for (int i = 0; i < sSplitQuestItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sSplitQuestItem[i] + ", " + sSplitQuestNum[i] + ", questID);\r\n";
                                }
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID, " + sQuestItemsOrItems[0] + ", " + sQuestItemsNumOrItemsNum[0] + ") == 1 and api_quest_CheckQuestInvenForAddItem( userObjID, " + sQuestItemsOrItems[1] + ", " + sQuestItemsNumOrItemsNum[1] + ") == 1  then\r\n";
                                ret_str += "\t\t\t\t\tapi_user_AddItem(userObjID, " + sQuestItemsOrItems[0] + ", " + sQuestItemsNumOrItemsNum[0] + ", questID);\r\n";
                                ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sQuestItemsOrItems[1] + ", " + sQuestItemsNumOrItemsNum[1] + ", questID);\r\n";
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        break;
                }
            }

            return ret_str;
        }

        private string _writeChkBlock(string p_step, string p_condition)
        {
            DataRow[] rows_conditions = m_savingDataSet.Tables["conditions"].Select("qstep_change_Id = '" + p_step + "'");
            DataRow[] rows_chk = m_savingDataSet.Tables["chk"].Select("conditions_Id = '" + rows_conditions[0]["conditions_Id"].ToString() + "'");

            string ret_string = "";

            string[] condition = p_condition.Split(' ');

            foreach (string str in condition)
            {
                int temp = 0;

                if (!int.TryParse(str, out temp))
                {
                    ret_string += " " + str.ToLower() + " ";
                }
                else
                {
                    switch (rows_chk[temp]["type"].ToString())
                    {
                        case "lvl":
                            ret_string += "api_user_GetUserLevel(userObjID) " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "item":
                            if (rows_chk[temp]["op"].ToString() == "[")
                                ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                            else if (rows_chk[temp]["op"].ToString() == "[=")
                                ret_string += "( api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                            else if (rows_chk[temp]["op"].ToString() == "[]")
                                ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                            else if (rows_chk[temp]["op"].ToString() == "=")
                                ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                            else
                                ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["num"].ToString() + " ";

                            break;
                        case "symbol":
                            if (rows_chk[temp]["op"].ToString() == "[")
                            {
                                if (m_nCompleteType == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "[=")
                            {
                                if (m_nCompleteType == 0)
                                    ret_string += "( api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                                else
                                    ret_string += "( api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "[]")
                            {
                                if (m_nCompleteType == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "=")
                            {
                                if (m_nCompleteType == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                            }
                            else
                            {
                                if (m_nCompleteType == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["num"].ToString() + " ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["num"].ToString() + " ";
                            }
                            break;
                        case "questitem":
                            if (rows_chk[temp]["op"].ToString() == "[")
                                ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                            else if (rows_chk[temp]["op"].ToString() == "[=")
                                ret_string += "( api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                            else if (rows_chk[temp]["op"].ToString() == "[]")
                                ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                            else if (rows_chk[temp]["op"].ToString() == "=")
                                ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                            else
                                ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["num"].ToString() + " ";

                            break;

                        case "class":
                            if (rows_chk[temp]["op"].ToString() == "=")
                                ret_string += "api_user_GetUserClassID(userObjID) == " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            else if (rows_chk[temp]["op"].ToString() == "[")
                                ret_string += "api_user_GetUserClassID(userObjID) " + rows_chk[temp]["op"].ToString().Replace('[', '<') + " " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            else if (rows_chk[temp]["op"].ToString() == "]")
                                ret_string += "api_user_GetUserClassID(userObjID) " + rows_chk[temp]["op"].ToString().Replace(']', '>') + " " + rows_chk[temp]["chk_Text"].ToString() + " ";

                            break;

                        case "prob":
                            ret_string += "math.random(1,1000) <= " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "hasquest":
                            ret_string += "api_quest_UserHasQuest(userObjID," + rows_chk[temp]["chk_Text"].ToString() + ") > -1 ";
                            break;
                        case "hascomq":
                            ret_string += "api_quest_IsMarkingCompleteQuest(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ") == 1 ";
                            break;
                        case "inven_eslot":
                            ret_string += "api_user_GetUserInvenBlankCount(userObjID) " + rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "custom":
                            ret_string += rows_chk[temp]["chk_Text"].ToString() + "  ";
                            break;
                        case "qcount":
                            ret_string += " api_quest_GetPlayingQuestCnt(userObjID) >= g_maxquest ";
                            break;
                        case "stageConstructionLevel":
                            ret_string += " api_user_GetStageConstructionLevel( userObjID ) ";
                            if (rows_chk[temp]["op"].ToString() == "=")
                                ret_string += rows_chk[temp]["op"].ToString().Replace("=", "==");
                            else
                                ret_string += rows_chk[temp]["op"].ToString().Replace('[', '<').Replace(']', '>');

                            ret_string += " " + rows_chk[temp]["chk_Text"].ToString() + " ";
                            break;
                    }
                }
            }

            return ret_string;
        }
    }
}