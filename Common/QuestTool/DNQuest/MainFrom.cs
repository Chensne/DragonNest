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
using Excel = Microsoft.Office.Interop.Excel;
using System.Text.RegularExpressions;

namespace DNQuest
{
    public partial class frm_Main : Form
    {
        int Journal_Count = 0; //저널 카운트 변수
        int General_Step_Count = 1; // 일반 단계 카운트 변수
        int Hunt_Count = 0; // 사냥횟수 카운트 변수
        string Quest_Path = ""; // 퀘스트 폴더 경로 변수
        public int Quest_Mid = 1; // 퀘스트 번역용 MID
        int m_nQuestRemoteCompleteStep = 0;
        string m_strRemoteStart = "";
        private int m_StyleIndex = 1;

        /// <summary>
        /// 저장 Path
        /// </summary>
        public string sSavePath = @"R:\GameRes\Resource\Script\Talk_Quest";

        private readonly string VillageServerTag_Start = "<VillageServer>";
        private readonly string VillageServerTag_End = "</VillageServer>";
        
        private readonly string GameServerTag_Start = "<GameServer>";
        private readonly string GameServerTag_End = "</GameServer>";

        DataSet loadedXmlDataSet = new DataSet(); // XML 파일 로드 데이터 셋
        DataSet savingDataSet; // 저장 전용 데이터 셋

        //전체 저장 시, "저장하시겠습니까?" 묻기를 띄울지 확인 true = 띄움, false = 안띄움.
        public static bool QuestAllSaveCheck = true;

        // 퀘스트 폼 생성자 (새로 생성시)
        public frm_Main()
        {
            InitializeComponent();
        }
        
        // 퀘스트 폼 생성자 (파일 로드시)
        public frm_Main(string savePath, string fileName )
        {
            InitializeComponent();
            sSavePath = savePath;

            if (fileName.Contains(@"\"))    Quest_Path = fileName;
            else                            Quest_Path = savePath + @"\" + fileName;
        }

        public void SetQuestRemoteCompleteStep(int nStep)
        {
            m_nQuestRemoteCompleteStep = nStep;

            if (0 >= nStep)
            {
                foreach (Control control in tabPage_General.Controls["panel_TabMainGeneral"].Controls)
                {
                    if (false == control is Quest_Unit)
                    {
                        continue;
                    }

                    Quest_Unit qunit = (Quest_Unit)control;
                    qunit.Enable_RemoteComplete();
                }
                return;
            }

            foreach (Control control in tabPage_General.Controls["panel_TabMainGeneral"].Controls)
            {
                if (false == control is Quest_Unit)
                {
                    continue;
                }

                int nTag = Convert.ToInt32( control.Tag );
                if (0 == nTag)
                {
                    nTag = 1;
                }

                if (nTag == nStep)
                {
                    continue;
                }

                Quest_Unit qunit = (Quest_Unit)control;
                qunit.Disable_RemoteComplete();
            }
        }

        public int GetQuestRemoteCompleteStep()
        {
            return m_nQuestRemoteCompleteStep;
        }

        // 창 닫기 메뉴 이벤트 
        public void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            QuestAllSaveCheck = true;
            this.Close();
        }

        // NPC리스트 리턴 함수
        public ListBox Get_NpcList()
        {
            return this.listBox_NPC;
        }

        // 폼 로딩 이벤트 
        private void frm_Main_Load(object sender, EventArgs e)
        {   
            //기본정보
            init_Basic();

            init_NoQuest(); // 퀘스트 없을 단계
            init_Journal(); //퀘스트 저널
            init_Accept();  //퀘스트 받기 단계
            init_General(); // 퀘스트 일반 단계
            init_Complete();    // 퀘스트 완료 단계

            #region 바로 불러오기(퀘스트 리스트 더블 클릭 혹은 불러오기 실행시)
        
            if (Quest_Path != "") // 퀘스트 경로 필수!
            {
                FileInfo loadFile = new FileInfo(Quest_Path);

                this.Text = "Quest-" + loadFile.Name;

                if (loadFile.IsReadOnly) 
                {
                    loadedXmlDataSet = new DataSet();
                    savingDataSet = null;

                    init_Basic();
                    init_NoQuest();
                    init_Complete();
                    init_Journal();
                    init_Accept();
                    init_General();

                    LoadXmlFile(Quest_Path);
                    Set_Basic_Info();
                    Set_Journal_Info();
                    Set_NoQuest_Info();
                    Set_Accepting_Info();
                    Set_General_Info();
                    Set_Complete_Info();
                }
                else
                {
                    loadedXmlDataSet = new DataSet();
                    savingDataSet = null;

                    init_Basic();
                    init_NoQuest();
                    init_Complete();
                    init_Journal();
                    init_Accept();
                    init_General();

                    LoadXmlFile(Quest_Path);
                    Set_Basic_Info();
                    Set_Journal_Info();
                    Set_NoQuest_Info();
                    try
                    {

                        Set_Accepting_Info();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.StackTrace);
                    }
                    Set_General_Info();
                    Set_Complete_Info();
                }

                m_StyleIndex = comboBox_UIStyle.SelectedIndex;
                _UpdateUIStyle(comboBox_UIStyle.SelectedIndex);
            }
            #endregion            
        }

        //퀘스트 난이도 숫자 검증
        private void textBox_QuestDifficulty_Validating(object sender, CancelEventArgs e)
        {
            int difficulty = 1;

            if (int.TryParse(textBox_QuestDifficulty.Text, out difficulty))
            {
                if (difficulty < 1 || difficulty > 3)
                {
                    MessageBox.Show("퀘스트 난이도는 1~3사이의 숫자만 입력하십시오.", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    textBox_QuestDifficulty.Text = "1";
                    textBox_QuestDifficulty.Focus();
                }
            }
            else
            {
                MessageBox.Show("퀘스트 난이도는 1~3사이의 숫자만 입력하십시오.", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                textBox_QuestDifficulty.Text = "1";
                textBox_QuestDifficulty.Focus();
            }
        }             

        #region 초기화

        private void init_Basic()
        {
            textBox_Chapter.Text = "-1";
            textBox_Abstrct.Text = "";
            textBox_Damdang.Text = "";
            textBox_EngName.Text = "";
            textBox_KorName.Text = "";
            textBox_QuestID.Text = "";
            textBox_StartLevel.Text = "";

            comboBox_CompleteType.SelectedIndex = 0;
            comboBox_Type.SelectedIndex = 0;
            comboBox_UIStyle.SelectedIndex = 0;

            listBox_NPC.Items.Clear();
        }

        private void init_NoQuest()
        {
            _ClearControlsAndDispose(tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls);

            Quest_Unit qu = new Quest_Unit();
            Quest_NPC qn = new Quest_NPC(qu);
            Quest_Step qs = new Quest_Step(qn);

            qs.Location = new Point(193, 2);
            qn.Location = new Point(140, 0);

            qs.Name = "step_init";
            qs.textBox_StepNo.Text = "단계시작";
            qs.Tag = 0;
            qn.Name = "npc_init";
            qn.Tag = 0;
            qu.Name = "unit_init";
            qu.Tag = 0;

            qn.Controls.Add(qs);
            qu.Controls.Add(qn);

            qu.Hide_RemoteComplete();

            tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls.Add(qu);
        }

        private void init_Complete()
        {            
            _ClearControlsAndDispose(tabPage_Complete.Controls["panel_TabMainComplete"].Controls);

            Quest_Unit qu = new Quest_Unit();
            Quest_NPC qn = new Quest_NPC(qu);
            Quest_Step qs = new Quest_Step(qn);

            qs.Location = new Point(193, 2);
            qn.Location = new Point(140, 0);

            qs.Name = "step_init";
            qs.textBox_StepNo.Text = "단계시작";
            qs.Tag = 0;
            qn.Name = "npc_init";
            qn.Tag = 0;
            qu.Name = "unit_init";
            qu.Tag = 0;

            qn.Controls.Add(qs);
            qu.Controls.Add(qn);

            qu.Hide_RemoteComplete();

            tabPage_Complete.Controls["panel_TabMainComplete"].Controls.Add(qu);
        }

        private void init_Journal()
        {
            _ClearControlsAndDispose(tabPage_Journal.Controls["panel_TabMainJournal"].Controls);

            Journal_Count = 0;
            Quest_Journal qj = new Quest_Journal();
            qj.Tag = Journal_Count;
            ++Journal_Count;

            tabPage_Journal.Controls["panel_TabMainJournal"].Controls.Add(qj);
        }

        private void init_Accept()
        {
            _ClearControlsAndDispose(tabPage_Accept.Controls["panel_TabMainAccept"].Controls);

            Quest_Unit qu = new Quest_Unit();
            Quest_NPC qn = new Quest_NPC(qu);
            Quest_Step qs = new Quest_Step(qn);
            Quest_Condition qc = new Quest_Condition();

            qs.Location = new Point(193, 2);
            qn.Location = new Point(140, 0);
            qc.Location = new Point(0, qu.Size.Height);

            qs.Name = "step_init";
            qs.textBox_StepNo.Text = "단계시작";
            qs.Tag = 0;
            qn.Name = "npc_init";
            qn.Tag = 0;
            qu.Name = "unit_init";
            qu.Tag = 0;
            qc.Name = "condition_accept";

           
            qn.Controls.Add(qs);
            qu.Controls.Add(qn);

            qu.groupBox_Unit.Text = "퀘스트단계(Accepting)";
            qu.Hide_RemoteComplete();

            tabPage_Accept.Controls["panel_TabMainAccept"].Controls.Add(qu);
            tabPage_Accept.Controls["panel_TabMainAccept"].Controls.Add(qc);
           
        }

        private void init_General()
        {
            General_Step_Count = 1;

            _ClearControlsAndDispose(tabPage_General.Controls["panel_TabMainGeneral"].Controls);

            // 초기 퀘스트 단계 설정 시작
            Quest_Unit qu = new Quest_Unit();
            Quest_NPC qn = new Quest_NPC(qu);
            Quest_Step qs = new Quest_Step(qn);
            Quest_Condition qc = new Quest_Condition();

            qs.Location = new Point(193, 2);
            qn.Location = new Point(140, 0);
            qc.Location = new Point(0, qu.Size.Height);  

            qs.Name = "step_init";
            qs.textBox_StepNo.Text = "단계시작";
            qs.Tag = 0;
            qn.Name = "npc_init";
            qn.Tag = 0;
            qu.Name = "unit_init";
            qu.Tag = 0;
            qc.Name = "condition_init";
            qc.Tag = 0;

            qu.groupBox_Unit.Text = "퀘스트 " + General_Step_Count.ToString() + "단계";            

            qn.Controls.Add(qs);
            qu.Controls.Add(qn);
            
            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qu);
            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qc);

            ++General_Step_Count;
            // 초기 퀘스트 단계 설정 끝
        }

        #endregion

        #region 버튼 클릭 이벤트 
        // 퀘스트 없는 단계 리셋 버튼클릭 이벤트
        private void button_NoQuestReset_Click(object sender, EventArgs e)
        {
            init_NoQuest();
        }

        // 
        private void button_Journal_Click(object sender, EventArgs e)
        {
            init_Journal();
        }

        // 저널 추가 버튼 클릭이벤트
        private void button_JournalAdd_Click(object sender, EventArgs e)
        {
            Quest_Journal qj = new Quest_Journal();
            qj.Tag = Journal_Count;

            qj.Location = new Point(0, getLastControlPosition((Panel)tabPage_Journal.Controls["panel_TabMainJournal"]) + 3);

            ++Journal_Count;

            tabPage_Journal.Controls["panel_TabMainJournal"].Controls.Add(qj);            
        }

        // 저널 삭제 버튼 클릭이벤트
        private void button_JournalDel_Click(object sender, EventArgs e)
        {
            if (Journal_Count > 1)
            {
                Control root = tabPage_Journal.Controls["panel_TabMainJournal"];
                Control del_journal = root.Controls[root.Controls.Count - 1];
                
                if (del_journal != null)
                {
                    del_journal.Dispose();
                    --Journal_Count;
                }
            }
        }  

        // 퀘스트 완료 단계 리셋 버튼 클릭이벤트
        private void button_CompleteReset_Click(object sender, EventArgs e)
        {
            init_Complete();
        }      

        // 퀘스트 받기 단계 리셋 버튼 클릭이벤트
        private void button_AcceptReset_Click(object sender, EventArgs e)
        {
            init_Accept();
        }

        // 퀘스트 일반 단계 리셋 버튼 클릭이벤트
        private void button_GeneralReset_Click(object sender, EventArgs e)
        {
            init_General();
        }

        // 퀘스트 일반 단계 추가 버튼 클릭 이벤트
        private void button_GeneralAdd_Click(object sender, EventArgs e)
        {
            Quest_Unit qu = new Quest_Unit();
            Quest_NPC qn = new Quest_NPC(qu);
            Quest_Step qs = new Quest_Step(qn);
            Quest_Condition qc = new Quest_Condition();

            qs.Location = new Point(193, 2);
            qn.Location = new Point(140, 0);
            qu.Location = new Point(0, getLastControlPosition((Panel)tabPage_General.Controls["panel_TabMainGeneral"]) + 3);
            qc.Location = new Point(0, getLastControlPosition((Panel)tabPage_General.Controls["panel_TabMainGeneral"]) + qu.Size.Height + 3);

            qs.Name = "step_init";
            qs.Tag = 0;
            qs.textBox_StepNo.Text = "단계시작";
            qn.Name = "npc_init";
            qn.Tag = 0;            
            qu.Name = "unit_" + General_Step_Count.ToString();
            qu.Tag = General_Step_Count;
            qc.Name = "condition_" + General_Step_Count.ToString();
            qc.Tag = General_Step_Count;

            qu.groupBox_Unit.Text = "퀘스트 " + General_Step_Count.ToString() + "단계";

            qn.Controls.Add(qs);
            qu.Controls.Add(qn);

            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qu);
            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qc);

            if (0 < m_nQuestRemoteCompleteStep)
            {
                qu.Disable_RemoteComplete();
            }

            qs.textBox_Script.Focus();

            ++General_Step_Count;
        }

        // 퀘스트 일반 단계 삭제 버튼 클릭 이벤트
        private void button_GeneralDel_Click(object sender, EventArgs e)
        {
            if (General_Step_Count > 2)
            {
                Control root = tabPage_General.Controls["panel_TabMainGeneral"];

                Control del_condition = root.Controls["condition_" + (General_Step_Count - 1).ToString()];
                Control del_unit = root.Controls["unit_" + (General_Step_Count - 1).ToString()];

                if (del_condition != null && del_unit != null)
                {
                    if (del_unit is Quest_Unit)
                    {
                        int step = Convert.ToInt32(del_unit.Tag);
                        if (step == m_nQuestRemoteCompleteStep)
                        {
                            this.SetQuestRemoteCompleteStep(0);
                        }
                    }

                    del_condition.Dispose();
                    del_unit.Dispose();

                    --General_Step_Count;
                }
            }
        }              

        private Quest_Unit Add_GeneralStep(string p_step)
        {
            Quest_Unit qu = new Quest_Unit();          
            Quest_Condition qc = new Quest_Condition();
            
            qu.Location = new Point(0, getLastControlPosition((Panel)tabPage_General.Controls["panel_TabMainGeneral"]) + 3);
            qc.Location = new Point(0, getLastControlPosition((Panel)tabPage_General.Controls["panel_TabMainGeneral"]) + qu.Size.Height + 3);

            qu.Name = "unit_" + p_step.ToString();
            qu.Tag = General_Step_Count;
            qc.Name = "condition_" + p_step;
            qc.Tag = General_Step_Count;
            

            qu.groupBox_Unit.Text = "퀘스트 " + General_Step_Count.ToString() + "단계";            

            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qu);
            tabPage_General.Controls["panel_TabMainGeneral"].Controls.Add(qc);

            ++General_Step_Count;

            if (0 < m_nQuestRemoteCompleteStep)
            {
                qu.Disable_RemoteComplete();
            }

            return qu;
        }

        // 마지막 컨트롤 얻어오는 메서드 (패널)
        private int getLastControlPosition(Panel p_page)
        {
            if (p_page.Controls.Count == 0)
                return 0;
            else
                return p_page.Controls[p_page.Controls.Count - 1].Location.Y + p_page.Controls[p_page.Controls.Count - 1].Height;
        }

        // 마지막 컨트롤 얻어오는 메서드 (컨트롤)
        private int getLastControlPosition(Control p_control)
        {
            if (p_control.Controls.Count == 0)
                return 0;
            else
                return p_control.Controls[p_control.Controls.Count - 1].Location.Y + p_control.Controls[p_control.Controls.Count - 1].Height;
        }

        // 새로 만들기 메뉴 클릭 이벤트
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("작성한 퀘스트 내용이 모두 초기화 됩니다. \n\n계속 진행하시겠습니까?", "New", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                loadedXmlDataSet = new DataSet();
                savingDataSet = null;
                init_Basic();
                init_NoQuest();
                init_Complete();
                init_Journal();
                init_Accept();
                init_General();
            }
        }

        // 불러오기 메뉴 클릭 이벤트
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

                this.Text = "Quest-" + loadFile.Name;

                if (loadFile.IsReadOnly)
                {
                    //MessageBox.Show("열려고 하는 파일이 읽기전용입니다. \n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다", "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                    loadedXmlDataSet = new DataSet();
                    savingDataSet = null;

                    init_Basic();
                    init_NoQuest();
                    init_Complete();
                    init_Journal();
                    init_Accept();
                    init_General();

                    LoadXmlFile(loadXmlDialog.FileName);
                    Set_Basic_Info();
                    Set_Journal_Info();
                    Set_NoQuest_Info();
                    Set_Accepting_Info();
                    Set_General_Info();
                    Set_Complete_Info();    
                }
                else
                {
                    loadedXmlDataSet = new DataSet();
                    savingDataSet = null;

                    init_Basic();
                    init_NoQuest();
                    init_Complete();
                    init_Journal();
                    init_Accept();
                    init_General();

                    LoadXmlFile(loadXmlDialog.FileName);
                    Set_Basic_Info();
                    Set_Journal_Info();
                    Set_NoQuest_Info();
                    Set_Accepting_Info();
                    Set_General_Info();
                    Set_Complete_Info();                    
                }
            }
        }

        // 저장 하기 메뉴 클릭 이벤트
        public void sAToolStripMenuItem_Click(object sender, EventArgs e)
        {
            allSave();
        }

        /// <summary>
        /// 작성된 내용을 저장한다.
        /// </summary>
        private void allSave()
        {
            if (ValidateChildren() != true) {
                MessageBox.Show("입력을 확인해 주십시오.");
                return;
            }

            if (6 == comboBox_Type.SelectedIndex)
            {
                int temp;
                if(false == int.TryParse(textBox_Questmark.Text, out temp))
                {
                    MessageBox.Show("퀘스트 마크 값이 잘못되었습니다.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    textBox_Questmark.Focus();
                    return;
                }

                if (false == int.TryParse(textBox_Statemark0.Text, out temp))
                {
                    MessageBox.Show("스테이트 마크(진행중) 값이 잘못되었습니다.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    textBox_Statemark0.Focus();
                    return;
                }

                if (false == int.TryParse(textBox_Statemark1.Text, out temp))
                {
                    MessageBox.Show("스테이트 마크(완료) 값이 잘못되었습니다.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    textBox_Statemark1.Focus();
                    return;
                }
            }

            SaveFileDialog loadXmlDialog = new SaveFileDialog();
            loadXmlDialog.Filter = "*.xml|*.xml";
            loadXmlDialog.InitialDirectory = String.IsNullOrEmpty(this.sSavePath) ? @"" : this.sSavePath;
            DirectoryInfo di = new DirectoryInfo(loadXmlDialog.InitialDirectory);
            if (!di.Exists)
                loadXmlDialog.InitialDirectory = @"C:\";
            
            loadXmlDialog.FileName = tabPage_Basic.Controls["panel_Basic"].Controls["groupBox_Basic"].Controls["textBox_EngName"].Text + ".xml";
            string lua_FileName = tabPage_Basic.Controls["panel_Basic"].Controls["groupBox_Basic"].Controls["textBox_EngName"].Text + ".lua";

            if (loadXmlDialog.ShowDialog() == DialogResult.OK)
            {
                FileInfo loadFile = new FileInfo(loadXmlDialog.FileName);
                FileInfo loadFile_Lua = new FileInfo(lua_FileName);

                DNQuest.SaveLua savelua = new DNQuest.SaveLua();

                if (loadFile.Exists)
                {
                    if (loadFile.IsReadOnly)
                    {
                        MessageBox.Show("저장하는 XML파일이 읽기전용입니다." 
                            + "\n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다."
                            , "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    else
                    {
                        savingDataSet = null;
                        SaveXML(loadXmlDialog.FileName);

                        if (loadFile_Lua.Exists)
                        {
                            if (loadFile_Lua.IsReadOnly)
                            {
                                MessageBox.Show("저장하는 Lua파일이 읽기전용입니다. "
                                    + "\n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다."
                                    , "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            }
                            else
                            {
                                //SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                                savelua.Execute(savingDataSet, loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua", comboBox_Type.SelectedIndex, comboBox_CompleteType.SelectedIndex);
                            }
                        }
                        else
                        {
                            //SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                            savelua.Execute(savingDataSet, loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua", comboBox_Type.SelectedIndex, comboBox_CompleteType.SelectedIndex);
                        }
                    }

                }
                else
                {
                    savingDataSet = null;
                    SaveXML(loadXmlDialog.FileName);

                    if (loadFile_Lua.Exists)
                    {
                        if (loadFile_Lua.IsReadOnly)
                            MessageBox.Show("저장하는 Lua파일이 읽기전용입니다. \n Get Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.", "확인", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        else
                        {
                            //SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                            savelua.Execute(savingDataSet, loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua", comboBox_Type.SelectedIndex, comboBox_CompleteType.SelectedIndex);
                        }
                    }
                    else
                    {
                        //SaveLua(loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua");
                        savelua.Execute(savingDataSet, loadXmlDialog.FileName.Substring(0, loadXmlDialog.FileName.LastIndexOf('.')) + ".lua", comboBox_Type.SelectedIndex, comboBox_CompleteType.SelectedIndex);
                    }
                }

                savelua.Clear();
            }
        }

        #endregion

        #region XML 파일 로드
        private void LoadXmlFile(string p_path)
        {
            try
            {
                using (FileStream fs = new FileStream(p_path, FileMode.Open, FileAccess.Read))
                {
                    using (StreamReader sr = new StreamReader(fs, System.Text.Encoding.UTF8))
                    {
                        loadedXmlDataSet.ReadXml(sr);
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        //XML 파일 구조 보기(테스트용)
        private string DataSet_View()
        {
            string List = "";
            
            foreach (DataRelation relation in loadedXmlDataSet.Relations)
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

            foreach (DataTable dt in loadedXmlDataSet.Tables)
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

        // 퀘스트 기본 정보 로딩
        private void Set_Basic_Info()
        {
            DataTable dt_Basic = loadedXmlDataSet.Tables["quest_desc"];

            DataTable dt_qkname = null;

            if (loadedXmlDataSet.Tables["qkname"] != null)
            {
                dt_qkname = loadedXmlDataSet.Tables["qkname"];
            }

            DataTable dt_Condition = loadedXmlDataSet.Tables["condition_data"];
            DataTable dt_JournalData = loadedXmlDataSet.Tables["journal_data"];

            if (dt_Basic != null)
            {
                if (dt_qkname != null)
                {
                    //textBox_KorName.Text = dt_qkname.Rows[0]["qkname_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                    textBox_KorName.Text = dt_qkname.Rows[0]["qkname_Text"].ToString();
                    textBox_KorName.Tag = dt_qkname.Rows[0]["mid"];
                }
                else
                {
                    textBox_KorName.Text = dt_Basic.Rows[0]["qkname"].ToString();
                }

                textBox_EngName.Text = dt_Basic.Rows[0]["qename"].ToString();
                textBox_Chapter.Text = dt_Basic.Rows[0]["qchapter"].ToString();
                textBox_QuestID.Text = dt_Basic.Rows[0]["qid"].ToString();
                textBox_Damdang.Text = dt_Basic.Rows[0]["designer"].ToString();
                textBox_StartLevel.Text = dt_Basic.Rows[0]["qlevel"].ToString();
                textBox_Abstrct.Text = dt_Basic.Rows[0]["qdesc"].ToString();

                if (dt_Basic.Columns["qdifficulty"] != null)
                    textBox_QuestDifficulty.Text = dt_Basic.Rows[0]["qdifficulty"].ToString();

                textBox_NextID.Text = dt_Basic.Rows[0]["nextqid"].ToString();
                textBox_NextEname.Text = dt_Basic.Rows[0]["nextqname"].ToString();
                textBox_NextNPCEname.Text = dt_Basic.Rows[0]["nextqnpc"].ToString();

                if (dt_Basic.Columns["uistyle"] != null)
                    comboBox_UIStyle.SelectedIndex = int.Parse(dt_Basic.Rows[0]["uistyle"].ToString());

                if (dt_Basic.Rows[0]["qtype"].ToString() == "mainquest")            //메인 퀘스트
                    comboBox_Type.SelectedIndex = 0;
                else if (dt_Basic.Rows[0]["qtype"].ToString() == "subquest")        //서브 퀘스트 
                    comboBox_Type.SelectedIndex = 1;
                else if (dt_Basic.Rows[0]["qtype"].ToString() == "venturerquest")   //모험자 퀘스트
                    comboBox_Type.SelectedIndex = 2;
                else if (dt_Basic.Rows[0]["qtype"].ToString() == "favorMalicequest")    //호감도 퀘스트
                    comboBox_Type.SelectedIndex = 4;
                else if (dt_Basic.Rows[0]["qtype"].ToString() == "dayquest")        //일일 퀘스트
                {
                    comboBox_Type.SelectedIndex = 5;
                    txtPeriodConditionOnDayQuest.Enabled = true;
                    chkBoxMonday.Enabled = true;
                    chkBoxTuesday.Enabled = true;
                    chkBoxWednesDay.Enabled = true;
                    chkBoxThursday.Enabled = true;
                    chkBoxFriday.Enabled = true;
                    chkBoxSaturday.Enabled = true;
                    chkBoxSunday.Enabled = true;
                }
                else if (dt_Basic.Rows[0]["qtype"].ToString() == "customquest")        // 커스텀 퀘스트
                {
                    comboBox_Type.SelectedIndex = 6;

                    // 기본값은 서브 퀘스트와 동일
                    textBox_Questmark.Text = "10";
                    textBox_Statemark0.Text = "11";
                    textBox_Statemark1.Text = "12";

                    string[] array_mark = dt_Basic.Rows[0]["custommark"].ToString().Split(',');

                    if (1 <= array_mark.Length)
                    {
                        textBox_Questmark.Text = array_mark[0];
                    }
                    if (2 <= array_mark.Length)
                    {
                        textBox_Statemark0.Text = array_mark[1];
                    }
                    if (3 <= array_mark.Length)
                    {
                        textBox_Statemark1.Text = array_mark[2];
                    }
                }

                if (dt_Basic.Rows[0]["qcomtype"].ToString() == "ONCE")
                    comboBox_CompleteType.SelectedIndex = 0;
                else
                    comboBox_CompleteType.SelectedIndex = 1;

                if (loadedXmlDataSet.Tables["qnpc"] != null)
                {
                    DataTable dt_NPC = loadedXmlDataSet.Tables["qnpc"];

                    foreach (DataRow row in dt_NPC.Rows)
                    {
                        listBox_NPC.Items.Add(row["npcid"].ToString() + "," + row["npcename"].ToString() + "," + row["npckname"].ToString());
                    }
                }
            }

            if (dt_Condition != null)
            {
                textBox_levelconditon.Text = dt_Condition.Rows[0]["user_level"].ToString();
                textBox_worldcondition.Text = dt_Condition.Rows[0]["user_worldzone_level"].ToString();
                textBox_classcondition.Text = dt_Condition.Rows[0]["user_class"].ToString();
                textBox_prevquestcondition.Text = dt_Condition.Rows[0]["prev_quest"].ToString();
                textBox_optionalQuestcondition.Text = dt_Condition.Columns.Contains("have_quest") ? dt_Condition.Rows[0]["have_quest"].ToString() : "";
                textBox_itemcondition.Text = dt_Condition.Rows[0]["have_normal_item"].ToString();
                textBox_UserMissionClearCondition.Text = dt_Condition.Columns.Contains("complete_mission") == false ? "" : dt_Condition.Rows[0]["complete_mission"].ToString();
                textBox_symbolcondition.Text = dt_Condition.Rows[0]["have_symbol_item"].ToString();
                if (true == dt_Condition.Rows[0].Table.Columns.Contains("have_cash_item"))
                {
                    textBox_cashitemcondition.Text = dt_Condition.Rows[0]["have_cash_item"].ToString();
                }
                
                if (dt_Condition.Columns.Contains("quest_date") && !string.IsNullOrEmpty(dt_Condition.Rows[0]["quest_date"].ToString()))
                    this.txtPeriodConditionOnDayQuest.Text = dt_Condition.Rows[0]["quest_date"].ToString();
                if (dt_Condition.Columns.Contains("quest_day") && !string.IsNullOrEmpty(dt_Condition.Rows[0]["quest_day"].ToString()))
                {
                    switch (dt_Condition.Rows[0]["quest_day"].ToString())
                    {
                        case "1":
                            chkBoxMonday.Checked = true;
                            break;
                        case "2":
                            chkBoxTuesday.Checked = true;
                            break;
                        case "3":
                            chkBoxWednesDay.Checked = true;
                            break;
                        case "4":
                            chkBoxThursday.Checked = true;
                            break;
                        case "5":
                            chkBoxFriday.Checked = true;
                            break;
                        case "6":
                            chkBoxSaturday.Checked = true;
                            break;
                        case "7":
                            chkBoxSunday.Checked = true;
                            break;
                    }
                }
                
                if (dt_Condition.Columns.Contains("npc_favor") && !string.IsNullOrEmpty(dt_Condition.Rows[0]["npc_favor"].ToString()))
                    this.txtNpc_favor.Text = dt_Condition.Rows[0]["npc_favor"].ToString();
                if (dt_Condition.Columns.Contains("npc_malice") && !string.IsNullOrEmpty(dt_Condition.Rows[0]["npc_malice"].ToString()))
                    this.txtNpc_malice.Text = dt_Condition.Rows[0]["npc_malice"].ToString();
            }

            if (dt_JournalData != null)
            {
                if (dt_JournalData.Columns["recompense"] != null)
                    this.txtRecompense.Text = dt_JournalData.Rows[0]["recompense"].ToString();

                if (dt_JournalData.Columns["use_item_list"] != null)
                    this.txtDeleteItem.Text = dt_JournalData.Rows[0]["use_item_list"].ToString();
            }
        }

        // 퀘스트 저널 정보 로딩
        private void Set_Journal_Info()
        {
            if (loadedXmlDataSet.Tables["journal"] != null)
            {
                DataTable dt_Journal = loadedXmlDataSet.Tables["journal"];
                DataTable dt_Basic = loadedXmlDataSet.Tables["quest_desc"];

                DataTable dt_title = null;
                DataTable dt_contnets = null;
                DataTable dt_destination = null;
                DataTable dt_todomsg = null;

                if (loadedXmlDataSet.Tables["title"] != null)
                    dt_title = loadedXmlDataSet.Tables["title"];

                if (loadedXmlDataSet.Tables["contents"] != null)
                    dt_contnets = loadedXmlDataSet.Tables["contents"];

                if (loadedXmlDataSet.Tables["destination"] != null)
                    dt_destination = loadedXmlDataSet.Tables["destination"];
                
                if (loadedXmlDataSet.Tables["todo_msg"] != null)
                    dt_todomsg = loadedXmlDataSet.Tables["todo_msg"];

                Journal_Count = 0;

                _ClearControlsAndDispose(panel_TabMainJournal.Controls);

                foreach (DataRow row in dt_Journal.Rows)
                {
                    Quest_Journal qj = new Quest_Journal();

                    if (dt_title == null)
                    {
                        qj.textBox_JournalTitle.Text = row["title"].ToString();
                    }
                    else
                    {
                        DataRow[] row_title = dt_title.Select("journal_Id = " + row["journal_Id"].ToString());
                        qj.textBox_JournalTitle.Text = row_title[0]["title_Text"].ToString();
                        
                        int mid = Convert.ToInt32(row_title[0]["mid"]);
                        qj.textBox_JournalTitle.Tag = mid;
                        if (Quest_Mid < mid)
                            Quest_Mid = mid + 1;
                    }

                    if (dt_contnets == null)
                    {
                        qj.textBox_JournalContents.Text = row["contents"].ToString();
                    }
                    else
                    {
                        DataRow[] row_contents = dt_contnets.Select("journal_Id = " + row["journal_Id"].ToString());
                        qj.textBox_JournalContents.Text = row_contents[0]["contents_Text"].ToString();

                        int mid = Convert.ToInt32(row_contents[0]["mid"]);
                        qj.textBox_JournalContents.Tag = mid;
                        if(Quest_Mid < mid )
                            Quest_Mid = mid + 1;
                    }

                    qj.textBox_JournalContentsImage.Text = row["contents_image"].ToString();

                    if (dt_destination == null)
                    {
                        qj.textBox_JournalObjectiveName.Text = row["destination"].ToString();
                    }
                    else
                    {
                        DataRow[] row_destination = dt_destination.Select("journal_Id = " + row["journal_Id"].ToString());
                        qj.textBox_JournalObjectiveName.Text = row_destination[0]["destination_Text"].ToString();

                        int mid = Convert.ToInt32(row_destination[0]["mid"]);
                        qj.textBox_JournalObjectiveName.Tag = mid;
                        if(Quest_Mid < mid )
                            Quest_Mid = mid + 1;
                    }

                    qj.textBox_JournalObjectivePos.Text = row["destination_pos"].ToString();

                    if (dt_Journal.Columns["only_minimap"] != null)
                        qj.textBox_JournalObjectiveHelper.Text = row["only_minimap"].ToString();

                    if (dt_Journal.Columns["destination_mode"] != null)
                        qj.textBox_JournalObjectiveHelper.Text = row["destination_mode"].ToString();

                    if (dt_todomsg == null)
                    {
                        if (dt_Journal.Columns["todo_msg"] != null)
                            qj.textBox_Message.Text = row["todo_msg"].ToString();
                    }
                    else
                    {
                        DataRow[] row_todomsg = dt_todomsg.Select("journal_Id = " + row["journal_Id"].ToString());
                        qj.textBox_Message.Text = row_todomsg[0]["todo_msg_Text"].ToString();

                        int mid = Convert.ToInt32(row_todomsg[0]["mid"]);
                        qj.textBox_Message.Tag = mid;
                        if(Quest_Mid < mid )
                            Quest_Mid = mid + 1;
                    }

                    if (dt_Journal.Columns["statemark_npcid"] != null)
                        qj.textBox_StartMarkNpc.Text = row["statemark_npcid"].ToString();

                    if (dt_Journal.Columns["statemark_type"] != null)
                    {
                        int state_value = 0;

                        if (int.TryParse(row["statemark_type"].ToString(), out state_value))
                        {
                            if( comboBox_Type.SelectedIndex == 0 ) {
                                if( state_value == 2 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                else if( state_value == 3 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                            }
                            else  if( comboBox_Type.SelectedIndex == 1 ) {
                                if( state_value == 11 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                else if( state_value == 12 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                            }
                            else if ( comboBox_Type.SelectedIndex == 5) {
                                if( state_value == 24 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                else if( state_value == 25 )
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                            }
                            else if (comboBox_Type.SelectedIndex == 4)
                            {
                                if (state_value == 16)
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                else if (state_value == 17)
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                            }
                            else if (comboBox_Type.SelectedIndex == 2)
                            {
                                if (state_value == 20)
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                else if (state_value == 21)
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                            }
                            else if(comboBox_Type.SelectedIndex == 6)
                            {
                                int statemark;
                                if (true == int.TryParse(textBox_Statemark0.Text, out statemark) && state_value == statemark)
                                {
                                    qj.comboBox_StatemarkType.SelectedIndex = 0;
                                }
                                else if (true == int.TryParse(textBox_Statemark1.Text, out statemark) && state_value == statemark)
                                {
                                    qj.comboBox_StatemarkType.SelectedIndex = 1;
                                }
                            }
                        }
                    }

                    if (dt_Journal.Columns["trigger_string"] != null)
                        qj.textBox_TriggerString.Text = row["trigger_string"].ToString();

                    if (dt_Journal.Columns["maptool_index"] != null)
                        qj.textBox_MapToolIndex.Text = row["maptool_index"].ToString();

                    qj.Tag = Journal_Count;

                    qj.Location = new Point(0, getLastControlPosition((Panel)tabPage_Journal.Controls["panel_TabMainJournal"]) + 3);

                    ++Journal_Count;

                    tabPage_Journal.Controls["panel_TabMainJournal"].Controls.Add(qj);
                }
            }            
        }

        // 퀘스트 없는 단계 정보 로딩
        private void Set_NoQuest_Info()
        {
            DataTable dt_talk = loadedXmlDataSet.Tables["talk_paragraph"];

            DataTable dt_question = null;

            if (loadedXmlDataSet.Tables["talk_question"] != null)
            {
                dt_question = loadedXmlDataSet.Tables["talk_question"];
            }

            DataTable dt_select = loadedXmlDataSet.Tables["talk_answer"];
            DataTable dt_exelink = loadedXmlDataSet.Tables["exelink"];

            DataRow[] noquest = dt_talk.Select("index like '%noquest%'");

            if (noquest.Length > 0)
            {
                Panel ControlPool = new Panel();

                #region 컨트롤 풀에 담기

                foreach (DataRow row in noquest)
                {
                    Quest_Unit qu = (Quest_Unit)tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls[0];

                    string[] arr_index = row["index"].ToString().Split('-');

                    if (arr_index.Length == 3)
                    {
                        Control[] arr_Control = ControlPool.Controls.Find(arr_index[0], false);
                        Quest_NPC temp_npc = (Quest_NPC)arr_Control[0];

                        Quest_Step qs = null;

                        if (dt_question != null)
                        {
                            DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                            qs = new Quest_Step(temp_npc, row_question[0]["talk_question_Text"].ToString(), arr_index[2]);
                            qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();

                            int mid = Convert.ToInt32(row_question[0]["mid"]);
                            qs.textBox_Script.Tag = mid;
                            if (Quest_Mid < mid )
                                Quest_Mid = mid + 1;
                        }
                        else
                        {
                            qs = new Quest_Step(temp_npc, row["talk_question"].ToString(), arr_index[2]);
                            qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        }

                        if (dt_talk.Columns["image"] != null)
                        {
                            qs.textBox_Image.Text = row["image"].ToString();
                        }
                        if (dt_talk.Columns["portrait_left"] != null)
                        {
                            qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                        }
                        if (dt_talk.Columns["portrait_right"] != null)
                        {
                            qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                        }

                        if (dt_select != null)
                        {
                            DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                            foreach (DataRow dr in arr_select)
                            {
                                string exeNo = "";

                                DataRow[] row_exe = dt_exelink.Select("qstep = 'noquest' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                {
                                    try
                                    {
                                        exeNo = row_exe[0]["exelink_Text"].ToString();
                                    }
                                    catch (Exception)
                                    {
                                        exeNo = "";
                                    }
                                }

                                int select_type = 0;
                                if (dr.Table.Columns.Contains("select_type"))
                                    select_type = int.Parse(dr["select_type"].ToString());

                                string select_text = "";
                                if(dr.Table.Columns.Contains("talk_answer_Text"))
                                {
                                    select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                                }
                                Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                                int mid = dt_select.Columns["mid"] != null ? Convert.ToInt32(dr["mid"]) : 0;
                                if (Quest_Mid < mid )
                                    Quest_Mid = mid + 1;

                                qs.Add_Select(select);
                            }
                        }

                        temp_npc.Add_SubStep(qs);
                    }
                    else
                    {
                        Quest_NPC qn = new Quest_NPC(qu, arr_index[0], "");
                        qn.Name = arr_index[0];

                        Quest_Step qs = null;

                        if (dt_question != null)
                        {
                            DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                            qs = new Quest_Step(qn, row_question[0]["talk_question_Text"].ToString(), "단계시작");
                            qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();

                            int mid = Convert.ToInt32(row_question[0]["mid"]);
                            qs.textBox_Script.Tag = mid;
                            if (Quest_Mid < mid )
                                Quest_Mid = mid + 1;
                        }
                        else
                        {
                            qs = new Quest_Step(qn, row["talk_question"].ToString(), "단계시작");
                            qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        }

                        if (dt_talk.Columns["image"] != null)
                            qs.textBox_Image.Text = row["image"].ToString();

                        if (dt_talk.Columns["itemindex"] != null)
                            qs.txtItemIndex.Text = row["itemindex"].ToString();

                        if (dt_talk.Columns["portrait_left"] != null)
                        {
                            qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                        }
                        if (dt_talk.Columns["portrait_right"] != null)
                        {
                            qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                        }

                        DataRow[] row_exe_npc = dt_exelink.Select("qstep = 'noquest' and npc ='" + arr_index[0] + "' and sub is null");

                        if (row_exe_npc.Length > 0)
                            qn.textBox_ExecuteNo.Text = row_exe_npc[0]["exelink_Text"].ToString();

                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            DataRow[] row_exe = dt_exelink.Select("qstep = 'noquest' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                            if (row_exe.Length > 0)
                                exeNo = row_exe[0]["exelink_Text"].ToString();

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                            int mid = dt_select.Columns["mid"] != null ? Convert.ToInt32(dr["mid"]) : 0;
                            if (Quest_Mid < mid )
                                Quest_Mid = mid + 1;

                            qs.Add_Select(select);
                        }

                        qs.Location = new Point(193, 2);
                        qn.Controls.Add(qs);

                        ControlPool.Controls.Add(qn);
                    }
                }
                #endregion

                #region 컨트롤 추가하기
                int count = ControlPool.Controls.Count;
                Quest_Unit temp_Unit = (Quest_Unit)tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls["unit_init"];

                if (count > 0)
                    temp_Unit.Controls["npc_init"].Dispose();

                for (int i = 0; i < count; ++i)
                {
                    Quest_NPC temp_npc = (Quest_NPC)ControlPool.Controls[0];
                    temp_Unit.Add_NPC(temp_npc);
                }
                #endregion
            }

        }

        // 퀘스트 완료 단계 정보 로딩
        private void Set_Complete_Info()
        {
            DataTable dt_talk = loadedXmlDataSet.Tables["talk_paragraph"];
            DataTable dt_question = null;

            if (loadedXmlDataSet.Tables["talk_question"] != null)
                dt_question = loadedXmlDataSet.Tables["talk_question"];
            
            DataTable dt_select = loadedXmlDataSet.Tables["talk_answer"];
            DataTable dt_exelink = loadedXmlDataSet.Tables["exelink"];

            DataRow[] noquest = dt_talk.Select("index like '%complete%'");

            Panel ControlPool = new Panel();

            #region 컨트롤 풀에 담기

            foreach (DataRow row in noquest)
            {
                Quest_Unit qu = (Quest_Unit)tabPage_Complete.Controls["panel_TabMainComplete"].Controls[0];

                string[] arr_index = row["index"].ToString().Split('-');

                if (arr_index.Length == 3)
                {
                    Control[] arr_Control = ControlPool.Controls.Find(arr_index[0], false);
                    Quest_NPC temp_npc = (Quest_NPC)arr_Control[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        qs = new Quest_Step(temp_npc, row_question[0]["talk_question_Text"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();

                        int mid = Convert.ToInt32(row_question[0]["mid"]);
                        qs.textBox_Script.Tag = mid;
                        if (Quest_Mid < mid )
                            Quest_Mid = mid + 1;
                    }
                    else
                    {
                        qs = new Quest_Step(temp_npc, row["talk_question"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();

                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }
                    

                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            DataRow[] row_exe = dt_exelink.Select("qstep = 'complete' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                            if (row_exe.Length > 0)
                                exeNo = row_exe[0]["exelink_Text"].ToString();

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl("", select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);
                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];
                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    temp_npc.Add_SubStep(qs);
                }
                else
                {
                    Quest_NPC qn = new Quest_NPC(qu, arr_index[0], "");
                    qn.Name = arr_index[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        qs = new Quest_Step(qn, row_question[0]["talk_question_Text"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        qs.textBox_Script.Tag = row_question[0]["mid"];

                        if (Quest_Mid < Convert.ToInt32(row_question[0]["mid"]))
                            Quest_Mid = Convert.ToInt32(row_question[0]["mid"]) + 1;
                    }
                    else
                    {
                        qs = new Quest_Step(qn, row["talk_question"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();

                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }

                    if (dt_exelink != null)
                    {
                        DataRow[] row_exe_npc = dt_exelink.Select("qstep = 'complete' and npc ='" + arr_index[0] + "' and sub is null");

                        if (row_exe_npc.Length > 0)
                            qn.textBox_ExecuteNo.Text = row_exe_npc[0]["exelink_Text"].ToString();
                    }

                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            DataRow[] row_exe = dt_exelink.Select("qstep = 'complete' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                            if (row_exe.Length > 0)
                                exeNo = row_exe[0]["exelink_Text"].ToString();

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl("", select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);
                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];
                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    qs.Location = new Point(193, 2);
                    qn.Controls.Add(qs);

                    ControlPool.Controls.Add(qn);
                }

            }
            #endregion

            #region 컨트롤 추가하기
            int count = ControlPool.Controls.Count;
            Quest_Unit temp_Unit = (Quest_Unit)tabPage_Complete.Controls["panel_TabMainComplete"].Controls["unit_init"];

            if (count > 0)
                temp_Unit.Controls["npc_init"].Dispose();

            for (int i = 0; i < count; ++i)
            {
                Quest_NPC temp_npc = (Quest_NPC)ControlPool.Controls[0];
                temp_Unit.Add_NPC(temp_npc);
            }
            #endregion
        }

        // 퀘스트 받기 단계 정보 로딩
        private void Set_Accepting_Info()
        {
            #region 퀘스트 받기NPC 로딩
            DataTable dt_talk = loadedXmlDataSet.Tables["talk_paragraph"];
            DataTable dt_question = null;

            if (loadedXmlDataSet.Tables["talk_question"] != null)
            {
                dt_question = loadedXmlDataSet.Tables["talk_question"];
            }
            DataTable dt_select = loadedXmlDataSet.Tables["talk_answer"];
            DataTable dt_exelink = loadedXmlDataSet.Tables["exelink"];

            DataRow[] noquest = dt_talk.Select("index like '%accepting%'");

            Panel ControlPool = new Panel();

            #region 컨트롤 풀에 담기

            foreach (DataRow row in noquest)
            {
                Quest_Unit qu = (Quest_Unit)tabPage_Accept.Controls["panel_TabMainAccept"].Controls[0];

                string[] arr_index = row["index"].ToString().Split('-');

                if (arr_index.Length == 3)
                {
                    Control[] arr_Control = ControlPool.Controls.Find(arr_index[0], false);
                    Quest_NPC temp_npc = (Quest_NPC)arr_Control[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        qs = new Quest_Step(temp_npc, row_question[0]["talk_question_Text"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        qs.textBox_Script.Tag = row_question[0]["mid"];

                        if (Quest_Mid < Convert.ToInt32(row_question[0]["mid"]))
                        {
                            Quest_Mid = Convert.ToInt32(row_question[0]["mid"]) + 1;
                        }
                    }
                    else
                    {
                        qs = new Quest_Step(temp_npc, row["talk_question"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();

                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }

                    if (dt_select != null)
                    {

                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            if (dt_exelink != null)
                            {
                                DataRow[] row_exe = dt_exelink.Select("qstep = 'accepting' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                    exeNo = row_exe[0]["exelink_Text"].ToString();
                            }

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];

                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    temp_npc.Add_SubStep(qs);
                }
                else
                {
                    Quest_NPC qn = new Quest_NPC(qu, arr_index[0], "");
                    qn.Name = arr_index[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        qs = new Quest_Step(qn, row_question[0]["talk_question_Text"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        qs.textBox_Script.Tag = row_question[0]["mid"];

                        if (Quest_Mid < Convert.ToInt32(row_question[0]["mid"]))
                            Quest_Mid = Convert.ToInt32(row_question[0]["mid"]) + 1;
                    }
                    else
                    {
                        qs = new Quest_Step(qn, row["talk_question"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();

                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }

                    if (dt_exelink != null)
                    {
                        DataRow[] row_exe_npc = dt_exelink.Select("qstep = 'accepting' and npc ='" + arr_index[0] + "' and sub is null");

                        if (row_exe_npc.Length > 0)
                            qn.textBox_ExecuteNo.Text = row_exe_npc[0]["exelink_Text"].ToString();
                    }


                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            if (dt_exelink != null)
                            {

                                DataRow[] row_exe = dt_exelink.Select("qstep = 'accepting' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                    exeNo = row_exe[0]["exelink_Text"].ToString();
                                
                            }

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];
                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    qs.Location = new Point(193, 2);
                    qn.Controls.Add(qs);

                    ControlPool.Controls.Add(qn);
                }

            }
            #endregion

            #region 컨트롤 추가하기
            int count = ControlPool.Controls.Count;
            Quest_Unit temp_Unit = (Quest_Unit)tabPage_Accept.Controls["panel_TabMainAccept"].Controls["unit_init"];

            if (count > 0)
            {

                temp_Unit.Controls["npc_init"].Dispose();
            }

            for (int i = 0; i < count; ++i)
            {
                Quest_NPC temp_npc = (Quest_NPC)ControlPool.Controls[0];

                temp_Unit.Add_NPC(temp_npc);
            }
            #endregion
            #endregion 

            #region 퀘스트 받기 조건실행문 로딩

            DataTable dt_change = loadedXmlDataSet.Tables["qstep_change"];
            if (dt_change != null)
            {
                DataRow[] Row_memo = dt_change.Select("qstep = 'accepting'");

                if (Row_memo.Length > 0)
                {
                    Quest_Condition qc = (Quest_Condition)tabPage_Accept.Controls["panel_TabMainAccept"].Controls.Find("condition_accept", false)[0];
                    qc.Set_Condition_Memo(Row_memo[0]["comment"].ToString()); // 설명 셋팅     

                    _loadConditionObject(qc, Row_memo[0]);
                    _loadExecutionObject(qc, Row_memo[0]);

                    #region 조건문 로딩
                    //DataTable dt_condition = loadedXmlDataSet.Tables["conditions"];

                    //if (dt_condition != null)
                    //{
                    //    DataRow[] Row_condition = dt_condition.Select("qstep_change_Id = " + Row_memo[0]["qstep_change_Id"]);

                    //    string condition_id = Row_condition[0]["conditions_Id"].ToString();

                    //    DataTable dt_chk = loadedXmlDataSet.Tables["chk"];
                    //    DataRow[] Row_chk = dt_chk.Select("conditions_Id = " + condition_id);

                    //    foreach (DataRow row in Row_chk)
                    //    {
                    //        switch (row["type"].ToString())
                    //        {
                    //            case "lvl":
                    //                Conditions.Condition_Level con_level = new DNQuest.Conditions.Condition_Level();
                    //                con_level.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_level);
                    //                break;
                    //            case "item":
                    //                Conditions.Condition_GeneralItem con_item = new DNQuest.Conditions.Condition_GeneralItem();
                    //                con_item.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                    //                qc.Add_Condition(con_item);
                    //                break;
                    //            case "symbol":
                    //                Conditions.Condition_SymbolItem con_symbol = new DNQuest.Conditions.Condition_SymbolItem();
                    //                con_symbol.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                    //                qc.Add_Condition(con_symbol);
                    //                break;
                    //            case "questitem":
                    //                Conditions.Condition_QuestItem con_qitem = new DNQuest.Conditions.Condition_QuestItem();
                    //                con_qitem.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                    //                qc.Add_Condition(con_qitem);
                    //                break;
                    //            case "class":
                    //                Conditions.Condition_Class con_class = new DNQuest.Conditions.Condition_Class();
                    //                con_class.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_class);
                    //                break;
                    //            case "prob":
                    //                Conditions.Condition_Prob con_prob = new DNQuest.Conditions.Condition_Prob();
                    //                con_prob.Set_Value(row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_prob);
                    //                break;
                    //            case "hasquest":
                    //                Conditions.Condition_Quest con_quest = new DNQuest.Conditions.Condition_Quest();
                    //                con_quest.Set_Value(row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_quest);
                    //                break;
                    //            case "hascomq":
                    //                Conditions.Condition_Compelete con_comp = new DNQuest.Conditions.Condition_Compelete();
                    //                con_comp.Set_Value(row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_comp);
                    //                break;
                    //            case "inven_eslot":
                    //                Conditions.Condition_Inven con_iven = new DNQuest.Conditions.Condition_Inven();
                    //                con_iven.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_iven);
                    //                break;
                    //            case "custom":
                    //                Conditions.Condition_Custum con_custom = new DNQuest.Conditions.Condition_Custum();
                    //                con_custom.textBox_Code.Text = row["chk_Text"].ToString();
                    //                qc.Add_Condition(con_custom);
                    //                break;
                    //            case "qcount":
                    //                Conditions.Condition_QuestCnt con_qcnt = new DNQuest.Conditions.Condition_QuestCnt();
                    //                qc.Add_Condition(con_qcnt);
                    //                break;
                    //            case "stageConstructionLevel":
                    //                Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = new DNQuest.Conditions.Condition_StageConstructionLevel();
                    //                con_stageConstructionLevel.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                    //                qc.Add_Condition(con_stageConstructionLevel);
                    //                break;
                    //        }
                    //    }
                    //}

                    #endregion

                    #region 실행문 로딩
                    //DataTable dt_execution = loadedXmlDataSet.Tables["executions"];
                    //if (dt_execution != null)
                    //{
                    //    DataRow[] Row_execution = dt_execution.Select("qstep_change_Id = " + Row_memo[0]["qstep_change_Id"]);

                    //    string execuition_id = Row_execution[0]["executions_Id"].ToString();

                    //    DataTable dt_exe = loadedXmlDataSet.Tables["exe"];
                    //    DataRow[] Row_exe = dt_exe.Select("executions_Id = " + execuition_id);

                    //    foreach (DataRow row in Row_exe)
                    //    {
                    //        switch (row["type"].ToString())
                    //        {
                    //            case "setstep":
                    //                Executions.Exe_QuestStepMod exe_qsm = new DNQuest.Executions.Exe_QuestStepMod();
                    //                exe_qsm.Set_Step(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_qsm);
                    //                break;
                    //            case "setjornal":
                    //                Executions.Exe_JournalMod exe_journal = new DNQuest.Executions.Exe_JournalMod();
                    //                exe_journal.Set_Journal(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_journal);
                    //                break;
                    //            case "delitem":
                    //                Executions.Exe_ItemDel exe_itemdel = new DNQuest.Executions.Exe_ItemDel();
                    //                exe_itemdel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_itemdel);
                    //                break;
                    //            case "delsymbol":
                    //                Executions.Exe_SymbolDel exe_symdel = new DNQuest.Executions.Exe_SymbolDel();
                    //                exe_symdel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_symdel);
                    //                break;
                    //            case "delquestitem":
                    //                Executions.Exe_QuestItemDel exe_qidel = new DNQuest.Executions.Exe_QuestItemDel();
                    //                exe_qidel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_qidel);
                    //                break;
                    //            case "additem":
                    //                Executions.Exe_ItemAdd exe_itemadd = new DNQuest.Executions.Exe_ItemAdd();
                    //                exe_itemadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_itemadd);
                    //                break;
                    //            case "addsymbol":
                    //                Executions.Exe_SymbolAdd exe_symadd = new DNQuest.Executions.Exe_SymbolAdd();
                    //                exe_symadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_symadd);
                    //                break;
                    //            case "addquestitem":
                    //                Executions.Exe_QuestItemAdd exe_qiadd = new DNQuest.Executions.Exe_QuestItemAdd();
                    //                exe_qiadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                    //                qc.Add_Execution(exe_qiadd);
                    //                break;
                    //            case "huntmon":
                    //                Executions.Exe_MonsterAdd exe_monadd = new DNQuest.Executions.Exe_MonsterAdd();
                    //                exe_monadd.Set_Value(row["exe_Text"].ToString(), row["party"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                    //                qc.Add_Execution(exe_monadd);
                    //                break;
                    //            case "huntitem":
                    //                Executions.Exe_ItemColAdd exe_itemcoladd = new DNQuest.Executions.Exe_ItemColAdd();
                    //                exe_itemcoladd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                    //                qc.Add_Execution(exe_itemcoladd);
                    //                break;
                    //            case "huntsymbol":
                    //                Executions.Exe_SymbolColAdd exe_symboladd = new DNQuest.Executions.Exe_SymbolColAdd();
                    //                exe_symboladd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                    //                qc.Add_Execution(exe_symboladd);
                    //                break;
                    //            case "huntall":
                    //                Executions.Exe_AllHunting exe_allhunt = new DNQuest.Executions.Exe_AllHunting();
                    //                exe_allhunt.Set_Value(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_allhunt);
                    //                break;
                    //            case "setDailyQuest" :
                    //                Executions.Exe_AddDailyQuest exe_AddDailyQuest = new DNQuest.Executions.Exe_AddDailyQuest();
                    //                exe_AddDailyQuest.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                    //                qc.Add_Execution(exe_AddDailyQuest);
                    //                break;
                    //            case "setquest":
                    //                Executions.Exe_QuestAdd exe_questadd = new DNQuest.Executions.Exe_QuestAdd();
                    //                exe_questadd.Set_Value(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_questadd);
                    //                break;
                    //            case "msg":
                    //                Executions.Exe_Message exe_msg = new DNQuest.Executions.Exe_Message();
                    //                exe_msg.Set_Value(row["exe_Text"].ToString(), row["msg_type"].ToString(), row["substring"].ToString());
                    //                qc.Add_Execution(exe_msg);
                    //                break;
                    //            case "talk":
                    //                Executions.Exe_Script exe_script = new DNQuest.Executions.Exe_Script();
                    //                exe_script.Set_Value(row["npc"].ToString(), row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_script);
                    //                break;
                    //            case "random_coice":
                    //                Executions.Exe_ProbChoice exe_choice = new DNQuest.Executions.Exe_ProbChoice();
                    //                exe_choice.Set_Value(row["rand"].ToString(), row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_choice);
                    //                break;
                    //            case "conexe":
                    //                Executions.Exe_Condition exe_condition = new DNQuest.Executions.Exe_Condition();
                    //                exe_condition.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                    //                qc.Add_Execution(exe_condition);
                    //                break;
                    //            case "moneyDelete":
                    //                Executions.Exe_MoneyDel exe_moneyDelete = new DNQuest.Executions.Exe_MoneyDel();
                    //                exe_moneyDelete.Set_Value(row["num"].ToString(), row["onexe"].ToString(), row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_moneyDelete);
                    //                break;
                    //            case "complete":
                    //                Executions.Exe_QuestComplete exe_questcomp = new DNQuest.Executions.Exe_QuestComplete();
                    //                exe_questcomp.Set_Value(row["exe_Text"].ToString());
                    //                if (true == row.Table.Columns.Contains("true"))
                    //                {
                    //                    exe_questcomp.Set_CompleteValue(row["true"].ToString());
                    //                }
                    //                qc.Add_Execution(exe_questcomp);
                    //                break;
                    //            case "movie":
                    //                break;
                    //            case "delcount":
                    //                Executions.Exe_AddDel exe_alldel = new DNQuest.Executions.Exe_AddDel();
                    //                qc.Add_Execution(exe_alldel);
                    //                break;
                    //            case "custom":
                    //                Executions.Exe_Custum exe_custum = new DNQuest.Executions.Exe_Custum();
                    //                exe_custum.Set_Code(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_custum);
                    //                break;
                    //            case "return":
                    //                Executions.Exe_Return exe_return = new DNQuest.Executions.Exe_Return();
                    //                qc.Add_Execution(exe_return);
                    //                break;
                    //            case "fullinven":
                    //                Executions.Exe_FullInven exe_fullinven = new DNQuest.Executions.Exe_FullInven();                                    
                    //                qc.Add_Execution(exe_fullinven);
                    //                break;
                    //            case "fullquest":
                    //                Executions.Exe_FullQuest exe_fullquest = new DNQuest.Executions.Exe_FullQuest();
                    //                qc.Add_Execution(exe_fullquest);
                    //                break;
                    //            case "recompense":
                    //                {
                    //                    Executions.Exe_Recompense exe_recompense = new DNQuest.Executions.Exe_Recompense();
                                    
                    //                    string exe_Text = row["exe_Text"].ToString();
                    //                    if (exe_Text.IndexOf(',') > -1)
                    //                    {
                    //                        string[] classes = exe_Text.Split(',');
                    //                        exe_recompense.textBox_Index.Text = classes[0];
                    //                        exe_recompense.txtRewardWarrior.Text = classes[1];
                    //                        exe_recompense.txtRewardArcher.Text = classes[2];
                    //                        exe_recompense.txtRewardCleric.Text = classes[3];
                    //                        exe_recompense.txtRewardSorcerer.Text = classes[4];
                    //                        if( classes.Length > 5)
                    //                            exe_recompense.txtRewardArcademic.Text = classes[5];
                    //                        if (classes.Length > 6)
                    //                            exe_recompense.txtRewardKally.Text = classes[6];
                    //                    }
                    //                    else
                    //                    {
                    //                        exe_recompense.textBox_Index.Text = exe_Text;
                    //                    }
                                    
                    //                    qc.Add_Execution(exe_recompense);
                    //                }
                    //                break;
                    //            case "recompense_view":
                    //                {
                    //                    Executions.Exe_Recompense_View exe_recompense_view = new DNQuest.Executions.Exe_Recompense_View();
                    //                    //exe_recompense_view.textBox_Index.Text = row["exe_Text"].ToString();
                    //                    string exe_Text = row["exe_Text"].ToString();
                    //                    if (exe_Text.IndexOf(',') > -1)
                    //                    {
                    //                        string[] classes = exe_Text.Split(',');
                    //                        exe_recompense_view.textBox_Index.Text = classes[0];
                    //                        exe_recompense_view.txtRewardWarrior.Text = classes[1];
                    //                        exe_recompense_view.txtRewardArcher.Text = classes[2];
                    //                        exe_recompense_view.txtRewardCleric.Text = classes[3];
                    //                        exe_recompense_view.txtRewardSorcerer.Text = classes[4];
                    //                        if( classes.Length > 5 )
                    //                            exe_recompense_view.txtRewardArcademic.Text = classes[5];
                    //                        if( classes.Length > 6 )
                    //                            exe_recompense_view.txtRewardKally.Text = classes[6];

                    //                    }
                    //                    else
                    //                    {
                    //                        exe_recompense_view.textBox_Index.Text = exe_Text;
                    //                    }
                    //                    qc.Add_Execution(exe_recompense_view);
                    //                }
                    //                break;
                    //            case "comment":
                    //                Executions.Exe_Comment exe_comment = new DNQuest.Executions.Exe_Comment();
                    //                exe_comment.Set_Code(row["exe_Text"].ToString());
                    //                qc.Add_Execution(exe_comment);
                    //                break;
                    //            case "checkStageClearRank":
                    //                Executions.Exe_CheckStageClearRank exe_CheckStageClearRank = new DNQuest.Executions.Exe_CheckStageClearRank();
                    //                exe_CheckStageClearRank.setValue(
                    //                    row["num"].ToString()
                    //                    , row["rand"].ToString()
                    //                    , row["true"].ToString().Replace('[', '<').Replace(']', '>')
                    //                    , row["onexe"].ToString());
                    //                qc.Add_Execution(exe_CheckStageClearRank);
                    //                break;
                    //        }
                    //    }
                    //}
                    #endregion
                }
            }
            #endregion
        }

        // 퀘스트 일반 단계 정보 로딩
        private void Set_General_Info()
        {
            DataTable dt_talk = loadedXmlDataSet.Tables["talk_paragraph"];
            DataTable dt_question = null;

            if (loadedXmlDataSet.Tables["talk_question"] != null)
            {
                dt_question = loadedXmlDataSet.Tables["talk_question"];
            }
            DataTable dt_select = loadedXmlDataSet.Tables["talk_answer"];
            DataTable dt_exelink = loadedXmlDataSet.Tables["exelink"];

            DataTable dt_remotecomplete = loadedXmlDataSet.Tables["remotecomplete"];
            
            DataRow[] noquest = dt_talk.Select(
@"index not like '%complete%' 
    AND index not like '%noquest%' 
    AND index not like '%accepting%' 
    AND index <> 'q_enter' 
    AND index <> '!quit' 
    AND index <> '_no_quest' 
    AND index <> '_full_quest' 
    AND index <> '_full_inventory' 
    AND index <> '_full_Qinventory' 
    AND index <> '_daily_quest_fail' 
    AND index not like '%acceptted%' ");

            string temp = "";
            foreach (DataRow dr in noquest)
                temp += dr["index"].ToString() + ";";
            
            Panel ControlPool = new Panel();

            #region 컨트롤 풀에 담기
            foreach (DataRow row in noquest)
            {
                Quest_Unit qu = (Quest_Unit)tabPage_General.Controls["panel_TabMainGeneral"].Controls[0];

                string[] arr_index = row["index"].ToString().Split('-');

                if (arr_index.Length == 3)
                {
                    Control[] arr_Control = ControlPool.Controls.Find(arr_index[0] + "_" + arr_index[1], false);
                    Quest_NPC temp_npc = (Quest_NPC)arr_Control[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        qs = new Quest_Step(temp_npc, row_question[0]["talk_question_Text"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();

                        int mid = Convert.ToInt32(row_question[0]["mid"]);
                        qs.textBox_Script.Tag = mid;
                        if (Quest_Mid < mid)
                            Quest_Mid = mid + 1;
                    }
                    else
                    {
                        qs = new Quest_Step(temp_npc, row["talk_question"].ToString(), arr_index[2]);
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();
                    
                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }
                    
                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            if (dt_exelink != null)
                            {
                                DataRow[] row_exe = dt_exelink.Select("qstep = '" + arr_index[1] + "' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                {
                                    try
                                    {
                                        exeNo = row_exe[0]["exelink_Text"].ToString();
                                    }
                                    catch (Exception)
                                    {
                                        exeNo = "";
                                    }
                                }
                            }

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                            int mid = dt_select.Columns["mid"] != null ? Convert.ToInt32(dr["mid"]) : 0;
                            select.textBox_Select.Tag = mid;
                            if (Quest_Mid < mid)
                                Quest_Mid = mid + 1;

                            qs.Add_Select(select);
                        }
                    }

                    temp_npc.Add_SubStep(qs);
                }
                else if (arr_index.Length == 2)
                {
                    Quest_NPC qn = new Quest_NPC(qu, arr_index[0], "");
                    qn.Name = arr_index[0] + "_" + arr_index[1];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());
                        qs = new Quest_Step(qn, row_question[0]["talk_question_Text"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        qs.textBox_Script.Tag = row_question[0]["mid"];

                        if (Quest_Mid < Convert.ToInt32(row_question[0]["mid"]))
                        {
                            Quest_Mid = Convert.ToInt32(row_question[0]["mid"]) + 1;
                        }
                    }
                    else
                    {
                        qs = new Quest_Step(qn, row["talk_question"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_talk.Columns["image"] != null)
                        qs.textBox_Image.Text = row["image"].ToString();

                    if (dt_talk.Columns["itemindex"] != null)
                        qs.txtItemIndex.Text = row["itemindex"].ToString();

                    if (dt_talk.Columns["portrait_left"] != null)
                    {
                        qs.textBox_PortraitLeft.Text = row["portrait_left"].ToString();
                    }
                    if (dt_talk.Columns["portrait_right"] != null)
                    {
                        qs.textBox_PortraitRight.Text = row["portrait_right"].ToString();
                    }

                    if (dt_exelink != null)
                    {
                        DataRow[] row_exe_npc = dt_exelink.Select("qstep = '" + arr_index[1] + "' and npc ='" + arr_index[0] + "' and sub is null");

                        if (row_exe_npc.Length > 0)
                            qn.textBox_ExecuteNo.Text = row_exe_npc[0]["exelink_Text"].ToString();
                    }
                    
                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            if (dt_exelink != null)
                            {
                                DataRow[] row_exe = dt_exelink.Select("qstep = '" + arr_index[1] + "' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                    exeNo = row_exe[0]["exelink_Text"].ToString();
                            }

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);

                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];

                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    qs.Location = new Point(193, 2);
                    qn.Controls.Add(qs);

                    ControlPool.Controls.Add(qn);
                }
                else
                {
                    Quest_NPC qn = new Quest_NPC(qu, arr_index[0], "");
                    qn.Name = arr_index[0];

                    Quest_Step qs = null;

                    if (dt_question != null)
                    {
                        DataRow[] row_question = dt_question.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());
                        qs = new Quest_Step(qn, row_question[0]["talk_question_Text"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                        qs.textBox_Script.Tag = row_question[0]["mid"];

                        if (Quest_Mid < Convert.ToInt32(row_question[0]["mid"]))
                            Quest_Mid = Convert.ToInt32(row_question[0]["mid"]) + 1;
                    }
                    else
                    {
                        qs = new Quest_Step(qn, row["talk_question"].ToString(), "단계시작");
                        qs.Name = arr_index[0] + "_" + row["talk_paragraph_Id"].ToString();
                    }

                    if (dt_exelink != null)
                    {
                        DataRow[] row_exe_npc = dt_exelink.Select("qstep = '" + arr_index[1] + "' and npc ='" + arr_index[0] + "' and sub=''");

                        if (row_exe_npc.Length > 0)
                            qn.textBox_ExecuteNo.Text = row_exe_npc[0]["exelink_Text"].ToString();
                    }

                    if (dt_select != null)
                    {
                        DataRow[] arr_select = dt_select.Select("talk_paragraph_Id = " + row["talk_paragraph_Id"].ToString());

                        foreach (DataRow dr in arr_select)
                        {
                            string exeNo = "";

                            if (dt_exelink != null)
                            {
                                DataRow[] row_exe = dt_exelink.Select("qstep = '" + arr_index[1] + "' and npc ='" + arr_index[0] + "' and sub='" + dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1) + "'");

                                if (row_exe.Length > 0)
                                    exeNo = row_exe[0]["exelink_Text"].ToString();
                            }

                            int select_type = 0;
                            if (dr.Table.Columns.Contains("select_type"))
                                select_type = int.Parse(dr["select_type"].ToString());

                            string select_text = "";
                            if (dr.Table.Columns.Contains("talk_answer_Text"))
                            {
                                select_text = dr["talk_answer_Text"].ToString().Replace("<![CDATA[", "").Replace("]]>", "");
                            }
                            Quest_SelectControl select = new Quest_SelectControl(exeNo, select_text, dr["link_index"].ToString().Substring(dr["link_index"].ToString().LastIndexOf('-') + 1), select_type);
                            if (dt_select.Columns["mid"] != null)
                            {
                                select.textBox_Select.Tag = dr["mid"];

                                if (Quest_Mid < Convert.ToInt32(dr["mid"]))
                                    Quest_Mid = Convert.ToInt32(dr["mid"]) + 1;
                            }

                            qs.Add_Select(select);
                        }
                    }

                    qs.Location = new Point(193, 2);
                    qn.Controls.Add(qs);

                    ControlPool.Controls.Add(qn);
                }
            }
            #endregion

            #region 컨트롤 추가하기
            int count = ControlPool.Controls.Count;
            int Prev_Step = 1;

            Quest_Unit temp_Unit = (Quest_Unit)tabPage_General.Controls["panel_TabMainGeneral"].Controls["unit_init"];

            if (count > 0)
                temp_Unit.Controls["npc_init"].Dispose();
            

            for (int i = 0; i < count; ++i)
            {                
                Quest_NPC temp_npc = (Quest_NPC)ControlPool.Controls[0];

                int step = Convert.ToInt16(temp_npc.Name.Substring(temp_npc.Name.LastIndexOf('_') + 1));

                if (step == 1)
                {
                    temp_Unit.Add_NPC(temp_npc);
                    temp_Unit.groupBox_Unit.Height = getLastControlPosition(temp_Unit) - 8;
                    temp_Unit.Height = getLastControlPosition(temp_Unit);
                }
                else
                {
                    if (Prev_Step < step)
                    {
                        Quest_Unit qu = Add_GeneralStep(step.ToString());
                        qu.Add_NPC(temp_npc);
                        qu.groupBox_Unit.Height = getLastControlPosition(qu) - 8;
                        qu.Height = getLastControlPosition(qu);
                        Prev_Step = step;
                    }
                    else
                    {
                        Quest_Unit qu = (Quest_Unit)tabPage_General.Controls["panel_TabMainGeneral"].Controls["unit_" + step];

                        if (qu != null)
                        {
                            qu.Add_NPC(temp_npc);
                            qu.groupBox_Unit.Height = getLastControlPosition(qu) - 8;
                            qu.Height = getLastControlPosition(qu);
                        }
                       
                    }                 
                }               
            }
            #endregion

            #region 원격완료 추가하기
            if(null != dt_remotecomplete)
            {
                Quest_Unit qu = null;
                int qstep = Convert.ToInt32( dt_remotecomplete.Rows[0]["qstep"].ToString() );
                if (1 == qstep)
                {
                    qu = (Quest_Unit)tabPage_General.Controls["panel_TabMainGeneral"].Controls["unit_init"];
                }
                else
                {
                    qu = (Quest_Unit)tabPage_General.Controls["panel_TabMainGeneral"].Controls["unit_" + qstep];
                }

                if (null == qu)
                {
                    qu = Add_GeneralStep(qstep.ToString());
                    qu.groupBox_Unit.Height = getLastControlPosition(qu) - 8;
                    qu.Height = getLastControlPosition(qu);

                    //Quest_NPC qn = new Quest_NPC(qu, "", "");
                    //qn.Controls.Add(qs);

                    //qu.Controls.Add(qn);
                    
                }

                qu.Add_RemoteComplete(dt_remotecomplete.Rows[0][1].ToString());
            }
            #endregion

            #region 조건 실행문 추가
            DataTable dt_change = loadedXmlDataSet.Tables["qstep_change"];
            DataRow[] Row_Condition = dt_change.Select("qstep <> 'accepting'");                   

            if (Row_Condition.Length > 0)
            {
                foreach (DataRow row in Row_Condition)
                {
                    int i;
                    if (Int32.TryParse(row["qstep"].ToString(), out i))
                    {
                        int step = i;

                        Quest_Condition qc;

                        if (step == 1)
                            qc = (Quest_Condition)tabPage_General.Controls["panel_TabMainGeneral"].Controls.Find("condition_init", false)[0];
                        else
                            qc = (Quest_Condition)tabPage_General.Controls["panel_TabMainGeneral"].Controls.Find("condition_" + step.ToString(), false)[0];

                        qc.Set_Condition_Memo(row["comment"].ToString()); // 설명 셋팅                    

                        _loadConditionObject(qc, row);
                        _loadExecutionObject(qc, row);

                        #region 조건문 로딩
                        //DataTable dt_condition = loadedXmlDataSet.Tables["conditions"];

                        //if (dt_condition != null)
                        //{
                        //    DataRow[] Row_condition = dt_condition.Select("qstep_change_Id = " + row["qstep_change_Id"]);

                        //    string condition_id = Row_condition[0]["conditions_Id"].ToString();

                        //    DataTable dt_chk = loadedXmlDataSet.Tables["chk"];
                        //    DataRow[] Row_chk = dt_chk.Select("conditions_Id = " + condition_id);

                        //    foreach (DataRow row_chk in Row_chk)
                        //    {
                        //        switch (row_chk["type"].ToString())
                        //        {
                        //            case "lvl":
                        //                Conditions.Condition_Level con_level = new DNQuest.Conditions.Condition_Level();
                        //                con_level.Set_Value(row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_level);
                        //                break;
                        //            case "item":
                        //                Conditions.Condition_GeneralItem con_item = new DNQuest.Conditions.Condition_GeneralItem();
                        //                con_item.Set_Value(row_chk["chk_Text"].ToString(), row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["num"].ToString());
                        //                qc.Add_Condition(con_item);
                        //                break;
                        //            case "symbol":
                        //                Conditions.Condition_SymbolItem con_symbol = new DNQuest.Conditions.Condition_SymbolItem();
                        //                con_symbol.Set_Value(row_chk["chk_Text"].ToString(), row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["num"].ToString());
                        //                qc.Add_Condition(con_symbol);
                        //                break;
                        //            case "questitem":
                        //                Conditions.Condition_QuestItem con_qitem = new DNQuest.Conditions.Condition_QuestItem();
                        //                con_qitem.Set_Value(row_chk["chk_Text"].ToString(), row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["num"].ToString());
                        //                qc.Add_Condition(con_qitem);
                        //                break;
                        //            case "class":
                        //                Conditions.Condition_Class con_class = new DNQuest.Conditions.Condition_Class();
                        //                con_class.Set_Value(row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_class);
                        //                break;
                        //            case "prob":
                        //                Conditions.Condition_Prob con_prob = new DNQuest.Conditions.Condition_Prob();
                        //                con_prob.Set_Value(row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_prob);
                        //                break;
                        //            case "hasquest":
                        //                Conditions.Condition_Quest con_quest = new DNQuest.Conditions.Condition_Quest();
                        //                con_quest.Set_Value(row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_quest);
                        //                break;
                        //            case "hascomq":
                        //                Conditions.Condition_Compelete con_comp = new DNQuest.Conditions.Condition_Compelete();
                        //                con_comp.Set_Value(row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_comp);
                        //                break;
                        //            case "inven_eslot":
                        //                Conditions.Condition_Inven con_iven = new DNQuest.Conditions.Condition_Inven();
                        //                con_iven.Set_Value(row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'), row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_iven);
                        //                break;
                        //            case "custom":
                        //                Conditions.Condition_Custum con_custom = new DNQuest.Conditions.Condition_Custum();
                        //                con_custom.textBox_Code.Text = row_chk["chk_Text"].ToString();
                        //                qc.Add_Condition(con_custom);
                        //                break;
                        //            case "qcount":
                        //                Conditions.Condition_QuestCnt con_qcnt = new DNQuest.Conditions.Condition_QuestCnt();
                        //                qc.Add_Condition(con_qcnt);
                        //                break;
                        //            case "stageConstructionLevel":
                        //                Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = 
                        //                    new DNQuest.Conditions.Condition_StageConstructionLevel();
                        //                con_stageConstructionLevel.Set_Value(
                        //                    row_chk["op"].ToString().Replace('[', '<').Replace(']', '>'),
                        //                    row_chk["chk_Text"].ToString());
                        //                qc.Add_Condition(con_stageConstructionLevel);
                        //                break;
                        //        }
                        //    }
                        //}

                        #endregion

                        #region 실행문 로딩
                        //DataTable dt_execution = loadedXmlDataSet.Tables["executions"];
                        //if (dt_execution != null)
                        //{
                        //    DataRow[] Row_execution = dt_execution.Select("qstep_change_Id = " + row["qstep_change_Id"]);

                        //    string execuition_id = Row_execution[0]["executions_Id"].ToString();

                        //    DataTable dt_exe = loadedXmlDataSet.Tables["exe"];
                        //    DataRow[] Row_exe = dt_exe.Select("executions_Id = " + execuition_id);

                        //    foreach (DataRow row_exe in Row_exe)
                        //    {
                        //        switch (row_exe["type"].ToString())
                        //        {
                        //            case "setstep":
                        //                Executions.Exe_QuestStepMod exe_qsm = new DNQuest.Executions.Exe_QuestStepMod();
                        //                exe_qsm.Set_Step(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_qsm);
                        //                break;
                        //            case "setjornal":
                        //                Executions.Exe_JournalMod exe_journal = new DNQuest.Executions.Exe_JournalMod();
                        //                exe_journal.Set_Journal(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_journal);
                        //                break;
                        //            case "delitem":
                        //                Executions.Exe_ItemDel exe_itemdel = new DNQuest.Executions.Exe_ItemDel();
                        //                exe_itemdel.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_itemdel);
                        //                break;
                        //            case "delsymbol":
                        //                Executions.Exe_SymbolDel exe_symdel = new DNQuest.Executions.Exe_SymbolDel();
                        //                exe_symdel.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_symdel);
                        //                break;
                        //            case "delquestitem":
                        //                Executions.Exe_QuestItemDel exe_qidel = new DNQuest.Executions.Exe_QuestItemDel();
                        //                exe_qidel.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_qidel);
                        //                break;
                        //            case "additem":
                        //                Executions.Exe_ItemAdd exe_itemadd = new DNQuest.Executions.Exe_ItemAdd();
                        //                exe_itemadd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_itemadd);
                        //                break;
                        //            case "addsymbol":
                        //                Executions.Exe_SymbolAdd exe_symadd = new DNQuest.Executions.Exe_SymbolAdd();
                        //                exe_symadd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_symadd);
                        //                break;
                        //            case "addquestitem":
                        //                Executions.Exe_QuestItemAdd exe_qiadd = new DNQuest.Executions.Exe_QuestItemAdd();
                        //                exe_qiadd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString());
                        //                qc.Add_Execution(exe_qiadd);
                        //                break;
                        //            case "huntmon":
                        //                Executions.Exe_MonsterAdd exe_monadd = new DNQuest.Executions.Exe_MonsterAdd();
                        //                exe_monadd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["party"].ToString(), row_exe["num"].ToString(), row_exe["onexe"].ToString(), row_exe["exe"].ToString());
                        //                qc.Add_Execution(exe_monadd);
                        //                break;
                        //            case "huntitem":
                        //                Executions.Exe_ItemColAdd exe_itemcoladd = new DNQuest.Executions.Exe_ItemColAdd();
                        //                exe_itemcoladd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString(), row_exe["onexe"].ToString(), row_exe["exe"].ToString());
                        //                qc.Add_Execution(exe_itemcoladd);
                        //                break;
                        //            case "huntsymbol":
                        //                Executions.Exe_SymbolColAdd exe_symboladd = new DNQuest.Executions.Exe_SymbolColAdd();
                        //                exe_symboladd.Set_Value(row_exe["exe_Text"].ToString(), row_exe["num"].ToString(), row_exe["onexe"].ToString(), row_exe["exe"].ToString());
                        //                qc.Add_Execution(exe_symboladd);
                        //                break;
                        //            case "huntall":
                        //                Executions.Exe_AllHunting exe_allhunt = new DNQuest.Executions.Exe_AllHunting();
                        //                exe_allhunt.Set_Value(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_allhunt);
                        //                break;
                        //            case "setDailyQuest":
                        //                Executions.Exe_AddDailyQuest exe_AddDailyQuest = new DNQuest.Executions.Exe_AddDailyQuest();
                        //                exe_AddDailyQuest.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                        //                qc.Add_Execution(exe_AddDailyQuest);
                        //                break;
                        //            case "setquest":
                        //                Executions.Exe_QuestAdd exe_questadd = new DNQuest.Executions.Exe_QuestAdd();
                        //                exe_questadd.Set_Value(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_questadd);
                        //                break;
                        //            case "msg":
                        //                Executions.Exe_Message exe_msg = new DNQuest.Executions.Exe_Message();
                        //                exe_msg.Set_Value(row_exe["exe_Text"].ToString(), row_exe["msg_type"].ToString(), row_exe["substring"].ToString());
                        //                qc.Add_Execution(exe_msg);
                        //                break;
                        //            case "talk":
                        //                Executions.Exe_Script exe_script = new DNQuest.Executions.Exe_Script();
                        //                exe_script.Set_Value(row_exe["npc"].ToString(), row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_script);
                        //                break;
                        //            case "random_coice":
                        //                Executions.Exe_ProbChoice exe_choice = new DNQuest.Executions.Exe_ProbChoice();
                        //                exe_choice.Set_Value(row_exe["rand"].ToString(), row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_choice);
                        //                break;
                        //            case "conexe":
                        //                Executions.Exe_Condition exe_condition = new DNQuest.Executions.Exe_Condition();
                        //                exe_condition.Set_Value(row_exe["exe_Text"].ToString(), row_exe["true"].ToString(), row_exe["false"].ToString());
                        //                qc.Add_Execution(exe_condition);
                        //                break;
                        //            case "moneyDelete":
                        //                Executions.Exe_MoneyDel exe_moneyDelete = new DNQuest.Executions.Exe_MoneyDel();
                        //                exe_moneyDelete.Set_Value(row_exe["num"].ToString(), row_exe["onexe"].ToString(), row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_moneyDelete);
                        //                break;
                        //            case "complete":
                        //                Executions.Exe_QuestComplete exe_questcomp = new DNQuest.Executions.Exe_QuestComplete();
                        //                exe_questcomp.Set_Value(row_exe["exe_Text"].ToString());
                        //                if (true == row_exe.Table.Columns.Contains("true"))
                        //                {
                        //                    exe_questcomp.Set_CompleteValue(row_exe["true"].ToString());
                        //                }
                        //                qc.Add_Execution(exe_questcomp);
                        //                break;
                        //            case "movie":
                        //                break;
                        //            case "delcount":
                        //                Executions.Exe_AddDel exe_alldel = new DNQuest.Executions.Exe_AddDel();
                        //                qc.Add_Execution(exe_alldel);
                        //                break;
                        //            case "custom":
                        //                Executions.Exe_Custum exe_custum = new DNQuest.Executions.Exe_Custum();
                        //                exe_custum.Set_Code(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_custum);
                        //                break;
                        //            case "return":
                        //                Executions.Exe_Return exe_return = new DNQuest.Executions.Exe_Return();
                        //                qc.Add_Execution(exe_return);
                        //                break;
                        //            case "fullinven":
                        //                Executions.Exe_FullInven exe_fullinven = new DNQuest.Executions.Exe_FullInven();
                        //                qc.Add_Execution(exe_fullinven);
                        //                break;
                        //            case "fullquest":
                        //                Executions.Exe_FullQuest exe_fullquest = new DNQuest.Executions.Exe_FullQuest();
                        //                qc.Add_Execution(exe_fullquest);
                        //                break;
                        //            case "recompense":
                        //                {
                        //                    Executions.Exe_Recompense exe_recompense = new DNQuest.Executions.Exe_Recompense();

                        //                    string exe_Text = row_exe["exe_Text"].ToString();
                        //                    if (exe_Text.IndexOf(',') > -1)
                        //                    {
                        //                        string[] classes = exe_Text.Split(',');
                        //                        exe_recompense.textBox_Index.Text = classes[0];
                        //                        exe_recompense.txtRewardWarrior.Text = classes[1];
                        //                        exe_recompense.txtRewardArcher.Text = classes[2];
                        //                        exe_recompense.txtRewardCleric.Text = classes[3];
                        //                        exe_recompense.txtRewardSorcerer.Text = classes[4];
                        //                        if (classes.Length > 5 )
                        //                            exe_recompense.txtRewardArcademic.Text = classes[5];
                        //                        if (classes.Length > 6)
                        //                            exe_recompense.txtRewardKally.Text = classes[6];
                        //                    }
                        //                    else
                        //                    {
                        //                        exe_recompense.textBox_Index.Text = exe_Text;
                        //                    }

                        //                    qc.Add_Execution(exe_recompense);
                        //                }
                        //                break;
                        //            case "recompense_view":
                        //                {
                        //                    Executions.Exe_Recompense_View exe_recompense_view = new DNQuest.Executions.Exe_Recompense_View();
                        //                    //exe_recompense_view.textBox_Index.Text = row["exe_Text"].ToString();
                        //                    string exe_Text = row_exe["exe_Text"].ToString();
                        //                    if (exe_Text.IndexOf(',') > -1)
                        //                    {
                        //                        string[] classes = exe_Text.Split(',');
                        //                        exe_recompense_view.textBox_Index.Text = classes[0];
                        //                        exe_recompense_view.txtRewardWarrior.Text = classes[1];
                        //                        exe_recompense_view.txtRewardArcher.Text = classes[2];
                        //                        exe_recompense_view.txtRewardCleric.Text = classes[3];
                        //                        exe_recompense_view.txtRewardSorcerer.Text = classes[4];
                        //                        if (classes.Length > 5)
                        //                            exe_recompense_view.txtRewardArcademic.Text = classes[5];
                        //                        if (classes.Length > 6)
                        //                            exe_recompense_view.txtRewardKally.Text = classes[6];
                        //                    }
                        //                    else
                        //                    {
                        //                        exe_recompense_view.textBox_Index.Text = exe_Text;
                        //                    }
                        //                    qc.Add_Execution(exe_recompense_view);
                        //                }
                        //                break;
                        //            case "comment":
                        //                Executions.Exe_Comment exe_comment = new DNQuest.Executions.Exe_Comment();
                        //                exe_comment.Set_Code(row_exe["exe_Text"].ToString());
                        //                qc.Add_Execution(exe_comment);
                        //                break;
                        //            case "checkStageClearRank":
                        //                Executions.Exe_CheckStageClearRank exe_CheckStageClearRank = new DNQuest.Executions.Exe_CheckStageClearRank();
                        //                exe_CheckStageClearRank.setValue(
                        //                    row_exe["num"].ToString()
                        //                , row_exe["rand"].ToString()
                        //                , row_exe["true"].ToString().Replace('[', '<').Replace(']', '>')
                        //                , row_exe["onexe"].ToString());
                        //                qc.Add_Execution(exe_CheckStageClearRank);
                        //                break;

                        //        }
                        //    }
                        //}
                        #endregion
                    }
                }
            }
            #endregion

        }

        private void _loadConditionObject(Quest_Condition qc, DataRow row_qstep)
        {
            DataTable dt_condition = loadedXmlDataSet.Tables["conditions"];

            if (dt_condition != null)
            {
                DataRow[] Row_condition = dt_condition.Select("qstep_change_Id = " + row_qstep["qstep_change_Id"]);

                string condition_id = Row_condition[0]["conditions_Id"].ToString();

                DataTable dt_chk = loadedXmlDataSet.Tables["chk"];
                DataRow[] Row_chk = dt_chk.Select("conditions_Id = " + condition_id);

                foreach (DataRow row in Row_chk)
                {
                    switch (row["type"].ToString())
                    {
                        case "lvl":
                            Conditions.Condition_Level con_level = new DNQuest.Conditions.Condition_Level();
                            con_level.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                            qc.Add_Condition(con_level);
                            break;
                        case "item":
                            Conditions.Condition_GeneralItem con_item = new DNQuest.Conditions.Condition_GeneralItem();
                            con_item.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                            qc.Add_Condition(con_item);
                            break;
                        case "symbol":
                            Conditions.Condition_SymbolItem con_symbol = new DNQuest.Conditions.Condition_SymbolItem();
                            con_symbol.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                            qc.Add_Condition(con_symbol);
                            break;
                        case "questitem":
                            Conditions.Condition_QuestItem con_qitem = new DNQuest.Conditions.Condition_QuestItem();
                            con_qitem.Set_Value(row["chk_Text"].ToString(), row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["num"].ToString());
                            qc.Add_Condition(con_qitem);
                            break;
                        case "class":
                            Conditions.Condition_Class con_class = new DNQuest.Conditions.Condition_Class();
                            con_class.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                            qc.Add_Condition(con_class);
                            break;
                        case "prob":
                            Conditions.Condition_Prob con_prob = new DNQuest.Conditions.Condition_Prob();
                            con_prob.Set_Value(row["chk_Text"].ToString());
                            qc.Add_Condition(con_prob);
                            break;
                        case "hasquest":
                            Conditions.Condition_Quest con_quest = new DNQuest.Conditions.Condition_Quest();
                            con_quest.Set_Value(row["chk_Text"].ToString());
                            qc.Add_Condition(con_quest);
                            break;
                        case "hascomq":
                            Conditions.Condition_Compelete con_comp = new DNQuest.Conditions.Condition_Compelete();
                            con_comp.Set_Value(row["chk_Text"].ToString());
                            qc.Add_Condition(con_comp);
                            break;
                        case "inven_eslot":
                            Conditions.Condition_Inven con_iven = new DNQuest.Conditions.Condition_Inven();
                            con_iven.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                            qc.Add_Condition(con_iven);
                            break;
                        case "custom":
                            Conditions.Condition_Custum con_custom = new DNQuest.Conditions.Condition_Custum();
                            con_custom.textBox_Code.Text = row["chk_Text"].ToString();
                            qc.Add_Condition(con_custom);
                            break;
                        case "qcount":
                            Conditions.Condition_QuestCnt con_qcnt = new DNQuest.Conditions.Condition_QuestCnt();
                            qc.Add_Condition(con_qcnt);
                            break;
                        case "stageConstructionLevel":
                            Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = new DNQuest.Conditions.Condition_StageConstructionLevel();
                            con_stageConstructionLevel.Set_Value(row["op"].ToString().Replace('[', '<').Replace(']', '>'), row["chk_Text"].ToString());
                            qc.Add_Condition(con_stageConstructionLevel);
                            break;
                    }
                }
            }
        }

        private void _loadExecutionObject(Quest_Condition qc, DataRow roe_qstep)
        {
            DataTable dt_execution = loadedXmlDataSet.Tables["executions"];
            if (dt_execution != null)
            {
                DataRow[] Row_execution = dt_execution.Select("qstep_change_Id = " + roe_qstep["qstep_change_Id"]);

                string execuition_id = Row_execution[0]["executions_Id"].ToString();

                DataTable dt_exe = loadedXmlDataSet.Tables["exe"];
                DataRow[] Row_exe = dt_exe.Select("executions_Id = " + execuition_id);

                foreach (DataRow row in Row_exe)
                {
                    switch (row["type"].ToString())
                    {
                        case "setstep":
                            Executions.Exe_QuestStepMod exe_qsm = new DNQuest.Executions.Exe_QuestStepMod();
                            exe_qsm.Set_Step(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_qsm);
                            break;
                        case "setjornal":
                            Executions.Exe_JournalMod exe_journal = new DNQuest.Executions.Exe_JournalMod();
                            exe_journal.Set_Journal(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_journal);
                            break;
                        case "delitem":
                            Executions.Exe_ItemDel exe_itemdel = new DNQuest.Executions.Exe_ItemDel();
                            exe_itemdel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_itemdel);
                            break;
                        case "delsymbol":
                            Executions.Exe_SymbolDel exe_symdel = new DNQuest.Executions.Exe_SymbolDel();
                            exe_symdel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_symdel);
                            break;
                        case "delquestitem":
                            Executions.Exe_QuestItemDel exe_qidel = new DNQuest.Executions.Exe_QuestItemDel();
                            exe_qidel.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_qidel);
                            break;
                        case "additem":
                            Executions.Exe_ItemAdd exe_itemadd = new DNQuest.Executions.Exe_ItemAdd();
                            exe_itemadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_itemadd);
                            break;
                        case "addsymbol":
                            Executions.Exe_SymbolAdd exe_symadd = new DNQuest.Executions.Exe_SymbolAdd();
                            exe_symadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_symadd);
                            break;
                        case "addquestitem":
                            Executions.Exe_QuestItemAdd exe_qiadd = new DNQuest.Executions.Exe_QuestItemAdd();
                            exe_qiadd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString());
                            qc.Add_Execution(exe_qiadd);
                            break;
                        case "huntmon":
                            Executions.Exe_MonsterAdd exe_monadd = new DNQuest.Executions.Exe_MonsterAdd();
                            exe_monadd.Set_Value(row["exe_Text"].ToString(), row["party"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                            qc.Add_Execution(exe_monadd);
                            break;
                        case "huntitem":
                            Executions.Exe_ItemColAdd exe_itemcoladd = new DNQuest.Executions.Exe_ItemColAdd();
                            exe_itemcoladd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                            qc.Add_Execution(exe_itemcoladd);
                            break;
                        case "huntsymbol":
                            Executions.Exe_SymbolColAdd exe_symboladd = new DNQuest.Executions.Exe_SymbolColAdd();
                            exe_symboladd.Set_Value(row["exe_Text"].ToString(), row["num"].ToString(), row["onexe"].ToString(), row["exe"].ToString());
                            qc.Add_Execution(exe_symboladd);
                            break;
                        case "huntall":
                            Executions.Exe_AllHunting exe_allhunt = new DNQuest.Executions.Exe_AllHunting();
                            exe_allhunt.Set_Value(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_allhunt);
                            break;
                        case "setDailyQuest":
                            Executions.Exe_AddDailyQuest exe_AddDailyQuest = new DNQuest.Executions.Exe_AddDailyQuest();
                            exe_AddDailyQuest.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                            qc.Add_Execution(exe_AddDailyQuest);
                            break;
                        case "setquest":
                            Executions.Exe_QuestAdd exe_questadd = new DNQuest.Executions.Exe_QuestAdd();
                            exe_questadd.Set_Value(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_questadd);
                            break;
                        case "msg":
                            Executions.Exe_Message exe_msg = new DNQuest.Executions.Exe_Message();
                            exe_msg.Set_Value(row["exe_Text"].ToString(), row["msg_type"].ToString(), row["substring"].ToString());
                            qc.Add_Execution(exe_msg);
                            break;
                        case "talk":
                            Executions.Exe_Script exe_script = new DNQuest.Executions.Exe_Script();
                            exe_script.Set_Value(row["npc"].ToString(), row["exe_Text"].ToString());
                            qc.Add_Execution(exe_script);
                            break;
                        case "random_coice":
                            Executions.Exe_ProbChoice exe_choice = new DNQuest.Executions.Exe_ProbChoice();
                            exe_choice.Set_Value(row["rand"].ToString(), row["exe_Text"].ToString());
                            qc.Add_Execution(exe_choice);
                            break;
                        case "conexe":
                            Executions.Exe_Condition exe_condition = new DNQuest.Executions.Exe_Condition();
                            exe_condition.Set_Value(row["exe_Text"].ToString(), row["true"].ToString(), row["false"].ToString());
                            qc.Add_Execution(exe_condition);
                            break;
                        case "moneyDelete":
                            Executions.Exe_MoneyDel exe_moneyDelete = new DNQuest.Executions.Exe_MoneyDel();
                            exe_moneyDelete.Set_Value(row["num"].ToString(), row["onexe"].ToString(), row["exe_Text"].ToString());
                            qc.Add_Execution(exe_moneyDelete);
                            break;
                        case "complete":
                            Executions.Exe_QuestComplete exe_questcomp = new DNQuest.Executions.Exe_QuestComplete();
                            exe_questcomp.Set_Value(row["exe_Text"].ToString());
                            if (true == row.Table.Columns.Contains("true"))
                            {
                                exe_questcomp.Set_CompleteValue(row["true"].ToString());
                            }
                            qc.Add_Execution(exe_questcomp);
                            break;
                        case "movie":
                            break;
                        case "delcount":
                            Executions.Exe_AddDel exe_alldel = new DNQuest.Executions.Exe_AddDel();
                            qc.Add_Execution(exe_alldel);
                            break;
                        case "custom":
                            Executions.Exe_Custum exe_custum = new DNQuest.Executions.Exe_Custum();
                            exe_custum.Set_Code(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_custum);
                            break;
                        case "return":
                            Executions.Exe_Return exe_return = new DNQuest.Executions.Exe_Return();
                            qc.Add_Execution(exe_return);
                            break;
                        case "fullinven":
                            Executions.Exe_FullInven exe_fullinven = new DNQuest.Executions.Exe_FullInven();
                            qc.Add_Execution(exe_fullinven);
                            break;
                        case "fullquest":
                            Executions.Exe_FullQuest exe_fullquest = new DNQuest.Executions.Exe_FullQuest();
                            qc.Add_Execution(exe_fullquest);
                            break;
                        case "recompense":
                            {
                                Executions.Exe_Recompense exe_recompense = new DNQuest.Executions.Exe_Recompense();

                                string exe_Text = row["exe_Text"].ToString();
                                if (exe_Text.IndexOf(',') > -1)
                                {
                                    string[] classes = exe_Text.Split(',');
                                    exe_recompense.textBox_Index.Text = classes[0];
                                    exe_recompense.txtRewardWarrior.Text = classes[1];
                                    exe_recompense.txtRewardArcher.Text = classes[2];
                                    exe_recompense.txtRewardCleric.Text = classes[3];
                                    exe_recompense.txtRewardSorcerer.Text = classes[4];
                                    if (classes.Length > 5)
                                        exe_recompense.txtRewardArcademic.Text = classes[5];
                                    if (classes.Length > 6)
                                        exe_recompense.txtRewardKally.Text = classes[6];
                                    if (classes.Length > 7)
                                        exe_recompense.txtRewardAssassin.Text = classes[7];
                                }
                                else
                                {
                                    exe_recompense.textBox_Index.Text = exe_Text;
                                }

                                qc.Add_Execution(exe_recompense);
                            }
                            break;
                        case "recompense_view":
                            {
                                Executions.Exe_Recompense_View exe_recompense_view = new DNQuest.Executions.Exe_Recompense_View();
                                //exe_recompense_view.textBox_Index.Text = row["exe_Text"].ToString();
                                string exe_Text = row["exe_Text"].ToString();
                                if (exe_Text.IndexOf(',') > -1)
                                {
                                    string[] classes = exe_Text.Split(',');
                                    exe_recompense_view.textBox_Index.Text = classes[0];
                                    exe_recompense_view.txtRewardWarrior.Text = classes[1];
                                    exe_recompense_view.txtRewardArcher.Text = classes[2];
                                    exe_recompense_view.txtRewardCleric.Text = classes[3];
                                    exe_recompense_view.txtRewardSorcerer.Text = classes[4];
                                    if (classes.Length > 5)
                                        exe_recompense_view.txtRewardArcademic.Text = classes[5];
                                    if (classes.Length > 6)
                                        exe_recompense_view.txtRewardKally.Text = classes[6];
                                    if (classes.Length > 7)
                                        exe_recompense_view.txtRewardAssassin.Text = classes[7];

                                }
                                else
                                {
                                    exe_recompense_view.textBox_Index.Text = exe_Text;
                                }
                                qc.Add_Execution(exe_recompense_view);
                            }
                            break;
                        case "comment":
                            Executions.Exe_Comment exe_comment = new DNQuest.Executions.Exe_Comment();
                            exe_comment.Set_Code(row["exe_Text"].ToString());
                            qc.Add_Execution(exe_comment);
                            break;
                        case "checkStageClearRank":
                            Executions.Exe_CheckStageClearRank exe_CheckStageClearRank = new DNQuest.Executions.Exe_CheckStageClearRank();
                            exe_CheckStageClearRank.setValue(
                                row["num"].ToString()
                                , row["rand"].ToString()
                                , row["true"].ToString().Replace('[', '<').Replace(']', '>')
                                , row["onexe"].ToString());
                            qc.Add_Execution(exe_CheckStageClearRank);
                            break;
                    }
                }
            }
        }

        #endregion // XML 파일 로드

        #region XML 파일 저장

        private void SaveXML(string p_path)
        {            
            savingDataSet = _makeDataSet();

            savingDataSet.DataSetName = "quest";             
            
            #region 퀘스트 기본정보
            DataRow row_Basic = savingDataSet.Tables["quest_desc"].NewRow();            

            row_Basic["qename"] = textBox_EngName.Text;

            if (comboBox_Type.SelectedIndex == 0)       row_Basic["qtype"] = "mainquest";
            else if (comboBox_Type.SelectedIndex == 1)  row_Basic["qtype"] = "subquest";
            else if (comboBox_Type.SelectedIndex == 2)  row_Basic["qtype"] = "venturerquest";
            else if (comboBox_Type.SelectedIndex == 4) row_Basic["qtype"] = "favorMalicequest";
            else if (comboBox_Type.SelectedIndex == 5)  row_Basic["qtype"] = "dayquest";
            else if (comboBox_Type.SelectedIndex == 6)
            {
                row_Basic["qtype"] = "customquest";
                row_Basic["custommark"] = textBox_Questmark.Text + "," + textBox_Statemark0.Text + "," + textBox_Statemark1.Text;
            }

            if (comboBox_CompleteType.SelectedIndex == 0) row_Basic["qcomtype"] = "ONCE";
            else row_Basic["qcomtype"] = "REPEAT";

            row_Basic["qchapter"] = textBox_Chapter.Text;
            row_Basic["qid"] = textBox_QuestID.Text;
            row_Basic["designer"] = textBox_Damdang.Text;
            row_Basic["qlevel"] = textBox_StartLevel.Text;
            row_Basic["qdesc"] = textBox_Abstrct.Text;
            row_Basic["qdifficulty"] = textBox_QuestDifficulty.Text;
            row_Basic["nextqid"] = textBox_NextID.Text;
            row_Basic["nextqname"] = textBox_NextEname.Text;
            row_Basic["nextqnpc"] = textBox_NextNPCEname.Text;
            row_Basic["quest_desc_Id"] = 0;
            row_Basic["uistyle"] = comboBox_UIStyle.SelectedIndex.ToString(); // 0: Basic, 1: Renewal
            
            savingDataSet.Tables["quest_desc"].Rows.Add(row_Basic);

            DataRow row_Condition = savingDataSet.Tables["condition_data"].NewRow();

            row_Condition["user_level"] = textBox_levelconditon.Text;
            row_Condition["user_worldzone_level"] = textBox_worldcondition.Text;
            row_Condition["user_class"] = textBox_classcondition.Text;
            row_Condition["prev_quest"] = textBox_prevquestcondition.Text;
            row_Condition["have_quest"] = textBox_optionalQuestcondition.Text;
            row_Condition["have_normal_item"] = textBox_itemcondition.Text;
            row_Condition["complete_mission"] = textBox_UserMissionClearCondition.Text;
            row_Condition["have_symbol_item"] = textBox_symbolcondition.Text;
            row_Condition["have_cash_item"] = textBox_cashitemcondition.Text;
            if (this.txtPeriodConditionOnDayQuest.Enabled == true)
            {
                row_Condition["quest_date"] =
                    string.IsNullOrEmpty(this.txtPeriodConditionOnDayQuest.Text) == true ? "" : this.txtPeriodConditionOnDayQuest.Text;
            }

            if (this.chkBoxMonday.Enabled == true) {
                if (chkBoxMonday.Checked == true)           row_Condition["quest_day"] = 1;
                else if (chkBoxTuesday.Checked == true)     row_Condition["quest_day"] = 2;
                else if (chkBoxWednesDay.Checked == true)   row_Condition["quest_day"] = 3;
                else if (chkBoxThursday.Checked == true)    row_Condition["quest_day"] = 4;
                else if (chkBoxFriday.Checked == true)      row_Condition["quest_day"] = 5;
                else if (chkBoxSaturday.Checked == true)    row_Condition["quest_day"] = 6;
                else if (chkBoxSunday.Checked == true)      row_Condition["quest_day"] = 7;
                else                                        row_Condition["quest_day"] = "";
            }

            row_Condition["npc_favor"] = string.IsNullOrEmpty(this.txtNpc_favor.Text) ? "" : this.txtNpc_favor.Text;
            row_Condition["npc_malice"] = string.IsNullOrEmpty(this.txtNpc_malice.Text) ? "" : this.txtNpc_malice.Text;

            savingDataSet.Tables["condition_data"].Rows.Add(row_Condition);

            #endregion

            #region 퀘스트 이름 정보 저장
            DataRow row_qkname = savingDataSet.Tables["qkname"].NewRow();

            if (textBox_KorName.Tag == null)
            {
                row_qkname["mid"] = 1;
                textBox_KorName.Tag = 1;                
            }
            else
            {
                row_qkname["mid"] = textBox_KorName.Tag.ToString();
                //Quest_Mid = Convert.ToInt32(textBox_KorName.Tag.ToString());
            }

            row_qkname["qkname_Text"] = "<![CDATA[" + textBox_KorName.Text + "]]>";
            row_qkname["quest_desc_Id"] = 0;

            savingDataSet.Tables["qkname"].Rows.Add(row_qkname);
            
            #endregion

            #region NPC 정보

            foreach (object item in listBox_NPC.Items)
            {
                DataRow row_NPC = savingDataSet.Tables["qnpc"].NewRow();

                string[] item_array = item.ToString().Split(',');

                row_NPC["npcid"] = item_array[0];
                row_NPC["npcename"] = item_array[1];
                row_NPC["npckname"] = item_array[2];
                row_NPC["quest_desc_Id"] = 0;

                savingDataSet.Tables["qnpc"].Rows.Add(row_NPC);
            }

            #endregion

            #region 대사 정보
            ComboBox cmb_culture = (ComboBox)ParentForm.Controls.Find("comboBox_Culture", true)[0];

            DataRow row_talk_enter = savingDataSet.Tables["talk_paragraph"].NewRow();            
            row_talk_enter["index"] = "q_enter";
            row_talk_enter["talk_paragraph_Id"] = "q_enter";
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_enter);

            DataRow row_talk_quit = savingDataSet.Tables["talk_paragraph"].NewRow();            
            row_talk_quit["index"] = "!quit";
            row_talk_quit["talk_paragraph_Id"] = "!quit";
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_quit);

            DataRow row_talk_noquest = savingDataSet.Tables["talk_paragraph"].NewRow();
            row_talk_noquest["index"] = "_no_quest";
            row_talk_noquest["talk_paragraph_Id"] = "_no_quest";
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_noquest);

            //번역 관련 추가
            DataRow row_talk_noquest_question = savingDataSet.Tables["talk_question"].NewRow();            
            row_talk_noquest_question["mid"] = 2;
            row_talk_noquest_question["talk_paragraph_Id"] = "_no_quest";
            row_talk_noquest_question["talk_question_Text"] = "<![CDATA[<html>{ui_string:1200001}</html>]]>";
            savingDataSet.Tables["talk_question"].Rows.Add(row_talk_noquest_question);
            // 번역관련 추가

            DataRow row_talk_fullquest = savingDataSet.Tables["talk_paragraph"].NewRow();
            row_talk_fullquest["index"] = "_full_quest";
            row_talk_fullquest["talk_paragraph_Id"] = "_full_quest";            
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_fullquest);

            //번역 관련 추가
            DataRow row_talk_fullquest_question = savingDataSet.Tables["talk_question"].NewRow();
            row_talk_fullquest_question["mid"] = 3;
            row_talk_fullquest_question["talk_paragraph_Id"] = "_full_quest";
            row_talk_fullquest_question["talk_question_Text"] = "<![CDATA[<html>{ui_string:1200002}</html>]]>";
            savingDataSet.Tables["talk_question"].Rows.Add(row_talk_fullquest_question);
            // 번역관련 추가

            DataRow row_talk_inven = savingDataSet.Tables["talk_paragraph"].NewRow();
            row_talk_inven["index"] = "_full_inventory";
            row_talk_inven["talk_paragraph_Id"] = "_full_inventory";            
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_inven);

            //번역 관련 추가
            DataRow row_talk_inven_question = savingDataSet.Tables["talk_question"].NewRow();
            row_talk_inven_question["mid"] = 4;       
            row_talk_inven_question["talk_paragraph_Id"] = "_full_inventory";
            row_talk_inven_question["talk_question_Text"] = "<![CDATA[<html>{ui_string:1200003}</html>]]>";
            savingDataSet.Tables["talk_question"].Rows.Add(row_talk_inven_question);
            // 번역관련 추가

            //daily quest fail
            DataRow row_daily_quest_fail = savingDataSet.Tables["talk_paragraph"].NewRow();
            row_daily_quest_fail["index"] = "_daily_quest_fail";
            row_daily_quest_fail["talk_paragraph_Id"] = "_daily_quest_fail";
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_daily_quest_fail);

            DataRow rowDailyQuestFail = savingDataSet.Tables["talk_question"].NewRow();
            rowDailyQuestFail["mid"] = 99998;
            rowDailyQuestFail["talk_paragraph_Id"] = "_daily_quest_fail";
            rowDailyQuestFail["talk_question_Text"] = "<![CDATA[<html>{ui_string:1200004}</html>]]>";
            savingDataSet.Tables["talk_question"].Rows.Add(rowDailyQuestFail);

            DataRow row_talk_Qinven = savingDataSet.Tables["talk_paragraph"].NewRow();
            row_talk_Qinven["index"] = "_full_Qinventory";
            row_talk_Qinven["talk_paragraph_Id"] = "_full_Qinventory";
            savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk_Qinven);

            //번역 관련 추가
            DataRow row_talk_Qinven_question = savingDataSet.Tables["talk_question"].NewRow();
            row_talk_Qinven_question["mid"] = 99999;
            row_talk_Qinven_question["talk_paragraph_Id"] = "_full_Qinventory";
            row_talk_Qinven_question["talk_question_Text"] = "<![CDATA[<html>{ui_string:1200005}</html>]]>";
            savingDataSet.Tables["talk_question"].Rows.Add(row_talk_Qinven_question);
            // 번역관련 추가

            if (Quest_Mid < 5)  Quest_Mid = 5;
            else                ++Quest_Mid;            

            #region 퀘스트 없을 때 대사 정보
            foreach (Control control in tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls["unit_init"].Controls)
            {
                if (control.Name.IndexOf("npc_") > -1)
                {
                    Quest_NPC npc = (Quest_NPC)control;

                    if (npc.textBox_NPCName.Text == "")
                    {
                        continue;
                    }

                    Quest_Step[] steps = npc.Get_Step();

                    foreach (Quest_Step step in steps)
                    {
                        if (step != null)
                        {
                            if (step.Get_StepNo() == "단계시작")
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-noquest";
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest";
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();
                                    
                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                }
                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest";

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                if (npc.textBox_ExecuteNo.Text != "")
                                {
                                    DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                    row_exelink["qstep"] = "noquest";
                                    row_exelink["npc"] = npc.Get_NPCName();
                                    row_exelink["exelink_Text"] = npc.textBox_ExecuteNo.Text;
                                    savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                }

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest";
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();
                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                            {
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                            }
                                        }
                                        
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-noquest" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "noquest";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-noquest-" + step.Get_StepNo();
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                //row_talk["talk_question"] = step.Get_Script();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest-" + step.Get_StepNo();
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();
                                                                
                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();
                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                    {
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                    }
                                }
                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest-" + step.Get_StepNo();

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-noquest-" + step.Get_StepNo();
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();
                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                            {
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                            }
                                        }
                                       
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-noquest" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "noquest";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }
                                    }
                                }
                            }                            
                        }
                    }
                }
            }
            #endregion

            #region 퀘스트 받기 단계 정보
            foreach (Control control in tabPage_Accept.Controls["panel_TabMainAccept"].Controls["unit_init"].Controls)
            {
                if (control.Name.IndexOf("npc_") > -1)
                {
                    Quest_NPC npc = (Quest_NPC)control;

                    if (npc.textBox_NPCName.Text == "")
                    {
                        continue;
                    }

                    Quest_Step[] steps = npc.Get_Step();

                    foreach (Quest_Step step in steps)
                    {
                        if (step != null)
                        {
                            if (step.Get_StepNo() == "단계시작")
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-accepting";
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting";
                                //row_talk["talk_question"] = step.Get_Script();
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();
                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                    {
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                    }
                                }
                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting";

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                if (npc.textBox_ExecuteNo.Text != "")
                                {
                                    DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                    row_exelink["qstep"] = "accepting"; 
                                    row_exelink["npc"] = npc.Get_NPCName();
                                    row_exelink["exelink_Text"] = npc.textBox_ExecuteNo.Text;
                                    savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                }

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting";
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();
                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                            {
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                            }
                                        }
                                        
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-accepting" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        //Accepted 및 !next 임시처리 시작 (2009.01.22)
                                        //if (select.Get_Target() == "acceptted" || select.Get_Target() == "!next")
                                        //{
                                        //    DataRow row_temp = ds_save.Tables["talk_paragraph"].NewRow();

                                        //    row_temp["index"] = npc.Get_NPCName() + "-accepting" + "-" + select.Get_Target();
                                        //    row_temp["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting" + "-" + select.Get_Target();                                            

                                        //    ds_save.Tables["talk_paragraph"].Rows.Add(row_temp);
                                        //}
                                        //Accepted 및 !next 임시처리 끝 (2009.01.22)

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "accepting";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }                                        
                                    }
                                }
                            }
                            else
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-accepting-" + step.Get_StepNo();
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting-" + step.Get_StepNo();
                                //row_talk["talk_question"] = step.Get_Script();
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();
                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                    {
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                    }
                                }
                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting-" + step.Get_StepNo();

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-accepting-" + step.Get_StepNo();
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();
                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                            {
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                            }
                                        }
                                        
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-accepting" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "accepting";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }                                       
                                    }
                                }
                            }
                        }
                    }
                }
            }

            foreach (Control control in tabPage_Accept.Controls["panel_TabMainAccept"].Controls)
            {
                if (control.Name.IndexOf("condition_") > -1)
                {                    
                    DataRow row_change = savingDataSet.Tables["qstep_change"].NewRow();

                    row_change["qstep"] = "accepting";
                    row_change["qstep_change_Id"] = "accepting";
                    row_change["comment"] = control.Controls["groupBox_Detail"].Controls["textBox_Detail"].Text;

                    savingDataSet.Tables["qstep_change"].Rows.Add(row_change);

                    DataRow row_executions = savingDataSet.Tables["executions"].NewRow();

                    row_executions["executions_Id"] = "accepting";
                    row_executions["qstep_change_Id"] = "accepting";

                    savingDataSet.Tables["executions"].Rows.Add(row_executions);

                    DataRow row_conditions = savingDataSet.Tables["conditions"].NewRow();

                    row_conditions["conditions_Id"] = "accepting";
                    row_conditions["qstep_change_Id"] = "accepting";

                    savingDataSet.Tables["conditions"].Rows.Add(row_conditions);

                    _saveConditionObject(control.Controls["groupBox_Condition"].Controls, "accepting");
                    _saveExecutionObject(control.Controls["groupBox_Execution"].Controls, "accepting");

                    #region 조건문
                    //foreach (Control con in control.Controls["groupBox_Condition"].Controls)
                    //{
                    //    switch (con.Name)
                    //    {
                    //        case "Condition_Level":
                    //            Conditions.Condition_Level con_level = (Conditions.Condition_Level)con;
                    //            DataRow row_level = savingDataSet.Tables["chk"].NewRow();
                    //            row_level["type"] = "lvl";
                    //            row_level["chk_Text"] = con_level.textBox_Value.Text;
                    //            row_level["op"] = con_level.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_level["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_level);
                    //            break;
                    //        case "Condition_GeneralItem":
                    //            Conditions.Condition_GeneralItem con_item = (Conditions.Condition_GeneralItem)con;
                    //            DataRow row_item = savingDataSet.Tables["chk"].NewRow();
                    //            row_item["type"] = "item";
                    //            row_item["chk_Text"] = con_item.textBox_Item.Text;
                    //            row_item["num"] = con_item.textBox_Value.Text;
                    //            row_item["op"] = con_item.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_item["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_item);
                    //            break;
                    //        case "Condition_SymbolItem":
                    //            Conditions.Condition_SymbolItem con_symbol = (Conditions.Condition_SymbolItem)con;
                    //            DataRow row_symbol = savingDataSet.Tables["chk"].NewRow();

                    //            int index_chksymbol = 0;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //            {
                    //                row_symbol["type"] = "questitem";
                    //                index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 300000;
                    //            }
                    //            else
                    //            {
                    //                row_symbol["type"] = "questitem";
                    //                index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 400000;
                    //            }


                    //            row_symbol["chk_Text"] = index_chksymbol.ToString();
                    //            row_symbol["num"] = con_symbol.textBox_Value.Text;
                    //            row_symbol["op"] = con_symbol.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_symbol["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_symbol);
                    //            break;
                    //        case "Condition_QuestItem":
                    //            Conditions.Condition_QuestItem con_qitem = (Conditions.Condition_QuestItem)con;
                    //            DataRow row_qitem = savingDataSet.Tables["chk"].NewRow();
                    //            row_qitem["type"] = "questitem";
                    //            row_qitem["chk_Text"] = con_qitem.textBox_Item.Text;
                    //            row_qitem["num"] = con_qitem.textBox_Value.Text;
                    //            row_qitem["op"] = con_qitem.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_qitem["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_qitem);
                    //            break;
                    //        case "Condition_Class":
                    //            Conditions.Condition_Class con_class = (Conditions.Condition_Class)con;
                    //            DataRow row_class = savingDataSet.Tables["chk"].NewRow();
                    //            row_class["type"] = "class";
                    //            row_class["chk_Text"] = con_class.textBox_Value.Text;
                    //            row_class["op"] = con_class.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_class["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_class);
                    //            break;
                    //        case "Condition_Prob":
                    //            Conditions.Condition_Prob con_prob = (Conditions.Condition_Prob)con;
                    //            DataRow row_prob = savingDataSet.Tables["chk"].NewRow();
                    //            row_prob["type"] = "prob";
                    //            row_prob["chk_Text"] = con_prob.textBox_Value.Text;
                    //            row_prob["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_prob);
                    //            break;
                    //        case "Condition_Quest":
                    //            Conditions.Condition_Quest con_quest = (Conditions.Condition_Quest)con;
                    //            DataRow row_quest = savingDataSet.Tables["chk"].NewRow();
                    //            row_quest["type"] = "hasquest";
                    //            row_quest["chk_Text"] = con_quest.textBox_Value.Text;
                    //            row_quest["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_quest);
                    //            break;
                    //        case "Condition_Compelete":
                    //            Conditions.Condition_Compelete con_comp = (Conditions.Condition_Compelete)con;
                    //            DataRow row_comp = savingDataSet.Tables["chk"].NewRow();
                    //            row_comp["type"] = "hascomq";
                    //            row_comp["chk_Text"] = con_comp.textBox_Value.Text;
                    //            row_comp["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_comp);
                    //            break;
                    //        case "Condition_Inven":
                    //            Conditions.Condition_Inven con_inven = (Conditions.Condition_Inven)con;
                    //            DataRow row_inven = savingDataSet.Tables["chk"].NewRow();
                    //            row_inven["type"] = "inven_eslot";
                    //            row_inven["chk_Text"] = con_inven.textBox_Value.Text;
                    //            row_inven["op"] = con_inven.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_inven["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_inven);
                    //            break;
                    //        case "Condition_Custum":
                    //            Conditions.Condition_Custum con_custum = (Conditions.Condition_Custum)con;
                    //            DataRow row_custom = savingDataSet.Tables["chk"].NewRow();
                    //            row_custom["type"] = "custom";
                    //            row_custom["chk_Text"] = con_custum.textBox_Code.Text;
                    //            row_custom["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_custom);
                    //            break;
                    //        case "Condition_QuestCnt":
                    //            Conditions.Condition_QuestCnt con_qcnt = (Conditions.Condition_QuestCnt)con;
                    //            DataRow row_qcnt = savingDataSet.Tables["chk"].NewRow();
                    //            row_qcnt["type"] = "qcount";
                    //            row_qcnt["chk_Text"] = ".";
                    //            row_qcnt["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_qcnt);
                    //            break;
                    //        case "Condition_StageConstructionLevel":
                    //            Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = (Conditions.Condition_StageConstructionLevel)con;
                    //            DataRow row_stageConstructionLevel = savingDataSet.Tables["chk"].NewRow();
                    //            row_stageConstructionLevel["type"] = "stageConstructionLevel";
                    //            row_stageConstructionLevel["chk_Text"] = con_stageConstructionLevel.textBox_Value.Text;
                    //            row_stageConstructionLevel["op"] = con_stageConstructionLevel.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_stageConstructionLevel["conditions_Id"] = "accepting";
                    //            savingDataSet.Tables["chk"].Rows.Add(row_stageConstructionLevel);
                    //            break;

                    //    }
                    //}
                    #endregion

                    #region 실행문
                    //foreach (Control con in control.Controls["groupBox_Execution"].Controls)
                    //{
                    //    switch (con.Name)
                    //    {
                    //        case "Exe_QuestStepMod":
                    //            Executions.Exe_QuestStepMod eq = (Executions.Exe_QuestStepMod)con;
                    //            DataRow row_exe = savingDataSet.Tables["exe"].NewRow();
                    //            row_exe["type"] = "setstep";
                    //            row_exe["exe_Text"] = eq.textBox_Step.Text;
                    //            row_exe["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_exe);
                    //            break;
                    //        case "Exe_JournalMod":
                    //            Executions.Exe_JournalMod exe_journal = (Executions.Exe_JournalMod)con;
                    //            DataRow row_journal = savingDataSet.Tables["exe"].NewRow();
                    //            row_journal["type"] = "setjornal";
                    //            row_journal["exe_Text"] = exe_journal.textBox_Journal.Text;
                    //            row_journal["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_journal);
                    //            break;
                    //        case "Exe_ItemDel":
                    //            Executions.Exe_ItemDel exe_itemdel = (Executions.Exe_ItemDel)con;
                    //            DataRow row_itemdel = savingDataSet.Tables["exe"].NewRow();
                    //            row_itemdel["type"] = "delitem";
                    //            row_itemdel["exe_Text"] = exe_itemdel.textBox_Item.Text;
                    //            row_itemdel["num"] = exe_itemdel.textBox_Count.Text;
                    //            row_itemdel["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_itemdel);
                    //            break;
                    //        case "Exe_SymbolDel":
                    //            Executions.Exe_SymbolDel exe_symdel = (Executions.Exe_SymbolDel)con;
                    //            DataRow row_symdel = savingDataSet.Tables["exe"].NewRow();

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //                row_symdel["type"] = "delquestitem";
                    //            else
                    //                row_symdel["type"] = "delitem";

                    //            int index_delsymbol = 0;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //                index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 300000;
                    //            else
                    //                index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 400000;

                    //            row_symdel["exe_Text"] = index_delsymbol.ToString();
                    //            row_symdel["num"] = exe_symdel.textBox_Count.Text;
                    //            row_symdel["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_symdel);
                    //            break;
                    //        case "Exe_QuestItemDel":
                    //            Executions.Exe_QuestItemDel exe_qidel = (Executions.Exe_QuestItemDel)con;
                    //            DataRow row_qidel = savingDataSet.Tables["exe"].NewRow();
                    //            row_qidel["type"] = "delquestitem";
                    //            row_qidel["exe_Text"] = exe_qidel.textBox_Item.Text;
                    //            row_qidel["num"] = exe_qidel.textBox_Count.Text;
                    //            row_qidel["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_qidel);
                    //            break;
                    //        case "Exe_ItemAdd":
                    //            Executions.Exe_ItemAdd exe_itemadd = (Executions.Exe_ItemAdd)con;
                    //            DataRow row_itemadd = savingDataSet.Tables["exe"].NewRow();
                    //            row_itemadd["type"] = "additem";
                    //            row_itemadd["exe_Text"] = exe_itemadd.textBox_Item.Text;
                    //            row_itemadd["num"] = exe_itemadd.textBox_Count.Text;
                    //            row_itemadd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_itemadd);
                    //            break;
                    //        case "Exe_SymbolAdd":
                    //            Executions.Exe_SymbolAdd exe_symadd = (Executions.Exe_SymbolAdd)con;
                    //            DataRow row_symadd = savingDataSet.Tables["exe"].NewRow();

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //                row_symadd["type"] = "addquestitem";
                    //            else
                    //                row_symadd["type"] = "additem";

                    //            int index_addsymbol = 0;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //                index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 300000;
                    //            else
                    //                index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 400000;

                    //            row_symadd["exe_Text"] = index_addsymbol.ToString();
                    //            row_symadd["num"] = exe_symadd.textBox_Count.Text;
                    //            row_symadd["executions_Id"] = "accepting";
                    //            break;
                    //        case "Exe_QuestItemAdd":
                    //            Executions.Exe_QuestItemAdd exe_qiadd = (Executions.Exe_QuestItemAdd)con;
                    //            DataRow row_qiadd = savingDataSet.Tables["exe"].NewRow();
                    //            row_qiadd["type"] = "addquestitem";
                    //            row_qiadd["exe_Text"] = exe_qiadd.textBox_Item.Text;
                    //            row_qiadd["num"] = exe_qiadd.textBox_Count.Text;
                    //            row_qiadd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_qiadd);
                    //            break;
                    //        case "Exe_MonsterAdd":
                    //            Executions.Exe_MonsterAdd exe_monadd = (Executions.Exe_MonsterAdd)con;
                    //            DataRow row_monadd = savingDataSet.Tables["exe"].NewRow();
                    //            row_monadd["type"] = "huntmon";
                    //            row_monadd["exe_Text"] = exe_monadd.textBox_MonID.Text;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //            {
                    //                try
                    //                {
                    //                    row_monadd["num"] = exe_monadd.textBox_Count.Text;
                    //                }
                    //                catch (Exception)
                    //                {
                    //                    MessageBox.Show("몬스터의 마리 수를 입력해 주십시오.");
                    //                    exe_monadd.textBox_Count.Focus();
                    //                }
                    //            }
                    //            else
                    //            {
                    //                row_monadd["num"] = exe_monadd.textBox_Count.Text;
                    //            }

                    //            row_monadd["party"] = exe_monadd.textBox_Party.Text;
                    //            row_monadd["onexe"] = exe_monadd.textBox_OnExe.Text;
                    //            row_monadd["exe"] = exe_monadd.textBox_ExeComp.Text;
                    //            row_monadd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_monadd);
                    //            break;
                    //        case "Exe_ItemColAdd":
                    //            Executions.Exe_ItemColAdd exe_itemcoladd = (Executions.Exe_ItemColAdd)con;
                    //            DataRow row_itemcoladd = savingDataSet.Tables["exe"].NewRow();
                    //            row_itemcoladd["type"] = "huntitem";
                    //            row_itemcoladd["exe_Text"] = exe_itemcoladd.textBox_Item.Text;
                    //            row_itemcoladd["num"] = exe_itemcoladd.textBox_Count.Text;
                    //            row_itemcoladd["onexe"] = exe_itemcoladd.textBox_OnExe.Text;
                    //            row_itemcoladd["exe"] = exe_itemcoladd.textBox_ExeComp.Text;
                    //            row_itemcoladd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_itemcoladd);
                    //            break;
                    //        case "Exe_SymbolColAdd":
                    //            Executions.Exe_SymbolColAdd exe_symboladd = (Executions.Exe_SymbolColAdd)con;
                    //            DataRow row_symboladd = savingDataSet.Tables["exe"].NewRow();
                    //            row_symboladd["type"] = "huntitem";

                    //            int index_colsymbol = 0;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //                index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 300000;
                    //            else
                    //                index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 400000;

                    //            row_symboladd["exe_Text"] = index_colsymbol.ToString();
                    //            row_symboladd["num"] = exe_symboladd.textBox_Count.Text;
                    //            row_symboladd["onexe"] = exe_symboladd.textBox_OnExe.Text;
                    //            row_symboladd["exe"] = exe_symboladd.textBox_Exe.Text;
                    //            row_symboladd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_symboladd);
                    //            break;
                    //        case "Exe_AllHunting":
                    //            Executions.Exe_AllHunting exe_allhunt = (Executions.Exe_AllHunting)con;
                    //            DataRow row_allhunt = savingDataSet.Tables["exe"].NewRow();
                    //            row_allhunt["type"] = "huntall";
                    //            row_allhunt["exe_Text"] = exe_allhunt.textBox_Exe.Text;
                    //            row_allhunt["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_allhunt);
                    //            break;
                    //        case "Exe_AddDailyQuest":
                    //            Executions.Exe_AddDailyQuest exe_AddDailyQuest = (Executions.Exe_AddDailyQuest)con;
                    //            DataRow row_addDailyQuest = savingDataSet.Tables["exe"].NewRow();
                    //            row_addDailyQuest["type"] = "setDailyQuest";
                    //            row_addDailyQuest["exe_Text"] = exe_AddDailyQuest.txt_QuestNo.Text;
                    //            row_addDailyQuest["true"] = exe_AddDailyQuest.txt_ExecuteOnSuccess.Text;
                    //            row_addDailyQuest["false"] = exe_AddDailyQuest.txt_ExecuteOnFail.Text;

                    //            row_addDailyQuest["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_addDailyQuest);
                    //            break;
                    //        case "Exe_QuestAdd":
                    //            Executions.Exe_QuestAdd exe_questadd = (Executions.Exe_QuestAdd)con;
                    //            DataRow row_questadd = savingDataSet.Tables["exe"].NewRow();
                    //            row_questadd["type"] = "setquest";
                    //            row_questadd["exe_Text"] = exe_questadd.textBox_Item.Text;
                    //            row_questadd["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_questadd);
                    //            break;
                    //        case "Exe_Message":
                    //            Executions.Exe_Message exe_msg = (Executions.Exe_Message)con;
                    //            DataRow row_msg = savingDataSet.Tables["exe"].NewRow();
                    //            row_msg["type"] = "msg";
                    //            row_msg["exe_Text"] = exe_msg.textBox_Index.Text;
                    //            row_msg["msg_type"] = exe_msg.textBox_Type.Text;
                    //            row_msg["substring"] = exe_msg.textBox_Substring.Text;
                    //            row_msg["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_msg);
                    //            break;
                    //        case "Exe_Script":
                    //            Executions.Exe_Script exe_script = (Executions.Exe_Script)con;
                    //            DataRow row_script = savingDataSet.Tables["exe"].NewRow();
                    //            row_script["type"] = "talk";
                    //            row_script["exe_Text"] = exe_script.textBox_Index.Text;
                    //            row_script["npc"] = exe_script.textBox_NPC.Text;
                    //            row_script["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_script);
                    //            break;
                    //        case "Exe_ProbChoice":
                    //            Executions.Exe_ProbChoice exe_choice = (Executions.Exe_ProbChoice)con;
                    //            DataRow row_choice = savingDataSet.Tables["exe"].NewRow();
                    //            row_choice["type"] = "random_coice";
                    //            row_choice["exe_Text"] = exe_choice.textBox_Seed.Text;
                    //            row_choice["rand"] = exe_choice.textBox_Rand.Text;
                    //            row_choice["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_choice);
                    //            break;
                    //        case "Exe_Condition":
                    //            Executions.Exe_Condition exe_condition = (Executions.Exe_Condition)con;
                    //            DataRow row_condition = savingDataSet.Tables["exe"].NewRow();
                    //            row_condition["type"] = "conexe";
                    //            row_condition["exe_Text"] = exe_condition.textBox_Condition.Text;
                    //            row_condition["true"] = exe_condition.textBox_true.Text;
                    //            row_condition["false"] = exe_condition.textBox_false.Text;
                    //            row_condition["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_condition);
                    //            break;
                    //        case "Exe_MoneyDel":
                    //            Executions.Exe_MoneyDel exe_moneyDelete = (Executions.Exe_MoneyDel)con;
                    //            DataRow row_moneyDelete = savingDataSet.Tables["exe"].NewRow();
                    //            row_moneyDelete["type"] = "moneyDelete";
                    //            row_moneyDelete["num"] = exe_moneyDelete.txtDeleteMoney.Text;
                    //            row_moneyDelete["onexe"] = exe_moneyDelete.txtMoreExecute.Text;
                    //            row_moneyDelete["exe_Text"] = exe_moneyDelete.txtLessExecute.Text;
                    //            row_moneyDelete["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_moneyDelete);
                    //            break;
                    //        case "Exe_QuestComplete":
                    //            Executions.Exe_QuestComplete exe_questcomp = (Executions.Exe_QuestComplete)con;
                    //            DataRow row_questcomp = savingDataSet.Tables["exe"].NewRow();
                    //            row_questcomp["type"] = "complete";
                    //            row_questcomp["exe_Text"] = exe_questcomp.textBox_Quest.Text;
                    //            row_questcomp["true"] = exe_questcomp.textBox_QuestComplete.Text;
                    //            row_questcomp["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_questcomp);
                    //            break;
                    //        case "Exe_AddDel":
                    //            Executions.Exe_AddDel exe_alldel = (Executions.Exe_AddDel)con;
                    //            DataRow row_alldel = savingDataSet.Tables["exe"].NewRow();
                    //            row_alldel["type"] = "delcount";
                    //            row_alldel["exe_Text"] = ".";
                    //            row_alldel["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_alldel);
                    //            break;
                    //        case "Exe_Custum":
                    //            Executions.Exe_Custum exe_custum = (Executions.Exe_Custum)con;
                    //            DataRow row_custum = savingDataSet.Tables["exe"].NewRow();
                    //            row_custum["type"] = "custom";
                    //            row_custum["exe_Text"] = exe_custum.textBox_Code.Text;
                    //            row_custum["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_custum);
                    //            break;
                    //        case "Exe_Return":
                    //            Executions.Exe_Return exe_return = (Executions.Exe_Return)con;
                    //            DataRow row_return = savingDataSet.Tables["exe"].NewRow();
                    //            row_return["type"] = "return";
                    //            row_return["exe_Text"] = ".";
                    //            row_return["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_return);
                    //            break;
                    //        case "Exe_FullInven":
                    //            Executions.Exe_FullInven exe_fullinven = (Executions.Exe_FullInven)con;
                    //            DataRow row_fullinven = savingDataSet.Tables["exe"].NewRow();
                    //            row_fullinven["type"] = "fullinven";
                    //            row_fullinven["exe_Text"] = ".";
                    //            row_fullinven["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_fullinven);
                    //            break;
                    //        case "Exe_FullQuest":
                    //            Executions.Exe_FullQuest exe_fullquest = (Executions.Exe_FullQuest)con;
                    //            DataRow row_fullquest = savingDataSet.Tables["exe"].NewRow();
                    //            row_fullquest["type"] = "fullquest";
                    //            row_fullquest["exe_Text"] = ".";
                    //            row_fullquest["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_fullquest);
                    //            break;
                    //        case "Exe_Recompense":
                    //            {
                    //                Executions.Exe_Recompense exe_recompense = (Executions.Exe_Recompense)con;
                    //                DataRow row_recompense = savingDataSet.Tables["exe"].NewRow();
                    //                row_recompense["type"] = "recompense";

                    //                string exe_Text = exe_recompense.textBox_Index.Text
                    //                    + "," + exe_recompense.txtRewardWarrior.Text
                    //                    + "," + exe_recompense.txtRewardArcher.Text
                    //                    + "," + exe_recompense.txtRewardCleric.Text
                    //                    + "," + exe_recompense.txtRewardSorcerer.Text
                    //                    + "," + exe_recompense.txtRewardArcademic.Text
                    //                    + "," + exe_recompense.txtRewardKally.Text;
                    //                row_recompense["exe_Text"] = exe_Text;
                    //                row_recompense["executions_Id"] = "accepting";
                    //                savingDataSet.Tables["exe"].Rows.Add(row_recompense);
                    //            }
                    //            break;
                    //        case "Exe_Recompense_View":
                    //            {
                    //                Executions.Exe_Recompense_View exe_recompense_view = (Executions.Exe_Recompense_View)con;
                    //                DataRow row_recompense_view = savingDataSet.Tables["exe"].NewRow();
                    //                row_recompense_view["type"] = "recompense_view";
                    //                string exe_Text = exe_recompense_view.textBox_Index.Text
                    //                    + "," + exe_recompense_view.txtRewardWarrior.Text
                    //                    + "," + exe_recompense_view.txtRewardArcher.Text
                    //                    + "," + exe_recompense_view.txtRewardCleric.Text
                    //                    + "," + exe_recompense_view.txtRewardSorcerer.Text
                    //                    + "," + exe_recompense_view.txtRewardArcademic.Text
                    //                    + "," + exe_recompense_view.txtRewardKally.Text;
                    //                row_recompense_view["exe_Text"] = exe_Text;
                    //                row_recompense_view["executions_Id"] = "accepting";
                    //                savingDataSet.Tables["exe"].Rows.Add(row_recompense_view);
                    //            }
                    //            break;
                    //        case "Exe_Comment":
                    //            Executions.Exe_Comment exe_comment = (Executions.Exe_Comment)con;
                    //            DataRow row_comment = savingDataSet.Tables["exe"].NewRow();
                    //            row_comment["type"] = "comment";
                    //            row_comment["exe_Text"] = exe_comment.textBox_Comment.Text;
                    //            row_comment["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_comment);
                    //            break;
                    //        case "Exe_AddItemAndQuestItem":
                    //            Executions.Exe_AddItemAndQuestItem exe_AddItemAndQuestItem = (Executions.Exe_AddItemAndQuestItem)con;
                    //            DataRow row_AddItemAndQuestItem = savingDataSet.Tables["exe"].NewRow();
                    //            row_AddItemAndQuestItem["type"] = "addItemAndQuestItem";
                    //            //Quest Item | Item 형태로 exe_Text에 추가
                    //            row_AddItemAndQuestItem["exe_Text"] = exe_AddItemAndQuestItem.txt_Item.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItem.Text;
                    //            //Quest Item 개수 | Item 개수 형탤고 num에 추가.
                    //            row_AddItemAndQuestItem["num"] = exe_AddItemAndQuestItem.txt_ItemCount.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItemCount.Text;
                    //            row_AddItemAndQuestItem["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_AddItemAndQuestItem);
                    //            break;
                    //        case "Exe_CheckStageClearRank":
                    //            Executions.Exe_CheckStageClearRank stageClearRank = (Executions.Exe_CheckStageClearRank)con;
                    //            DataRow row_CheckStageClearRank = savingDataSet.Tables["exe"].NewRow();
                    //            row_CheckStageClearRank["type"] = "checkStageClearRank";
                    //            row_CheckStageClearRank["num"] = stageClearRank.txtStageIndex.Text;
                    //            row_CheckStageClearRank["rand"] = stageClearRank.txtRank.Text;
                    //            row_CheckStageClearRank["true"] = stageClearRank.txtInequality.Text.Replace('>', ']').Replace('<', '[');
                    //            row_CheckStageClearRank["onexe"] = stageClearRank.txtExecute.Text;
                    //            row_CheckStageClearRank["executions_Id"] = "accepting";
                    //            savingDataSet.Tables["exe"].Rows.Add(row_CheckStageClearRank);
                    //            break;
                    //    }
                    //}
                    #endregion
                }
            }
            #endregion

            #region 퀘스트 일반 단계 정보

            foreach (Control control in tabPage_General.Controls["panel_TabMainGeneral"].Controls)
            {
                if (control.Name.IndexOf("unit_") > -1)
                {
                    if (control.Name == "unit_init")
                        control.Tag = 1;

                    foreach (Control npc_control in control.Controls)
                    {
                        if (npc_control is Quest_RemoteComplete)
                        {
                            Quest_RemoteComplete rc = (Quest_RemoteComplete)npc_control;
                            string exeNo = rc.GetExecuteNo();
                            if ("" == exeNo)
                            {
                                continue;
                            }

                            DataRow row_rc = savingDataSet.Tables["remotecomplete"].NewRow();
                            row_rc["qstep"] = control.Tag.ToString();
                            row_rc["rc_Text"] = exeNo;

                            savingDataSet.Tables["remotecomplete"].Rows.Add(row_rc);
                            continue;
                        }
                        
                        if (npc_control.Name.IndexOf("npc_") > -1)
                        {
                            Quest_NPC npc = (Quest_NPC)npc_control;

                            if (npc.textBox_NPCName.Text == "")
                                continue;

                            Quest_Step[] steps = npc.Get_Step();                           

                            foreach (Quest_Step step in steps)
                            {
                                if (step != null)
                                {
                                    if (step.Get_StepNo() == "단계시작")
                                    {
                                        DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                        row_talk["index"] = npc.Get_NPCName() + "-" + control.Tag.ToString();
                                        row_talk["image"] = step.textBox_Image.Text;
                                        row_talk["itemindex"] = step.Get_ItemIndex();
                                        row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString();
                                        //row_talk["talk_question"] = step.Get_Script();
                                        row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                        row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                        savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                        //번역 관련추가
                                        DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                        if (step.textBox_Script.Tag == null)
                                        {
                                            row_question["mid"] = Quest_Mid;
                                            step.textBox_Script.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_question["mid"] = step.textBox_Script.Tag.ToString();

                                            if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                                Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                        }
                                      
                                        row_question["talk_question_Text"] = step.Get_Script();
                                        row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString();

                                        savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                        //번역 관련추가 끝

                                        if (npc.textBox_ExecuteNo.Text != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = control.Tag.ToString();
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["exelink_Text"] = npc.textBox_ExecuteNo.Text;
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }

                                        Quest_SelectControl[] selects = step.Get_Select();

                                        foreach (Quest_SelectControl select in selects)
                                        {
                                            if (select != null)
                                            {
                                                DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                                row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString();
                                                row_select["select_type"] = select.GetSelectType();

                                                if (select.textBox_Select.Tag == null)
                                                {
                                                    row_select["mid"] = Quest_Mid;
                                                    select.textBox_Select.Tag = Quest_Mid;
                                                    ++Quest_Mid;
                                                }
                                                else
                                                {
                                                    row_select["mid"] = select.textBox_Select.Tag.ToString();

                                                    if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                                        Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                                }

                                                
                                                row_select["talk_answer_Text"] = select.Get_Select();

                                                // 타겟인덱스가 숫자일 경우 해당 숫자의 단계로 인덱스 저장
                                                int temp = 0;

                                                if (!Int32.TryParse(select.Get_Target(), out temp))
                                                    row_select["link_index"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + select.Get_Target();
                                                else
                                                    row_select["link_index"] = npc.Get_NPCName() + "-" + temp.ToString();

                                                savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                                if (select.Get_ExeNo() != "")
                                                {
                                                    DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                                    row_exelink["qstep"] = control.Tag.ToString();
                                                    row_exelink["npc"] = npc.Get_NPCName();
                                                    row_exelink["sub"] = select.Get_Target();
                                                    row_exelink["exelink_Text"] = select.Get_ExeNo();
                                                    savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                                }                                                
                                            }
                                        }
                                    }
                                    else
                                    {
                                        DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                        row_talk["index"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + step.Get_StepNo();
                                        row_talk["image"] = step.textBox_Image.Text;
                                        row_talk["itemindex"] = step.Get_ItemIndex();
                                        row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + step.Get_StepNo();
                                        //row_talk["talk_question"] = step.Get_Script();
                                        row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                        row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                        savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                        //번역 관련추가
                                        DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                        if (step.textBox_Script.Tag == null)
                                        {
                                            row_question["mid"] = Quest_Mid;
                                            step.textBox_Script.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_question["mid"] = step.textBox_Script.Tag.ToString();

                                            if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                                Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                        }

                                        
                                        row_question["talk_question_Text"] = step.Get_Script();
                                        row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + step.Get_StepNo();

                                        savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                        //번역 관련추가 끝

                                        Quest_SelectControl[] selects = step.Get_Select();

                                        foreach (Quest_SelectControl select in selects)
                                        {
                                            if (select != null)
                                            {
                                                DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                                row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + step.Get_StepNo();
                                                row_select["select_type"] = select.GetSelectType();

                                                if (select.textBox_Select.Tag == null)
                                                {
                                                    row_select["mid"] = Quest_Mid;
                                                    select.textBox_Select.Tag = Quest_Mid;
                                                    ++Quest_Mid;
                                                }
                                                else
                                                {
                                                    row_select["mid"] = select.textBox_Select.Tag.ToString();

                                                    if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                                        Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                                }

                                                
                                                row_select["talk_answer_Text"] = select.Get_Select();
                                                
                                                // 타겟인덱스가 숫자일 경우 해당 숫자의 단계로 인덱스 저장
                                                int temp = 0;

                                                if (!Int32.TryParse(select.Get_Target(), out temp))
                                                    row_select["link_index"] = npc.Get_NPCName() + "-" + control.Tag.ToString() + "-" + select.Get_Target();
                                                else
                                                    row_select["link_index"] = npc.Get_NPCName() + "-" + temp.ToString();

                                                savingDataSet.Tables["talk_answer"].Rows.Add(row_select);
                                                
                                                DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                                row_exelink["qstep"] = control.Tag.ToString();
                                                row_exelink["npc"] = npc.Get_NPCName();
                                                row_exelink["sub"] = select.Get_Target();
                                                row_exelink["exelink_Text"] = select.Get_ExeNo();
                                                savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (control.Name.IndexOf("condition_") > -1)
                {
                    string count = "0";

                    if (control.Name == "condition_init")
                        count = "1";
                    else
                        count = control.Tag.ToString();

                    DataRow row_change = savingDataSet.Tables["qstep_change"].NewRow();

                    row_change["qstep"] = count;
                    row_change["qstep_change_Id"] = count;
                    row_change["comment"] = control.Controls["groupBox_Detail"].Controls["textBox_Detail"].Text;

                    savingDataSet.Tables["qstep_change"].Rows.Add(row_change);

                    DataRow row_executions = savingDataSet.Tables["executions"].NewRow();

                    row_executions["executions_Id"] = count;
                    row_executions["qstep_change_Id"] = count;

                    savingDataSet.Tables["executions"].Rows.Add(row_executions);

                    DataRow row_conditions = savingDataSet.Tables["conditions"].NewRow();

                    row_conditions["conditions_Id"] = count;
                    row_conditions["qstep_change_Id"] = count;

                    savingDataSet.Tables["conditions"].Rows.Add(row_conditions);

                    _saveConditionObject(control.Controls["groupBox_Condition"].Controls, count);
                    _saveExecutionObject(control.Controls["groupBox_Execution"].Controls, count);

                    #region 조건문 Object 생성
                    //foreach (Control con in control.Controls["groupBox_Condition"].Controls)
                    //{
                    //    switch (con.Name)
                    //    {
                    //        case "Condition_Level":
                    //            Conditions.Condition_Level con_level = (Conditions.Condition_Level)con;
                    //            DataRow row_level = savingDataSet.Tables["chk"].NewRow();
                    //            row_level["type"] = "lvl";
                    //            row_level["chk_Text"] = con_level.textBox_Value.Text;
                    //            row_level["op"] = con_level.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_level["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_level);
                    //            break;
                    //        case "Condition_GeneralItem":
                    //            Conditions.Condition_GeneralItem con_item = (Conditions.Condition_GeneralItem)con;
                    //            DataRow row_item = savingDataSet.Tables["chk"].NewRow();
                    //            row_item["type"] = "item";
                    //            row_item["chk_Text"] = con_item.textBox_Item.Text;
                    //            row_item["num"] = con_item.textBox_Value.Text;
                    //            row_item["op"] = con_item.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_item["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_item);
                    //            break;
                    //        case "Condition_SymbolItem":
                    //            Conditions.Condition_SymbolItem con_symbol = (Conditions.Condition_SymbolItem)con;
                    //            DataRow row_symbol = savingDataSet.Tables["chk"].NewRow();

                    //            int index_chksymbol = 0;

                    //            if (comboBox_CompleteType.SelectedIndex == 0)
                    //            {
                    //                row_symbol["type"] = "questitem";
                    //                index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 300000;
                    //            }
                    //            else
                    //            {
                    //                row_symbol["type"] = "item";
                    //                index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 400000;
                    //            }

                    //            row_symbol["chk_Text"] = index_chksymbol.ToString();
                    //            row_symbol["num"] = con_symbol.textBox_Value.Text;
                    //            row_symbol["op"] = con_symbol.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_symbol["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_symbol);
                    //            break;
                    //        case "Condition_QuestItem":
                    //            Conditions.Condition_QuestItem con_qitem = (Conditions.Condition_QuestItem)con;
                    //            DataRow row_qitem = savingDataSet.Tables["chk"].NewRow();
                    //            row_qitem["type"] = "questitem";
                    //            row_qitem["chk_Text"] = con_qitem.textBox_Item.Text;
                    //            row_qitem["num"] = con_qitem.textBox_Value.Text;
                    //            row_qitem["op"] = con_qitem.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_qitem["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_qitem);
                    //            break;
                    //        case "Condition_Class":
                    //            Conditions.Condition_Class con_class = (Conditions.Condition_Class)con;
                    //            DataRow row_class = savingDataSet.Tables["chk"].NewRow();
                    //            row_class["type"] = "class";
                    //            row_class["chk_Text"] = con_class.textBox_Value.Text;
                    //            row_class["op"] = con_class.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_class["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_class);
                    //            break;
                    //        case "Condition_Prob":
                    //            Conditions.Condition_Prob con_prob = (Conditions.Condition_Prob)con;
                    //            DataRow row_prob = savingDataSet.Tables["chk"].NewRow();
                    //            row_prob["type"] = "prob";
                    //            row_prob["chk_Text"] = con_prob.textBox_Value.Text;
                    //            row_prob["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_prob);
                    //            break;
                    //        case "Condition_Quest":
                    //            Conditions.Condition_Quest con_quest = (Conditions.Condition_Quest)con;
                    //            DataRow row_quest = savingDataSet.Tables["chk"].NewRow();
                    //            row_quest["type"] = "hasquest";
                    //            row_quest["chk_Text"] = con_quest.textBox_Value.Text;
                    //            row_quest["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_quest);
                    //            break;
                    //        case "Condition_Compelete":
                    //            Conditions.Condition_Compelete con_comp = (Conditions.Condition_Compelete)con;
                    //            DataRow row_comp = savingDataSet.Tables["chk"].NewRow();
                    //            row_comp["type"] = "hascomq";
                    //            row_comp["chk_Text"] = con_comp.textBox_Value.Text;
                    //            row_comp["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_comp);
                    //            break;
                    //        case "Condition_Inven":
                    //            Conditions.Condition_Inven con_inven = (Conditions.Condition_Inven)con;
                    //            DataRow row_inven = savingDataSet.Tables["chk"].NewRow();
                    //            row_inven["type"] = "inven_eslot";
                    //            row_inven["chk_Text"] = con_inven.textBox_Value.Text;
                    //            row_inven["op"] = con_inven.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_inven["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_inven);
                    //            break;
                    //        case "Condition_Custum":
                    //            Conditions.Condition_Custum con_custum = (Conditions.Condition_Custum)con;
                    //            DataRow row_custom = savingDataSet.Tables["chk"].NewRow();
                    //            row_custom["type"] = "custom";
                    //            row_custom["chk_Text"] = con_custum.textBox_Code.Text;
                    //            row_custom["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_custom);
                    //            break;
                    //        case "Condition_QuestCnt":
                    //            Conditions.Condition_QuestCnt con_qcnt = (Conditions.Condition_QuestCnt)con;
                    //            DataRow row_qcnt = savingDataSet.Tables["chk"].NewRow();
                    //            row_qcnt["type"] = "qcount";
                    //            row_qcnt["chk_Text"] = ".";
                    //            row_qcnt["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_qcnt);
                    //            break;
                    //        case "Condition_StageConstructionLevel":
                    //            Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = (Conditions.Condition_StageConstructionLevel)con;
                    //            DataRow row_stageConstructionLevel = savingDataSet.Tables["chk"].NewRow();
                    //            row_stageConstructionLevel["type"] = "stageConstructionLevel";
                    //            row_stageConstructionLevel["chk_Text"] = con_stageConstructionLevel.textBox_Value.Text.ToString();
                    //            row_stageConstructionLevel["op"] = con_stageConstructionLevel.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                    //            row_stageConstructionLevel["conditions_Id"] = count;
                    //            savingDataSet.Tables["chk"].Rows.Add(row_stageConstructionLevel);
                    //            break;
                    //    }
                    //}
                    #endregion

                    #region 실행문 Object 생성

                    //foreach (Control con in control.Controls["groupBox_Execution"].Controls)
                    //{
                    //    try
                    //    {
                    //        switch (con.Name)
                    //        {
                    //            case "Exe_QuestStepMod":
                    //                Executions.Exe_QuestStepMod eq = (Executions.Exe_QuestStepMod)con;
                    //                DataRow row_exe = savingDataSet.Tables["exe"].NewRow();
                    //                row_exe["type"] = "setstep";
                    //                row_exe["exe_Text"] = eq.textBox_Step.Text;
                    //                row_exe["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_exe);
                    //                break;
                    //            case "Exe_JournalMod":
                    //                Executions.Exe_JournalMod exe_journal = (Executions.Exe_JournalMod)con;
                    //                DataRow row_journal = savingDataSet.Tables["exe"].NewRow();
                    //                row_journal["type"] = "setjornal";
                    //                row_journal["exe_Text"] = exe_journal.textBox_Journal.Text;
                    //                row_journal["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_journal);
                    //                break;
                    //            case "Exe_ItemDel":
                    //                Executions.Exe_ItemDel exe_itemdel = (Executions.Exe_ItemDel)con;
                    //                DataRow row_itemdel = savingDataSet.Tables["exe"].NewRow();
                    //                row_itemdel["type"] = "delitem";
                    //                row_itemdel["exe_Text"] = exe_itemdel.textBox_Item.Text;
                    //                row_itemdel["num"] = exe_itemdel.textBox_Count.Text;
                    //                row_itemdel["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_itemdel);
                    //                break;
                    //            case "Exe_SymbolDel":
                    //                Executions.Exe_SymbolDel exe_symdel = (Executions.Exe_SymbolDel)con;
                    //                DataRow row_symdel = savingDataSet.Tables["exe"].NewRow();

                    //                if (comboBox_CompleteType.SelectedIndex == 0)
                    //                {
                    //                    row_symdel["type"] = "delquestitem";
                    //                }
                    //                else
                    //                {
                    //                    row_symdel["type"] = "delitem";
                    //                }

                    //                int index_delsymbol = 0;

                    //                if (comboBox_CompleteType.SelectedIndex == 0)
                    //                {
                    //                    index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 300000;
                    //                }
                    //                else
                    //                {
                    //                    index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 400000;
                    //                }

                    //                row_symdel["exe_Text"] = index_delsymbol.ToString();
                    //                row_symdel["num"] = exe_symdel.textBox_Count.Text;
                    //                row_symdel["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_symdel);
                    //                break;
                    //            case "Exe_QuestItemDel":
                    //                Executions.Exe_QuestItemDel exe_qidel = (Executions.Exe_QuestItemDel)con;
                    //                DataRow row_qidel = savingDataSet.Tables["exe"].NewRow();
                    //                row_qidel["type"] = "delquestitem";
                    //                row_qidel["exe_Text"] = exe_qidel.textBox_Item.Text;
                    //                row_qidel["num"] = exe_qidel.textBox_Count.Text;
                    //                row_qidel["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_qidel);
                    //                break;
                    //            case "Exe_ItemAdd":
                    //                Executions.Exe_ItemAdd exe_itemadd = (Executions.Exe_ItemAdd)con;
                    //                DataRow row_itemadd = savingDataSet.Tables["exe"].NewRow();
                    //                row_itemadd["type"] = "additem";
                    //                row_itemadd["exe_Text"] = exe_itemadd.textBox_Item.Text;
                    //                row_itemadd["num"] = exe_itemadd.textBox_Count.Text;
                    //                row_itemadd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_itemadd);
                    //                break;
                    //            case "Exe_SymbolAdd":
                    //                Executions.Exe_SymbolAdd exe_symadd = (Executions.Exe_SymbolAdd)con;
                    //                DataRow row_symadd = savingDataSet.Tables["exe"].NewRow();

                    //                if (comboBox_CompleteType.SelectedIndex == 0)
                    //                {
                    //                    row_symadd["type"] = "addquestitem";
                    //                }
                    //                else
                    //                {
                    //                    row_symadd["type"] = "additem";
                    //                }

                    //                int index_addsymbol = 0;

                    //                if (comboBox_CompleteType.SelectedIndex == 0)
                    //                    index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 300000;
                    //                else
                    //                    index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 400000;

                    //                row_symadd["exe_Text"] = index_addsymbol.ToString();
                    //                row_symadd["num"] = exe_symadd.textBox_Count.Text;
                    //                row_symadd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_symadd);
                    //                break;
                    //            case "Exe_QuestItemAdd":
                    //                Executions.Exe_QuestItemAdd exe_qiadd = (Executions.Exe_QuestItemAdd)con;
                    //                DataRow row_qiadd = savingDataSet.Tables["exe"].NewRow();
                    //                row_qiadd["type"] = "addquestitem";
                    //                row_qiadd["exe_Text"] = exe_qiadd.textBox_Item.Text;
                    //                row_qiadd["num"] = exe_qiadd.textBox_Count.Text;
                    //                row_qiadd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_qiadd);
                    //                break;
                    //            case "Exe_MonsterAdd":
                    //                Executions.Exe_MonsterAdd exe_monadd = (Executions.Exe_MonsterAdd)con;
                    //                DataRow row_monadd = savingDataSet.Tables["exe"].NewRow();
                    //                row_monadd["type"] = "huntmon";
                    //                row_monadd["exe_Text"] = exe_monadd.textBox_MonID.Text;
                    //                row_monadd["num"] = exe_monadd.textBox_Count.Text;
                    //                row_monadd["party"] = exe_monadd.textBox_Party.Text;
                    //                row_monadd["onexe"] = exe_monadd.textBox_OnExe.Text;
                    //                row_monadd["exe"] = exe_monadd.textBox_ExeComp.Text;
                    //                row_monadd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_monadd);
                    //                break;
                    //            case "Exe_ItemColAdd":
                    //                Executions.Exe_ItemColAdd exe_itemcoladd = (Executions.Exe_ItemColAdd)con;
                    //                DataRow row_itemcoladd = savingDataSet.Tables["exe"].NewRow();
                    //                row_itemcoladd["type"] = "huntitem";
                    //                row_itemcoladd["exe_Text"] = exe_itemcoladd.textBox_Item.Text;
                    //                row_itemcoladd["num"] = exe_itemcoladd.textBox_Count.Text;
                    //                row_itemcoladd["onexe"] = exe_itemcoladd.textBox_OnExe.Text;
                    //                row_itemcoladd["exe"] = exe_itemcoladd.textBox_ExeComp.Text;
                    //                row_itemcoladd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_itemcoladd);
                    //                break;
                    //            case "Exe_SymbolColAdd":
                    //                Executions.Exe_SymbolColAdd exe_symboladd = (Executions.Exe_SymbolColAdd)con;
                    //                DataRow row_symboladd = savingDataSet.Tables["exe"].NewRow();
                    //                row_symboladd["type"] = "huntitem";

                    //                int index_colsymbol = 0;
                    //                if (comboBox_CompleteType.SelectedIndex == 0)
                    //                    index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 300000;
                    //                else
                    //                    index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 400000;

                    //                row_symboladd["exe_Text"] = index_colsymbol.ToString();
                    //                row_symboladd["num"] = exe_symboladd.textBox_Count.Text;
                    //                row_symboladd["onexe"] = exe_symboladd.textBox_OnExe.Text;
                    //                row_symboladd["exe"] = exe_symboladd.textBox_Exe.Text;
                    //                row_symboladd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_symboladd);
                    //                break;
                    //            case "Exe_AllHunting":
                    //                Executions.Exe_AllHunting exe_allhunt = (Executions.Exe_AllHunting)con;
                    //                DataRow row_allhunt = savingDataSet.Tables["exe"].NewRow();
                    //                row_allhunt["type"] = "huntall";
                    //                row_allhunt["exe_Text"] = exe_allhunt.textBox_Exe.Text;
                    //                row_allhunt["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_allhunt);
                    //                break;
                    //            case "Exe_AddDailyQuest":
                    //                Executions.Exe_AddDailyQuest exe_AddDailyQuest = (Executions.Exe_AddDailyQuest)con;
                    //                DataRow row_addDailyQuest = savingDataSet.Tables["exe"].NewRow();
                    //                row_addDailyQuest["type"] = "setDailyQuest";
                    //                row_addDailyQuest["exe_Text"] = exe_AddDailyQuest.txt_QuestNo.Text;
                    //                row_addDailyQuest["true"] = exe_AddDailyQuest.txt_ExecuteOnSuccess.Text;
                    //                row_addDailyQuest["false"] = exe_AddDailyQuest.txt_ExecuteOnFail.Text;
                    //                row_addDailyQuest["executions_Id"] = "count";
                    //                savingDataSet.Tables["exe"].Rows.Add(row_addDailyQuest);
                    //                break;
                    //            case "Exe_QuestAdd":
                    //                Executions.Exe_QuestAdd exe_questadd = (Executions.Exe_QuestAdd)con;
                    //                DataRow row_questadd = savingDataSet.Tables["exe"].NewRow();
                    //                row_questadd["type"] = "setquest";
                    //                row_questadd["exe_Text"] = exe_questadd.textBox_Item.Text;
                    //                row_questadd["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_questadd);
                    //                break;
                    //            case "Exe_Message":
                    //                Executions.Exe_Message exe_msg = (Executions.Exe_Message)con;
                    //                DataRow row_msg = savingDataSet.Tables["exe"].NewRow();
                    //                row_msg["type"] = "msg";
                    //                row_msg["exe_Text"] = exe_msg.textBox_Index.Text;
                    //                row_msg["msg_type"] = exe_msg.textBox_Type.Text;
                    //                row_msg["substring"] = exe_msg.textBox_Substring.Text;
                    //                row_msg["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_msg);
                    //                break;
                    //            case "Exe_Script":
                    //                Executions.Exe_Script exe_script = (Executions.Exe_Script)con;
                    //                DataRow row_script = savingDataSet.Tables["exe"].NewRow();
                    //                row_script["type"] = "talk";
                    //                row_script["exe_Text"] = exe_script.textBox_Index.Text;
                    //                row_script["npc"] = exe_script.textBox_NPC.Text;
                    //                row_script["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_script);
                    //                break;
                    //            case "Exe_ProbChoice":
                    //                Executions.Exe_ProbChoice exe_choice = (Executions.Exe_ProbChoice)con;
                    //                DataRow row_choice = savingDataSet.Tables["exe"].NewRow();
                    //                row_choice["type"] = "random_coice";
                    //                row_choice["exe_Text"] = exe_choice.textBox_Seed.Text;
                    //                row_choice["rand"] = exe_choice.textBox_Rand.Text;
                    //                row_choice["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_choice);
                    //                break;
                    //            case "Exe_Condition":
                    //                Executions.Exe_Condition exe_condition = (Executions.Exe_Condition)con;
                    //                DataRow row_condition = savingDataSet.Tables["exe"].NewRow();
                    //                row_condition["type"] = "conexe";
                    //                row_condition["exe_Text"] = exe_condition.textBox_Condition.Text;
                    //                row_condition["true"] = exe_condition.textBox_true.Text;
                    //                row_condition["false"] = exe_condition.textBox_false.Text;
                    //                row_condition["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_condition);
                    //                break;
                    //            case "Exe_MoneyDel":
                    //                Executions.Exe_MoneyDel exe_moneyDelete = (Executions.Exe_MoneyDel)con;
                    //                DataRow row_moneyDelete = savingDataSet.Tables["exe"].NewRow();
                    //                row_moneyDelete["type"] = "moneyDelete";
                    //                row_moneyDelete["num"] = exe_moneyDelete.txtDeleteMoney.Text;
                    //                row_moneyDelete["onexe"] = exe_moneyDelete.txtMoreExecute.Text;
                    //                row_moneyDelete["exe_Text"] = exe_moneyDelete.txtLessExecute.Text;
                    //                row_moneyDelete["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_moneyDelete);
                    //                break;
                    //            case "Exe_QuestComplete":
                    //                Executions.Exe_QuestComplete exe_questcomp = (Executions.Exe_QuestComplete)con;
                    //                DataRow row_questcomp = savingDataSet.Tables["exe"].NewRow();
                    //                row_questcomp["type"] = "complete";
                    //                row_questcomp["exe_Text"] = exe_questcomp.textBox_Quest.Text;
                    //                row_questcomp["true"] = exe_questcomp.textBox_QuestComplete.Text;
                    //                row_questcomp["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_questcomp);
                    //                break;
                    //            case "Exe_AddDel":
                    //                Executions.Exe_AddDel exe_alldel = (Executions.Exe_AddDel)con;
                    //                DataRow row_alldel = savingDataSet.Tables["exe"].NewRow();
                    //                row_alldel["type"] = "delcount";
                    //                row_alldel["exe_Text"] = ".";
                    //                row_alldel["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_alldel);
                    //                break;
                    //            case "Exe_Custum":
                    //                Executions.Exe_Custum exe_custum = (Executions.Exe_Custum)con;
                    //                DataRow row_custum = savingDataSet.Tables["exe"].NewRow();
                    //                row_custum["type"] = "custom";
                    //                row_custum["exe_Text"] = exe_custum.textBox_Code.Text;
                    //                row_custum["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_custum);
                    //                break;
                    //            case "Exe_Return":
                    //                Executions.Exe_Return exe_return = (Executions.Exe_Return)con;
                    //                DataRow row_return = savingDataSet.Tables["exe"].NewRow();
                    //                row_return["type"] = "return";
                    //                row_return["exe_Text"] = ".";
                    //                row_return["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_return);
                    //                break;
                    //            case "Exe_FullInven":
                    //                Executions.Exe_FullInven exe_fullinven = (Executions.Exe_FullInven)con;
                    //                DataRow row_fullinven = savingDataSet.Tables["exe"].NewRow();
                    //                row_fullinven["type"] = "fullinven";
                    //                row_fullinven["exe_Text"] = ".";
                    //                row_fullinven["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_fullinven);
                    //                break;
                    //            case "Exe_FullQuest":
                    //                Executions.Exe_FullQuest exe_fullquest = (Executions.Exe_FullQuest)con;
                    //                DataRow row_fullquest = savingDataSet.Tables["exe"].NewRow();
                    //                row_fullquest["type"] = "fullquest";
                    //                row_fullquest["exe_Text"] = ".";
                    //                row_fullquest["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_fullquest);
                    //                break;
                    //            case "Exe_Recompense":
                    //                {
                    //                    Executions.Exe_Recompense exe_recompense = (Executions.Exe_Recompense)con;
                    //                    DataRow row_recompense = savingDataSet.Tables["exe"].NewRow();
                    //                    row_recompense["type"] = "recompense";
                    //                    string exe_Text = exe_recompense.textBox_Index.Text
                    //                        + "," + exe_recompense.txtRewardWarrior.Text
                    //                        + "," + exe_recompense.txtRewardArcher.Text
                    //                        + "," + exe_recompense.txtRewardCleric.Text
                    //                        + "," + exe_recompense.txtRewardSorcerer.Text
                    //                        + "," + exe_recompense.txtRewardArcademic.Text
                    //                        + "," + exe_recompense.txtRewardKally.Text;
                    //                    row_recompense["exe_Text"] = exe_Text;
                    //                    row_recompense["executions_Id"] = count;
                    //                    savingDataSet.Tables["exe"].Rows.Add(row_recompense);
                    //                }
                    //                break;
                    //            case "Exe_Recompense_View":
                    //                {
                    //                    Executions.Exe_Recompense_View exe_recompense_view = (Executions.Exe_Recompense_View)con;
                    //                    DataRow row_recompense_view = savingDataSet.Tables["exe"].NewRow();
                    //                    row_recompense_view["type"] = "recompense_view";
                    //                    string exe_Text = exe_recompense_view.textBox_Index.Text
                    //                        + "," + exe_recompense_view.txtRewardWarrior.Text
                    //                        + "," + exe_recompense_view.txtRewardArcher.Text
                    //                        + "," + exe_recompense_view.txtRewardCleric.Text
                    //                        + "," + exe_recompense_view.txtRewardSorcerer.Text
                    //                        + "," + exe_recompense_view.txtRewardArcademic.Text
                    //                        + "," + exe_recompense_view.txtRewardKally.Text;
                    //                    row_recompense_view["exe_Text"] = exe_Text;
                    //                    row_recompense_view["executions_Id"] = count;
                    //                    savingDataSet.Tables["exe"].Rows.Add(row_recompense_view);
                    //                }
                    //                break;
                    //            case "Exe_Comment":
                    //                Executions.Exe_Comment exe_comment = (Executions.Exe_Comment)con;
                    //                DataRow row_comment = savingDataSet.Tables["exe"].NewRow();
                    //                row_comment["type"] = "comment";
                    //                row_comment["exe_Text"] = exe_comment.textBox_Comment.Text;
                    //                row_comment["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_comment);
                    //                break;
                    //            case "Exe_AddItemAndQuestItem":
                    //                Executions.Exe_AddItemAndQuestItem exe_AddItemAndQuestItem = (Executions.Exe_AddItemAndQuestItem)con;
                    //                DataRow row_AddItemAndQuestItem = savingDataSet.Tables["exe"].NewRow();
                    //                row_AddItemAndQuestItem["type"] = "addItemAndQuestItem";
                    //                //Quest Item | Item 형태로 exe_Text에 추가
                    //                row_AddItemAndQuestItem["exe_Text"] = exe_AddItemAndQuestItem.txt_Item.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItem.Text;
                    //                //Quest Item 개수 | Item 개수 형탤고 num에 추가.
                    //                row_AddItemAndQuestItem["num"] = exe_AddItemAndQuestItem.txt_ItemCount.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItemCount.Text;
                    //                row_AddItemAndQuestItem["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_AddItemAndQuestItem);
                    //                break;
                    //            case "Exe_CheckStageClearRank":
                    //                Executions.Exe_CheckStageClearRank stageClearRank = (Executions.Exe_CheckStageClearRank)con;
                    //                DataRow row_CheckStageClearRank = savingDataSet.Tables["exe"].NewRow();
                    //                row_CheckStageClearRank["type"] = "checkStageClearRank";
                    //                row_CheckStageClearRank["num"] = stageClearRank.txtStageIndex.Text;
                    //                row_CheckStageClearRank["rand"] = stageClearRank.txtRank.Text;
                    //                row_CheckStageClearRank["true"] = stageClearRank.txtInequality.Text.Replace('>', ']').Replace('<', '[');
                    //                row_CheckStageClearRank["onexe"] = stageClearRank.txtExecute.Text;
                    //                row_CheckStageClearRank["executions_Id"] = count;
                    //                savingDataSet.Tables["exe"].Rows.Add(row_CheckStageClearRank);
                    //                break;
                    //        }
                    //    }
                    //    catch (Exception e)
                    //    {
                    //        MessageBox.Show(e.Message + "\n" + e.StackTrace);
                    //    }
                    //}
                    #endregion
                }
            }

            #endregion

            #region 퀘스트 완료 단계 정보

            foreach (Control control in tabPage_Complete.Controls["panel_TabMainComplete"].Controls["unit_init"].Controls)
            {
                if (control.Name.IndexOf("npc_") > -1)
                {
                    Quest_NPC npc = (Quest_NPC)control;

                    if (npc.textBox_NPCName.Text == "")
                        continue;

                    Quest_Step[] steps = npc.Get_Step();

                    foreach (Quest_Step step in steps)
                    {
                        if (step != null)
                        {
                            if (step.Get_StepNo() == "단계시작")
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-complete";
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete";
                                //row_talk["talk_question"] = step.Get_Script();
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();

                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                }
                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete";

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                if (npc.textBox_ExecuteNo.Text != "")
                                {
                                    DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                    row_exelink["qstep"] = "complete";
                                    row_exelink["npc"] = npc.Get_NPCName();
                                    row_exelink["exelink_Text"] = npc.textBox_ExecuteNo.Text;
                                    savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                }

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete";
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();

                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                        }

                                        
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-complete" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "complete";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                DataRow row_talk = savingDataSet.Tables["talk_paragraph"].NewRow();

                                row_talk["index"] = npc.Get_NPCName() + "-complete-" + step.Get_StepNo();
                                row_talk["image"] = step.textBox_Image.Text;
                                row_talk["itemindex"] = step.Get_ItemIndex();
                                row_talk["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete-" + step.Get_StepNo();
                                row_talk["talk_question"] = step.Get_Script();
                                row_talk["portrait_left"] = step.textBox_PortraitLeft.Text;
                                row_talk["portrait_right"] = step.textBox_PortraitRight.Text;

                                savingDataSet.Tables["talk_paragraph"].Rows.Add(row_talk);

                                //번역 관련추가
                                DataRow row_question = savingDataSet.Tables["talk_question"].NewRow();

                                if (step.textBox_Script.Tag == null)
                                {
                                    row_question["mid"] = Quest_Mid;
                                    step.textBox_Script.Tag = Quest_Mid;
                                    ++Quest_Mid;
                                }
                                else
                                {
                                    row_question["mid"] = step.textBox_Script.Tag.ToString();

                                    if (Quest_Mid < Convert.ToInt32(step.textBox_Script.Tag.ToString()))
                                        Quest_Mid = Convert.ToInt32(step.textBox_Script.Tag.ToString()) + 1;
                                }

                                
                                row_question["talk_question_Text"] = step.Get_Script();
                                row_question["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete-" + step.Get_StepNo();

                                savingDataSet.Tables["talk_question"].Rows.Add(row_question);
                                //번역 관련추가 끝

                                Quest_SelectControl[] selects = step.Get_Select();

                                foreach (Quest_SelectControl select in selects)
                                {
                                    if (select != null)
                                    {
                                        DataRow row_select = savingDataSet.Tables["talk_answer"].NewRow();

                                        row_select["talk_paragraph_Id"] = npc.Get_NPCName() + "-complete-" + step.Get_StepNo();
                                        row_select["select_type"] = select.GetSelectType();

                                        if (select.textBox_Select.Tag == null)
                                        {
                                            row_select["mid"] = Quest_Mid;
                                            select.textBox_Select.Tag = Quest_Mid;
                                            ++Quest_Mid;
                                        }
                                        else
                                        {
                                            row_select["mid"] = select.textBox_Select.Tag.ToString();

                                            if (Quest_Mid < Convert.ToInt32(select.textBox_Select.Tag.ToString()))
                                                Quest_Mid = Convert.ToInt32(select.textBox_Select.Tag.ToString()) + 1;
                                        }
                                        
                                        row_select["talk_answer_Text"] = select.Get_Select();
                                        row_select["link_index"] = npc.Get_NPCName() + "-complete" + "-" + select.Get_Target();

                                        savingDataSet.Tables["talk_answer"].Rows.Add(row_select);

                                        if (select.Get_ExeNo() != "")
                                        {
                                            DataRow row_exelink = savingDataSet.Tables["exelink"].NewRow();
                                            row_exelink["qstep"] = "complete";
                                            row_exelink["npc"] = npc.Get_NPCName();
                                            row_exelink["sub"] = select.Get_Target();
                                            row_exelink["exelink_Text"] = select.Get_ExeNo();
                                            savingDataSet.Tables["exelink"].Rows.Add(row_exelink);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            #endregion

            #endregion

            #region 저널 정보

            DataRow row_JournalData = savingDataSet.Tables["journal_data"].NewRow();

            row_JournalData["quest_title"] = textBox_KorName.Text;
            row_JournalData["recompense"] = this.txtRecompense.Text.ToString();
            row_JournalData["quest_level"] = textBox_StartLevel.Text;
            row_JournalData["use_item_list"] = this.txtDeleteItem.Text.ToString();

            int difficulty = Convert.ToInt32(textBox_QuestDifficulty.Text);

            switch (comboBox_Type.SelectedIndex)
            {
                //qstate 가 0 = 진행 중, else 완료
                case 0: //메인 퀘스트
                    row_JournalData["quest_mark_type"] = 1;
                    break;
                case 1: //서브 퀘스트
                    row_JournalData["quest_mark_type"] = 10;
                    break;
                case 5: //일일 퀘스트
                    row_JournalData["quest_mark_type"] = 23;
                    break;
                case 4: //호감도 퀘스트
                    row_JournalData["quest_mark_type"] = 15;
                    break;
                case 2: //모험자 퀘스트
                    row_JournalData["quest_mark_type"] = 19;
                    break;
                case 6: // 커스텀 퀘스트
                    row_JournalData["quest_mark_type"] = Int32.Parse(textBox_Questmark.Text);
                    break;
                default:
                    throw new Exception("Quest Type을 설정해 주세요.");
            }

            if (comboBox_Type.SelectedIndex == 0)   row_JournalData["chapterid"] = textBox_Chapter.Text;
            else                                    row_JournalData["chapterid"] = "";

            row_JournalData["journal_data_Id"] = "0";

            savingDataSet.Tables["journal_data"].Rows.Add(row_JournalData);

            int journal_Count = 1;

            foreach (Control control in tabPage_Journal.Controls["panel_TabMainJournal"].Controls)
            {
                Quest_Journal journal = (Quest_Journal)control;

                if (journal.textBox_JournalTitle.Text == "")
                    continue;

                DataRow row_Journal = savingDataSet.Tables["journal"].NewRow();
                
                row_Journal["index"] = journal_Count;
                //row_Journal["title_image"] = journal.Get_Title_Image();                
                row_Journal["contents_image"] = journal.Get_ContentsImage();
                row_Journal["destination_pos"] = journal.Get_Destination_Pos();
                row_Journal["statemark_npcid"] = journal.Get_Statemark_NPC();
                row_Journal["destination_mode"] = journal.getJournalObjectiveHelper();

                int qstate = journal.Get_Statemark_type();

                switch (comboBox_Type.SelectedIndex)
                {
                    //qstate 가 0 = 진행 중, else 완료
                    case 0: //메인 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? 2 : 3;
                        break;
                    case 1: //서브 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? 11 : 12;
                        break;
                    case 5: //일일 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? 24 : 25;
                        break;
                    case 4: //호감도 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? 16 : 17;
                        break;
                    case 2: //모험자 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? 20 : 21;
                        break;
                    case 6: // 커스텀 퀘스트
                        row_Journal["statemark_type"] = qstate == 0 ? Int32.Parse(textBox_Statemark0.Text) : Int32.Parse(textBox_Statemark1.Text);
                        break;
                    default:
                        throw new Exception("Quest Type을 설정해 주세요.");
                }

                row_Journal["trigger_string"] = journal.Get_TriggerString();
                row_Journal["maptool_index"] = journal.Get_MapToolIndex();

                row_Journal["journal_data_Id"] = "0";
                row_Journal["journal_Id"] = journal_Count;

                savingDataSet.Tables["journal"].Rows.Add(row_Journal);

                DataRow row_title = savingDataSet.Tables["title"].NewRow();

                row_title["title_Text"] = "<![CDATA[" + journal.Get_Title() + "]]>";

                if (journal.textBox_JournalTitle.Tag == null)
                {
                    row_title["mid"] = Quest_Mid;
                    journal.textBox_JournalTitle.Tag = Quest_Mid;
                    ++Quest_Mid;
                }
                else
                {
                    row_title["mid"] = journal.textBox_JournalTitle.Tag.ToString();
                    if (Quest_Mid < Convert.ToInt32(journal.textBox_JournalTitle.Tag.ToString()))
                        Quest_Mid = Convert.ToInt32(journal.textBox_JournalTitle.Tag.ToString()) + 1;

                }
                
                
                row_title["journal_Id"] = row_Journal["index"];

                savingDataSet.Tables["title"].Rows.Add(row_title);

                DataRow row_contents = savingDataSet.Tables["contents"].NewRow();

                row_contents["contents_Text"] = journal.Get_Contents();

                if (journal.textBox_JournalContents.Tag == null)
                {
                    row_contents["mid"] = Quest_Mid;
                    journal.textBox_JournalContents.Tag = Quest_Mid;
                    ++Quest_Mid;
                }
                else
                {
                    row_contents["mid"] = journal.textBox_JournalContents.Tag.ToString();
                    if (Quest_Mid < Convert.ToInt32(journal.textBox_JournalContents.Tag.ToString()))
                        Quest_Mid = Convert.ToInt32(journal.textBox_JournalContents.Tag.ToString()) + 1;
                }                
                
                row_contents["journal_Id"] = row_Journal["index"];

                savingDataSet.Tables["contents"].Rows.Add(row_contents);

                DataRow row_destination = savingDataSet.Tables["destination"].NewRow();

                row_destination["destination_Text"] = "<![CDATA[" + journal.Get_Destination() + "]]>";

                if (journal.textBox_JournalObjectiveName.Tag == null)
                {
                    row_destination["mid"] = Quest_Mid;
                    journal.textBox_JournalObjectiveName.Tag = Quest_Mid;
                    ++Quest_Mid;
                }
                else
                {
                    row_destination["mid"] = journal.textBox_JournalObjectiveName.Tag.ToString();
                    if (Quest_Mid < Convert.ToInt32(journal.textBox_JournalObjectiveName.Tag.ToString()))
                        Quest_Mid = Convert.ToInt32(journal.textBox_JournalObjectiveName.Tag.ToString()) + 1;
                    
                }
                
                row_destination["journal_Id"] = row_Journal["index"];

                savingDataSet.Tables["destination"].Rows.Add(row_destination);

                DataRow row_todomsg = savingDataSet.Tables["todo_msg"].NewRow();

                row_todomsg["todo_msg_Text"] = "<![CDATA[" + journal.Get_Journal_Message() + "]]>";

                if (journal.textBox_Message.Tag == null)
                {
                    row_todomsg["mid"] = Quest_Mid;
                    journal.textBox_Message.Tag = Quest_Mid;
                    ++Quest_Mid;
                }
                else
                {
                    row_todomsg["mid"] = journal.textBox_Message.Tag.ToString();
                    if (Quest_Mid < Convert.ToInt32(journal.textBox_Message.Tag.ToString()))
                        Quest_Mid = Convert.ToInt32(journal.textBox_Message.Tag.ToString()) + 1;
                    
                }
                
                row_todomsg["journal_Id"] = row_Journal["index"];
                savingDataSet.Tables["todo_msg"].Rows.Add(row_todomsg);
                ++journal_Count;
            }
            #endregion

            #region MID 중복 오류 체크
            Dictionary<string, int> dic_midcheck = new Dictionary<string,int>();
            Dictionary<string, string> dic_miderror = new Dictionary<string,string>();
            foreach (DataTable table in savingDataSet.Tables)
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
                    int midcount = ++ dic_midcheck[mid];

                    if( 1 >= dic_midcheck[mid] )
                    {
                        continue;
                    }

                    string errmsg = "mid : ";
                    errmsg += mid;
                    errmsg += " count : ";
                    errmsg += midcount;
                    errmsg += "\r\n";

                    if( false == dic_miderror.ContainsKey(mid) )
                    {
                        dic_miderror.Add(mid, errmsg );
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
                foreach( string error in dic_miderror.Values )
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

                savingDataSet.WriteXml(xtw);
            }

            // &lt;, &gt; 처리 루틴 시작
            FileInfo fi = new FileInfo(p_path);
            string str_read = "";

            if (fi.Exists)
            {
                using (StreamReader sr = fi.OpenText())
                {
                    str_read = sr.ReadToEnd();

                    int k = str_read.LastIndexOf("&lt;");

                    str_read = str_read.Replace("&lt;", "<");
                    str_read = str_read.Replace("&gt;", ">");
                    str_read = str_read.Replace("#000000", "#ffffff");

                    // 검정색 변경
                    str_read = str_read.Replace("#000000", "#2b2723");
                    str_read = str_read.Replace("#ffffff", "#2b2723");

                    str_read = str_read.Replace("#ff9345", "#db4f4f");
                    str_read = str_read.Replace("#fff332", "#d5700d");
                    str_read = str_read.Replace("#00ff00", "#62ad01");
                    str_read = str_read.Replace("#8d38c9", "#a32dfe");
                    str_read = str_read.Replace("#75aef5", "#18a5a7");

                    str_read = str_read.Replace("#db4f4f", "#a82424");
                    str_read = str_read.Replace("#d5700d", "#a94407");
                    str_read = str_read.Replace("#62ad01", "#387206");
                    str_read = str_read.Replace("#a32dfe", "#7e12d2");
                    str_read = str_read.Replace("#18a5a7", "#086171");

                    str_read = str_read.Replace("green", "#387206");
                    str_read = str_read.Replace("violet", "#7e12d2");
                }
            }

            using (StreamWriter sw = new StreamWriter(p_path))
            {
                sw.Write(str_read);
            }
            // &lt;, &gt; 처리 루틴 끝           
        }

        private void _saveConditionObject(Control.ControlCollection controlcollection, string qstep)
        {
            foreach (Control con in controlcollection)
            {
                switch (con.Name)
                {
                    case "Condition_Level":
                        Conditions.Condition_Level con_level = (Conditions.Condition_Level)con;
                        DataRow row_level = savingDataSet.Tables["chk"].NewRow();
                        row_level["type"] = "lvl";
                        row_level["chk_Text"] = con_level.textBox_Value.Text;
                        row_level["op"] = con_level.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_level["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_level);
                        break;
                    case "Condition_GeneralItem":
                        Conditions.Condition_GeneralItem con_item = (Conditions.Condition_GeneralItem)con;
                        DataRow row_item = savingDataSet.Tables["chk"].NewRow();
                        row_item["type"] = "item";
                        row_item["chk_Text"] = con_item.textBox_Item.Text;
                        row_item["num"] = con_item.textBox_Value.Text;
                        row_item["op"] = con_item.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_item["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_item);
                        break;
                    case "Condition_SymbolItem":
                        Conditions.Condition_SymbolItem con_symbol = (Conditions.Condition_SymbolItem)con;
                        DataRow row_symbol = savingDataSet.Tables["chk"].NewRow();

                        int index_chksymbol = 0;

                        if (comboBox_CompleteType.SelectedIndex == 0)
                        {
                            row_symbol["type"] = "questitem";
                            index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 300000;
                        }
                        else
                        {
                            row_symbol["type"] = "item";
                            index_chksymbol = Convert.ToInt32(con_symbol.textBox_Item.Text) + 400000;
                        }

                        row_symbol["chk_Text"] = index_chksymbol.ToString();
                        row_symbol["num"] = con_symbol.textBox_Value.Text;
                        row_symbol["op"] = con_symbol.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_symbol["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_symbol);
                        break;
                    case "Condition_QuestItem":
                        Conditions.Condition_QuestItem con_qitem = (Conditions.Condition_QuestItem)con;
                        DataRow row_qitem = savingDataSet.Tables["chk"].NewRow();
                        row_qitem["type"] = "questitem";
                        row_qitem["chk_Text"] = con_qitem.textBox_Item.Text;
                        row_qitem["num"] = con_qitem.textBox_Value.Text;
                        row_qitem["op"] = con_qitem.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_qitem["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_qitem);
                        break;
                    case "Condition_Class":
                        Conditions.Condition_Class con_class = (Conditions.Condition_Class)con;
                        DataRow row_class = savingDataSet.Tables["chk"].NewRow();
                        row_class["type"] = "class";
                        row_class["chk_Text"] = con_class.textBox_Value.Text;
                        row_class["op"] = con_class.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_class["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_class);
                        break;
                    case "Condition_Prob":
                        Conditions.Condition_Prob con_prob = (Conditions.Condition_Prob)con;
                        DataRow row_prob = savingDataSet.Tables["chk"].NewRow();
                        row_prob["type"] = "prob";
                        row_prob["chk_Text"] = con_prob.textBox_Value.Text;
                        row_prob["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_prob);
                        break;
                    case "Condition_Quest":
                        Conditions.Condition_Quest con_quest = (Conditions.Condition_Quest)con;
                        DataRow row_quest = savingDataSet.Tables["chk"].NewRow();
                        row_quest["type"] = "hasquest";
                        row_quest["chk_Text"] = con_quest.textBox_Value.Text;
                        row_quest["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_quest);
                        break;
                    case "Condition_Compelete":
                        Conditions.Condition_Compelete con_comp = (Conditions.Condition_Compelete)con;
                        DataRow row_comp = savingDataSet.Tables["chk"].NewRow();
                        row_comp["type"] = "hascomq";
                        row_comp["chk_Text"] = con_comp.textBox_Value.Text;
                        row_comp["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_comp);
                        break;
                    case "Condition_Inven":
                        Conditions.Condition_Inven con_inven = (Conditions.Condition_Inven)con;
                        DataRow row_inven = savingDataSet.Tables["chk"].NewRow();
                        row_inven["type"] = "inven_eslot";
                        row_inven["chk_Text"] = con_inven.textBox_Value.Text;
                        row_inven["op"] = con_inven.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_inven["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_inven);
                        break;
                    case "Condition_Custum":
                        Conditions.Condition_Custum con_custum = (Conditions.Condition_Custum)con;
                        DataRow row_custom = savingDataSet.Tables["chk"].NewRow();
                        row_custom["type"] = "custom";
                        row_custom["chk_Text"] = con_custum.textBox_Code.Text;
                        row_custom["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_custom);
                        break;
                    case "Condition_QuestCnt":
                        Conditions.Condition_QuestCnt con_qcnt = (Conditions.Condition_QuestCnt)con;
                        DataRow row_qcnt = savingDataSet.Tables["chk"].NewRow();
                        row_qcnt["type"] = "qcount";
                        row_qcnt["chk_Text"] = ".";
                        row_qcnt["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_qcnt);
                        break;
                    case "Condition_StageConstructionLevel":
                        Conditions.Condition_StageConstructionLevel con_stageConstructionLevel = (Conditions.Condition_StageConstructionLevel)con;
                        DataRow row_stageConstructionLevel = savingDataSet.Tables["chk"].NewRow();
                        row_stageConstructionLevel["type"] = "stageConstructionLevel";
                        row_stageConstructionLevel["chk_Text"] = con_stageConstructionLevel.textBox_Value.Text.ToString();
                        row_stageConstructionLevel["op"] = con_stageConstructionLevel.textBox_Oper.Text.Replace('>', ']').Replace('<', '[');
                        row_stageConstructionLevel["conditions_Id"] = qstep;
                        savingDataSet.Tables["chk"].Rows.Add(row_stageConstructionLevel);
                        break;
                }
            }
        }

        private void _saveExecutionObject(Control.ControlCollection controlcollection, string qstep)
        {
            foreach (Control con in controlcollection)
            {
                try
                {
                    switch (con.Name)
                    {
                        case "Exe_QuestStepMod":
                            Executions.Exe_QuestStepMod eq = (Executions.Exe_QuestStepMod)con;
                            DataRow row_exe = savingDataSet.Tables["exe"].NewRow();
                            row_exe["type"] = "setstep";
                            row_exe["exe_Text"] = eq.textBox_Step.Text;
                            row_exe["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_exe);
                            break;
                        case "Exe_JournalMod":
                            Executions.Exe_JournalMod exe_journal = (Executions.Exe_JournalMod)con;
                            DataRow row_journal = savingDataSet.Tables["exe"].NewRow();
                            row_journal["type"] = "setjornal";
                            row_journal["exe_Text"] = exe_journal.textBox_Journal.Text;
                            row_journal["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_journal);
                            break;
                        case "Exe_ItemDel":
                            Executions.Exe_ItemDel exe_itemdel = (Executions.Exe_ItemDel)con;
                            DataRow row_itemdel = savingDataSet.Tables["exe"].NewRow();
                            row_itemdel["type"] = "delitem";
                            row_itemdel["exe_Text"] = exe_itemdel.textBox_Item.Text;
                            row_itemdel["num"] = exe_itemdel.textBox_Count.Text;
                            row_itemdel["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_itemdel);
                            break;
                        case "Exe_SymbolDel":
                            Executions.Exe_SymbolDel exe_symdel = (Executions.Exe_SymbolDel)con;
                            DataRow row_symdel = savingDataSet.Tables["exe"].NewRow();

                            if (comboBox_CompleteType.SelectedIndex == 0)
                            {
                                row_symdel["type"] = "delquestitem";
                            }
                            else
                            {
                                row_symdel["type"] = "delitem";
                            }

                            int index_delsymbol = 0;

                            if (comboBox_CompleteType.SelectedIndex == 0)
                            {
                                index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 300000;
                            }
                            else
                            {
                                index_delsymbol = Convert.ToInt32(exe_symdel.textBox_Item.Text) + 400000;
                            }

                            row_symdel["exe_Text"] = index_delsymbol.ToString();
                            row_symdel["num"] = exe_symdel.textBox_Count.Text;
                            row_symdel["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_symdel);
                            break;
                        case "Exe_QuestItemDel":
                            Executions.Exe_QuestItemDel exe_qidel = (Executions.Exe_QuestItemDel)con;
                            DataRow row_qidel = savingDataSet.Tables["exe"].NewRow();
                            row_qidel["type"] = "delquestitem";
                            row_qidel["exe_Text"] = exe_qidel.textBox_Item.Text;
                            row_qidel["num"] = exe_qidel.textBox_Count.Text;
                            row_qidel["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_qidel);
                            break;
                        case "Exe_ItemAdd":
                            Executions.Exe_ItemAdd exe_itemadd = (Executions.Exe_ItemAdd)con;
                            DataRow row_itemadd = savingDataSet.Tables["exe"].NewRow();
                            row_itemadd["type"] = "additem";
                            row_itemadd["exe_Text"] = exe_itemadd.textBox_Item.Text;
                            row_itemadd["num"] = exe_itemadd.textBox_Count.Text;
                            row_itemadd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_itemadd);
                            break;
                        case "Exe_SymbolAdd":
                            Executions.Exe_SymbolAdd exe_symadd = (Executions.Exe_SymbolAdd)con;
                            DataRow row_symadd = savingDataSet.Tables["exe"].NewRow();

                            if (comboBox_CompleteType.SelectedIndex == 0)
                            {
                                row_symadd["type"] = "addquestitem";
                            }
                            else
                            {
                                row_symadd["type"] = "additem";
                            }

                            int index_addsymbol = 0;

                            if (comboBox_CompleteType.SelectedIndex == 0)
                                index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 300000;
                            else
                                index_addsymbol = Convert.ToInt32(exe_symadd.textBox_Item.Text) + 400000;

                            row_symadd["exe_Text"] = index_addsymbol.ToString();
                            row_symadd["num"] = exe_symadd.textBox_Count.Text;
                            row_symadd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_symadd);
                            break;
                        case "Exe_QuestItemAdd":
                            Executions.Exe_QuestItemAdd exe_qiadd = (Executions.Exe_QuestItemAdd)con;
                            DataRow row_qiadd = savingDataSet.Tables["exe"].NewRow();
                            row_qiadd["type"] = "addquestitem";
                            row_qiadd["exe_Text"] = exe_qiadd.textBox_Item.Text;
                            row_qiadd["num"] = exe_qiadd.textBox_Count.Text;
                            row_qiadd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_qiadd);
                            break;
                        case "Exe_MonsterAdd":
                            Executions.Exe_MonsterAdd exe_monadd = (Executions.Exe_MonsterAdd)con;
                            DataRow row_monadd = savingDataSet.Tables["exe"].NewRow();
                            row_monadd["type"] = "huntmon";
                            row_monadd["exe_Text"] = exe_monadd.textBox_MonID.Text;
                            row_monadd["num"] = exe_monadd.textBox_Count.Text;
                            row_monadd["party"] = exe_monadd.textBox_Party.Text;
                            row_monadd["onexe"] = exe_monadd.textBox_OnExe.Text;
                            row_monadd["exe"] = exe_monadd.textBox_ExeComp.Text;
                            row_monadd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_monadd);
                            break;
                        case "Exe_ItemColAdd":
                            Executions.Exe_ItemColAdd exe_itemcoladd = (Executions.Exe_ItemColAdd)con;
                            DataRow row_itemcoladd = savingDataSet.Tables["exe"].NewRow();
                            row_itemcoladd["type"] = "huntitem";
                            row_itemcoladd["exe_Text"] = exe_itemcoladd.textBox_Item.Text;
                            row_itemcoladd["num"] = exe_itemcoladd.textBox_Count.Text;
                            row_itemcoladd["onexe"] = exe_itemcoladd.textBox_OnExe.Text;
                            row_itemcoladd["exe"] = exe_itemcoladd.textBox_ExeComp.Text;
                            row_itemcoladd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_itemcoladd);
                            break;
                        case "Exe_SymbolColAdd":
                            Executions.Exe_SymbolColAdd exe_symboladd = (Executions.Exe_SymbolColAdd)con;
                            DataRow row_symboladd = savingDataSet.Tables["exe"].NewRow();
                            row_symboladd["type"] = "huntitem";

                            int index_colsymbol = 0;
                            if (comboBox_CompleteType.SelectedIndex == 0)
                                index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 300000;
                            else
                                index_colsymbol = Convert.ToInt32(exe_symboladd.textBox_Symbol.Text) + 400000;

                            row_symboladd["exe_Text"] = index_colsymbol.ToString();
                            row_symboladd["num"] = exe_symboladd.textBox_Count.Text;
                            row_symboladd["onexe"] = exe_symboladd.textBox_OnExe.Text;
                            row_symboladd["exe"] = exe_symboladd.textBox_Exe.Text;
                            row_symboladd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_symboladd);
                            break;
                        case "Exe_AllHunting":
                            Executions.Exe_AllHunting exe_allhunt = (Executions.Exe_AllHunting)con;
                            DataRow row_allhunt = savingDataSet.Tables["exe"].NewRow();
                            row_allhunt["type"] = "huntall";
                            row_allhunt["exe_Text"] = exe_allhunt.textBox_Exe.Text;
                            row_allhunt["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_allhunt);
                            break;
                        case "Exe_AddDailyQuest":
                            Executions.Exe_AddDailyQuest exe_AddDailyQuest = (Executions.Exe_AddDailyQuest)con;
                            DataRow row_addDailyQuest = savingDataSet.Tables["exe"].NewRow();
                            row_addDailyQuest["type"] = "setDailyQuest";
                            row_addDailyQuest["exe_Text"] = exe_AddDailyQuest.txt_QuestNo.Text;
                            row_addDailyQuest["true"] = exe_AddDailyQuest.txt_ExecuteOnSuccess.Text;
                            row_addDailyQuest["false"] = exe_AddDailyQuest.txt_ExecuteOnFail.Text;
                            row_addDailyQuest["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_addDailyQuest);
                            break;
                        case "Exe_QuestAdd":
                            Executions.Exe_QuestAdd exe_questadd = (Executions.Exe_QuestAdd)con;
                            DataRow row_questadd = savingDataSet.Tables["exe"].NewRow();
                            row_questadd["type"] = "setquest";
                            row_questadd["exe_Text"] = exe_questadd.textBox_Item.Text;
                            row_questadd["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_questadd);
                            break;
                        case "Exe_Message":
                            Executions.Exe_Message exe_msg = (Executions.Exe_Message)con;
                            DataRow row_msg = savingDataSet.Tables["exe"].NewRow();
                            row_msg["type"] = "msg";
                            row_msg["exe_Text"] = exe_msg.textBox_Index.Text;
                            row_msg["msg_type"] = exe_msg.textBox_Type.Text;
                            row_msg["substring"] = exe_msg.textBox_Substring.Text;
                            row_msg["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_msg);
                            break;
                        case "Exe_Script":
                            Executions.Exe_Script exe_script = (Executions.Exe_Script)con;
                            DataRow row_script = savingDataSet.Tables["exe"].NewRow();
                            row_script["type"] = "talk";
                            row_script["exe_Text"] = exe_script.textBox_Index.Text;
                            row_script["npc"] = exe_script.textBox_NPC.Text;
                            row_script["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_script);
                            break;
                        case "Exe_ProbChoice":
                            Executions.Exe_ProbChoice exe_choice = (Executions.Exe_ProbChoice)con;
                            DataRow row_choice = savingDataSet.Tables["exe"].NewRow();
                            row_choice["type"] = "random_coice";
                            row_choice["exe_Text"] = exe_choice.textBox_Seed.Text;
                            row_choice["rand"] = exe_choice.textBox_Rand.Text;
                            row_choice["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_choice);
                            break;
                        case "Exe_Condition":
                            Executions.Exe_Condition exe_condition = (Executions.Exe_Condition)con;
                            DataRow row_condition = savingDataSet.Tables["exe"].NewRow();
                            row_condition["type"] = "conexe";
                            row_condition["exe_Text"] = exe_condition.textBox_Condition.Text;
                            row_condition["true"] = exe_condition.textBox_true.Text;
                            row_condition["false"] = exe_condition.textBox_false.Text;
                            row_condition["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_condition);
                            break;
                        case "Exe_MoneyDel":
                            Executions.Exe_MoneyDel exe_moneyDelete = (Executions.Exe_MoneyDel)con;
                            DataRow row_moneyDelete = savingDataSet.Tables["exe"].NewRow();
                            row_moneyDelete["type"] = "moneyDelete";
                            row_moneyDelete["num"] = exe_moneyDelete.txtDeleteMoney.Text;
                            row_moneyDelete["onexe"] = exe_moneyDelete.txtMoreExecute.Text;
                            row_moneyDelete["exe_Text"] = exe_moneyDelete.txtLessExecute.Text;
                            row_moneyDelete["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_moneyDelete);
                            break;
                        case "Exe_QuestComplete":
                            Executions.Exe_QuestComplete exe_questcomp = (Executions.Exe_QuestComplete)con;
                            DataRow row_questcomp = savingDataSet.Tables["exe"].NewRow();
                            row_questcomp["type"] = "complete";
                            row_questcomp["exe_Text"] = exe_questcomp.textBox_Quest.Text;
                            row_questcomp["true"] = exe_questcomp.textBox_QuestComplete.Text;
                            row_questcomp["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_questcomp);
                            break;
                        case "Exe_AddDel":
                            Executions.Exe_AddDel exe_alldel = (Executions.Exe_AddDel)con;
                            DataRow row_alldel = savingDataSet.Tables["exe"].NewRow();
                            row_alldel["type"] = "delcount";
                            row_alldel["exe_Text"] = ".";
                            row_alldel["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_alldel);
                            break;
                        case "Exe_Custum":
                            Executions.Exe_Custum exe_custum = (Executions.Exe_Custum)con;
                            DataRow row_custum = savingDataSet.Tables["exe"].NewRow();
                            row_custum["type"] = "custom";
                            row_custum["exe_Text"] = exe_custum.textBox_Code.Text;
                            row_custum["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_custum);
                            break;
                        case "Exe_Return":
                            Executions.Exe_Return exe_return = (Executions.Exe_Return)con;
                            DataRow row_return = savingDataSet.Tables["exe"].NewRow();
                            row_return["type"] = "return";
                            row_return["exe_Text"] = ".";
                            row_return["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_return);
                            break;
                        case "Exe_FullInven":
                            Executions.Exe_FullInven exe_fullinven = (Executions.Exe_FullInven)con;
                            DataRow row_fullinven = savingDataSet.Tables["exe"].NewRow();
                            row_fullinven["type"] = "fullinven";
                            row_fullinven["exe_Text"] = ".";
                            row_fullinven["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_fullinven);
                            break;
                        case "Exe_FullQuest":
                            Executions.Exe_FullQuest exe_fullquest = (Executions.Exe_FullQuest)con;
                            DataRow row_fullquest = savingDataSet.Tables["exe"].NewRow();
                            row_fullquest["type"] = "fullquest";
                            row_fullquest["exe_Text"] = ".";
                            row_fullquest["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_fullquest);
                            break;
                        case "Exe_Recompense":
                            {
                                Executions.Exe_Recompense exe_recompense = (Executions.Exe_Recompense)con;
                                DataRow row_recompense = savingDataSet.Tables["exe"].NewRow();
                                row_recompense["type"] = "recompense";
                                string exe_Text = exe_recompense.textBox_Index.Text
                                    + "," + exe_recompense.txtRewardWarrior.Text
                                    + "," + exe_recompense.txtRewardArcher.Text
                                    + "," + exe_recompense.txtRewardCleric.Text
                                    + "," + exe_recompense.txtRewardSorcerer.Text
                                    + "," + exe_recompense.txtRewardArcademic.Text
                                    + "," + exe_recompense.txtRewardKally.Text
                                    + "," + exe_recompense.txtRewardAssassin.Text;
                                row_recompense["exe_Text"] = exe_Text;
                                row_recompense["executions_Id"] = qstep;
                                savingDataSet.Tables["exe"].Rows.Add(row_recompense);
                            }
                            break;
                        case "Exe_Recompense_View":
                            {
                                Executions.Exe_Recompense_View exe_recompense_view = (Executions.Exe_Recompense_View)con;
                                DataRow row_recompense_view = savingDataSet.Tables["exe"].NewRow();
                                row_recompense_view["type"] = "recompense_view";
                                string exe_Text = exe_recompense_view.textBox_Index.Text
                                    + "," + exe_recompense_view.txtRewardWarrior.Text
                                    + "," + exe_recompense_view.txtRewardArcher.Text
                                    + "," + exe_recompense_view.txtRewardCleric.Text
                                    + "," + exe_recompense_view.txtRewardSorcerer.Text
                                    + "," + exe_recompense_view.txtRewardArcademic.Text
                                    + "," + exe_recompense_view.txtRewardKally.Text
                                    + "," + exe_recompense_view.txtRewardAssassin.Text;
                                row_recompense_view["exe_Text"] = exe_Text;
                                row_recompense_view["executions_Id"] = qstep;
                                savingDataSet.Tables["exe"].Rows.Add(row_recompense_view);
                            }
                            break;
                        case "Exe_Comment":
                            Executions.Exe_Comment exe_comment = (Executions.Exe_Comment)con;
                            DataRow row_comment = savingDataSet.Tables["exe"].NewRow();
                            row_comment["type"] = "comment";
                            row_comment["exe_Text"] = exe_comment.textBox_Comment.Text;
                            row_comment["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_comment);
                            break;
                        case "Exe_AddItemAndQuestItem":
                            Executions.Exe_AddItemAndQuestItem exe_AddItemAndQuestItem = (Executions.Exe_AddItemAndQuestItem)con;
                            DataRow row_AddItemAndQuestItem = savingDataSet.Tables["exe"].NewRow();
                            row_AddItemAndQuestItem["type"] = "addItemAndQuestItem";
                            //Quest Item | Item 형태로 exe_Text에 추가
                            row_AddItemAndQuestItem["exe_Text"] = exe_AddItemAndQuestItem.txt_Item.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItem.Text;
                            //Quest Item 개수 | Item 개수 형탤고 num에 추가.
                            row_AddItemAndQuestItem["num"] = exe_AddItemAndQuestItem.txt_ItemCount.Text + "|" + exe_AddItemAndQuestItem.txt_QuestItemCount.Text;
                            row_AddItemAndQuestItem["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_AddItemAndQuestItem);
                            break;
                        case "Exe_CheckStageClearRank":
                            Executions.Exe_CheckStageClearRank stageClearRank = (Executions.Exe_CheckStageClearRank)con;
                            DataRow row_CheckStageClearRank = savingDataSet.Tables["exe"].NewRow();
                            row_CheckStageClearRank["type"] = "checkStageClearRank";
                            row_CheckStageClearRank["num"] = stageClearRank.txtStageIndex.Text;
                            row_CheckStageClearRank["rand"] = stageClearRank.txtRank.Text;
                            row_CheckStageClearRank["true"] = stageClearRank.txtInequality.Text.Replace('>', ']').Replace('<', '[');
                            row_CheckStageClearRank["onexe"] = stageClearRank.txtExecute.Text;
                            row_CheckStageClearRank["executions_Id"] = qstep;
                            savingDataSet.Tables["exe"].Rows.Add(row_CheckStageClearRank);
                            break;
                    }
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message + "\n" + e.StackTrace);
                }
            }
        }

        private DataSet _makeDataSet()
        {
            DataSet SaveDataSet = new DataSet();

            #region 데이터 테이블 생성
            DataTable quest_desc = new DataTable("quest_desc");
            DataTable qkname_table = new DataTable("qkname");
            DataTable qnpc = new DataTable("qnpc");
            DataTable talk_paragraph = new DataTable("talk_paragraph");
            DataTable talk_question = new DataTable("talk_question");
            DataTable talk_answer = new DataTable("talk_answer");
            DataTable qstep_change = new DataTable("qstep_change");
            DataTable conditions = new DataTable("conditions");
            DataTable executions = new DataTable("executions");
            DataTable chk = new DataTable("chk");
            DataTable exe = new DataTable("exe");
            DataTable exelink = new DataTable("exelink");
            DataTable journal_data = new DataTable("journal_data");
            DataTable journal = new DataTable("journal");
            DataTable journal_title = new DataTable("title");
            DataTable journal_contents = new DataTable("contents");
            DataTable journal_destination = new DataTable("destination");
            DataTable journal_todomsg = new DataTable("todo_msg");
            DataTable condition_data = new DataTable("condition_data");
            DataTable remotecomplete = new DataTable("remotecomplete");
            #endregion

            #region quest_desc
            //DataColumn qkname = new DataColumn("qkname"); //번역관련 수정
            DataColumn qename = new DataColumn("qename");
            DataColumn qtype = new DataColumn("qtype");
            DataColumn qcomtype = new DataColumn("qcomtype");
            DataColumn qchapter = new DataColumn("qchapter");
            DataColumn qid = new DataColumn("qid");
            DataColumn designer = new DataColumn("designer");
            DataColumn qlevel = new DataColumn("qlevel");
            DataColumn qdesc = new DataColumn("qdesc");
            DataColumn qdifficulty = new DataColumn("qdifficulty");
            DataColumn nextqid = new DataColumn("nextqid");
            DataColumn nextqname = new DataColumn("nextqname");
            DataColumn nextqnpc = new DataColumn("nextqnpc");
            DataColumn quest_desc_Id = new DataColumn("quest_desc_Id");
            quest_desc_Id.ColumnMapping = MappingType.Hidden;
            DataColumn quest_remotecomplete = new DataColumn("qremotecomplete_step");
            DataColumn custommark = new DataColumn("custommark");
            DataColumn uistyle = new DataColumn("uistyle");

            //quest_desc.Columns.Add(qkname); 번역관련 수정
            quest_desc.Columns.Add(qename);
            quest_desc.Columns.Add(qtype);
            quest_desc.Columns.Add(qcomtype);
            quest_desc.Columns.Add(qchapter);
            quest_desc.Columns.Add(qid);
            quest_desc.Columns.Add(designer);
            quest_desc.Columns.Add(qlevel);
            quest_desc.Columns.Add(qdesc);
            quest_desc.Columns.Add(qdifficulty);
            quest_desc.Columns.Add(nextqid);
            quest_desc.Columns.Add(nextqname);
            quest_desc.Columns.Add(nextqnpc);
            quest_desc.Columns.Add(quest_desc_Id);
            quest_desc.Columns.Add(quest_remotecomplete);
            quest_desc.Columns.Add(custommark);
            quest_desc.Columns.Add(uistyle);
            #endregion            

            #region qnpc
            DataColumn npcid = new DataColumn("npcid");
            DataColumn npcename = new DataColumn("npcename");
            DataColumn npckname = new DataColumn("npckname");
            DataColumn npc_quest_desc_Id = new DataColumn("quest_desc_Id");
            npc_quest_desc_Id.ColumnMapping = MappingType.Hidden;

            qnpc.Columns.Add(npcid);
            qnpc.Columns.Add(npcename);
            qnpc.Columns.Add(npckname);
            qnpc.Columns.Add(npc_quest_desc_Id);
            #endregion

            #region qkname(번역관련 추가)
            DataColumn qkname_mid = new DataColumn("mid");
            qkname_mid.ColumnMapping = MappingType.Attribute;
            DataColumn qkname_text = new DataColumn("qkname_Text");
            qkname_text.ColumnMapping = MappingType.SimpleContent;
            DataColumn qkname_id = new DataColumn("quest_desc_Id");
            qkname_id.ColumnMapping = MappingType.Hidden;

            qkname_table.Columns.Add(qkname_mid);
            qkname_table.Columns.Add(qkname_text);
            qkname_table.Columns.Add(qkname_id);
            #endregion

            #region talk_paragraph

            //DataColumn talk_question = new DataColumn("talk_question");
            DataColumn talk_paragraph_Id = new DataColumn("talk_paragraph_Id");
            talk_paragraph_Id.ColumnMapping = MappingType.Hidden;
            DataColumn index = new DataColumn("index");
            index.ColumnMapping = MappingType.Attribute;
            DataColumn talk_image = new DataColumn("image");
            talk_image.ColumnMapping = MappingType.Attribute;
            DataColumn itemIndex = new DataColumn("itemindex");
            itemIndex.ColumnMapping = MappingType.Attribute;
            DataColumn portrait_left = new DataColumn("portrait_left");
            portrait_left.ColumnMapping = MappingType.Attribute;
            DataColumn portrait_right = new DataColumn("portrait_right");
            portrait_right.ColumnMapping = MappingType.Attribute;

            //talk_paragraph.Columns.Add(talk_question);
            talk_paragraph.Columns.Add(talk_paragraph_Id);
            talk_paragraph.Columns.Add(index);
            talk_paragraph.Columns.Add(talk_image);
            talk_paragraph.Columns.Add(itemIndex);
            talk_paragraph.Columns.Add(portrait_left);
            talk_paragraph.Columns.Add(portrait_right);

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

            #region talk_answer(번역 관련 수정)

            DataColumn link_index = new DataColumn("link_index");
            link_index.ColumnMapping = MappingType.Attribute;
            DataColumn link_mid = new DataColumn("mid");
            link_mid.ColumnMapping = MappingType.Attribute;
            DataColumn talk_answer_Text = new DataColumn("talk_answer_Text");
            talk_answer_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn answer_talk_paragraph_Id = new DataColumn("talk_paragraph_Id");
            answer_talk_paragraph_Id.ColumnMapping = MappingType.Hidden;
            DataColumn select_type = new DataColumn("select_type");
            select_type.ColumnMapping = MappingType.Attribute;

            talk_answer.Columns.Add(link_index);
            talk_answer.Columns.Add(link_mid);
            talk_answer.Columns.Add(talk_answer_Text);
            talk_answer.Columns.Add(answer_talk_paragraph_Id);
            talk_answer.Columns.Add(select_type);

            #endregion

            #region qstep_change

            DataColumn qstep_change_Id = new DataColumn("qstep_change_Id");
            qstep_change_Id.ColumnMapping = MappingType.Hidden;
            DataColumn comment = new DataColumn("comment");
            DataColumn qstep = new DataColumn("qstep");
            qstep.ColumnMapping = MappingType.Attribute;

            qstep_change.Columns.Add(qstep_change_Id);
            qstep_change.Columns.Add(comment);
            qstep_change.Columns.Add(qstep);

            #endregion

            #region conditions

            DataColumn conditions_Id = new DataColumn("conditions_Id");
            conditions_Id.ColumnMapping = MappingType.Hidden;
            DataColumn con_qstep_change_Id = new DataColumn("qstep_change_Id");
            con_qstep_change_Id.ColumnMapping = MappingType.Hidden;

            conditions.Columns.Add(conditions_Id);
            conditions.Columns.Add(con_qstep_change_Id);

            #endregion

            #region executions

            DataColumn executions_Id = new DataColumn("executions_Id");
            executions_Id.ColumnMapping = MappingType.Hidden;
            DataColumn exe_qstep_change_Id = new DataColumn("qstep_change_Id");
            exe_qstep_change_Id.ColumnMapping = MappingType.Hidden;

            executions.Columns.Add(executions_Id);
            executions.Columns.Add(exe_qstep_change_Id);

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
            DataColumn rand = new DataColumn("rand");
            rand.ColumnMapping = MappingType.Attribute;
            DataColumn exe_true = new DataColumn("true");
            exe_true.ColumnMapping = MappingType.Attribute;
            DataColumn exe_false = new DataColumn("false");
            exe_false.ColumnMapping = MappingType.Attribute;
            DataColumn substring = new DataColumn("substring");
            substring.ColumnMapping = MappingType.Attribute;
            DataColumn msg_type = new DataColumn("msg_type");
            msg_type.ColumnMapping = MappingType.Attribute;
            DataColumn exe_npc = new DataColumn("npc");
            exe_npc.ColumnMapping = MappingType.Attribute;
            DataColumn num = new DataColumn("num");
            num.ColumnMapping = MappingType.Attribute;
            DataColumn party = new DataColumn("party");
            party.ColumnMapping = MappingType.Attribute;
            DataColumn exes = new DataColumn("exe");
            exes.ColumnMapping = MappingType.Attribute;
            DataColumn onexe = new DataColumn("onexe");
            onexe.ColumnMapping = MappingType.Attribute;
            DataColumn exe_Text = new DataColumn("exe_Text");
            exe_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn exe_executions_Id = new DataColumn("executions_Id");
            exe_executions_Id.ColumnMapping = MappingType.Hidden;

            exe.Columns.Add(type);
            exe.Columns.Add(rand);
            exe.Columns.Add(exe_npc);
            exe.Columns.Add(num);
            exe.Columns.Add(party);
            exe.Columns.Add(exes);
            exe.Columns.Add(onexe);
            exe.Columns.Add(msg_type);
            exe.Columns.Add(substring);
            exe.Columns.Add(exe_true);
            exe.Columns.Add(exe_false);
            exe.Columns.Add(exe_Text);
            exe.Columns.Add(exe_executions_Id);


            #endregion

            #region exelink

            DataColumn exe_qstep = new DataColumn("qstep");
            exe_qstep.ColumnMapping = MappingType.Attribute;
            DataColumn npc = new DataColumn("npc");
            npc.ColumnMapping = MappingType.Attribute;
            DataColumn sub = new DataColumn("sub");
            sub.ColumnMapping = MappingType.Attribute;
            DataColumn exelink_Text = new DataColumn("exelink_Text");
            exelink_Text.ColumnMapping = MappingType.SimpleContent;

            exelink.Columns.Add(exe_qstep);
            exelink.Columns.Add(npc);
            exelink.Columns.Add(sub);
            exelink.Columns.Add(exelink_Text);

            #endregion

            #region remotecomplete

            DataColumn rc_qstep = new DataColumn("qstep");
            rc_qstep.ColumnMapping = MappingType.Attribute;
            DataColumn rc_Text = new DataColumn("rc_Text");
            rc_Text.ColumnMapping = MappingType.SimpleContent;

            remotecomplete.Columns.Add(rc_qstep);
            remotecomplete.Columns.Add(rc_Text);

            #endregion

            #region journal_data

            DataColumn journal_data_Id = new DataColumn("journal_data_Id");
            journal_data_Id.ColumnMapping = MappingType.Hidden;
            DataColumn quest_title = new DataColumn("quest_title");
            quest_title.ColumnMapping = MappingType.Attribute;
            DataColumn quest_recompense = new DataColumn("recompense");
            quest_recompense.ColumnMapping = MappingType.Attribute;
            DataColumn chapterid = new DataColumn("chapterid");
            chapterid.ColumnMapping = MappingType.Attribute;
            DataColumn quest_level = new DataColumn("quest_level");
            quest_level.ColumnMapping = MappingType.Attribute;
            DataColumn quest_mark_type = new DataColumn("quest_mark_type");
            quest_mark_type.ColumnMapping = MappingType.Attribute;
            DataColumn use_item_list = new DataColumn("use_item_list");
            use_item_list.ColumnMapping = MappingType.Attribute;

            journal_data.Columns.Add(journal_data_Id);
            journal_data.Columns.Add(quest_recompense);
            journal_data.Columns.Add(quest_title);
            journal_data.Columns.Add(chapterid);
            journal_data.Columns.Add(quest_level);
            journal_data.Columns.Add(quest_mark_type);
            journal_data.Columns.Add(use_item_list);
            #endregion

            #region journal

            //DataColumn title = new DataColumn("title");
            DataColumn title_image = new DataColumn("title_image");
            //DataColumn contents = new DataColumn("contents");
            DataColumn contents_image = new DataColumn("contents_image");
            DataColumn need_itemlist = new DataColumn("need_itemlist");
            //DataColumn destination = new DataColumn("destination");
            DataColumn destination_pos = new DataColumn("destination_pos");
            DataColumn only_minimap = new DataColumn("destination_mode");

            DataColumn trigger_string = new DataColumn("trigger_string");
            DataColumn maptool_index = new DataColumn("maptool_index");

            DataColumn j_index = new DataColumn("index"); 
            j_index.ColumnMapping = MappingType.Attribute;
            DataColumn j_journal_data_Id = new DataColumn("journal_data_Id");
            j_journal_data_Id.ColumnMapping = MappingType.Hidden;
            DataColumn statemark_npcid = new DataColumn("statemark_npcid");
            DataColumn statemark_type = new DataColumn("statemark_type");
            DataColumn journal_ID = new DataColumn("journal_Id");            
            journal_ID.ColumnMapping = MappingType.Hidden;

            
            journal.Columns.Add(title_image);
            journal.Columns.Add(contents_image);
            journal.Columns.Add(need_itemlist);
            journal.Columns.Add(destination_pos);
            journal.Columns.Add(only_minimap);
            journal.Columns.Add(statemark_npcid);
            journal.Columns.Add(statemark_type);
            journal.Columns.Add(trigger_string);
            journal.Columns.Add(maptool_index);
            // 저널 정보 Export 를 위한 부분 끝

            journal.Columns.Add(j_index);
            journal.Columns.Add(j_journal_data_Id);
            journal.Columns.Add(journal_ID);

            #endregion

            #region journal_title

            DataColumn journal_title_Text = new DataColumn("title_Text");
            journal_title_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn journal_title_mid = new DataColumn("mid");
            journal_title_mid.ColumnMapping = MappingType.Attribute;
            DataColumn journal_title_journal_ID = new DataColumn("journal_Id");
            journal_title_journal_ID.ColumnMapping = MappingType.Hidden;

            journal_title.Columns.Add(journal_title_mid);
            journal_title.Columns.Add(journal_title_Text);
            journal_title.Columns.Add(journal_title_journal_ID);

            #endregion

            #region journal_contents

            DataColumn journal_contents_Text = new DataColumn("contents_Text");
            journal_contents_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn journal_contents_mid = new DataColumn("mid");
            journal_contents_mid.ColumnMapping = MappingType.Attribute;
            DataColumn journal_contents_journal_ID = new DataColumn("journal_Id");
            journal_contents_journal_ID.ColumnMapping = MappingType.Hidden;

            journal_contents.Columns.Add(journal_contents_mid);
            journal_contents.Columns.Add(journal_contents_Text);
            journal_contents.Columns.Add(journal_contents_journal_ID);

            #endregion

            #region journal_destination

            DataColumn journal_dest_Text = new DataColumn("destination_Text");
            journal_dest_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn journal_dest_mid = new DataColumn("mid");
            journal_dest_mid.ColumnMapping = MappingType.Attribute;
            DataColumn journal_dest_journal_ID = new DataColumn("journal_Id");
            journal_dest_journal_ID.ColumnMapping = MappingType.Hidden;

            journal_destination.Columns.Add(journal_dest_Text);
            journal_destination.Columns.Add(journal_dest_mid);
            journal_destination.Columns.Add(journal_dest_journal_ID);

            #endregion

            #region journal_todomsg

            DataColumn journal_msg_Text = new DataColumn("todo_msg_Text");
            journal_msg_Text.ColumnMapping = MappingType.SimpleContent;
            DataColumn journal_msg_mid = new DataColumn("mid");
            journal_msg_mid.ColumnMapping = MappingType.Attribute;
            DataColumn journal_msg_journal_ID = new DataColumn("journal_Id");
            journal_msg_journal_ID.ColumnMapping = MappingType.Hidden;

            journal_todomsg.Columns.Add(journal_msg_Text);
            journal_todomsg.Columns.Add(journal_msg_mid);
            journal_todomsg.Columns.Add(journal_msg_journal_ID);

            #endregion


            #region condition_data

            DataColumn user_level = new DataColumn("user_level");
            DataColumn user_worldzone_level = new DataColumn("user_worldzone_level");
            DataColumn user_class = new DataColumn("user_class");
            DataColumn prev_quest = new DataColumn("prev_quest");
            DataColumn have_quest = new DataColumn("have_quest");
            DataColumn have_normal_item = new DataColumn("have_normal_item");
            DataColumn complete_mission = new DataColumn("complete_mission");
            DataColumn have_symbol_item = new DataColumn("have_symbol_item");
            DataColumn periodConditionOnDayQuest = new DataColumn("quest_date");
            DataColumn weekdayConditionOnDayQuest = new DataColumn("quest_day");
            DataColumn npc_favor = new DataColumn("npc_favor");
            DataColumn npc_malice = new DataColumn("npc_malice");
            DataColumn have_cash_item = new DataColumn("have_cash_item");

            condition_data.Columns.Add(user_level);
            condition_data.Columns.Add(user_worldzone_level);
            condition_data.Columns.Add(user_class);
            condition_data.Columns.Add(prev_quest);
            condition_data.Columns.Add(have_quest);
            condition_data.Columns.Add(have_normal_item);
            condition_data.Columns.Add(complete_mission);
            condition_data.Columns.Add(have_symbol_item);
            condition_data.Columns.Add(periodConditionOnDayQuest);
            condition_data.Columns.Add(weekdayConditionOnDayQuest);
            condition_data.Columns.Add(npc_favor);
            condition_data.Columns.Add(npc_malice);
            condition_data.Columns.Add(have_cash_item);

            #endregion

            SaveDataSet.Tables.Add(quest_desc);
            SaveDataSet.Tables.Add(qkname_table);
            SaveDataSet.Tables.Add(qnpc);
            SaveDataSet.Tables.Add(talk_paragraph);
            SaveDataSet.Tables.Add(talk_question);
            SaveDataSet.Tables.Add(talk_answer);
            SaveDataSet.Tables.Add(qstep_change);
            SaveDataSet.Tables.Add(conditions);
            SaveDataSet.Tables.Add(executions);
            SaveDataSet.Tables.Add(chk);
            SaveDataSet.Tables.Add(exe);
            SaveDataSet.Tables.Add(exelink);
            SaveDataSet.Tables.Add(remotecomplete);
            SaveDataSet.Tables.Add(journal_data);
            SaveDataSet.Tables.Add(journal);
            SaveDataSet.Tables.Add(journal_title);
            SaveDataSet.Tables.Add(journal_contents);
            SaveDataSet.Tables.Add(journal_destination);
            SaveDataSet.Tables.Add(journal_todomsg);
            SaveDataSet.Tables.Add(condition_data);

            #region Relation 생성
            SaveDataSet.Relations.Add("quest_desc_qkname", quest_desc_Id, qkname_id);
            SaveDataSet.Relations["quest_desc_qkname"].Nested = true;

            SaveDataSet.Relations.Add("quest_desc_qnpc", quest_desc_Id, npc_quest_desc_Id);
            SaveDataSet.Relations["quest_desc_qnpc"].Nested = true;

            SaveDataSet.Relations.Add("talk_paragraph_talk_answer", talk_paragraph_Id, answer_talk_paragraph_Id);
            SaveDataSet.Relations["talk_paragraph_talk_answer"].Nested = true;

            SaveDataSet.Relations.Add("talk_paragraph_talk_question", talk_paragraph_Id, talk_question_paragraph_Id);
            SaveDataSet.Relations["talk_paragraph_talk_question"].Nested = true;

            SaveDataSet.Relations.Add("executions_exe", executions_Id, exe_executions_Id);
            SaveDataSet.Relations["executions_exe"].Nested = true;

            SaveDataSet.Relations.Add("conditions_chk", conditions_Id, chk_conditions_Id);
            SaveDataSet.Relations["conditions_chk"].Nested = true;

            SaveDataSet.Relations.Add("qstep_change_conditions", qstep_change_Id, con_qstep_change_Id);
            SaveDataSet.Relations["qstep_change_conditions"].Nested = true;

            SaveDataSet.Relations.Add("qstep_change_executions", qstep_change_Id, exe_qstep_change_Id);
            SaveDataSet.Relations["qstep_change_executions"].Nested = true;

            SaveDataSet.Relations.Add("journal_data_journal", journal_data_Id, j_journal_data_Id);
            SaveDataSet.Relations["journal_data_journal"].Nested = true;

            SaveDataSet.Relations.Add("journal_journal_title", journal_ID, journal_title_journal_ID);
            SaveDataSet.Relations["journal_journal_title"].Nested = true;

            SaveDataSet.Relations.Add("journal_journal_contents", journal_ID, journal_contents_journal_ID);
            SaveDataSet.Relations["journal_journal_contents"].Nested = true;

            SaveDataSet.Relations.Add("journal_journal_destination", journal_ID, journal_dest_journal_ID);
            SaveDataSet.Relations["journal_journal_destination"].Nested = true;

            SaveDataSet.Relations.Add("journal_journal_todomsg", journal_ID, journal_msg_journal_ID);
            SaveDataSet.Relations["journal_journal_todomsg"].Nested = true;
            #endregion

            return SaveDataSet;
        }      

        #endregion

        #region Lua 파일 저장
        // 루아 스크립트 파싱 함수
        //private void SaveLua(string p_Path)
        //{
        //    m_strRemoteStart = "";
        //    Hunt_Count = 0;
        //    string qename = savingDataSet.Tables["quest_desc"].Rows[0]["qename"].ToString(); // 퀘스트 영문이름
        //    string qid = savingDataSet.Tables["quest_desc"].Rows[0]["qid"].ToString(); // 퀘스트 아이디
        //    string next_qename = savingDataSet.Tables["quest_desc"].Rows[0]["nextqname"].ToString(); // 다음 퀘스트 이름
        //    string next_qid = savingDataSet.Tables["quest_desc"].Rows[0]["nextqid"].ToString(); // 다음 퀘스트 아이디
        //    string next_qnpc = savingDataSet.Tables["quest_desc"].Rows[0]["nextqnpc"].ToString(); // 다음 퀘스트 NPC 이름
        //    string str = "";
        //    string str2 = "";
        //    string data = "";
        //    string oncount = "\r\nfunction " + qename + "_OnCounting( userObjID, CountingType, CountIndex, Count, TargetCount, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n";
        //    string oncomplete = "\r\nfunction " + qename + "_OnCompleteCounting( userObjID, CountingType, CountIndex, Count, TargetCount, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n";
        //    string onall = "\r\nfunction " + qename + "_OnCompleteAllCounting( userObjID, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, " + qid + ");\r\n\tlocal questID=" + qid + ";\r\n";

        //    data += "function "+ qename + "_OnTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)";
        //    data += "\r\n-----------------------------------------------------------------------------------------------";
        //    data += "\r\n\r\n\tlocal npcID = api_npc_GetNpcIndex(userObjID, npcObjID);\r\n";

        //    DataRowCollection rows_npc = savingDataSet.Tables["qnpc"].Rows;

        //    foreach (DataRow row_npc in rows_npc)
        //    {
        //        data += "\r\n\tif npcID == " + row_npc["npcid"].ToString() + " then";
        //        data += "\r\n\t\t" + qename + "_OnTalk_" + row_npc["npcename"].ToString() + "(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID);";
        //        data += "\r\n\t\treturn;";
        //        data += "\r\n\tend\r\n";                
        //    }

        //    data += "\r\nend\r\n------------------------------------------------------------------------------------------------\r\n\r\n";

        //    foreach (DataRow row_npc in rows_npc)
        //    {
        //        str += "--" + row_npc["npcename"].ToString() + "--------------------------------------------------------------------------------";
        //        str += "\r\nfunction " + qename + "_OnTalk_" + row_npc["npcename"].ToString() + "(userObjID, npcObjID, npc_talk_index, npc_talk_target, questID)";
        //        str += "\r\n------------------------------------------------------------------------------------------";
        //        str += "\r\n\r\n\tif npc_talk_index == \"!quit\" then";
        //        str += "\r\n\t\treturn;";
        //        str += "\r\n\tend\r\n\tlocal queststep = api_quest_GetQuestStep(userObjID, questID);";
        //        str += "\r\n\r\n\tif npc_talk_index == \"q_enter\" then\r\n";
        //        str2 = "\r\n\t---------------------------------\r\n\telse\r\n\r\n";

        //        #region 퀘스트 없을 단계

        //        DataRow[] rows_noquest = savingDataSet.Tables["talk_paragraph"].Select("index = '" + row_npc["npcename"].ToString() + "-noquest'");

        //        if (rows_noquest.Length > 0)
        //        {
        //            DataRow row_noquest = rows_noquest[0];
        //            str += "\r\n\t\t if api_quest_UserHasQuest(userObjID, questID) <= 0 then";
        //            str += "\r\n\t\t\t npc_talk_index = \"" + row_noquest["index"].ToString() + "\";";
        //            str += "\r\n\t\t\t if  api_quest_IsPlayingQuestMaximum(userObjID) == 1 then ";
        //            str += "\r\n\t\t\t\t npc_talk_index = \"_full_quest\";";
        //            str += "\r\n\t\t\t end ";
        //            str += "\r\n\t\t\t\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
        //            str += "\r\n\t\t end";
        //        }

        //        #endregion

        //        #region 퀘스트 받는 단계

        //        DataRow[] rows_accept = savingDataSet.Tables["talk_paragraph"].Select("index = '" + row_npc["npcename"].ToString() + "-accepting'");

        //        if (rows_accept.Length > 0)
        //        {
        //            int talk_Count = 0;

        //            DataRow row_accept = rows_accept[0];

        //            string[] index = row_accept["index"].ToString().Split('-');

        //            str += "\r\n\t\t if api_quest_UserHasQuest(userObjID, questID) <= 0 then";

        //            DataRow[] rows_exelink = savingDataSet.Tables["exelink"].Select("qstep = 'accepting' and npc = '" + index[0] + "'");

        //            string str_sub = "";

        //            foreach (DataRow row_exelink in rows_exelink)
        //            {                        
        //                if (row_exelink["sub"].ToString() == "")
        //                {
        //                    str += "\r\n\t\t" + WriteExeBlock(row_exelink, "accepting", ref talk_Count, next_qid, next_qnpc, next_qename, false);
        //                }
        //                else
        //                {
        //                    if (str_sub.IndexOf(row_exelink["sub"].ToString() + ":" + row_exelink["exelink_Text"].ToString()) < 0)
        //                    {
        //                        string sub = row_exelink["sub"].ToString().Replace("accepting","");

        //                        if (sub == "")
        //                        {
        //                            str2 += "\tif npc_talk_index == \"" + index[0] + "-accepting\" then\r\n";
        //                            str2 += WriteExeBlock(row_exelink, "accepting", ref talk_Count, next_qid, next_qnpc, next_qename, false) + "\r\n";
        //                            str2 += "\tend\r\n";
        //                        }
        //                        else
        //                        {
        //                            str2 += "\tif npc_talk_index == \"" + index[0] + "-accepting-" + row_exelink["sub"].ToString() + "\" then\r\n";
        //                            str2 += WriteExeBlock(row_exelink, "accepting", ref talk_Count, next_qid, next_qnpc, next_qename, false) + "\r\n";
        //                            str2 += "\tend\r\n";
        //                        }

        //                        str_sub += row_exelink["sub"].ToString() + ":" + row_exelink["exelink_Text"].ToString() + ";";
        //                    }                            
        //                }                        
        //            }

        //            str += "\r\n\t\t\t if  api_quest_IsPlayingQuestMaximum(userObjID) == 1 then ";
        //            str += "\r\n\t\t\t\t npc_talk_index = \"_full_quest\";";
        //            str += "\r\n\t\t\t\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
        //            str += "\r\n\t\t\t\t return; ";
        //            str += "\r\n\t\t\t end ";
        //            if (rows_exelink.Length > 0)
        //            {
        //                str += "\r\n\t\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + index[0] + "-accepting\", npc_talk_target);";
        //            }    
                   
        //            str += "\r\n\t\tend";
        //        }

        //        #endregion

        //        #region 퀘스트 일반 단계

        //        str += "\r\n\t\twhile queststep > -1 do \r\n\t\t---------------------------------\r\n";

        //        DataRow[] rows_general = savingDataSet.Tables["talk_paragraph"].Select("index like '" + row_npc["npcename"].ToString() + "-%' and index not like'%noquest%' and index not like '%accepting%' and index <> '!quit' and index <> 'q_enter' and index <> '_no_quest' and index <> '_full_quest' and index <> '_full_inventory'");

        //        string flag = "";

        //        foreach(DataRow row_general in rows_general)
        //        {
        //            string[] index = row_general["index"].ToString().Split('-');                    

        //            if (index.Length == 2)
        //            {                        
        //                str += "\r\n\t\t\tif api_quest_UserHasQuest(userObjID, questID) > 0 and queststep == " + index[1] + " then\r\n";
        //                str += "\t\t\t\tnpc_talk_index = \"" + row_general["index"].ToString() + "\";\r\n";

        //                DataRow[] rows_exelink = savingDataSet.Tables["exelink"].Select("qstep = '" + index[1] + "' and npc = '" + index[0] + "'");

        //                int talk_count = 0;

        //                foreach (DataRow row_exelink in rows_exelink)
        //                {
        //                    if (row_exelink["sub"].ToString() == "")
        //                    {
        //                        str += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename, false);
        //                    }
        //                    else
        //                    {
        //                        if (row_exelink["sub"].ToString() != "!next")
        //                        {
        //                            int i;
        //                            if (Int32.TryParse(row_exelink["sub"].ToString(), out i))
        //                                str2 += "\tif npc_talk_index == \"" + index[0] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";
        //                            else
        //                                str2 += "\tif npc_talk_index == \"" + index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";

        //                            str2 += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename, false);
        //                            str2 += "\tend\r\n";
        //                        }
        //                        else
        //                        {
        //                            if (flag != index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString())
        //                            {
        //                                flag = index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString();
        //                                str2 += "\tif npc_talk_index == \"" + index[0] + "-" + index[1] + "-" + row_exelink["sub"].ToString() + "\" then \r\n";
        //                                str2 += "\t\tlocal cqresult = 1\r\n";
        //                                str2 += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename, false);
        //                                str2 += "\r\n\t\tif cqresult == 1 then\r\n";
        //                                str2 += "\t\t\tapi_npc_NextScript(userObjID, npcObjID, \"" + next_qnpc + "-1\", \"" + next_qename + ".xml\");\r\n";
        //                                str2 += "\t\t\treturn;\r\n";
        //                                str2 += "\t\tend\r\n";
        //                                str2 += "\tend\r\n";
        //                            }
        //                        }
        //                    }

        //                    if (talk_count == 1)
        //                        str2 += "\treturn;\r\n";
        //                }

                        
        //                str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);";
        //                str += "\r\n\t\t\t\tbreak;\r\n";
        //                str += "\t\t\tend\r\n";
        //            }
        //            else
        //            {
        //                DataRow[] rows_answer = savingDataSet.Tables["talk_answer"].Select("talk_paragraph_Id = '" + row_general["talk_paragraph_Id"] + "'");

        //                foreach (DataRow row_answer in rows_answer)
        //                {
        //                    if (row_answer["link_index"].ToString().IndexOf("!next") > -1)
        //                    {
        //                        if (flag != row_answer["link_index"].ToString())
        //                        {
        //                            flag = row_answer["link_index"].ToString();

        //                            str2 += "\tif npc_talk_index == \"" + row_answer["link_index"].ToString() + "\" then \r\n";

        //                            DataRow[] rows_exelink = savingDataSet.Tables["exelink"].Select("qstep = '" + index[1] + "' and npc = '" + index[0] + "'");

        //                            int talk_count = 0;

        //                            foreach (DataRow row_exelink in rows_exelink)
        //                            {
        //                                if (row_exelink["sub"].ToString() == "!next")
        //                                {
        //                                    str2 += "\r\n\t\tlocal cqresult = 1";
        //                                    str2 += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename, false);
        //                                    str2 += "\r\n\t\tif cqresult == 1 then\r\n";
        //                                    str2 += "\t\t\tapi_npc_NextScript(userObjID, npcObjID, \"" + next_qnpc + "-1\", \"" + next_qename + ".xml\");\r\n";
        //                                    str2 += "\t\t\treturn;\r\n";
        //                                    str2 += "\t\tend\r\n";
        //                                }
        //                                else
        //                                {
        //                                    //str2 += WriteExeBlock(row_exelink, index[1], ref talk_count, next_qid, next_qnpc, next_qename);
        //                                }
        //                            }

        //                            if (talk_count == 1)
        //                                str2 += "\treturn;\r\n";

        //                            str2 += "\tend\r\n";
        //                        }
        //                    }
        //                }
        //            }
        //        }

        //        str += "\r\n\t\t\tqueststep = queststep -1;\r\n\t\tend\r\n\r\n";
        //        str2 += "\r\n\t---------------------------------\r\n";
        //        str2 += "\t api_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);\r\n";
        //        str2 += "\t end\r\n";
        //        #endregion
                
        //        str += str2;
        //        str += "------------------------------------------------------------------------------------------------\r\nend\r\n";
        //    }            
            
        //    data += str;

        //    #region 사냥 처리 루틴
        //    DataRow[] rows_exe = savingDataSet.Tables["exe"].Select("type='huntmon' or type='huntitem' or type='huntsymbol' or type='huntall' or type='checkStageClearRank' ");

        //    int talk_count_blank = 0;

        //    foreach (DataRow row_exe in rows_exe)
        //    {
        //        DataRow[] executions = savingDataSet.Tables["executions"].Select("executions_Id = '" + row_exe["executions_Id"].ToString() + "'");
        //        DataRow[] qstep_change = savingDataSet.Tables["qstep_change"].Select("qstep_change_Id = '" + executions[0]["qstep_change_Id"].ToString() + "'");
        //        switch (row_exe["type"].ToString())
        //        {
        //            case "checkStageClearRank":
        //                {
        //                    oncount += "\t local qstep= " + qstep_change[0]["qstep"].ToString() + "; \r\n";
        //                    oncount += "\t if qstep == " + qstep_change[0]["qstep"].ToString() + " then \r\n";
        //                    oncount += "\t\t if api_user_GetLastStageClearRank( userObjID ) " + row_exe["true"].ToString().Replace("[", "<").Replace("]", ">") + " " + row_exe["rand"].ToString() + " then \r\n";
        //                    oncount += WriteExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "onexe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
        //                    oncount += "\t\t end \r\n";
        //                    oncount += "\t end \r\n";
        //                    break;
        //                }
        //            case "huntmon":
        //            case "huntitem":
        //            case "huntsymbol":
        //                {
        //                    string[] string_monster = row_exe["exe_Text"].ToString().Split(',');
        //                    int monsterCount = row_exe["num"] != null ? Convert.ToInt32(row_exe["num"].ToString()) : 0;

        //                    foreach (string monster in string_monster)
        //                    {
        //                        int temp = 0;

        //                        if (Int32.TryParse(qstep_change[0]["qstep"].ToString(), out temp))
        //                        {
        //                            oncount += "\tif qstep == " + (temp + 1).ToString() + " and CountIndex == " + monster;
        //                            oncount += " then\r\n";
        //                        }
        //                        else
        //                        {
        //                            if (qstep_change[0]["qstep"].ToString() == "accepting")
        //                                oncount += "\tif qstep == 1 and CountIndex == " + monster + " then\r\n";
        //                            else
        //                                oncount += "\tif qstep == " + qstep_change[0]["qstep"].ToString() + " and CountIndex == " + monster + " then\r\n";
        //                        }

        //                        if (row_exe["onexe"].ToString() != "")
        //                        {
        //                            oncount += WriteExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "onexe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
        //                        }

        //                        oncount += "\r\n\tend\r\n";

        //                        if (Int32.TryParse(qstep_change[0]["qstep"].ToString(), out temp))
        //                        {
        //                            oncomplete += "\tif qstep == " + (temp + 1).ToString() + " and CountIndex == " + monster;
        //                            if (row_exe["num"].ToString().Trim().Length > 0)
        //                                oncomplete += " and Count >= TargetCount ";
                                    
        //                            oncomplete += " then\r\n";
        //                        }
        //                        else
        //                        {
        //                            if (qstep_change[0]["qstep"].ToString() == "accepting")
        //                            {
        //                                oncomplete += "\tif qstep == 1 and CountIndex == " + monster;
        //                                if (row_exe["num"].ToString().Trim().Length > 0)
        //                                    oncomplete += " and Count >= TargetCount ";

        //                                oncomplete += " then\r\n";
        //                            }
        //                            else
        //                            {
        //                                oncomplete += "\tif qstep == " + qstep_change[0]["qstep"].ToString() + " and CountIndex == " + monster;
        //                                if (row_exe["num"].ToString().Trim().Length > 0)
        //                                    oncomplete += " and Count >= TargetCount ";
                                        
        //                                oncomplete += " then\r\n";
        //                            }
        //                        }

        //                        if (row_exe["exe"].ToString() != "")
        //                            oncomplete += WriteExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "exe", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);

        //                        oncomplete += "\r\n\tend\r\n";
        //                    }
        //                }
        //                break;
        //            case "huntall":
        //                onall += "\tif qstep == " + (Convert.ToInt16(qstep_change[0]["qstep"].ToString()) + 1).ToString() + " then\r\n";
        //                onall += WriteExeBlock(row_exe, qstep_change[0]["qstep"].ToString(), "all", ref talk_count_blank, next_qid, next_qnpc, next_qename, true);
        //                onall += "\tend\r\n";
        //                break;
        //        }
        //    }
        //    #endregion

        //    oncount = oncount + "end\r\n";
        //    oncomplete = oncomplete + "end\r\n";
        //    onall = onall + "end\r\n";

        //    data += oncount + oncomplete + onall;

        //    DataRow[] rows_remotecomplete = savingDataSet.Tables["remotecomplete"].Select();
        //    if (0 < rows_remotecomplete.Length)
        //    {
        //        int qstep = Convert.ToInt32(rows_remotecomplete[0]["qstep"].ToString());

        //        #region 원격 시작 루틴
        //        string onremotestart = "\r\nfunction " + qename + "_OnRemoteStart( userObjID, questID )\r\n";
        //        onremotestart += m_strRemoteStart;
        //        onremotestart += "end\r\n";
        //        data += onremotestart;
        //        #endregion

        //        #region 원격 완료 루틴
        //        string onremotecomplete = "\r\nfunction " + qename + "_OnRemoteComplete( userObjID, questID )\r\n\tlocal qstep=api_quest_GetQuestStep(userObjID, questID);\r\n";
        //        onremotecomplete += "\tif qstep == " + qstep.ToString() + " then\r\n";
        //        onremotecomplete += WriteExeBlock(rows_remotecomplete[0], qstep.ToString(), "rc_Text", ref talk_count_blank, next_qid, next_qnpc, next_qename, false);
        //        onremotecomplete += "\tend\r\nend\r\n";
        //        data += onremotecomplete;
        //        #endregion

        //        #region 원격 완료 체크 루틴
        //        string canremotecompletestep = "\r\nfunction " + qename + "_CanRemoteCompleteStep( userObjID, questID, questStep )\r\n" + "\tif questStep == " + qstep.ToString() + " then\r\n\t\treturn true;\r\n\tend\r\n\r\n\treturn false;\r\nend\r\n";
        //        data += canremotecompletestep;
        //        #endregion
        //    }

        //    using (FileStream fs = new FileStream(p_Path, FileMode.Create))
        //    {
        //        byte[] data_byte = Encoding.UTF8.GetBytes(replaceScript(ref data));
        //        fs.Write(data_byte, 0, data_byte.Length);
        //    }

        //    // Lua 컴파일
        //    // 20090813 Tag 추가로 인한 complie 로직 삭제
        //    //CompileLua(p_Path);

        //}

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

        /// <summary>
        /// 실행문 파싱 함수(exelink)
        /// 퀘스트 플로우/ 퀫트 각 단계에 서브단계 / 선택지 정보를 Lua Script 생성
        /// </summary>
        /// <param name="p_exelink"></param>
        /// <param name="p_step"></param>
        /// <param name="p_count"></param>
        /// <param name="p_next_qid"></param>
        /// <param name="p_next_npc"></param>
        /// <param name="p_next_qename"></param>
        /// <returns></returns>
        private string WriteExeBlock(DataRow p_exelink, string p_step, ref int p_count, string p_next_qid, string p_next_npc, string p_next_qename, bool b_condition)
        {
            string ret_str = "";

            int count_qitem = 0;
            int count_addqitem = 0;

            if (p_exelink == null || p_exelink["qstep"].ToString() == "complete")
                return "";
            
            string[] array_exelink = p_exelink["exelink_Text"].ToString().Split(',');
            DataRow[] rows_qstep = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_exelink["qstep"].ToString() + "'");
            DataRow row_qstep = rows_qstep[0];
            DataRow[] rows_executions = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_qstep["qstep_change_Id"].ToString() + "'");
            DataRow row_executions = rows_executions[0];
            DataRow[] rows_exe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_executions["executions_Id"].ToString() + "'");

            foreach (string index in array_exelink)
            {
                if (index == "")
                    continue;

                if (Convert.ToInt16(index) > rows_exe.Length-1)
                    continue;

                DataRow row_exe = rows_exe[Convert.ToInt16(index)];

                string str_Quest_Type = "";

                if (comboBox_Type.SelectedIndex == 0)   //main Quest
                    str_Quest_Type = "2";
                else                                //Sub Quest, 모험자 퀘스트, 일일 Quest
                    str_Quest_Type = "1";

                switch (row_exe["type"].ToString())
                {
                    case "setstep":
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString() && false == b_condition)
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";
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
                            if (comboBox_CompleteType.SelectedIndex == 0)
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
                            if (comboBox_CompleteType.SelectedIndex == 0)
                                ret_str += "\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                            else
                                ret_str += "\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
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
                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + " ) == 1 then\r\n";
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
                        if (comboBox_CompleteType.SelectedIndex == 0)
                        {
                            ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + " ) == 1 then\r\n";
                            ret_str += "\t\t\t\tapi_quest_AddQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                            ret_str += "\t\t\t\telse\r\n";
                            ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                            ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                            ret_str += "\t\t\t\t\treturn;\r\n";
                            ret_str += "\t\t\t\tend\r\n";
                        }
                        else
                        {
                            ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + " ) == 1 then\r\n";
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
                            int count_addQuestItem = 0;
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

                                ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem) == 1 then\r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
                                    ++count_qitem;
                                    count_addQuestItem += Convert.ToInt32(sSplitNum[i]);
                                }
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";

                                count_addqitem = count_addqitem + count_addQuestItem;
                            }
                            else
                            {
                                ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ") == 1 then \r\n";
                                ret_str += "\t\t\t\tapi_quest_AddQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        break;
                    case "checkStageClearRank":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                        
                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                        }

                        ++Hunt_Count;
                        break;
                    case "huntmon":
                        string[] array_monster = row_exe["exe_Text"].ToString().Split(',');

                        foreach (string monster in array_monster)
                        {
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                            
                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                            {
                                m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                            }

                            ++Hunt_Count;
                        }                        
                        break;
                    case "huntitem":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        
                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        }

                        ++Hunt_Count;
                        break;
                    case "huntsymbol":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";                        
                        ++Hunt_Count;
                        break;
                    case "huntall":
                        break;
                    case "setDailyQuest":
                        ret_str += "\t\t\t\t if api_quest_AddDailyQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ") == 1 then \r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false);
                        ret_str += "\t\t\t\t else \r\n";
                        if (row_exe["false"] == null || string.IsNullOrEmpty(row_exe["false"].ToString()))
                            ret_str += "\t\t\t\tnpc_talk_index = \"_daily_quest_fail\";";
                        else
                            ret_str += "\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["false"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false);

                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "setquest":
                        ret_str += "\t\t\t\tapi_quest_AddQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ");\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        break;
                    case "msg":
                        ret_str += "\n\t\t\t\tlocal " + "TP_" + p_step + "_" + index[0].ToString().Replace(',', '_') + " = {};\n\t\t\t\tapi_user_UserMessage(userObjID," + row_exe["msg_type"].ToString() + ", " + row_exe["exe_Text"].ToString() + ", " + "TP_" + p_step + "_" + index[0].ToString().Replace(',', '_') + ");\r\n";
                        break;
                    case "talk":

                        int p_temp = 0;

                        if (Int32.TryParse(row_exe["exe_Text"].ToString(), out p_temp))
                        {
                            //ret_str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\", npc_talk_target);";
                            ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            p_count = 1;
                        }
                        else
                        {
                            //ret_str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\", npc_talk_target);";
                            ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            p_count = 1;
                        }

                        ++p_count;
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

                            DataRow[] row_tempqstepchange = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");
                            DataRow[] row_tempexecution = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_tempqstepchange[0]["qstep_change_Id"].ToString() + "'");
                            DataRow[] row_tempexe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_tempexecution[0]["executions_Id"].ToString() + "'");

                            ret_str += "\t" + WriteExeBlock_Detail(row_tempexe, p_step, exe[i], ref p_count, p_next_qid, p_next_npc, p_next_qename, p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false);

                            ret_str += "\t\t\t\tend\r\n";

                            value_prev = value;
                        }

                        break;
                    case "conexe":
                        ret_str += "\t\t\t\tif " + WriteChkBlock(p_step, row_exe["exe_Text"].ToString()) + "then\r\n";

                        if (row_exe["true"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false) + "\r\n";

                        ret_str += "\t\t\t\telse" + "\r\n";

                        if (row_exe["false"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["false"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false) + "\r\n";

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
                        ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["onexe"].ToString(),
                                                                        ref p_count, p_next_qid, p_next_npc, p_next_qename,
                                                                        p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false) + "\r\n";
                        ret_str += "\t\t\t\t else \r\n";
                        ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["exe_Text"].ToString(),
                                                                        ref p_count, p_next_qid, p_next_npc, p_next_qename,
                                                                        p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false) + "\r\n";
                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "complete":
                        // 반복성일 경우 - true, 일회성일 경우 - false
                        // comboBox_CompleteType[0] - 1회성, comboBox_CompleteType[1] - 반복성
                        if (comboBox_CompleteType.SelectedIndex == 0)
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, false);";
                        else
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, true)";

                        ret_str += "\r\n\t\t\t\tif cqresult == 1  then";

                        if (p_next_qid != "")
                        {
                            ret_str += "\r\n\t\t\t\t\tapi_quest_AddQuest(userObjID, " + p_next_qid + ", " + str_Quest_Type + ");";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetQuestStep(userObjID, " + p_next_qid + ", 1);";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetJournalStep(userObjID, " + p_next_qid + ", 1);\r\n";
                        }

                        if ("" != row_exe["true"].ToString())
                        {
                            ret_str += "\r\n\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(),
                                                                        ref p_count, p_next_qid, p_next_npc, p_next_qename,
                                                                        p_exelink["sub"].ToString(), ref count_qitem, ref count_addqitem, false) + "\r\n";
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
                        Hunt_Count = 0;
                        break;
                    case "custom":
                        //커스텀 코드로 api_quest_RewardQuestUser를 넣었고 questID가 없을 경우 questID를 넣어준.
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
                                for( int i=0; i<classesReward.Length; i++ ) {
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
                                ret_str += "\t\t\t end \r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tapi_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", false);\r\n";
                            }
                        }
                        break;
                    case "recompense":
                        //Class 별 보상창 출력 구현 20091030
                        {
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
                                //ret_str += "\t\t\t\t if api_quest_CheckInvenForReward( userObjID, " + classesReward[1] + " ) == 1 then \r\n";
                                ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[1] + ", true);\r\n";
                                ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[1] + ", questID, 1);\r\n";
                                //ret_str += "\t\t\t\t else \r\n ";
                                //ret_str += "\t\t\t\t\tnpc_talk_index = \"_full_inventory\";\r\n";
                                //ret_str += "\t\t\t\t end \r\n ";

                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 2 then \r\n";
                                ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[2] + ", true);\r\n";
                                ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[2] + ", questID, 1);\r\n";

                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 3 then \r\n";
                                ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[3] + ", true);\r\n";
                                ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[3] + ", questID, 1);\r\n";

                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 4 then \r\n";
                                ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[4] + ", true);\r\n";
                                ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[4] + ", questID, 1);\r\n";

                                if (classesReward.Length > 5)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 5 then \r\n";
                                    ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[5] + ", true);\r\n";
                                    ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[5] + ", questID, 1);\r\n";
                                }
                                if (classesReward.Length > 6)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 6 then \r\n";
                                    ret_str += "\t\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[6] + ", true);\r\n";
                                    ret_str += "\t\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[6] + ", questID, 1);\r\n";
                                }
                                ret_str += "\t\t\t end \r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tapi_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", true);\r\n";
                                ret_str += "\t\t\t\tapi_quest_RewardQuestUser(userObjID, " + row_exe["exe_Text"].ToString() + ", questID, 1);\r\n";
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

                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItemList(userObjID, TableItem) == 1 " 
                                    + " and api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem2) == 1 then\r\n";

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
                                ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID , " 
                                    + sQuestItemsOrItems[0] + " ," + sQuestItemsNumOrItemsNum[0] 
                                    + " ) == 1 and api_quest_CheckQuestInvenForAddItem( userObjID, " + sQuestItemsOrItems[1] + ", " 
                                    + sQuestItemsNumOrItemsNum[1] + " ) == 1 then\r\n";
                                ret_str += "\t\t\t\t\tapi_user_AddItem(userObjID, " + sQuestItemsOrItems[0] + ", " + sQuestItemsNumOrItemsNum[0] + ", questID);\r\n";
                                ret_str += "\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sQuestItemsOrItems[1] + ", " + sQuestItemsNumOrItemsNum[1] + ", questID);\r\n";
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

        /// <summary>
        /// 퀘스트 플로우/ 각 단계에 조건 실행문 Lua Script 생성
        /// 실행문 파싱 함수(exe)
        /// </summary>
        /// <param name="p_exe"></param>
        /// <param name="p_step"></param>
        /// <param name="p_type"></param>
        /// <param name="p_count"></param>
        /// <param name="p_next_qid"></param>
        /// <param name="p_next_npc"></param>
        /// <param name="p_next_qename"></param>
        /// <returns></returns>
        private string WriteExeBlock(DataRow p_exe, string p_step, string p_type, ref int p_count, string p_next_qid, string p_next_npc, string p_next_qename, bool b_condition)
        {
            string ret_str = "";

            int count_qitem = 0;
            int count_addqitem = 0;

            if (p_exe == null)
                return "";

            ////Quest 수행 중일 때,
            //string[] array_exe = p_exe["exe"].ToString().Split(',');
            ////Quest 수행 다 되었을 때
            //string[] array_onexe = p_exe["onexe"].ToString().Split(',');
            ////
            //string[] array_text = p_exe["exe_Text"].ToString().Split(',');

            //string[] array_select;

            //if (p_type == "onexe")      array_select = array_onexe;
            //else if (p_type == "exe")   array_select = array_exe;
            //else if (p_type == "all")   array_select = array_text;
            //else                        array_select = array_text;

            /////////////////////

            string[] array_select;

            switch (p_type)
            {
                case "onexe":
                    array_select = p_exe["onexe"].ToString().Split(',');
                    break;
                case "exe":
                    array_select = p_exe["exe"].ToString().Split(',');
                    break;
                case "all":
                    array_select = p_exe["exe_Text"].ToString().Split(',');
                    break;
                case"rc_Text":
                    array_select = p_exe["rc_Text"].ToString().Split(',');
                    break;
                default:
                    array_select = p_exe["exe_Text"].ToString().Split(',');
                    break;
            }

            DataRow[] rows_qstep = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");

            DataRow row_qstep = rows_qstep[0];

            DataRow[] rows_executions = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_qstep["qstep_change_Id"].ToString() + "'");

            DataRow row_executions = rows_executions[0];

            DataRow[] rows_exe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_executions["executions_Id"].ToString() + "'");

            string str_Quest_Type = "";

            if (comboBox_Type.SelectedIndex == 0) //main Quest
                str_Quest_Type = "2";
            else                        //subQuest, 모험자 quest, 일일 Quest
                str_Quest_Type = "1";
            
            foreach (string index in array_select)
            {
                if (Convert.ToInt16(index) < rows_exe.Length)
                {
                    DataRow row_exe = rows_exe[Convert.ToInt16(index)];

                    switch (row_exe["type"].ToString())
                    {
                        case "setstep":
                            ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";

                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString() && false == b_condition)
                            {
                                m_strRemoteStart += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";
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
                                if (comboBox_CompleteType.SelectedIndex == 0)
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
                                if (comboBox_CompleteType.SelectedIndex == 0)
                                    ret_str += "\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                                else
                                    ret_str += "\t\t\t\tapi_user_DelItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
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
                                    ret_str += "\t\t\t\tif api_user_CheckInvenForAddItem(userObjID , " + row_exe["exe_Text"].ToString()  + ", " + row_exe["num"].ToString() + " ) == 1 then\r\n";
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
                            if (comboBox_CompleteType.SelectedIndex == 0)
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

                                    ret_str += "\t\t\t\t local TableItem = { ";
                                    for (int i = 0; i < sSplitItem.Length; i++)
                                    {
                                        ret_str += "\t\t\t\t { " + sSplitItem[i] + ", " + sSplitNum[i] + " } ";
                                        ret_str += (sSplitItem.Length - 1 > i) ? " , \r\n" : " \r\n";
                                    }

                                    ret_str += " \t\t\t\t} \r\n";

                                    ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem) == 1 then\r\n";
                                    for (int i = 0; i < sSplitItem.Length; i++)
                                    {
                                        ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
                                        ++count_qitem;
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
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                            
                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                            {
                                m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                            }

                            ++Hunt_Count;
                            break;
                        case "huntmon":
                            string[] array_monster = row_exe["exe_Text"].ToString().Split(',');

                            foreach (string monster in array_monster)
                            {
                                ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                                
                                if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                                {
                                    m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                                }

                                ++Hunt_Count;
                            }
                            
                            break;
                        case "huntitem":
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                            
                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                            {
                                m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                            }

                            ++Hunt_Count;
                            break;
                        case "huntsymbol":
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";                            
                            ++Hunt_Count;
                            break;
                        case "huntall":
                            string exe_all = row_exe["exe_Text"].ToString();

                            ret_str += "\t\t\t\tif qstep == " + p_step + " then\r\n";

                            DataRow[] row_allqstepchange = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");
                            DataRow[] row_allexecution = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_allqstepchange[0]["qstep_change_Id"].ToString() + "'");
                            DataRow[] row_allexe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_allexecution[0]["executions_Id"].ToString() + "'");

                            ret_str += "\t" + WriteExeBlock_Detail(row_allexe, p_step, exe_all, ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, true);

                            ret_str += "\t\t\t\tend\r\n";
                            break;
                        case "setDailyQuest":
                            ret_str += "\t\t\t\t if api_quest_AddDailyQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ") == 1 then \r\n";
                            ret_str += "\t\t\t\t\t api_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                            ret_str += "\t\t\t\t\t api_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                            ret_str += "\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false);
                            ret_str += "\t\t\t\t else \r\n";
                            if (row_exe["false"] == null || string.IsNullOrEmpty(row_exe["false"].ToString()))
                            {
                                ret_str += "\t\t\t\tnpc_talk_index = \"_daily_quest_fail\";";
                            }
                            else
                            {
                                ret_str += "\t" 
                                    + WriteExeBlock_Detail(rows_exe, p_step, row_exe["false"].ToString(),
                                        ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false);
                            }
                            ret_str += "\t\t\t\t end \r\n";
                            break;
                        case "setquest":
                            ret_str += "\t\t\t\tapi_quest_AddQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ");\r\n";
                            ret_str += "\t\t\t\tapi_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                            ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                            break;
                        case "msg":
                            ret_str += "\n\t\t\t\tlocal " + "TP_" + p_step + "_" + index[0].ToString().Replace(',', '_') + " = {};\n\t\t\t\tapi_user_UserMessage(userObjID," + row_exe["msg_type"].ToString() + ", " + row_exe["exe_Text"].ToString() + ", " + "TP_" + p_step + "_" + index[0].ToString().Replace(',', '_') + ");\r\n";
                            break;
                        case "talk":
                            int p_temp = 0;

                            if (Int32.TryParse(row_exe["exe_Text"].ToString(), out p_temp))
                            {
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                                p_count = 1;
                            }
                            else
                            {
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                                p_count = 1;
                            }

                            ++p_count;
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

                                DataRow[] row_tempqstepchange = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");
                                DataRow[] row_tempexecution = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_tempqstepchange[0]["qstep_change_Id"].ToString() + "'");
                                DataRow[] row_tempexe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_tempexecution[0]["executions_Id"].ToString() + "'");

                                ret_str += "\t" + WriteExeBlock_Detail(row_tempexe, p_step, exe[i], ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false);

                                ret_str += "\t\t\t\tend\r\n";

                                value_prev = value;
                            }
                            break;
                        case "conexe":
                            ret_str += "\t\t\t\tif " + WriteChkBlock(p_step, row_exe["exe_Text"].ToString()) + "then\r\n";

                            if (row_exe["true"].ToString() != "")
                            {
                                ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false) + "\r\n";
                            }

                            ret_str += "\t\t\t\telse" + "\r\n";

                            if (row_exe["false"].ToString() != "")
                            {
                                ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["false"].ToString(), ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false) + "\r\n";
                            }

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
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["onexe"].ToString(),
                                                                            ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false) + "\r\n";
                            ret_str += "\t\t\t\t else \r\n";
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["exe_Text"].ToString(),
                                                                            ref p_count, p_next_qid, p_next_npc, p_next_qename, "1", ref count_qitem, ref count_addqitem, false) + "\r\n";
                            ret_str += "\t\t\t\t end \r\n";
                            break;
                        case "complete":
                            // 반복성일 경우 - true, 일회성일 경우 - false
                            // comboBox_CompleteType[0] - 1회성, comboBox_CompleteType[1] - 반복성
                            if (comboBox_CompleteType.SelectedIndex == 0)
                                ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, false);";
                            else
                                ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, true);";

                            ret_str += "\r\n\t\t\t\tif cqresult == 1 then";
                            
                            if (p_next_qid != "")
                            {
                                ret_str += "\r\n\t\t\t\tapi_quest_AddQuest(userObjID, " + p_next_qid + ", " + str_Quest_Type + ");";
                                ret_str += "\r\n\t\t\t\tapi_quest_SetQuestStep(userObjID, " + p_next_qid + ", 1);";
                                ret_str += "\r\n\t\t\t\tapi_quest_SetJournalStep(userObjID, " + p_next_qid + ", 1);";
                                ret_str += "\r\n";
                            }

                            if ("" != row_exe["true"].ToString())
                            {
                                ret_str += "\r\n\t\t\t\t" + WriteExeBlock_Detail(rows_exe, p_step, row_exe["true"].ToString(),
                                                                            ref p_count, p_next_qid, p_next_npc, p_next_qename,
                                                                            "1", ref count_qitem, ref count_addqitem, false) + "\r\n";
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
                            //커스텀 코드로 api_quest_RewardQuestUser를 넣었고 questID가 없을 경우 questID를 넣어준다
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
                            //ret_str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);\r\n";
                            break;
                        case "fullquest":
                            ret_str += "\t\t\t\tnpc_talk_index = \"_full_quest\";\r\n";
                            //ret_str += "\t\t\t\tapi_npc_NextTalk(userObjID, npcObjID, npc_talk_index, npc_talk_target);\r\n";
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
                                        {
                                            classesReward[i] = classesReward[0];
                                        }
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
                                bool bRemoteComplete = false;
                                if ("rc_Text" == p_type)
                                {
                                    bRemoteComplete = true;
                                }

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
                                    if (false == bRemoteComplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[1] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[1] + ", questID, 1);\r\n";
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 2 then \r\n";
                                    if (false == bRemoteComplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[2] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[2] + ", questID, 1);\r\n";
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 3 then \r\n";
                                    if (false == bRemoteComplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[3] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[3] + ", questID, 1);\r\n";
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 4 then \r\n";
                                    if (false == bRemoteComplete)
                                    {
                                        ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[4] + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[4] + ", questID, 1);\r\n";
                                    if (classesReward.Length > 5)
                                    {
                                        ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 5 then \r\n";
                                        if (false == bRemoteComplete)
                                        {
                                            ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[5] + ", true);\r\n";
                                        }
                                        ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[5] + ", questID, 1);\r\n";
                                    }
                                    if (classesReward.Length > 6)
                                    {
                                        ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 6 then \r\n";
                                        if (false == bRemoteComplete)
                                        {
                                            ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[6] + ", true);\r\n";
                                        }
                                        ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[6] + ", questID, 1);\r\n";
                                    }

                                    ret_str += "\t\t\t end \r\n";
                                }
                                else
                                {
                                    if (false == bRemoteComplete)
                                    {
                                        ret_str += "\t\t\t\tapi_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", true);\r\n";
                                    }
                                    ret_str += "\t\t\t\tapi_quest_RewardQuestUser(userObjID, " + row_exe["exe_Text"].ToString() + ", questID, 1);\r\n";
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

                                    ++count_qitem;
                                    count_addqitem = count_addqitem + Convert.ToInt32(row_exe["num"]);
                                }
                            }
                            break;
                    }
                }
            }

            return ret_str;

        }

        /// <summary>
        /// 실행문 파싱 함수(exelink배열)
        /// </summary>
        /// <param name="p_exelink"></param>
        /// <param name="p_step"></param>
        /// <param name="p_count"></param>
        /// <param name="p_talk"></param>
        /// <param name="p_next_qid"></param>
        /// <param name="p_next_npc"></param>
        /// <param name="p_next_qename"></param>
        /// <param name="talk_flag"></param>
        /// <param name="qitem_Cnt"></param>
        /// <param name="addqitem_Cnt"></param>
        /// <returns></returns>
        private string WriteExeBlock_Detail(DataRow[] p_exelink, string p_step, string p_count,
            ref int p_talk, string p_next_qid, string p_next_npc, string p_next_qename,
            string talk_flag, ref int qitem_Cnt, ref int addqitem_Cnt, bool b_condition)
        {
            string ret_str = "";

            if (p_exelink == null)
            {
                return "";
            }

            string[] arr_index = p_count.Split(',');

            foreach (string index in arr_index)
            {
                if (Convert.ToInt16(index) >= p_exelink.Length)
                {
                    continue;
                }

                DataRow row_exe = p_exelink[Convert.ToInt16(index)];

                string str_Quest_Type = "";

                if (comboBox_Type.SelectedIndex == 0)       //메인 퀘스트
                    str_Quest_Type = "2";
                else                                        //일일 퀘스트, 서브퀘스트, 모험자 퀘스트
                    str_Quest_Type = "1";

                int rcstep = 0;
                DataRow[] rows_remotecomplete = savingDataSet.Tables["remotecomplete"].Select();
                if (0 < rows_remotecomplete.Length)
                {
                    rcstep = Convert.ToInt32(rows_remotecomplete[0]["qstep"].ToString());
                }

                switch (row_exe["type"].ToString())
                {
                    case "setstep":
                        //if (0 < rcstep && rcstep == Convert.ToInt32(row_exe["exe_Text"].ToString()))
                        //{
                        //    ret_str += "\t\t\t\tapi_quest_EnableRemoteComplete(userObjID, questID);\r\n";
                        //}
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString() && false == b_condition)
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetQuestStep(userObjID, questID," + row_exe["exe_Text"].ToString() + ");\r\n";
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
                            if (comboBox_CompleteType.SelectedIndex == 0)
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
                            //변환 코드 시작
                            if (comboBox_CompleteType.SelectedIndex == 0)
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
                            ret_str += "\t\t\t\tif api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1) > 0 then \r\n";
                            ret_str += "\t\t\t\t\tapi_quest_DelQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", api_user_HasItem(userObjID, " + row_exe["exe_Text"].ToString() + ", 1));\r\n";
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
                        if (comboBox_CompleteType.SelectedIndex == 0)
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

                        ++qitem_Cnt;
                        addqitem_Cnt = addqitem_Cnt + Convert.ToInt32(row_exe["num"].ToString());
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

                                    ++qitem_Cnt;
                                    addqitem_Cnt = addqitem_Cnt + Convert.ToInt32(sSplitNum[i].Trim());
                                }

                                ret_str += " \t\t\t\t} \r\n";

                                ret_str += "\t\t\t\tif api_quest_CheckQuestInvenForAddItemList(userObjID, TableItem) == 1 then\r\n";
                                for (int i = 0; i < sSplitItem.Length; i++)
                                {
                                    ret_str += "\t\t\t\t\tapi_quest_AddQuestItem(userObjID, " + sSplitItem[i] + ", " + sSplitNum[i] + ", questID);\r\n";
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
                                ret_str += "\t\t\t\tapi_quest_AddQuestItem(userObjID, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ", questID);\r\n";
                                ret_str += "\t\t\t\telse\r\n";
                                ret_str += "\t\t\t\t\tlocal Ts = {};\r\n";
                                ret_str += "\t\t\t\t\tapi_user_UserMessage(userObjID,6,1200101, Ts);\r\n";
                                ret_str += "\t\t\t\t\treturn;\r\n";
                                ret_str += "\t\t\t\tend\r\n";
                            }
                        }
                        break;
                    case "checkStageClearRank":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 5, " + row_exe["num"].ToString() + ", 1 );\r\n";
                        }

                        ++Hunt_Count;
                        break;
                    case "huntmon":
                        string[] array_monster = row_exe["exe_Text"].ToString().Split(',');

                        foreach (string monster in array_monster)
                        {
                            ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";

                            if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                            {
                                m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", " + (Convert.ToInt16(row_exe["party"].ToString()) + 1).ToString() + ", " + monster + ", " + row_exe["num"].ToString() + ");\r\n";
                            }

                            ++Hunt_Count;
                        }
                        break;
                    case "huntitem":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";

                        if (null != row_exe["executions_id"] && "accepting" == row_exe["executions_id"].ToString())
                        {
                            m_strRemoteStart += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        }

                        ++Hunt_Count;
                        break;
                    case "huntsymbol":
                        ret_str += "\t\t\t\tapi_quest_SetCountingInfo(userObjID, questID, " + (Hunt_Count % 30).ToString() + ", 3, " + row_exe["exe_Text"].ToString() + ", " + row_exe["num"].ToString() + ");\r\n";
                        ++Hunt_Count;
                        break;
                    case "huntall":
                        break;
                    case "setDailyQuest":
                        ret_str += "\t\t\t\t if api_quest_AddDailyQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ") == 1 then \r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\t\t api_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        //if (1 == rcstep)
                        //{
                        //    ret_str += "\t\t\t\t\t api_quest_EnableRemoteComplete(userObjID, " + row_exe["exe_Text"].ToString() + ");\r\n";
                        //}
                        ret_str += "\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["true"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false);
                        ret_str += "\t\t\t\t else \r\n";
                        if (row_exe["false"] == null || string.IsNullOrEmpty(row_exe["false"].ToString()))
                            ret_str += "\t\t\t\tnpc_talk_index = \"_daily_quest_fail\";";
                        else
                            ret_str += "\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["false"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false);

                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "setquest":
                        ret_str += "\t\t\t\tapi_quest_AddQuest(userObjID," + row_exe["exe_Text"].ToString() + ", " + str_Quest_Type + ");\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetJournalStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        ret_str += "\t\t\t\tapi_quest_SetQuestStep(userObjID," + row_exe["exe_Text"].ToString() + ", 1);\r\n";
                        //if (1 == rcstep)
                        //{
                        //    ret_str += "\t\t\t\tapi_quest_EnableRemoteComplete(userObjID, " + row_exe["exe_Text"].ToString() + ");\r\n";
                        //}
                        break;
                    case "msg":
                        ret_str += "\n\t\t\t\tlocal " + "TP_" + p_step + "_" + p_count.Replace(',', '_') + " = {};\n\t\t\t\tapi_user_UserMessage(userObjID," + row_exe["msg_type"].ToString() + ", " + row_exe["exe_Text"].ToString() + ", " + "TP_" + p_step + "_" + p_count.Replace(',', '_') + ");\r\n";
                        break;
                    case "talk":
                        int p_temp = 0;

                        if (Int32.TryParse(row_exe["exe_Text"].ToString(), out p_temp))
                        {
                            if (talk_flag == "")
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            else
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                        }
                        else
                        {
                            if (talk_flag == "")
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
                            else
                                ret_str += "\t\t\t\tnpc_talk_index = \"" + row_exe["npc"].ToString() + "-" + p_step.ToString() + "-" + row_exe["exe_Text"].ToString() + "\";";
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

                            DataRow[] row_tempqstepchange = savingDataSet.Tables["qstep_change"].Select("qstep = '" + p_step + "'");
                            DataRow[] row_tempexecution = savingDataSet.Tables["executions"].Select("qstep_change_Id = '" + row_tempqstepchange[0]["qstep_change_Id"].ToString() + "'");
                            DataRow[] row_tempexe = savingDataSet.Tables["exe"].Select("executions_Id = '" + row_tempexecution[0]["executions_Id"].ToString() + "'");

                            ret_str += "\t" + WriteExeBlock_Detail(row_tempexe, p_step, exe[i], ref p_talk, p_next_qid, p_next_npc, p_next_qename, talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false);

                            ret_str += "\t\t\t\tend\r\n";

                            value_prev = value;
                        }

                        break;
                    case "conexe":
                        ret_str += "\t\t\t\tif " + WriteChkBlock(p_step, row_exe["exe_Text"].ToString()) + "then\r\n";

                        if (row_exe["true"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["true"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false) + "\r\n";

                        ret_str += "\t\t\t\telse" + "\r\n";

                        if (row_exe["false"].ToString() != "")
                            ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["false"].ToString(), ref p_talk, p_next_qid, p_next_npc, p_next_qename, talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false) + "\r\n";

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
                        ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["onexe"].ToString(),
                                                                        ref p_talk, p_next_qid, p_next_npc, p_next_qename,
                                                                        talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false) + "\r\n";
                        ret_str += "\t\t\t\t else \r\n";
                        ret_str += "\t\t\t\t\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["exe_Text"].ToString(),
                                                                        ref p_talk, p_next_qid, p_next_npc, p_next_qename,
                                                                        talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false) + "\r\n";
                        ret_str += "\t\t\t\t end \r\n";
                        break;
                    case "complete":
                        // 반복성일 경우 - true, 일회성일 경우 - false
                        // comboBox_CompleteType[0] - 1회성, comboBox_CompleteType[1] - 반복성
                        if (comboBox_CompleteType.SelectedIndex == 0)
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, false);";
                        else
                            ret_str += "\r\n\t\t\t\tcqresult = api_quest_CompleteQuest(userObjID, questID, true, true);";

                        ret_str += "\r\n\t\t\t\tif cqresult == 1 then";

                        if (p_next_qid != "")
                        {
                            ret_str += "\r\n\t\t\t\t\tapi_quest_AddQuest(userObjID, " + p_next_qid + ", " + str_Quest_Type + ");";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetQuestStep(userObjID, " + p_next_qid + ", 1);";
                            ret_str += "\r\n\t\t\t\t\tapi_quest_SetJournalStep(userObjID, " + p_next_qid + ", 1);\r\n";
                        }

                        if ("" != row_exe["true"].ToString())
                        {
                            ret_str += "\r\n\t\t\t\t\t" + WriteExeBlock_Detail(p_exelink, p_step, row_exe["exe_Text"].ToString(),
                                                                        ref p_talk, p_next_qid, p_next_npc, p_next_qename,
                                                                        talk_flag, ref qitem_Cnt, ref addqitem_Cnt, false) + "\r\n";
                        }

                        ret_str += "\r\n\t\t\t\telse";
                        ret_str += "\r\n\t\t\t\t\tnpc_talk_index = \"_full_inventory\";\r\n";
                        ret_str += "\r\n\t\t\t\tend";
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
                                        classesReward[i] = classesReward[0];
                                }
                                ret_str += "\t\t\t if api_user_GetUserClassID(userObjID) == 1 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[1] + ", true);\r\n";
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[1] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 2 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[2] + ", true);\r\n";
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[2] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 3 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[3] + ", true);\r\n";
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[3] + ", questID, 1);\r\n";
                                ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 4 then \r\n";
                                ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[4] + ", true);\r\n";
                                ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[4] + ", questID, 1);\r\n";
                                if (classesReward.Length > 5)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 5 then \r\n";
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[5] + ", true);\r\n";
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[5] + ", questID, 1);\r\n";
                                }
                                if (classesReward.Length > 6)
                                {
                                    ret_str += "\t\t\t elseif api_user_GetUserClassID(userObjID) == 6 then \r\n";
                                    ret_str += "\t\t\t\t api_ui_OpenQuestReward(userObjID, " + classesReward[6] + ", true);\r\n";
                                    ret_str += "\t\t\t\t api_quest_RewardQuestUser(userObjID, " + classesReward[6] + ", questID, 1);\r\n";
                                }

                                ret_str += "\t\t\t end \r\n";
                            }
                            else
                            {
                                ret_str += "\t\t\t\tapi_ui_OpenQuestReward(userObjID, " + row_exe["exe_Text"].ToString() + ", true);\r\n";
                                ret_str += "\t\t\t\tapi_quest_RewardQuestUser(userObjID, " + row_exe["exe_Text"].ToString() + ", questID, 1);\r\n";
                            }
                        }
                        break;
                }
            }


            return ret_str;

        }

        // 조건문 파싱 함수
        private string WriteChkBlock(string p_step, string p_condition)
        {
            DataRow[] rows_conditions = savingDataSet.Tables["conditions"].Select("qstep_change_Id = '" + p_step + "'");
            DataRow[] rows_chk = savingDataSet.Tables["chk"].Select("conditions_Id = '" + rows_conditions[0]["conditions_Id"].ToString() + "'");

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
                                if (comboBox_CompleteType.SelectedIndex == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 ";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "[=")
                            {
                                if (comboBox_CompleteType.SelectedIndex == 0)
                                    ret_string += "( api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                                else
                                    ret_string += "( api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == -3 or api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ")== " + rows_chk[temp]["num"].ToString() + " )";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "[]")
                            {
                                if (comboBox_CompleteType.SelectedIndex == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") ~= " + rows_chk[temp]["num"].ToString() + " ";
                            }
                            else if (rows_chk[temp]["op"].ToString() == "=")
                            {
                                if (comboBox_CompleteType.SelectedIndex == 0)
                                    ret_string += "api_quest_HasQuestItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                                else
                                    ret_string += "api_user_HasItem(userObjID, " + rows_chk[temp]["chk_Text"].ToString() + ", " + rows_chk[temp]["num"].ToString() + ") == " + rows_chk[temp]["num"].ToString() + " ";
                            }
                            else
                            {
                                if (comboBox_CompleteType.SelectedIndex == 0)
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
                            else if( rows_chk[temp]["op"].ToString() == "]")
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
                            ret_string += " api_user_GetStageConstructionLevel( userObjID ) " ;
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
            //TODO 20090812
            //TODO Server Tag 추가 작업으로 인해 compile 로직 삭제.

            //FileInfo compile_fi = new FileInfo(@"C:\lua\lua5.1.exe");

            //if (!compile_fi.Exists)
            //{
            //    MessageBox.Show("Lua 컴파일러를 찾을수 없어 컴파일 없이 저장됩니다.", "저장", MessageBoxButtons.OK, MessageBoxIcon.Information);  
            //    return;
            //}
            
            //Process compiler = new Process();
            
            //compiler.StartInfo.FileName = @"C:\lua\lua5.1.exe";
            //compiler.StartInfo.Arguments = ToShortPathName(p_Path);
            //compiler.StartInfo.RedirectStandardOutput = true;
            //compiler.StartInfo.RedirectStandardError = true;
            //compiler.StartInfo.UseShellExecute = false;            
            //compiler.Start();
            //compiler.WaitForExit();

            //if (compiler.ExitCode.ToString() == "0")
            //{
            //    //MessageBox.Show("성공적으로 저장 되었습니다.","저장",MessageBoxButtons.OK,MessageBoxIcon.Information);                
            //}
            //else
            //{                
            //    MessageBox.Show("다음과 같은 Lua 컴파일 오류가 발생하였습니다.\n\n" + compiler.StandardError.ReadToEnd(),"저장",MessageBoxButtons.OK,MessageBoxIcon.Error);

            //}            
        }

        #endregion

        private void button_NPCAdd_Click(object sender, EventArgs e)
        {
            SelectNPC select_npc = new SelectNPC();
            if (select_npc.ShowDialog() == DialogResult.OK)
            {
                ListBox.SelectedObjectCollection selected_npcs = select_npc.selected_npcs;

                foreach (object npc in selected_npcs)
                {
                    listBox_NPC.Items.Remove(npc);
                    listBox_NPC.Items.Add(npc);
                    listBox_NPC.Sorted = true;
                }
            }
        }

        private void button_NPCDel_Click(object sender, EventArgs e)
        {
            int count = listBox_NPC.SelectedItems.Count;

            for (int i = 0; i < count; ++i)
            {
                listBox_NPC.Items.Remove(listBox_NPC.SelectedItems[0]);
            }

            listBox_NPC.Sorted = true;
        }

        public void Hide_Menu()
        {
            this.menu_Main.Visible = false;
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

        private void textBox_KorName_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SelectRewardItem selectRewardItem = new SelectRewardItem(this.txtRecompense, this.textBox_QuestID.Text);
            if (selectRewardItem.ShowDialog() == DialogResult.OK)
            {
                ListBox.SelectedObjectCollection selectedRewardItems = selectRewardItem.selectedRewardItems;

                foreach (object npc in selectedRewardItems)
                {
                    listBox_NPC.Items.Remove(npc);
                    listBox_NPC.Items.Add(npc);
                    listBox_NPC.Sorted = true;
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SelectItem selectItem = new SelectItem(this.txtDeleteItem);
            if (selectItem.ShowDialog() == DialogResult.OK)
            {
                ListBox.SelectedObjectCollection selectedItems = selectItem.selectedItems;

                foreach (object npc in selectedItems)
                {
                    listBox_NPC.Items.Remove(npc);
                    listBox_NPC.Items.Add(npc);
                    listBox_NPC.Sorted = true;
                }
            }
        }

        private void groupBox_NextQuest_DragDrop(object sender, DragEventArgs e)
        {
            using (FileStream fs = new FileStream(e.Data.GetData(DataFormats.Text).ToString(), FileMode.Open, FileAccess.Read))
            {
                using (StreamReader sr = new StreamReader(fs, System.Text.Encoding.UTF8))
                {
                    using (DataSet ds = new DataSet())
                    {
                        ds.ReadXml(sr);
                        this.textBox_NextEname.Text = ds.Tables["quest_desc"].Rows[0]["qename"].ToString();
                        textBox_NextID.Text = ds.Tables["quest_desc"].Rows[0]["qid"].ToString();
                        this.textBox_NextNPCEname.Text = ds.Tables["qnpc"].Rows[0]["npcename"].ToString();
                    }
                }
            }
        }

        private void groupBox_NextQuest_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        private void btnDelete4NextQuest_Enter(object sender, EventArgs e)
        {
            textBox_NextID.Text = "";
            textBox_NextEname.Text = "";
            textBox_NextNPCEname.Text = "";
        }

        public bool TestScript(ref string message)
        {
            try
            {
                FileInfo loadFile = new FileInfo(Quest_Path);
                FileInfo loadFile_Lua = new FileInfo(Quest_Path.Substring(0, Quest_Path.LastIndexOf('.')) + ".lua");

                if (false == loadFile.Exists)
                {
                    message = "XML파일을 찾을 수 없습니다.";
                    return false;
                }
                if (true == loadFile.IsReadOnly)
                {
                    message = "저장하는 XML파일이 읽기전용입니다.\nGet Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.";
                    return false;
                }
                if (true == loadFile_Lua.Exists && true == loadFile_Lua.IsReadOnly)
                {
                    message = "저장하는 Lua파일이 읽기전용입니다.\nGet Lock을 해주고 작업하거나 SVN 작업폴더가 아닌 경우 읽기전용 속성을 해제하시기 바랍니다.";
                    return false;
                }

                savingDataSet = null;
                SaveXML(Quest_Path);
                //SaveLua(Quest_Path.Substring(0, Quest_Path.LastIndexOf('.')) + ".lua");

                DNQuest.SaveLua savelua = new DNQuest.SaveLua();
                savelua.Execute(savingDataSet, Quest_Path.Substring(0, Quest_Path.LastIndexOf('.')) + ".lua", comboBox_Type.SelectedIndex, comboBox_CompleteType.SelectedIndex);
                savelua.Clear();

                loadFile = null;
                loadFile_Lua = null;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return false;
            }

            return true;
        }

        private void comboBox_Type_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox combo = (ComboBox)sender;

            if (6 == combo.SelectedIndex) // 커스텀 퀘스트
            {
                label_Questmark.Show();
                label_Statemark0.Show();
                label_Statemark1.Show();
                textBox_Questmark.Show();
                textBox_Statemark0.Show();
                textBox_Statemark1.Show();
            }
            else
            {
                label_Questmark.Hide();
                label_Statemark0.Hide();
                label_Statemark1.Hide();
                textBox_Questmark.Hide();
                textBox_Statemark0.Hide();
                textBox_Statemark1.Hide();
            }

            if (0 == combo.SelectedIndex) // 메인 퀘스트
            {
                comboBox_UIStyle.Enabled = true;
            }
            else
            {
                comboBox_UIStyle.SelectedIndex = 0;
                comboBox_UIStyle.Enabled = false;
            }
        }

        private void _ClearControlsAndDispose(Control.ControlCollection controls)
        {
            foreach (Control control in controls)
            {
                controls.Remove(control);
                control.Dispose();
            }
            controls.Clear();
        }

        private void _UpdateUIStyle(int styleindex)
        {
            _UpdateUIStyle(tabPage_NoQuest.Controls["panel_TabMainNoQuest"].Controls, comboBox_UIStyle.SelectedIndex);
            _UpdateUIStyle(tabPage_Accept.Controls["panel_TabMainAccept"].Controls, comboBox_UIStyle.SelectedIndex);
            _UpdateUIStyle(tabPage_General.Controls["panel_TabMainGeneral"].Controls, comboBox_UIStyle.SelectedIndex);
            _UpdateUIStyle(tabPage_Complete.Controls["panel_TabMainComplete"].Controls, comboBox_UIStyle.SelectedIndex);
        }

        private void _UpdateUIStyle(System.Windows.Forms.Control.ControlCollection base_controls, int styleindex)
        {
            foreach (Control qu_control in base_controls)
            {
                if (typeof(Quest_Unit) != qu_control.GetType())
                    continue;

                Quest_Unit qu = (Quest_Unit)qu_control;
                foreach (Control qn_control in qu.Controls)
                {
                    if (typeof(Quest_NPC) != qn_control.GetType())
                        continue;

                    Quest_NPC qn = (Quest_NPC)qn_control;
                    foreach (Control qs_control in qn.Controls)
                    {
                        if (typeof(Quest_Step) != qs_control.GetType())
                            continue;

                        Quest_Step qs = (Quest_Step)qs_control;
                        qs.SetUIStyle(styleindex);
                    }
                }
            }
        }

        private void comboBox_UIStyle_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(m_StyleIndex == comboBox_UIStyle.SelectedIndex)
                return;

            m_StyleIndex = comboBox_UIStyle.SelectedIndex;
            _UpdateUIStyle(comboBox_UIStyle.SelectedIndex);
        }

        private void button_JournalCopy_Click(object sender, EventArgs e)
        {
            // 저널 복사 기능 구현
        }
    }
}