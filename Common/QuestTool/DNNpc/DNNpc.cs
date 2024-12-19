using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Xml;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace DNNpc
{
    public partial class DNNpc : Form
    {
        private int Script_Count = 0;

        private XmlDocument LoadedXml = new XmlDocument();
        private DataSet LoadedDataSet = new DataSet();
        private DataSet ds_save;

        private String sSavePath = @"R:\GameRes\Resource\Script\Talk_Npc";

        private readonly string VillageServerTag_Start = "<VillageServer>\r\n";
        private readonly string VillageServerTag_End = "\r\n</VillageServer>\r\n\r\n";

        private readonly string GameServerTag_Start = "<GameServer>\r\n";
        private readonly string GameServerTag_End = "\r\n</GameServer>\r\n";

        private string NPC_Path = "";

        private int Quest_Mid = 2; // 퀘스트 번역용 MID

        //모두 저장 시, "저장하시겠습니까?"를 띄울 지 확인 true = 띄움, false = 띄우지 않음.
        public static bool NpcAllSaveCheck = true;

        public DNNpc()
        {
            InitializeComponent();            
        }

        public DNNpc(string savePath, string fileName)
        {
            InitializeComponent();
            this.sSavePath = savePath;
            if (fileName.Contains(@"\"))
                NPC_Path = fileName;
            else
                NPC_Path = savePath + @"\" + fileName;
        }

        public void TestScript()
        {
            try
            {
                FileInfo loadFile = new FileInfo(NPC_Path);
                FileInfo loadFile_Lua = new FileInfo(NPC_Path.Substring(0, NPC_Path.LastIndexOf('.')) + ".lua");

                if (loadFile.Exists)
                {
                    if (loadFile.IsReadOnly)
                    {
                        MessageBox.Show( @"저장하는 XML파일이 읽기전용입니다.\n
Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다."
                            , "확인"
                            , MessageBoxButtons.OK
                            , MessageBoxIcon.Warning);
                    }
                    else
                    {
                        ds_save = null;
                        SaveXML(NPC_Path);

                        if (loadFile_Lua.Exists)
                        {
                            if (loadFile_Lua.IsReadOnly)
                            {
                                MessageBox.Show( @"저장하는 Lua파일이 읽기전용입니다.\n
Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다."
                                    , "확인"
                                    , MessageBoxButtons.OK
                                    , MessageBoxIcon.Warning);
                            }
                            else
                            {
                                SaveLua(NPC_Path.Substring(0, NPC_Path.LastIndexOf('.')) + ".lua");
                            }
                        }
                        else
                        {
                            SaveLua(NPC_Path.Substring(0, NPC_Path.LastIndexOf('.')) + ".lua");
                        }
                    }
                }
                else
                {
                    ds_save = null;
                    SaveXML(NPC_Path);
                    SaveLua(NPC_Path.Substring(0, NPC_Path.LastIndexOf('.')) + ".lua");
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void DNNpc_Load(object sender, EventArgs e)
        {
            Quest_Condition qc = new Quest_Condition();

            qc.Location = new Point(0, 166);
            qc.Name = "condition_init";
            tabPage_Basic.Controls.Add(qc);

            init_Script();

            if (NPC_Path != "")
            {
                FileInfo loadFile = new FileInfo(NPC_Path);

                this.Text = "NPC-" + loadFile.Name;

                if (loadFile.IsReadOnly)
                {
                    LoadedDataSet = new DataSet();
                    ds_save = null;

                    init_Basic();
                    init_Script();

                    LoadXmlFile(NPC_Path);

                    Set_Basic_Info();
                    Set_Condition_Info();
                    Set_Execution_Info();
                    Set_Talk_Info();

                }
                else
                {
                    LoadedDataSet = new DataSet();
                    ds_save = null;

                    init_Basic();
                    init_Script();

                    LoadXmlFile(NPC_Path);

                    Set_Basic_Info();
                    Set_Condition_Info();
                    Set_Execution_Info();
                    Set_Talk_Info();

                }
            }
        }

        #region 초기화
        private void init_Basic()
        {
            textBox_click.Text = "";
            textBox_nename.Text = "";
            textBox_nid.Text = "";
            textBox_nkname.Text = "";

            tabPage_Basic.Controls["condition_init"].Dispose();

            Quest_Condition qc = new Quest_Condition();

            qc.Location = new Point(0, 166);
            qc.Name = "condition_init";
            tabPage_Basic.Controls.Add(qc);
        }

        private void init_Script()
        {
            Script_Count = 0;

            panel_ScriptMain.Controls.Clear();

            Quest_Step qstep = new Quest_Step();
            qstep.Name = "script_" + Script_Count.ToString();
            qstep.Tag = Script_Count;

            panel_ScriptMain.Controls.Add(qstep);

            ++Script_Count;
        }
        #endregion

        #region 대사 추가
        private void Add_Script()
        {
            Quest_Step qs = new Quest_Step();

            qs.Location = new Point(0, getLastControlPosition(panel_ScriptMain));

            qs.Name = "script_" + Script_Count.ToString(); ;
            qs.Tag = Script_Count;

            panel_ScriptMain.Controls.Add(qs);

            ++Script_Count;
        }

        private Quest_Step Add_Script(string index, string script)
        {
            Quest_Step qs = new Quest_Step();

            qs.Location = new Point(0, getLastControlPosition(panel_ScriptMain));

            qs.Name = "script_" + Script_Count.ToString(); ;
            qs.Tag = Script_Count;
            qs.Set_Vaule(script, index);

            panel_ScriptMain.Controls.Add(qs);

            ++Script_Count;

            return qs;
        }

        private int getLastControlPosition(Panel p_page)
        {
            if (p_page.Controls.Count == 0)
                return 0;
            else
                return p_page.Controls[p_page.Controls.Count - 1].Location.Y + p_page.Controls[p_page.Controls.Count - 1].Height;
        }
        #endregion

        #region 메뉴 클릭 이벤트
        public void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NpcAllSaveCheck = true;
            this.Close();
        }
        
        private void button_ScriptReset_Click(object sender, EventArgs e)
        {
            Script_Count = 0;
            panel_ScriptMain.Controls.Clear();
            Quest_Step qstep = new Quest_Step();
            panel_ScriptMain.Controls.Add(qstep);
            ++Script_Count;
        }

        private void button_ScriptAdd_Click(object sender, EventArgs e)
        {
            Add_Script();
        }        
                
        private void button_ScriptDel_Click(object sender, EventArgs e)
        {
            if (Script_Count > 0)
            {
                panel_ScriptMain.Height = panel_ScriptMain.Height - panel_ScriptMain.Controls[panel_ScriptMain.Controls.Count - 1].Height;
                panel_ScriptMain.Controls[panel_ScriptMain.Controls.Count - 1].Dispose();
                --Script_Count;
            }
        }        

        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog loadXmlDialog = new OpenFileDialog();

            loadXmlDialog.Filter = "*.xml|*.XML";
            loadXmlDialog.InitialDirectory = this.sSavePath;

            DirectoryInfo di = new DirectoryInfo(loadXmlDialog.InitialDirectory);
            if (!di.Exists)
                loadXmlDialog.InitialDirectory = @"C:\";

            if (loadXmlDialog.ShowDialog() == DialogResult.OK)
            {
                FileInfo loadFile = new FileInfo(loadXmlDialog.FileName);

                this.Text = "NPC-" + loadFile.Name;

                if (loadFile.IsReadOnly)
                {
                    LoadedDataSet = new DataSet();
                    ds_save = null;

                    init_Basic();
                    init_Script();

                    LoadXmlFile(loadXmlDialog.FileName);

                    Set_Basic_Info();
                    Set_Condition_Info();
                    Set_Execution_Info();
                    Set_Talk_Info();
                }
                else
                {
                    LoadedDataSet = new DataSet();
                    ds_save = null;

                    init_Basic();                    
                    init_Script();

                    LoadXmlFile(loadXmlDialog.FileName);

                    Set_Basic_Info();
                    Set_Condition_Info();
                    Set_Execution_Info();
                    Set_Talk_Info();   
                }
            }
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LoadedDataSet = new DataSet();
            ds_save = null;

            init_Basic();
            init_Script();
        }

        private void allSave()
        {
            SaveFileDialog loadXmlDialog = new SaveFileDialog();

            loadXmlDialog.Filter = "*.xml|*.xml";
            loadXmlDialog.InitialDirectory = this.sSavePath;

            DirectoryInfo di = new DirectoryInfo(loadXmlDialog.InitialDirectory);

            if (!di.Exists)
            {
                loadXmlDialog.InitialDirectory = @"C:\";
            }

            loadXmlDialog.FileName = tabPage_Basic.Controls["groupBox_Basic"].Controls["textBox_nename"].Text + ".xml";
            string lua_FileName = tabPage_Basic.Controls["groupBox_Basic"].Controls["textBox_nename"].Text + ".lua";

            if (loadXmlDialog.ShowDialog() == DialogResult.OK)
            {
                FileInfo loadFile = new FileInfo(loadXmlDialog.FileName);
                FileInfo loadFile_Lua = new FileInfo(lua_FileName);

                if (loadFile.Exists)
                {
                    if (loadFile.IsReadOnly)
                    {
                        MessageBox.Show("저장하는 XML파일이 읽기전용입니다. \n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.", "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    else
                    {
                        ds_save = null;
                        SaveXML(loadXmlDialog.FileName);

                        if (loadFile_Lua.Exists)
                        {
                            if (loadFile_Lua.IsReadOnly)
                            {
                                MessageBox.Show("저장하는 Lua파일이 읽기전용입니다. \n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.", "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            }
                            else
                            {
                                SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                            }
                        }
                        else
                        {
                            SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                        }
                    }
                }
                else
                {
                    ds_save = null;
                    SaveXML(loadXmlDialog.FileName);

                    if (loadFile_Lua.Exists)
                    {

                        if (loadFile_Lua.IsReadOnly)
                        {
                            MessageBox.Show("저장하는 Lua파일이 읽기전용입니다. \n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.", "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                        else
                        {
                            SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                        }
                    }
                    else
                    {
                        SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                    }
                }
            }
        }

        public void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            allSave();
        }

        #endregion        

        //XML 파일 구조 보기(테스트용)
        private string DataSet_View()
        {
            string List = "";

            foreach (DataRelation relation in LoadedDataSet.Relations)
            {
                List += relation.RelationName + "," + relation.Nested.ToString() + "&" + relation.ParentTable.TableName + "&" + relation.ChildTable.TableName + ";\n";
                foreach (DataColumn parent_col in relation.ParentColumns)
                {
                    List += "\t" + parent_col.ColumnName + ";";
                }
                List += "\n";
                foreach (DataColumn child_col in relation.ChildColumns)
                {
                    List += "\t" + child_col.ColumnName + ";";
                }
                List += "\n";
            }

            List += "\n";

            foreach (DataTable dt in LoadedDataSet.Tables)
            {
                List += dt.TableName + ";\n";
                foreach (DataColumn col in dt.Columns)
                {
                    List += "\t" + col.ColumnName + "&" + col.ColumnMapping.ToString() + ";\n";
                }

                foreach (DataRow row in dt.Rows)
                {
                    List += "\t\t";
                    foreach (DataColumn col in dt.Columns)
                    {
                        List += row[col].ToString() + ";";
                    }
                    List += "\n";
                }
            }

            return List;
        } 
        
        #region XML파일 로딩
        private void LoadXmlFile(string p_path)
        {
            try
            {
                using (FileStream fs = new FileStream(p_path, FileMode.Open, FileAccess.Read))
                {
                    using (StreamReader sr = new StreamReader(fs, System.Text.Encoding.UTF8))
                    {
                        LoadedDataSet.ReadXml(sr);
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

            textBox1.Text = DataSet_View();
        }

        private void Set_Basic_Info()
        {
            if (LoadedDataSet.Tables["talk"] != null)
            {
                DataTable dt_Basic = LoadedDataSet.Tables["talk"];

                DataTable dt_npckname = null;

                if (LoadedDataSet.Tables["npckname"] == null)
                {
                    textBox_nkname.Text = dt_Basic.Rows[0]["npckname"].ToString();                    
                }
                else
                {
                    dt_npckname = LoadedDataSet.Tables["npckname"];

                    textBox_nkname.Text = dt_npckname.Rows[0]["npckname_Text"].ToString();
                    textBox_nkname.Tag = dt_npckname.Rows[0]["mid"];
                }

                textBox_nename.Text = dt_Basic.Rows[0]["npcename"].ToString();
                textBox_nid.Text = dt_Basic.Rows[0]["npcid"].ToString();
                textBox_click.Text = dt_Basic.Rows[0]["startexe"].ToString();
            }
        }

        private void Set_Condition_Info()
        {
            if (LoadedDataSet.Tables["chk"] != null)
            {
                DataTable dt_Condition = LoadedDataSet.Tables["chk"];
                Quest_Condition qc = (Quest_Condition)tabPage_Basic.Controls["condition_init"];

                if (dt_Condition != null)
                {
                    foreach (DataRow row in dt_Condition.Rows)
                    {
                        switch (row["type"].ToString())
                        {
                            case "lvl":
                                Conditions.Condition_Level con_lvl = new Conditions.Condition_Level();
                                con_lvl.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                                qc.Add_Condition(con_lvl);
                                break;
                            case "item":
                                Conditions.Condition_GeneralItem con_item = new Conditions.Condition_GeneralItem();
                                con_item.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                                qc.Add_Condition(con_item);
                                break;
                            case "symbol":
                                Conditions.Condition_SymbolItem con_symbol = new Conditions.Condition_SymbolItem();
                                con_symbol.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                                qc.Add_Condition(con_symbol);
                                break;
                            case "class":
                                Conditions.Condition_Class con_class = new Conditions.Condition_Class();
                                con_class.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                                qc.Add_Condition(con_class);
                                break;
                            case "prob":
                                Conditions.Condition_Prob con_prob = new Conditions.Condition_Prob();
                                con_prob.Set_Value(row["chk_Text"].ToString());
                                qc.Add_Condition(con_prob);
                                break;
                            case "hasquest":
                                Conditions.Condition_Quest con_quest = new Conditions.Condition_Quest();
                                con_quest.Set_Value(row["chk_Text"].ToString());
                                qc.Add_Condition(con_quest);
                                break;
                            case "hascomq":
                                Conditions.Condition_Compelete con_comp = new Conditions.Condition_Compelete();
                                con_comp.Set_Value(row["chk_Text"].ToString());
                                qc.Add_Condition(con_comp);
                                break;
                            case "inven_eslot":
                                Conditions.Condition_Inven con_inven = new Conditions.Condition_Inven();
                                con_inven.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                                qc.Add_Condition(con_inven);
                                break;
                            case "custom":
                                Conditions.Condition_Custum con_custom = new Conditions.Condition_Custum();
                                con_custom.Set_Value(row["chk_Text"].ToString());
                                qc.Add_Condition(con_custom);
                                break;
                        }
                    }
                }
            }
        }

        private void Set_Execution_Info()
        {
            if (LoadedDataSet.Tables["exe"] != null)
            {
                DataTable dt_Execution = LoadedDataSet.Tables["exe"];
                Quest_Condition qc = (Quest_Condition)tabPage_Basic.Controls["condition_init"];

                if (dt_Execution != null)
                {
                    foreach (DataRow row in dt_Execution.Rows)
                    {
                        switch (row["type"].ToString())
                        {
                            case "enterquest":
                                Executions.Exe_Quest exe_quest = new Executions.Exe_Quest();
                                exe_quest.Set_Value(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_quest);
                                break;
                            case "talk":
                                Executions.Exe_Script exe_talk = new Executions.Exe_Script();
                                exe_talk.Set_Value(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_talk);
                                break;
                            case "msg":
                                Executions.Exe_Message exe_message = new Executions.Exe_Message();
                                exe_message.Set_Value(row["exe_Text"].ToString(), row["mag_type"].ToString(), row["substring"].ToString());
                                qc.Add_Execution(exe_message);
                                break;
                            case "movie":
                                Executions.Exe_Movie exe_movie = new Executions.Exe_Movie();
                                exe_movie.Set_Value(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_movie);
                                break;
                            case "conexe":
                                Executions.Exe_Condition exe_condition = new Executions.Exe_Condition();
                                exe_condition.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                                qc.Add_Execution(exe_condition);
                                break;
                            case "openshop":
                                Executions.Exe_Store exe_shop = new Executions.Exe_Store();
                                exe_shop.Set_Value(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_shop);
                                break;
                            case "openskillshop":
                                Executions.Exe_SkillStore exe_skillshop = new Executions.Exe_SkillStore();
                                exe_skillshop.Set_Value(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_skillshop);
                                break;
                            case "openbazar":
                                Executions.Exe_NoManStore exe_bazar = new Executions.Exe_NoManStore();
                                qc.Add_Execution(exe_bazar);
                                break;
                            case "openplate":
                                Executions.Exe_StringCombi exe_plate = new Executions.Exe_StringCombi();
                                qc.Add_Execution(exe_plate);
                                break;
                            case "openjewel":
                                Executions.Exe_BoOkAdd exe_jewel = new Executions.Exe_BoOkAdd();
                                qc.Add_Execution(exe_jewel);
                                break;
                            case "custom":
                                Executions.Exe_Custum exe_custom = new Executions.Exe_Custum();
                                exe_custom.Set_Code(row["exe_Text"].ToString());
                                qc.Add_Execution(exe_custom);
                                break;
                            case "cutsceneCompleteChangeMap":
                                {
                                    Executions.Exe_CutsceneCompleteChangeMap exe_cccm = new Executions.Exe_CutsceneCompleteChangeMap();
                                    exe_cccm.txtCutSceneNo.Text = row["cutsceneNo"].ToString();
                                    exe_cccm.txtStageNo.Text = row["stageNo"].ToString();
                                    exe_cccm.txtGateNo.Text = row["gateNo"].ToString();
                                break;
                                }
                        }
                    }
                }
            }
        }

        private void Set_Talk_Info()
        {
            tabPage_Paragraph.Controls["panel_ScriptMain"].Controls.Clear();

            DataTable dt_talk = LoadedDataSet.Tables["talk_paragraph"];
            DataTable dt_question = null;

            if (dt_talk != null)
            {
                foreach (DataRow row_talk in dt_talk.Rows)
                {
                    Quest_Step qs = null;

                    if (LoadedDataSet.Tables["talk_question"] == null)
                    {
                        qs = Add_Script(row_talk["index"].ToString(), row_talk["talk_question"].ToString());
                    }
                    else
                    {
                        dt_question = LoadedDataSet.Tables["talk_question"];

                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row_talk["talk_paragraph_Id"].ToString());

                        qs = Add_Script(row_talk["index"].ToString(), row_question[0]["talk_question_Text"].ToString());
                        
                        int mid = Convert.ToInt32(row_question[0]["mid"]);
                        qs.textBox_Script.Tag = mid;
                        if (Quest_Mid < mid)
                            Quest_Mid = mid + 1;
                    }

                    if (dt_talk.Columns["image"] != null)
                    {
                        qs.textBox_Image.Text = row_talk["image"].ToString();
                    }

                    if (dt_talk.Columns["itemindex"] != null)
                    {
                        qs.txtItemIndex.Text = row_talk["itemindex"].ToString();
                    }

                    DataTable dt_answer = LoadedDataSet.Tables["talk_answer"];

                    if (dt_answer != null)
                    {
                        DataRow[] rows_answer = dt_answer.Select("talk_paragraph_Id = " + row_talk["talk_paragraph_Id"].ToString() + "");

                        foreach (DataRow dr_answer in rows_answer)
                        {
                            if (dr_answer["link_index"].ToString() == "q_enter")
                            {
                                Quest_Link link = new Quest_Link();

                                link.Set_Value(dr_answer["talk_answer_Text"].ToString(), dr_answer["link_target"].ToString());

                                if (dt_answer.Columns["mid"] != null)
                                {
                                    int mid = Convert.ToInt32(dr_answer["mid"]);
                                    link.textBox_Select.Tag = mid;
                                    if (Quest_Mid < mid)
                                        Quest_Mid = mid + 1;
                                }

                                qs.Add_Xml(link);
                            }
                            else
                            {
                                if (LoadedDataSet.Tables["exelink"] != null)
                                {
                                    DataTable dt_link = LoadedDataSet.Tables["exelink"];
                                    DataRow[] rows_link = dt_link.Select("idx='" + dr_answer["link_index"].ToString() + "'");

                                    Quest_SelectControl select = new Quest_SelectControl();

                                    if (rows_link.Length > 0)
                                    {
                                        select.Set_Value(rows_link[0]["exelink_Text"].ToString(), dr_answer["talk_answer_Text"].ToString(), dr_answer["link_index"].ToString());
                                    }
                                    else
                                    {
                                        select.Set_Value("", dr_answer["talk_answer_Text"].ToString(), dr_answer["link_index"].ToString());
                                    }

                                    if (dt_answer.Columns["mid"] != null)
                                    {
                                        int mid = Convert.ToInt32(dr_answer["mid"]);
                                        select.textBox_Select.Tag = mid;
                                        if (Quest_Mid < mid)
                                            Quest_Mid = mid + 1;
                                    }

                                    qs.Add_Select(select);
                                }
                                else
                                {
                                    Quest_SelectControl select = new Quest_SelectControl();
                                    select.Set_Value("", dr_answer["talk_answer_Text"].ToString(), dr_answer["link_index"].ToString());

                                    if (dt_answer.Columns["mid"] != null)
                                    {
                                        int mid = Convert.ToInt32(dr_answer["mid"]);
                                        select.textBox_Select.Tag = mid;
                                        if (Quest_Mid < mid)
                                            Quest_Mid = mid + 1;
                                    }

                                    qs.Add_Select(select);
                                }
                            }
                        }
                    }
                }
            }

            ++Quest_Mid;
        }
        #endregion

        #region XML 파일 저장

        private DataSet MakeDataSet()
        {
            DataSet SaveDataSet = new DataSet();

            #region 데이터 테이블 생성
            DataTable talk = new DataTable("talk");
            DataTable npckname = new DataTable("npckname");   
            DataTable talk_paragraph = new DataTable("talk_paragraph");
            DataTable talk_question = new DataTable("talk_question");
            DataTable talk_answer = new DataTable("talk_answer");            
            DataTable conditions = new DataTable("conditions");
            DataTable executions = new DataTable("executions");
            DataTable chk = new DataTable("chk");
            DataTable exe = new DataTable("exe");
            DataTable exelink = new DataTable("exelink");            
            #endregion

            #region talk
            //DataColumn npckname = new DataColumn("npckname");
            DataColumn npcename = new DataColumn("npcename");
            DataColumn npcid = new DataColumn("npcid");
            DataColumn startexe = new DataColumn("startexe");
            DataColumn talk_Id = new DataColumn("talk_Id");
            talk_Id.ColumnMapping = MappingType.Hidden;
            //DataColumn talk_conditions = new DataColumn("conditions");

            //talk.Columns.Add(npckname);
            talk.Columns.Add(npcename);
            talk.Columns.Add(npcid);
            talk.Columns.Add(startexe);
            talk.Columns.Add(talk_Id);
            //talk.Columns.Add(talk_conditions);
            
            #endregion            

            #region npckname (번역)

            DataColumn npckname_Text = new DataColumn("npckname_Text");
            npckname_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn npcmid = new DataColumn("mid");
            npcmid.ColumnMapping = MappingType.Attribute;
            DataColumn ncpkname_talk_Id = new DataColumn("talk_Id");
            ncpkname_talk_Id.ColumnMapping = MappingType.Hidden;

            npckname.Columns.Add(npcmid);
            npckname.Columns.Add(npckname_Text);
            npckname.Columns.Add(ncpkname_talk_Id);           

            #endregion

            #region talk_paragraph

            //DataColumn talk_question = new DataColumn("talk_question");
            DataColumn talk_paragraph_Id = new DataColumn("talk_paragraph_Id");
            talk_paragraph_Id.ColumnMapping = MappingType.Hidden;
            DataColumn index = new DataColumn("index");
            index.ColumnMapping = MappingType.Attribute;
            DataColumn paragraph_talk_Id = new DataColumn("talk_Id");
            paragraph_talk_Id.ColumnMapping = MappingType.Hidden;
            DataColumn talk_image = new DataColumn("image");
            talk_image.ColumnMapping = MappingType.Attribute;
            DataColumn itemIndex = new DataColumn("itemindex");
            itemIndex.ColumnMapping = MappingType.Attribute;

            //talk_paragraph.Columns.Add(talk_question);
            talk_paragraph.Columns.Add(talk_paragraph_Id);
            talk_paragraph.Columns.Add(index);
            talk_paragraph.Columns.Add(talk_image);
            talk_paragraph.Columns.Add(paragraph_talk_Id);
            talk_paragraph.Columns.Add(itemIndex);

            #endregion

            #region talk_question(번역관련 추가)

            DataColumn talk_question_mid = new DataColumn("mid");
            talk_question_mid.ColumnMapping = MappingType.Attribute;
            DataColumn talk_question_Text = new DataColumn("talk_question_Text");
            talk_question_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn talk_question_paragraph_Id = new DataColumn("talk_paragraph_Id");
            talk_question_paragraph_Id.ColumnMapping = MappingType.Hidden;

            talk_question.Columns.Add(talk_question_mid);
            talk_question.Columns.Add(talk_question_Text);
            talk_question.Columns.Add(talk_question_paragraph_Id);

            #endregion

            #region talk_answer

            DataColumn link_index = new DataColumn("link_index");
            link_index.ColumnMapping = MappingType.Attribute;
            DataColumn link_mid = new DataColumn("mid");
            link_mid.ColumnMapping = MappingType.Attribute;
            DataColumn link_target = new DataColumn("link_target");
            link_target.ColumnMapping = MappingType.Attribute;
            DataColumn talk_answer_Text = new DataColumn("talk_answer_Text");
            talk_answer_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn answer_talk_paragraph_Id = new DataColumn("talk_paragraph_Id");
            answer_talk_paragraph_Id.ColumnMapping = MappingType.Hidden;

            talk_answer.Columns.Add(link_index);
            talk_answer.Columns.Add(link_mid);
            talk_answer.Columns.Add(link_target);
            talk_answer.Columns.Add(talk_answer_Text);
            talk_answer.Columns.Add(answer_talk_paragraph_Id);

            #endregion            

            #region conditions

            DataColumn conditions_Id = new DataColumn("conditions_Id");
            conditions_Id.ColumnMapping = MappingType.Hidden;
            DataColumn con_talk_Id = new DataColumn("talk_Id");
            con_talk_Id.ColumnMapping = MappingType.Hidden;

            conditions.Columns.Add(conditions_Id);
            conditions.Columns.Add(con_talk_Id);

            #endregion

            #region executions

            DataColumn executions_Id = new DataColumn("executions_Id");
            executions_Id.ColumnMapping = MappingType.Hidden;
            DataColumn exe_talk_Id = new DataColumn("talk_Id");
            exe_talk_Id.ColumnMapping = MappingType.Hidden;

            executions.Columns.Add(executions_Id);
            executions.Columns.Add(exe_talk_Id);

            #endregion

            #region chk

            DataColumn chk_type = new DataColumn("type");
            chk_type.ColumnMapping = MappingType.Attribute;
            DataColumn op = new DataColumn("op");
            op.ColumnMapping = MappingType.Attribute;
            DataColumn chk_num = new DataColumn("num");
            chk_num.ColumnMapping = MappingType.Attribute;
            DataColumn chk_Text = new DataColumn("chk_Text");
            chk_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn chk_conditions_Id = new DataColumn("conditions_Id");
            chk_conditions_Id.ColumnMapping = MappingType.Hidden;

            chk.Columns.Add(chk_type);
            chk.Columns.Add(chk_num);
            chk.Columns.Add(op);
            chk.Columns.Add(chk_Text);
            chk.Columns.Add(chk_conditions_Id);

            #endregion

            #region exe

            DataColumn type = new DataColumn("type");
            type.ColumnMapping = MappingType.Attribute;            
            DataColumn exe_true = new DataColumn("true");
            exe_true.ColumnMapping = MappingType.Attribute;
            DataColumn exe_false = new DataColumn("false");
            exe_false.ColumnMapping = MappingType.Attribute;
            DataColumn substring = new DataColumn("substring");
            substring.ColumnMapping = MappingType.Attribute;
            DataColumn msg_type = new DataColumn("mag_type");
            msg_type.ColumnMapping = MappingType.Attribute;            
            DataColumn exe_Text = new DataColumn("exe_Text");
            exe_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn exe_executions_Id = new DataColumn("executions_Id");
            exe_executions_Id.ColumnMapping = MappingType.Hidden;

            exe.Columns.Add(type); 
            exe.Columns.Add(msg_type);
            exe.Columns.Add(substring);
            exe.Columns.Add(exe_true);
            exe.Columns.Add(exe_false);
            exe.Columns.Add(exe_Text);
            exe.Columns.Add(exe_executions_Id);

            #endregion

            #region exelink

            DataColumn exe_idx = new DataColumn("idx");
            exe_idx.ColumnMapping = MappingType.Attribute;
            DataColumn exelink_talk_Id = new DataColumn("talk_Id");
            exelink_talk_Id.ColumnMapping = MappingType.Hidden;        
            DataColumn exelink_Text = new DataColumn("exelink_Text");
            exelink_Text.ColumnMapping = MappingType.SimpleContent;

            exelink.Columns.Add(exe_idx);
            exelink.Columns.Add(exelink_talk_Id);
            exelink.Columns.Add(exelink_Text);

            #endregion

            SaveDataSet.Tables.Add(talk);
            SaveDataSet.Tables.Add(npckname);
            SaveDataSet.Tables.Add(talk_paragraph);
            SaveDataSet.Tables.Add(talk_question);
            SaveDataSet.Tables.Add(talk_answer);            
            SaveDataSet.Tables.Add(conditions);
            SaveDataSet.Tables.Add(executions);
            SaveDataSet.Tables.Add(chk);
            SaveDataSet.Tables.Add(exe);
            SaveDataSet.Tables.Add(exelink);            

            #region Relation 생성  
          
            SaveDataSet.Relations.Add("talk_npckname", talk_Id, ncpkname_talk_Id);
            SaveDataSet.Relations["talk_npckname"].Nested = true;

            SaveDataSet.Relations.Add("talk_paragraph_talk_answer", talk_paragraph_Id, answer_talk_paragraph_Id);
            SaveDataSet.Relations["talk_paragraph_talk_answer"].Nested = true;

            SaveDataSet.Relations.Add("talk_paragraph_talk_question", talk_paragraph_Id, talk_question_paragraph_Id);
            SaveDataSet.Relations["talk_paragraph_talk_question"].Nested = true;


            SaveDataSet.Relations.Add("executions_exe", executions_Id, exe_executions_Id);
            SaveDataSet.Relations["executions_exe"].Nested = true;

            SaveDataSet.Relations.Add("conditions_chk", conditions_Id, chk_conditions_Id);
            SaveDataSet.Relations["conditions_chk"].Nested = true;

            SaveDataSet.Relations.Add("talk_talk_paragraph", talk_Id, paragraph_talk_Id);
            SaveDataSet.Relations["talk_talk_paragraph"].Nested = true;

            SaveDataSet.Relations.Add("talk_executions", talk_Id, exe_talk_Id);
            SaveDataSet.Relations["talk_executions"].Nested = true;

            SaveDataSet.Relations.Add("talk_conditions", talk_Id, con_talk_Id);
            SaveDataSet.Relations["talk_conditions"].Nested = true;

            SaveDataSet.Relations.Add("talk_exelink", talk_Id, exelink_talk_Id);
            SaveDataSet.Relations["talk_exelink"].Nested = true;
            #endregion

            return SaveDataSet;

        }

        private void SaveXML(string p_path)
        {
            ds_save = MakeDataSet();

            //ds_save.DataSetName = "talk";            

            #region NPC 기본정보
            DataRow row_Basic = ds_save.Tables["talk"].NewRow();

            //row_Basic["npckname"] = textBox_nkname.Text;
            row_Basic["npcename"] = textBox_nename.Text;
            row_Basic["npcid"] = textBox_nid.Text;
            row_Basic["startexe"] = textBox_click.Text;
            row_Basic["talk_Id"] = 0;

            ds_save.Tables["talk"].Rows.Add(row_Basic);

            #region NPC 이름 정보 저장
            DataRow row_qkname = ds_save.Tables["npckname"].NewRow();

            if (textBox_nkname.Tag == null)
            {
                row_qkname["mid"] = 1;
                textBox_nkname.Tag = 1;
            }
            else
            {
                row_qkname["mid"] = textBox_nkname.Tag.ToString();
            }

            row_qkname["npckname_Text"] = "<![CDATA[" + textBox_nkname.Text + "]]>";
            row_qkname["talk_Id"] = 0;

            ds_save.Tables["npckname"].Rows.Add(row_qkname);
            
            #endregion

            DataRow row_executions = ds_save.Tables["executions"].NewRow();

            row_executions["executions_Id"] = 0;
            row_executions["talk_Id"] = 0;

            ds_save.Tables["executions"].Rows.Add(row_executions);

            DataRow row_conditions = ds_save.Tables["conditions"].NewRow();

            row_conditions["conditions_Id"] = 0;
            row_conditions["talk_Id"] = 0;

            ds_save.Tables["conditions"].Rows.Add(row_conditions);
            #endregion

            #region 조건문 정보

            Quest_Condition qc = (Quest_Condition)tabPage_Basic.Controls["condition_init"];

            Control.ControlCollection conditions = qc.Get_Conditions();

            foreach (Control condition in conditions)
            {                
                switch (condition.Name)
                {
                    case "Condition_Class":
                        Conditions.Condition_Class con_class = (Conditions.Condition_Class)condition;
                        DataRow row_class = ds_save.Tables["chk"].NewRow();
                        row_class["type"] = "class"; 
                        row_class["op"] = con_class.textBox_Oper.Text.Replace('[','<').Replace(']','>'); 
                        row_class["chk_Text"] = con_class.textBox_Value.Text; 
                        row_class["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_class);
                        break;
                    case "Condition_Compelete":
                        Conditions.Condition_Compelete con_comp = (Conditions.Condition_Compelete)condition;
                        DataRow row_comp = ds_save.Tables["chk"].NewRow();
                        row_comp["type"] = "hascomq";                         
                        row_comp["chk_Text"] = con_comp.textBox_Value.Text; 
                        row_comp["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_comp);
                        break;
                    case "Condition_Custum":
                        Conditions.Condition_Custum con_custom = (Conditions.Condition_Custum)condition;
                        DataRow row_custom = ds_save.Tables["chk"].NewRow();
                        row_custom["type"] = "custom";                         
                        row_custom["chk_Text"] = con_custom.textBox_Custum.Text; 
                        row_custom["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_custom);
                        break;
                    case "Condition_GeneralItem":
                        Conditions.Condition_GeneralItem con_item = (Conditions.Condition_GeneralItem)condition;
                        DataRow row_item = ds_save.Tables["chk"].NewRow();
                        row_item["type"] = "item";  
                        row_item["op"] = con_item.textBox_Oper.Text.Replace('[','<').Replace(']','>');  
                        row_item["num"] = con_item.textBox_Value.Text;  
                        row_item["chk_Text"] = con_item.textBox_Item.Text; 
                        row_item["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_item);
                        break;
                    case "Condition_Inven":
                        Conditions.Condition_Inven con_inven = (Conditions.Condition_Inven)condition;
                        DataRow row_inven = ds_save.Tables["chk"].NewRow();
                        row_inven["type"] = "inven_eslot";  
                        row_inven["op"] = con_inven.textBox_Oper.Text.Replace('[','<').Replace(']','>');                           
                        row_inven["chk_Text"] = con_inven.textBox_Value.Text; 
                        row_inven["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_inven);
                        break;
                    case "Condition_Level":
                        Conditions.Condition_Level con_level = (Conditions.Condition_Level)condition;
                        DataRow row_level = ds_save.Tables["chk"].NewRow();
                        row_level["type"] = "lvl";  
                        row_level["op"] = con_level.textBox_Oper.Text.Replace('[','<').Replace(']','>');                           
                        row_level["chk_Text"] = con_level.textBox_Value.Text; 
                        row_level["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_level);
                        break;
                    case "Condition_Prob":
                        Conditions.Condition_Prob con_prob = (Conditions.Condition_Prob)condition;
                        DataRow row_prob = ds_save.Tables["chk"].NewRow();
                        row_prob["type"] = "prob";                          
                        row_prob["chk_Text"] = con_prob.textBox_Value.Text; 
                        row_prob["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_prob);
                        break;
                    case "Condition_Quest":
                        Conditions.Condition_Quest con_quest = (Conditions.Condition_Quest)condition;
                        DataRow row_quest = ds_save.Tables["chk"].NewRow();
                        row_quest["type"] = "hasquest";                          
                        row_quest["chk_Text"] = con_quest.textBox_Value.Text; 
                        row_quest["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_quest);
                        break;
                    case "Condition_SymbolItem":
                        Conditions.Condition_SymbolItem con_symbol = (Conditions.Condition_SymbolItem)condition;
                        DataRow row_symbol = ds_save.Tables["chk"].NewRow();
                        row_symbol["type"] = "symbol";                         
                        row_symbol["op"] = con_symbol.textBox_Oper.Text; 
                        row_symbol["num"] = con_symbol.textBox_Value.Text;
                        row_symbol["chk_Text"] = con_symbol.textBox_Item.Text; 
                        row_symbol["conditions_Id"] = 0;
                        ds_save.Tables["chk"].Rows.Add(row_symbol);
                        break;                        
                }
            }

            #endregion

            #region 실행문 정보

            Control.ControlCollection executions = qc.Get_Executions();

            foreach (Control execution in executions)
            {
                switch (execution.Name)
                {
                    case "Exe_BoOkAdd":
                        {
                            Executions.Exe_BoOkAdd eb = (Executions.Exe_BoOkAdd)execution;
                            DataRow row_exe = ds_save.Tables["exe"].NewRow();
                            row_exe["type"] = "openjewel";
                            row_exe["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_exe);
                            break;
                        }
                    case "Exe_Condition":
                        {
                            Executions.Exe_Condition ec = (Executions.Exe_Condition)execution;
                            DataRow row_condition = ds_save.Tables["exe"].NewRow();
                            row_condition["type"] = "conexe";
                            row_condition["true"] = ec.textBox_true.Text;
                            row_condition["false"] = ec.textBox_false.Text;
                            row_condition["exe_Text"] = ec.textBox_Condition.Text;
                            row_condition["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_condition);
                            break;
                        }
                    case "Exe_Custum":
                        {
                            Executions.Exe_Custum ecu = (Executions.Exe_Custum)execution;
                            DataRow row_custom = ds_save.Tables["exe"].NewRow();
                            row_custom["type"] = "custom";
                            row_custom["exe_Text"] = ecu.textBox_Code.Text;
                            row_custom["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_custom);
                            break;
                        }
                    case "Exe_Message":
                        {
                            Executions.Exe_Message em = (Executions.Exe_Message)execution;
                            DataRow row_message = ds_save.Tables["exe"].NewRow();
                            row_message["type"] = "msg";
                            row_message["mag_type"] = em.textBox_Type.Text;
                            row_message["substring"] = em.textBox_Substring.Text;
                            row_message["exe_Text"] = em.textBox_Index.Text;
                            row_message["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_message);
                            break;
                        }
                    case "Exe_Movie":
                        {
                            Executions.Exe_Movie emov = (Executions.Exe_Movie)execution;
                            DataRow row_movie = ds_save.Tables["exe"].NewRow();
                            row_movie["type"] = "movie";
                            row_movie["exe_Text"] = emov.textBox_Movie.Text;
                            row_movie["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_movie);
                            break;
                        }
                    case "Exe_NoManStore":
                        {
                            Executions.Exe_NoManStore enoman = (Executions.Exe_NoManStore)execution;
                            DataRow row_noman = ds_save.Tables["exe"].NewRow();
                            row_noman["type"] = "openbazar";
                            row_noman["exe_Text"] = ".";
                            row_noman["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_noman);
                            break;
                        }
                    case "Exe_Quest":
                        {
                            Executions.Exe_Quest equest = (Executions.Exe_Quest)execution;
                            DataRow row_quest = ds_save.Tables["exe"].NewRow();
                            row_quest["type"] = "enterquest";
                            row_quest["exe_Text"] = equest.textBox_Quest.Text;
                            row_quest["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_quest);
                            break;
                        }
                    case "Exe_Script":
                        {
                            Executions.Exe_Script escript = (Executions.Exe_Script)execution;
                            DataRow row_script = ds_save.Tables["exe"].NewRow();
                            row_script["type"] = "talk";
                            row_script["exe_Text"] = escript.textBox_Script.Text;
                            row_script["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_script);
                            break;
                        }
                    case "Exe_SkillStore":
                        {
                            Executions.Exe_SkillStore eskill = (Executions.Exe_SkillStore)execution;
                            DataRow row_skill = ds_save.Tables["exe"].NewRow();
                            row_skill["type"] = "openskillshop";
                            row_skill["exe_Text"] = eskill.textBox_Skill.Text;
                            row_skill["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_skill);
                            break;
                        }
                    case "Exe_Store":
                        {
                            Executions.Exe_Store eshop = (Executions.Exe_Store)execution;
                            DataRow row_shop = ds_save.Tables["exe"].NewRow();
                            row_shop["type"] = "openshop";
                            row_shop["exe_Text"] = eshop.textBox_Store.Text;
                            row_shop["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_shop);
                            break;
                        }
                    case "Exe_StringCombi":
                        {
                            Executions.Exe_StringCombi estring = (Executions.Exe_StringCombi)execution;
                            DataRow row_string = ds_save.Tables["exe"].NewRow();
                            row_string["type"] = "openplate";
                            row_string["exe_Text"] = ".";
                            row_string["executions_Id"] = 0;
                            ds_save.Tables["exe"].Rows.Add(row_string);
                            break;
                        }
                    case "Exe_CutSceneCompleteChangeMap":
                        {
                            Executions.Exe_CutsceneCompleteChangeMap CCCM = (Executions.Exe_CutsceneCompleteChangeMap)execution;
                            DataRow row_string = ds_save.Tables["exe"].NewRow();
                            row_string["type"] = "custsceneCompleteChangeMap";
                            row_string["cutSceneNo"] = CCCM.txtCutSceneNo;
                            row_string["stageNo"] = CCCM.txtStageNo;
                            row_string["gateNo"] = CCCM.txtGateNo;
                            ds_save.Tables["exe"].Rows.Add(row_string);
                            break;
                        }
                }
            }


            #endregion

            #region 대사 정보

            foreach (Control talk in panel_ScriptMain.Controls)
            {
                Quest_Step qs = (Quest_Step)talk;

                DataRow dr_talk = ds_save.Tables["talk_paragraph"].NewRow();

                //dr_talk["talk_question"] = qs.Get_Script();
                dr_talk["talk_paragraph_Id"] = qs.Tag.ToString();
                dr_talk["index"] = qs.textBox_StepNo.Text;
                dr_talk["image"] = qs.textBox_Image.Text;
                dr_talk["itemindex"] = qs.Get_ItemIndex();
                dr_talk["talk_Id"] = 0;

                ds_save.Tables["talk_paragraph"].Rows.Add(dr_talk);

                //번역 관련추가
                DataRow row_question = ds_save.Tables["talk_question"].NewRow();

                if (qs.textBox_Script.Tag == null)
                {
                    ++Quest_Mid;
                    row_question["mid"] = Quest_Mid;
                    qs.textBox_Script.Tag = Quest_Mid;                    
                }
                else
                {
                    row_question["mid"] = qs.textBox_Script.Tag.ToString();
                    if (Quest_Mid < Convert.ToInt32(qs.textBox_Script.Tag.ToString()))
                    {
                        Quest_Mid = Convert.ToInt32(qs.textBox_Script.Tag.ToString()) + 1;
                    }
                }

                row_question["talk_question_Text"] = qs.Get_Script();
                row_question["talk_paragraph_Id"] = qs.Tag.ToString();

                ds_save.Tables["talk_question"].Rows.Add(row_question);
                //번역 관련추가 끝

                Control[] sel_List = qs.Get_Select();

                if (sel_List.Length > 0)
                {
                    foreach (Control select in sel_List)
                    {
                        if (select != null)
                        {
                            if (select.GetType().ToString() == "DNNpc.Quest_Link")
                            {
                                Quest_Link npc_link = (Quest_Link)select;

                                DataRow dr_select = ds_save.Tables["talk_answer"].NewRow();

                                dr_select["link_index"] = "q_enter";
                                dr_select["link_target"] = npc_link.Get_Xml();

                                if (npc_link.textBox_Select.Tag == null)
                                {
                                    ++Quest_Mid;
                                    dr_select["mid"] = Quest_Mid;                                    
                                }
                                else
                                {
                                    int mid = Convert.ToInt32(npc_link.textBox_Select.Tag.ToString());
                                    dr_select["mid"] = mid;

                                    if (Quest_Mid < mid)
                                        Quest_Mid = mid + 1;
                                }

                                dr_select["talk_answer_Text"] = npc_link.Get_Link();
                                dr_select["talk_paragraph_Id"] = qs.Tag.ToString();

                                ds_save.Tables["talk_answer"].Rows.Add(dr_select);
                            }
                            else
                            {
                                Quest_SelectControl npc_select = (Quest_SelectControl)select;

                                DataRow dr_select = ds_save.Tables["talk_answer"].NewRow();

                                dr_select["link_index"] = npc_select.Get_Target();

                                if (npc_select.textBox_Select.Tag == null)
                                {
                                    dr_select["mid"] = ++Quest_Mid;
                                }
                                else
                                {
                                    int mid = Convert.ToInt32(npc_select.textBox_Select.Tag.ToString());
                                    dr_select["mid"] = mid;

                                    if ( Quest_Mid < mid )
                                        Quest_Mid = mid + 1;
                                }

                                dr_select["talk_answer_Text"] = npc_select.Get_Select();
                                dr_select["talk_paragraph_Id"] = npc_select.Tag.ToString();

                                ds_save.Tables["talk_answer"].Rows.Add(dr_select);

                                if (npc_select.Get_ExeNo() != "")
                                {
                                    DataRow dr_link = ds_save.Tables["exelink"].NewRow();

                                    dr_link["idx"] = npc_select.Get_Target();
                                    dr_link["exelink_Text"] = npc_select.Get_ExeNo();
                                    dr_link["talk_Id"] = 0;

                                    ds_save.Tables["exelink"].Rows.Add(dr_link);
                                }
                            }
                        }
                    }                    
                }
            }

            #endregion

            #region MID 중복 오류 체크
            Dictionary<string, int> dic_midcheck = new Dictionary<string, int>();
            Dictionary<string, string> dic_miderror = new Dictionary<string, string>();
            foreach (DataTable table in ds_save.Tables)
            {
                foreach (DataRow row in table.Rows)
                {
                    if (false == row.Table.Columns.Contains("mid"))
                    {
                        continue;
                    }

                    string mid = row["mid"].ToString();
                    if (false == dic_midcheck.ContainsKey(mid))
                    {
                        dic_midcheck.Add(mid, 0);
                    }
                    int midcount = ++dic_midcheck[mid];

                    if (1 >= dic_midcheck[mid])
                    {
                        continue;
                    }

                    string errmsg = "mid : ";
                    errmsg += mid;
                    errmsg += " count : ";
                    errmsg += midcount;
                    errmsg += "\r\n";

                    if (false == dic_miderror.ContainsKey(mid))
                    {
                        dic_miderror.Add(mid, errmsg);
                    }
                    else
                    {
                        dic_miderror[mid] = errmsg;
                    }
                }
            }
            if (0 < dic_miderror.Count)
            {
                string errmsg = "";
                foreach (string error in dic_miderror.Values)
                {
                    errmsg += error;
                }

                MessageBox.Show(errmsg, "MID 중복 오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            dic_midcheck.Clear();
            dic_miderror.Clear();
            #endregion

            using (XmlTextWriter xtw = new XmlTextWriter(p_path, Encoding.UTF8))
            {
                xtw.Formatting = Formatting.Indented;
                xtw.Indentation = 3;

                ds_save.WriteXml(xtw);
            }

            string temp = "";
            string ret_str= "";

            FileInfo fi = new FileInfo(p_path);

            using (StreamReader sr = fi.OpenText())
            {

                while ((temp = sr.ReadLine()) != null)
                {
                    if (temp.IndexOf("NewDataSet") < 0)
                    {
                        ret_str += temp + "\r\n";
                    }
                }
            }

            ret_str = ret_str.Replace("&lt;", "<").Replace("&gt;", ">");

            // 검정색 변경
            // TODO 글자색 변경 히스토리 - 모든 파일이 변경되면 replace 코스 삭제.
            ret_str = ret_str.Replace("#000000", "#2b2723");
            ret_str = ret_str.Replace("#ffffff", "#2b2723");

            ret_str = ret_str.Replace("#ff9345", "#db4f4f");
            ret_str = ret_str.Replace("#fff332", "#d5700d");
            ret_str = ret_str.Replace("#00ff00", "#62ad01");
            ret_str = ret_str.Replace("#8d38c9", "#a32dfe");
            ret_str = ret_str.Replace("#75aef5", "#18a5a7");

            ret_str = ret_str.Replace("#db4f4f", "#a82424");
            ret_str = ret_str.Replace("#d5700d", "#a94407");
            ret_str = ret_str.Replace("#62ad01", "#387206");
            ret_str = ret_str.Replace("#a32dfe", "#7e12d2");
            ret_str = ret_str.Replace("#18a5a7", "#086171");

            ret_str = ret_str.Replace("green", "#387206");
            ret_str = ret_str.Replace("violet", "#7e12d2");

            using (StreamWriter sw = new StreamWriter(p_path))
            {
                sw.Write(ret_str);
            }   
        }

        #endregion

        #region Lua 파일 저장
        private void SaveLua(string p_path)
        {
            try
            {
                string npcename = ds_save.Tables["talk"].Rows[0]["npcename"].ToString();
                string startexe = ds_save.Tables["talk"].Rows[0]["startexe"].ToString();
                string strattalk = "";

                string data = "";

                data += "function " + npcename + "_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target)\r\n";
                data += "\r\n\tif npc_talk_index == \"\" then\r\n\t\tapi_log_AddLog(\"npc_talk_index is null ..\\n\");";
                data += "\r\n\telseif npc_talk_index == \"start\" then";

                int temp;

                if (Int32.TryParse(startexe, out temp))
                {
                    WriteExeBlock(ds_save.Tables["exe"].Rows[temp], ref data);
                    strattalk = ds_save.Tables["exe"].Rows[temp]["exe_Text"].ToString();
                }

                for (int i = 0; i < ds_save.Tables["exelink"].Rows.Count; ++i)
                {
                    DataRow dr_exelink = ds_save.Tables["exelink"].Rows[i];

                    data += "\r\n\telseif npc_talk_index == \"" + dr_exelink["idx"].ToString() + "\" then";

                    string[] array_exelink = dr_exelink["exelink_Text"].ToString().Split(','); // 선택지 실행구문은 콤마로 구분

                    // 콤마로 구분된 실행구문의 수만큼 스크립트 생성
                    foreach (string exelink in array_exelink)
                    {
                        int row_index = 0;

                        if (Int32.TryParse(exelink, out row_index))
                        {
                            WriteExeBlock(ds_save.Tables["exe"].Rows[row_index], ref data);
                        }
                    }
                }

                data += "\r\n\telse\r\n\t\tapi_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);\r\n\tend\r\n\r\nend";

                using (FileStream fs = new FileStream(p_path, FileMode.Create))
                {
                    byte[] data_byte = Encoding.UTF8.GetBytes(replaceScript(ref data));
                    fs.Write(data_byte, 0, data_byte.Length);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show("Lua Script 생성 중, 오류가 발생했습니다.");
            }

            // Lua 컴파일
            //CompileLua(p_path);
        }

        /// <summary>
        /// 
        /// DNQuest에도 같은 함수가 있으므로 수정할 시에는 꼭 같이 수정한다. Refactoring 대상.
        /// 
        /// 
        /// Script 저장 시, server tag를 추가하고, 환경에 맞게 replace한다.
        /// villageServer 일 경우, village tag만 추가하고,
        /// Game Server 일 경우, game tag와 각 function, api 마다 pRoom parameter를 추가한다.
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        private string replaceScript(ref string str)
        {
            //VillageServerTag 추가
            StringBuilder sbVillageServer = new StringBuilder();
            sbVillageServer.AppendLine(VillageServerTag_Start);
            sbVillageServer.AppendLine("");
            sbVillageServer.Append(str);
            sbVillageServer.AppendLine("");
            sbVillageServer.AppendLine(VillageServerTag_End);

            //function, api 마다 "pRoom," 추가
            //function pattern (function)\s\w+\s?\(
            //api_ pattern (api_)\w+\s?\(
            //함수 명에 onTalk 포함 .*_OnTalk_.*\s?\
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

        private void WriteExeBlock(DataRow p_row, ref string p_data)
        {
            switch (p_row["type"].ToString())
            {
                case "enterquest":
                    p_data += "\r\n\t\tapi_npc_NextScript(userObjID, npcObjID, \"q_enter\",\"" + p_row["exe_Text"].ToString() + ".xml\");";
                    break;
                case "talk":
                    p_data += "\r\n\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + p_row["exe_Text"].ToString() + "\", npc_talk_target);";
                    break;
                case "msg":
                    p_data += "\r\n\t\tlocal TP_" + "0" + " = {};\r\n\t\tapi_user_UserMessage(userObjID," + p_row["mag_type"].ToString() + ", " + p_row["exe_Text"].ToString() + ", " + p_row["substring"].ToString() + " );";
                    break;
                case "movie":
                    p_data += "\r\n\t\tapi_user_PlayCutScene(userObjID, " + p_row["exe_Text"].ToString() + ");";
                    break;
                case "conexe":
                    p_data += "\r\n\t\tif " + WriteChkBlock(p_row["exe_Text"].ToString())[0] + "then";

                    if (p_row["true"].ToString() != "")
                    {
                        p_data += "\t\t\t\t" + WriteExeBlock_Condition(p_row["true"].ToString())[0] + "\r\n";
                    }

                    p_data += "\t\telse";

                    if (p_row["false"].ToString() != "")
                    {
                        p_data += "\t\t\t\t" + WriteExeBlock_Condition(p_row["false"].ToString())[0] + "\r\n";
                    }

                    p_data += "\t\tend\r\n";
                    break;
                case "openshop":
                    p_data += "\r\n\t\tapi_ui_OpenShop(userObjID," + p_row["exe_Text"].ToString() + ");";
                    break;
                case "openskillshop":
                    p_data += "\r\n\t\tapi_ui_OpenSkillShop(userObjID," + p_row["exe_Text"].ToString() + ");";
                    break;
                case "openbazar":                    
                    break;
                case "openplate":                    
                    break;
                case "openjewel":
                    break;
                case "custom":
                    p_data += "\r\n\t\t" + p_row["exe_Text"].ToString();
                    break;
                
            }
        }

        private string[] WriteExeBlock_Condition(string p_value)
        {
            string[] ret_string = new string[2];
            int index = 0;

            string[] arr_index = p_value.Split(',');

            foreach (string _index in arr_index)
            {

                if (Int32.TryParse(_index, out index))
                {
                    DataRow write_row = ds_save.Tables["exe"].Rows[index];

                    switch (write_row["type"].ToString())
                    {
                        case "enterquest":
                            ret_string[0] += "\r\n\t\tapi_npc_NextScript(userObjID, npcObjID, \"q_enter\",\"" + write_row["exe_Text"].ToString() + ".xml\");";
                            break;
                        case "talk":
                            ret_string[0] += "\r\n\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + write_row["exe_Text"].ToString() + "\", npc_talk_target);";
                            break;
                        case "msg":
                            ret_string[0] += "\r\n\t\tlocal TP_" + "0" + " = {};\r\n\t\tapi_user_UserMessage(userObjID," + write_row["mag_type"].ToString() + ", " + write_row["exe_Text"].ToString() + ", " + write_row["substring"].ToString() + " );";
                            break;
                        case "movie":
                            ret_string[0] += "\r\n\t\tapi_user_PlayCutScene(userObjID, " + write_row["exe_Text"].ToString() + ");";
                            break;
                        case "conexe":
                            ret_string[0] += "\r\n\t\tif " + WriteChkBlock(write_row["exe_Text"].ToString())[0] + "then";

                            if (write_row["true"].ToString() != "")
                            {
                                ret_string[0] += "\t\t\t\t" + WriteExeBlock_Condition(write_row["true"].ToString())[0] + "\r\n";
                            }

                            ret_string[0] += "\t\telse";

                            if (write_row["false"].ToString() != "")
                            {
                                ret_string[0] += "\t\t\t\t" + WriteExeBlock_Condition(write_row["false"].ToString())[0] + "\r\n";
                            }

                            ret_string[0] += "\t\tend\r\n";
                            break;
                        case "openshop":
                            ret_string[0] += "\r\n\t\tapi_ui_OpenShop(userObjID," + write_row["exe_Text"].ToString() + ");";
                            break;
                        case "openskillshop":
                            ret_string[0] += "\r\n\t\tapi_ui_OpenSkillShop(userObjID," + write_row["exe_Text"].ToString() + ");";
                            break;
                        case "openbazar":
                            break;
                        case "openplate":
                            break;
                        case "openjewel":
                            break;
                        case "custom":
                            ret_string[0] += "\r\n\t\t" + write_row["exe_Text"].ToString();
                            break;
                    }
                }
            }

            return ret_string;
        }

        private string[] WriteChkBlock(string p_condition)
        {
            string[] ret_string = new string[2];

            string[] condition = p_condition.Split(' ');

            foreach (string str in condition)
            {
                int temp = 0;

                if (!int.TryParse(str, out temp))
                {
                    ret_string[0] = " " + str.ToLower() + " ";
                }
                else
                {
                    switch (ds_save.Tables["chk"].Rows[temp]["type"].ToString())
                    {
                        case "lvl":
                            ret_string[0] = "api_user_GetUserLevel(userObjID) " + ds_save.Tables["chk"].Rows[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "item":
                            if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[")
                            {
                                ret_string[0] = "api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == -3 ";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[=")
                            {
                                ret_string[0] = "( api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == -3 or api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ")== " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " )";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[]")
                            {
                                ret_string[0] = "api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") ~= " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "=")
                            {
                                ret_string[0] = "api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            else
                            {
                                ret_string[0] = "api_user_HasItem(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") " + ds_save.Tables["chk"].Rows[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            break;
                        case "symbol":
                            if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[")
                            {
                                ret_string[0] = "api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == -3 ";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[=")
                            {
                                ret_string[0] = "( api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == -3 or api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ")== " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " )";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[]")
                            {
                                ret_string[0] = "api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") ~= " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "=")
                            {
                                ret_string[0] = "api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") == " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            else
                            {
                                ret_string[0] = "api_quest_HasSymbol(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ", " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + ") " + ds_save.Tables["chk"].Rows[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + ds_save.Tables["chk"].Rows[temp]["num"].ToString() + " ";
                            }
                            break;
                        case "class":
                            if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "=")
                            {
                                ret_string[0] = "api_user_GetUserClassID(userObjID) == " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            }
                            else if (ds_save.Tables["chk"].Rows[temp]["op"].ToString() == "[]")
                            {
                                ret_string[0] = "api_user_GetUserClassID(userObjID) ~= " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            }
                            else
                            {
                            }
                            break;
                        case "prob":
                            ret_string[0] = "math.random(1,1000) <= " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "hasquest":
                            ret_string[0] = "api_quest_UserHasQuest(userObjID," + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ") > -1 ";
                            break;
                        case "hascomq":
                            ret_string[0] = "api_quest_IsMarkingCompleteQuest(userObjID, " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + ") == 1 ";
                            break;
                        case "inven_eslot":
                            ret_string[0] = "api_user_GetUserInvenBlankCount(userObjID) " + ds_save.Tables["chk"].Rows[temp]["op"].ToString().Replace('[', '<').Replace(']', '>') + " " + ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            break;
                        case "custom":
                            ret_string[0] = ds_save.Tables["chk"].Rows[temp]["chk_Text"].ToString() + " ";
                            break;
                    }
                }
            }

            return ret_string;
        }

        #endregion        

        #region Lua 파일 컴파일

        // 긴경로를 짧은 이름으로 바꾸는 코드 시작
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]

        static extern uint GetShortPathName([MarshalAs(UnmanagedType.LPTStr)]string lpszLongPath,
                                            [MarshalAs(UnmanagedType.LPTStr)]StringBuilder lpszShortPath,
                                            uint cchBuffer);

        /// <summary>
        /// The ToLongPathNameToShortPathName function retrieves the short path form of a specified long input path
        /// </summary>
        /// <param name="longName">The long name path</param>
        /// <returns>A short name path string</returns>
        private static string ToShortPathName(string longName)
        {
            uint bufferSize = 256;
            StringBuilder shortNameBuffer = new StringBuilder((int)bufferSize);

            uint result = GetShortPathName(longName, shortNameBuffer, bufferSize);

            return shortNameBuffer.ToString();
        }

        //긴경로를 짧은 이름으로 바꾸는 코드 끝

        private void CompileLua(string p_Path)
        {
            FileInfo compile_fi = new FileInfo(@"C:\lua\lua5.1.exe");

            if (!compile_fi.Exists)
            {
                MessageBox.Show("Lua 컴파일러를 찾을수 없어 컴파일 없이 저장됩니다.", "저장", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            Process compiler = new Process();

            compiler.StartInfo.FileName = @"C:\lua\lua5.1.exe";
            compiler.StartInfo.Arguments = ToShortPathName(p_Path);
            compiler.StartInfo.RedirectStandardOutput = true;
            compiler.StartInfo.RedirectStandardError = true;
            compiler.StartInfo.UseShellExecute = false;
            compiler.Start();
            compiler.WaitForExit();

            if (compiler.ExitCode.ToString() == "0")
            {
                //MessageBox.Show("성공적으로 저장 되었습니다.", "저장", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("다음과 같은 Lua 컴파일 오류가 발생하였습니다.\n\n" + compiler.StandardError.ReadToEnd(), "저장", MessageBoxButtons.OK, MessageBoxIcon.Error);

            }
        }
        #endregion

        public void Hide_Menu()
        {
            this.menuStrip1.Visible = false;            
        }

        //꺽쇠 체크
        private void Check_GT(TextBox p_txt, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    p_txt.Text = p_txt.Text.Substring(0, p_txt.Text.Length - 1);
                    p_txt.SelectionStart = p_txt.Text.Length;
                }
            }
        }

        private void textBox_nkname_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }        
    }
}
