using System.Windows.Forms;
namespace DNQuest
{
    partial class frm_Main
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (QuestAllSaveCheck == true)
            {
                DialogResult result = MessageBox.Show("저장하시겠습니까?", "저장", MessageBoxButtons.YesNoCancel);
                if (result == System.Windows.Forms.DialogResult.Yes)
                {
                    allSave();
                }
                else if (result == System.Windows.Forms.DialogResult.Cancel)
                {
                    return;
                }
            }
        
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.menu_Main = new System.Windows.Forms.MenuStrip();
            this.초기화ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.불러오기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.저장하기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.닫기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel_Main = new System.Windows.Forms.Panel();
            this.tabControl_Main = new System.Windows.Forms.TabControl();
            this.tabPage_Basic = new System.Windows.Forms.TabPage();
            this.panel_Basic = new System.Windows.Forms.Panel();
            this.groupBox_QuestCondition = new System.Windows.Forms.GroupBox();
            this.label19 = new System.Windows.Forms.Label();
            this.textBox_cashitemcondition = new System.Windows.Forms.TextBox();
            this.textBox_optionalQuestcondition = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.chkBoxSunday = new System.Windows.Forms.CheckBox();
            this.chkBoxSaturday = new System.Windows.Forms.CheckBox();
            this.chkBoxFriday = new System.Windows.Forms.CheckBox();
            this.chkBoxThursday = new System.Windows.Forms.CheckBox();
            this.chkBoxWednesDay = new System.Windows.Forms.CheckBox();
            this.chkBoxTuesday = new System.Windows.Forms.CheckBox();
            this.chkBoxMonday = new System.Windows.Forms.CheckBox();
            this.label16 = new System.Windows.Forms.Label();
            this.txtNpc_malice = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.txtNpc_favor = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.txtPeriodConditionOnDayQuest = new System.Windows.Forms.TextBox();
            this.textBox_UserMissionClearCondition = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.textBox_symbolcondition = new System.Windows.Forms.TextBox();
            this.textBox_itemcondition = new System.Windows.Forms.TextBox();
            this.textBox_prevquestcondition = new System.Windows.Forms.TextBox();
            this.textBox_classcondition = new System.Windows.Forms.TextBox();
            this.textBox_worldcondition = new System.Windows.Forms.TextBox();
            this.textBox_levelconditon = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox_NPC = new System.Windows.Forms.GroupBox();
            this.button_NPCDel = new System.Windows.Forms.Button();
            this.button_NPCAdd = new System.Windows.Forms.Button();
            this.listBox_NPC = new System.Windows.Forms.ListBox();
            this.groupBox_NextQuest = new System.Windows.Forms.GroupBox();
            this.btnDelete4NextQuest = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_NextNPCEname = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_NextID = new System.Windows.Forms.TextBox();
            this.textBox_NextEname = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.panel_splitter = new System.Windows.Forms.Panel();
            this.groupBox_Basic = new System.Windows.Forms.GroupBox();
            this.comboBox_UIStyle = new System.Windows.Forms.ComboBox();
            this.label_UIStyle = new System.Windows.Forms.Label();
            this.textBox_Statemark1 = new System.Windows.Forms.TextBox();
            this.textBox_Statemark0 = new System.Windows.Forms.TextBox();
            this.textBox_Questmark = new System.Windows.Forms.TextBox();
            this.label_Statemark1 = new System.Windows.Forms.Label();
            this.label_Statemark0 = new System.Windows.Forms.Label();
            this.label_Questmark = new System.Windows.Forms.Label();
            this.txtDeleteItem = new System.Windows.Forms.TextBox();
            this.txtRecompense = new System.Windows.Forms.TextBox();
            this.btnDeleteRecompense = new System.Windows.Forms.Button();
            this.btnDeleteUsedItem = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.textBox_QuestDifficulty = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label_Abstract = new System.Windows.Forms.Label();
            this.label_StartLevel = new System.Windows.Forms.Label();
            this.label_Damdang = new System.Windows.Forms.Label();
            this.textBox_Abstrct = new System.Windows.Forms.TextBox();
            this.textBox_StartLevel = new System.Windows.Forms.TextBox();
            this.textBox_Damdang = new System.Windows.Forms.TextBox();
            this.textBox_QuestID = new System.Windows.Forms.TextBox();
            this.textBox_Chapter = new System.Windows.Forms.TextBox();
            this.label_QuestID = new System.Windows.Forms.Label();
            this.label_Chapter = new System.Windows.Forms.Label();
            this.comboBox_CompleteType = new System.Windows.Forms.ComboBox();
            this.label_CompleteType = new System.Windows.Forms.Label();
            this.comboBox_Type = new System.Windows.Forms.ComboBox();
            this.label_Type = new System.Windows.Forms.Label();
            this.textBox_EngName = new System.Windows.Forms.TextBox();
            this.label_EngName = new System.Windows.Forms.Label();
            this.textBox_KorName = new System.Windows.Forms.TextBox();
            this.label_KorName = new System.Windows.Forms.Label();
            this.tabPage_Flow = new System.Windows.Forms.TabPage();
            this.tabControl_Flow = new System.Windows.Forms.TabControl();
            this.tabPage_NoQuest = new System.Windows.Forms.TabPage();
            this.panel_TabMainNoQuest = new System.Windows.Forms.Panel();
            this.panel_TopNoQuest = new System.Windows.Forms.Panel();
            this.button_NoQuestReset = new System.Windows.Forms.Button();
            this.tabPage_Accept = new System.Windows.Forms.TabPage();
            this.panel_TabMainAccept = new System.Windows.Forms.Panel();
            this.panel_TopAccept = new System.Windows.Forms.Panel();
            this.button_AcceptReset = new System.Windows.Forms.Button();
            this.tabPage_General = new System.Windows.Forms.TabPage();
            this.panel_TabMainGeneral = new System.Windows.Forms.Panel();
            this.panel_TopGeneral = new System.Windows.Forms.Panel();
            this.button_GeneralDel = new System.Windows.Forms.Button();
            this.button_GeneralAdd = new System.Windows.Forms.Button();
            this.button_GeneralReset = new System.Windows.Forms.Button();
            this.tabPage_Complete = new System.Windows.Forms.TabPage();
            this.panel_TabMainComplete = new System.Windows.Forms.Panel();
            this.panel_TopComplete = new System.Windows.Forms.Panel();
            this.button_CompleteReset = new System.Windows.Forms.Button();
            this.tabPage_Journal = new System.Windows.Forms.TabPage();
            this.panel_TabMainJournal = new System.Windows.Forms.Panel();
            this.panel_TopJournal = new System.Windows.Forms.Panel();
            this.button_JournalCopy = new System.Windows.Forms.Button();
            this.button_JournalDel = new System.Windows.Forms.Button();
            this.button_JournalAdd = new System.Windows.Forms.Button();
            this.button_Journal = new System.Windows.Forms.Button();
            this.menu_Main.SuspendLayout();
            this.panel_Main.SuspendLayout();
            this.tabControl_Main.SuspendLayout();
            this.tabPage_Basic.SuspendLayout();
            this.panel_Basic.SuspendLayout();
            this.groupBox_QuestCondition.SuspendLayout();
            this.groupBox_NPC.SuspendLayout();
            this.groupBox_NextQuest.SuspendLayout();
            this.groupBox_Basic.SuspendLayout();
            this.tabPage_Flow.SuspendLayout();
            this.tabControl_Flow.SuspendLayout();
            this.tabPage_NoQuest.SuspendLayout();
            this.panel_TopNoQuest.SuspendLayout();
            this.tabPage_Accept.SuspendLayout();
            this.panel_TopAccept.SuspendLayout();
            this.tabPage_General.SuspendLayout();
            this.panel_TopGeneral.SuspendLayout();
            this.tabPage_Complete.SuspendLayout();
            this.panel_TopComplete.SuspendLayout();
            this.tabPage_Journal.SuspendLayout();
            this.panel_TopJournal.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu_Main
            // 
            this.menu_Main.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.초기화ToolStripMenuItem,
            this.불러오기ToolStripMenuItem,
            this.저장하기ToolStripMenuItem,
            this.닫기ToolStripMenuItem});
            this.menu_Main.Location = new System.Drawing.Point(0, 0);
            this.menu_Main.Name = "menu_Main";
            this.menu_Main.Size = new System.Drawing.Size(1190, 24);
            this.menu_Main.TabIndex = 0;
            this.menu_Main.Text = "menuStrip1";
            // 
            // 초기화ToolStripMenuItem
            // 
            this.초기화ToolStripMenuItem.Name = "초기화ToolStripMenuItem";
            this.초기화ToolStripMenuItem.Size = new System.Drawing.Size(55, 20);
            this.초기화ToolStripMenuItem.Text = "초기화";
            this.초기화ToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // 불러오기ToolStripMenuItem
            // 
            this.불러오기ToolStripMenuItem.Name = "불러오기ToolStripMenuItem";
            this.불러오기ToolStripMenuItem.Size = new System.Drawing.Size(67, 20);
            this.불러오기ToolStripMenuItem.Text = "불러오기";
            this.불러오기ToolStripMenuItem.Click += new System.EventHandler(this.loadToolStripMenuItem_Click);
            // 
            // 저장하기ToolStripMenuItem
            // 
            this.저장하기ToolStripMenuItem.Name = "저장하기ToolStripMenuItem";
            this.저장하기ToolStripMenuItem.Size = new System.Drawing.Size(67, 20);
            this.저장하기ToolStripMenuItem.Text = "저장하기";
            this.저장하기ToolStripMenuItem.Click += new System.EventHandler(this.sAToolStripMenuItem_Click);
            // 
            // 닫기ToolStripMenuItem
            // 
            this.닫기ToolStripMenuItem.Name = "닫기ToolStripMenuItem";
            this.닫기ToolStripMenuItem.Size = new System.Drawing.Size(43, 20);
            this.닫기ToolStripMenuItem.Text = "닫기";
            this.닫기ToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
            // 
            // panel_Main
            // 
            this.panel_Main.Controls.Add(this.tabControl_Main);
            this.panel_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_Main.Location = new System.Drawing.Point(0, 24);
            this.panel_Main.Name = "panel_Main";
            this.panel_Main.Size = new System.Drawing.Size(1190, 771);
            this.panel_Main.TabIndex = 1;
            // 
            // tabControl_Main
            // 
            this.tabControl_Main.Controls.Add(this.tabPage_Basic);
            this.tabControl_Main.Controls.Add(this.tabPage_Flow);
            this.tabControl_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl_Main.Location = new System.Drawing.Point(0, 0);
            this.tabControl_Main.Name = "tabControl_Main";
            this.tabControl_Main.SelectedIndex = 0;
            this.tabControl_Main.Size = new System.Drawing.Size(1190, 771);
            this.tabControl_Main.TabIndex = 0;
            // 
            // tabPage_Basic
            // 
            this.tabPage_Basic.Controls.Add(this.panel_Basic);
            this.tabPage_Basic.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Basic.Name = "tabPage_Basic";
            this.tabPage_Basic.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Basic.Size = new System.Drawing.Size(1182, 745);
            this.tabPage_Basic.TabIndex = 0;
            this.tabPage_Basic.Text = "퀘스트 기본정보";
            this.tabPage_Basic.UseVisualStyleBackColor = true;
            // 
            // panel_Basic
            // 
            this.panel_Basic.Controls.Add(this.groupBox_QuestCondition);
            this.panel_Basic.Controls.Add(this.groupBox_NPC);
            this.panel_Basic.Controls.Add(this.groupBox_NextQuest);
            this.panel_Basic.Controls.Add(this.panel_splitter);
            this.panel_Basic.Controls.Add(this.groupBox_Basic);
            this.panel_Basic.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_Basic.Location = new System.Drawing.Point(3, 3);
            this.panel_Basic.Name = "panel_Basic";
            this.panel_Basic.Size = new System.Drawing.Size(1176, 739);
            this.panel_Basic.TabIndex = 0;
            // 
            // groupBox_QuestCondition
            // 
            this.groupBox_QuestCondition.Controls.Add(this.label19);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_cashitemcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_optionalQuestcondition);
            this.groupBox_QuestCondition.Controls.Add(this.label18);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxSunday);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxSaturday);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxFriday);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxThursday);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxWednesDay);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxTuesday);
            this.groupBox_QuestCondition.Controls.Add(this.chkBoxMonday);
            this.groupBox_QuestCondition.Controls.Add(this.label16);
            this.groupBox_QuestCondition.Controls.Add(this.txtNpc_malice);
            this.groupBox_QuestCondition.Controls.Add(this.label17);
            this.groupBox_QuestCondition.Controls.Add(this.txtNpc_favor);
            this.groupBox_QuestCondition.Controls.Add(this.label15);
            this.groupBox_QuestCondition.Controls.Add(this.label14);
            this.groupBox_QuestCondition.Controls.Add(this.txtPeriodConditionOnDayQuest);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_UserMissionClearCondition);
            this.groupBox_QuestCondition.Controls.Add(this.label13);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_symbolcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_itemcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_prevquestcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_classcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_worldcondition);
            this.groupBox_QuestCondition.Controls.Add(this.textBox_levelconditon);
            this.groupBox_QuestCondition.Controls.Add(this.label7);
            this.groupBox_QuestCondition.Controls.Add(this.label8);
            this.groupBox_QuestCondition.Controls.Add(this.label9);
            this.groupBox_QuestCondition.Controls.Add(this.label1);
            this.groupBox_QuestCondition.Controls.Add(this.label5);
            this.groupBox_QuestCondition.Controls.Add(this.label6);
            this.groupBox_QuestCondition.Location = new System.Drawing.Point(487, 307);
            this.groupBox_QuestCondition.Name = "groupBox_QuestCondition";
            this.groupBox_QuestCondition.Size = new System.Drawing.Size(465, 353);
            this.groupBox_QuestCondition.TabIndex = 28;
            this.groupBox_QuestCondition.TabStop = false;
            this.groupBox_QuestCondition.Text = "퀘스트 조건 데이터";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(11, 177);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(125, 12);
            this.label19.TabIndex = 59;
            this.label19.Text = "유저 캐시 아이템 조건";
            // 
            // textBox_cashitemcondition
            // 
            this.textBox_cashitemcondition.Location = new System.Drawing.Point(156, 174);
            this.textBox_cashitemcondition.Name = "textBox_cashitemcondition";
            this.textBox_cashitemcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_cashitemcondition.TabIndex = 58;
            // 
            // textBox_optionalQuestcondition
            // 
            this.textBox_optionalQuestcondition.Location = new System.Drawing.Point(156, 118);
            this.textBox_optionalQuestcondition.Name = "textBox_optionalQuestcondition";
            this.textBox_optionalQuestcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_optionalQuestcondition.TabIndex = 57;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(11, 124);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(125, 12);
            this.label18.TabIndex = 56;
            this.label18.Text = "선택 선행 퀘스트 조건";
            // 
            // chkBoxSunday
            // 
            this.chkBoxSunday.AutoSize = true;
            this.chkBoxSunday.Location = new System.Drawing.Point(349, 335);
            this.chkBoxSunday.Name = "chkBoxSunday";
            this.chkBoxSunday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxSunday.TabIndex = 55;
            this.chkBoxSunday.Text = "일";
            this.chkBoxSunday.UseVisualStyleBackColor = true;
            // 
            // chkBoxSaturday
            // 
            this.chkBoxSaturday.AutoSize = true;
            this.chkBoxSaturday.Location = new System.Drawing.Point(316, 335);
            this.chkBoxSaturday.Name = "chkBoxSaturday";
            this.chkBoxSaturday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxSaturday.TabIndex = 54;
            this.chkBoxSaturday.Text = "토";
            this.chkBoxSaturday.UseVisualStyleBackColor = true;
            // 
            // chkBoxFriday
            // 
            this.chkBoxFriday.AutoSize = true;
            this.chkBoxFriday.Location = new System.Drawing.Point(283, 335);
            this.chkBoxFriday.Name = "chkBoxFriday";
            this.chkBoxFriday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxFriday.TabIndex = 53;
            this.chkBoxFriday.Text = "금";
            this.chkBoxFriday.UseVisualStyleBackColor = true;
            // 
            // chkBoxThursday
            // 
            this.chkBoxThursday.AutoSize = true;
            this.chkBoxThursday.Location = new System.Drawing.Point(252, 335);
            this.chkBoxThursday.Name = "chkBoxThursday";
            this.chkBoxThursday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxThursday.TabIndex = 52;
            this.chkBoxThursday.Text = "목";
            this.chkBoxThursday.UseVisualStyleBackColor = true;
            // 
            // chkBoxWednesDay
            // 
            this.chkBoxWednesDay.AutoSize = true;
            this.chkBoxWednesDay.Location = new System.Drawing.Point(221, 335);
            this.chkBoxWednesDay.Name = "chkBoxWednesDay";
            this.chkBoxWednesDay.Size = new System.Drawing.Size(36, 16);
            this.chkBoxWednesDay.TabIndex = 51;
            this.chkBoxWednesDay.Text = "수";
            this.chkBoxWednesDay.UseVisualStyleBackColor = true;
            // 
            // chkBoxTuesday
            // 
            this.chkBoxTuesday.AutoSize = true;
            this.chkBoxTuesday.Location = new System.Drawing.Point(189, 335);
            this.chkBoxTuesday.Name = "chkBoxTuesday";
            this.chkBoxTuesday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxTuesday.TabIndex = 50;
            this.chkBoxTuesday.Text = "화";
            this.chkBoxTuesday.UseVisualStyleBackColor = true;
            // 
            // chkBoxMonday
            // 
            this.chkBoxMonday.AutoSize = true;
            this.chkBoxMonday.Location = new System.Drawing.Point(156, 335);
            this.chkBoxMonday.Name = "chkBoxMonday";
            this.chkBoxMonday.Size = new System.Drawing.Size(36, 16);
            this.chkBoxMonday.TabIndex = 49;
            this.chkBoxMonday.Text = "월";
            this.chkBoxMonday.UseVisualStyleBackColor = true;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(11, 280);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(111, 12);
            this.label16.TabIndex = 48;
            this.label16.Text = "NPC 비호감도 조건";
            // 
            // txtNpc_malice
            // 
            this.txtNpc_malice.Location = new System.Drawing.Point(156, 276);
            this.txtNpc_malice.Name = "txtNpc_malice";
            this.txtNpc_malice.Size = new System.Drawing.Size(225, 21);
            this.txtNpc_malice.TabIndex = 47;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(11, 255);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(99, 12);
            this.label17.TabIndex = 46;
            this.label17.Text = "NPC 호감도 조건";
            // 
            // txtNpc_favor
            // 
            this.txtNpc_favor.Location = new System.Drawing.Point(156, 251);
            this.txtNpc_favor.Name = "txtNpc_favor";
            this.txtNpc_favor.Size = new System.Drawing.Size(225, 21);
            this.txtNpc_favor.TabIndex = 45;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(11, 330);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(125, 12);
            this.label15.TabIndex = 40;
            this.label15.Text = "일일 퀘스트 요일 조건";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(11, 305);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(125, 12);
            this.label14.TabIndex = 38;
            this.label14.Text = "일일 퀘스트 기간 조건";
            // 
            // txtPeriodConditionOnDayQuest
            // 
            this.txtPeriodConditionOnDayQuest.Location = new System.Drawing.Point(156, 302);
            this.txtPeriodConditionOnDayQuest.Name = "txtPeriodConditionOnDayQuest";
            this.txtPeriodConditionOnDayQuest.Size = new System.Drawing.Size(225, 21);
            this.txtPeriodConditionOnDayQuest.TabIndex = 37;
            // 
            // textBox_UserMissionClearCondition
            // 
            this.textBox_UserMissionClearCondition.Location = new System.Drawing.Point(156, 201);
            this.textBox_UserMissionClearCondition.Name = "textBox_UserMissionClearCondition";
            this.textBox_UserMissionClearCondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_UserMissionClearCondition.TabIndex = 36;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(11, 228);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(85, 12);
            this.label13.TabIndex = 35;
            this.label13.Text = "유저 심볼 조건";
            // 
            // textBox_symbolcondition
            // 
            this.textBox_symbolcondition.Location = new System.Drawing.Point(156, 225);
            this.textBox_symbolcondition.Name = "textBox_symbolcondition";
            this.textBox_symbolcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_symbolcondition.TabIndex = 34;
            // 
            // textBox_itemcondition
            // 
            this.textBox_itemcondition.Location = new System.Drawing.Point(156, 147);
            this.textBox_itemcondition.Name = "textBox_itemcondition";
            this.textBox_itemcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_itemcondition.TabIndex = 33;
            // 
            // textBox_prevquestcondition
            // 
            this.textBox_prevquestcondition.Location = new System.Drawing.Point(156, 92);
            this.textBox_prevquestcondition.Name = "textBox_prevquestcondition";
            this.textBox_prevquestcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_prevquestcondition.TabIndex = 32;
            // 
            // textBox_classcondition
            // 
            this.textBox_classcondition.Location = new System.Drawing.Point(156, 67);
            this.textBox_classcondition.Name = "textBox_classcondition";
            this.textBox_classcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_classcondition.TabIndex = 31;
            // 
            // textBox_worldcondition
            // 
            this.textBox_worldcondition.Location = new System.Drawing.Point(156, 44);
            this.textBox_worldcondition.Name = "textBox_worldcondition";
            this.textBox_worldcondition.Size = new System.Drawing.Size(225, 21);
            this.textBox_worldcondition.TabIndex = 30;
            // 
            // textBox_levelconditon
            // 
            this.textBox_levelconditon.Location = new System.Drawing.Point(156, 19);
            this.textBox_levelconditon.Name = "textBox_levelconditon";
            this.textBox_levelconditon.Size = new System.Drawing.Size(225, 21);
            this.textBox_levelconditon.TabIndex = 25;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(11, 98);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(125, 12);
            this.label7.TabIndex = 27;
            this.label7.Text = "필수 선행 퀘스트 조건";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(11, 205);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(125, 12);
            this.label8.TabIndex = 29;
            this.label8.Text = "유저 미션 클리어 조건";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(11, 151);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(97, 12);
            this.label9.TabIndex = 28;
            this.label9.Text = "유저 아이템 조건";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 12);
            this.label1.TabIndex = 24;
            this.label1.Text = "유저 레벨 조건";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(11, 74);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(97, 12);
            this.label5.TabIndex = 26;
            this.label5.Text = "유저 클래스 조건";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 49);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(125, 12);
            this.label6.TabIndex = 25;
            this.label6.Text = "유저 월드존 레벨 조건";
            // 
            // groupBox_NPC
            // 
            this.groupBox_NPC.Controls.Add(this.button_NPCDel);
            this.groupBox_NPC.Controls.Add(this.button_NPCAdd);
            this.groupBox_NPC.Controls.Add(this.listBox_NPC);
            this.groupBox_NPC.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox_NPC.Location = new System.Drawing.Point(487, 0);
            this.groupBox_NPC.Name = "groupBox_NPC";
            this.groupBox_NPC.Size = new System.Drawing.Size(689, 208);
            this.groupBox_NPC.TabIndex = 1;
            this.groupBox_NPC.TabStop = false;
            this.groupBox_NPC.Text = "퀘스트 NPC";
            // 
            // button_NPCDel
            // 
            this.button_NPCDel.Location = new System.Drawing.Point(124, 15);
            this.button_NPCDel.Name = "button_NPCDel";
            this.button_NPCDel.Size = new System.Drawing.Size(114, 23);
            this.button_NPCDel.TabIndex = 2;
            this.button_NPCDel.Text = "NPC 삭제";
            this.button_NPCDel.UseVisualStyleBackColor = true;
            this.button_NPCDel.Click += new System.EventHandler(this.button_NPCDel_Click);
            // 
            // button_NPCAdd
            // 
            this.button_NPCAdd.Location = new System.Drawing.Point(6, 15);
            this.button_NPCAdd.Name = "button_NPCAdd";
            this.button_NPCAdd.Size = new System.Drawing.Size(114, 23);
            this.button_NPCAdd.TabIndex = 1;
            this.button_NPCAdd.Text = "NPC 추가";
            this.button_NPCAdd.UseVisualStyleBackColor = true;
            this.button_NPCAdd.Click += new System.EventHandler(this.button_NPCAdd_Click);
            // 
            // listBox_NPC
            // 
            this.listBox_NPC.FormattingEnabled = true;
            this.listBox_NPC.ItemHeight = 12;
            this.listBox_NPC.Location = new System.Drawing.Point(6, 41);
            this.listBox_NPC.MultiColumn = true;
            this.listBox_NPC.Name = "listBox_NPC";
            this.listBox_NPC.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.listBox_NPC.Size = new System.Drawing.Size(470, 160);
            this.listBox_NPC.TabIndex = 0;
            // 
            // groupBox_NextQuest
            // 
            this.groupBox_NextQuest.AllowDrop = true;
            this.groupBox_NextQuest.Controls.Add(this.btnDelete4NextQuest);
            this.groupBox_NextQuest.Controls.Add(this.label2);
            this.groupBox_NextQuest.Controls.Add(this.textBox_NextNPCEname);
            this.groupBox_NextQuest.Controls.Add(this.label4);
            this.groupBox_NextQuest.Controls.Add(this.textBox_NextID);
            this.groupBox_NextQuest.Controls.Add(this.textBox_NextEname);
            this.groupBox_NextQuest.Controls.Add(this.label3);
            this.groupBox_NextQuest.Location = new System.Drawing.Point(487, 214);
            this.groupBox_NextQuest.Name = "groupBox_NextQuest";
            this.groupBox_NextQuest.Size = new System.Drawing.Size(465, 87);
            this.groupBox_NextQuest.TabIndex = 27;
            this.groupBox_NextQuest.TabStop = false;
            this.groupBox_NextQuest.Text = "다음 퀘스트(Quest List에서 Drag Drop)";
            this.groupBox_NextQuest.DragOver += new System.Windows.Forms.DragEventHandler(this.groupBox_NextQuest_DragOver);
            this.groupBox_NextQuest.DragDrop += new System.Windows.Forms.DragEventHandler(this.groupBox_NextQuest_DragDrop);
            // 
            // btnDelete4NextQuest
            // 
            this.btnDelete4NextQuest.Location = new System.Drawing.Point(416, 12);
            this.btnDelete4NextQuest.Name = "btnDelete4NextQuest";
            this.btnDelete4NextQuest.Size = new System.Drawing.Size(43, 23);
            this.btnDelete4NextQuest.TabIndex = 35;
            this.btnDelete4NextQuest.Text = "삭제";
            this.btnDelete4NextQuest.UseVisualStyleBackColor = true;
            this.btnDelete4NextQuest.Enter += new System.EventHandler(this.btnDelete4NextQuest_Enter);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 12);
            this.label2.TabIndex = 19;
            this.label2.Text = "퀘스트 ID";
            // 
            // textBox_NextNPCEname
            // 
            this.textBox_NextNPCEname.Location = new System.Drawing.Point(156, 59);
            this.textBox_NextNPCEname.Name = "textBox_NextNPCEname";
            this.textBox_NextNPCEname.Size = new System.Drawing.Size(254, 21);
            this.textBox_NextNPCEname.TabIndex = 24;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(11, 67);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(139, 12);
            this.label4.TabIndex = 23;
            this.label4.Text = "퀘스트 시작 NPC 영문명";
            // 
            // textBox_NextID
            // 
            this.textBox_NextID.Location = new System.Drawing.Point(156, 12);
            this.textBox_NextID.Name = "textBox_NextID";
            this.textBox_NextID.Size = new System.Drawing.Size(254, 21);
            this.textBox_NextID.TabIndex = 20;
            // 
            // textBox_NextEname
            // 
            this.textBox_NextEname.Location = new System.Drawing.Point(156, 35);
            this.textBox_NextEname.Name = "textBox_NextEname";
            this.textBox_NextEname.Size = new System.Drawing.Size(254, 21);
            this.textBox_NextEname.TabIndex = 22;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 43);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(81, 12);
            this.label3.TabIndex = 21;
            this.label3.Text = "퀘스트 영문명";
            // 
            // panel_splitter
            // 
            this.panel_splitter.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel_splitter.Location = new System.Drawing.Point(477, 0);
            this.panel_splitter.Name = "panel_splitter";
            this.panel_splitter.Size = new System.Drawing.Size(10, 739);
            this.panel_splitter.TabIndex = 2;
            // 
            // groupBox_Basic
            // 
            this.groupBox_Basic.Controls.Add(this.comboBox_UIStyle);
            this.groupBox_Basic.Controls.Add(this.label_UIStyle);
            this.groupBox_Basic.Controls.Add(this.textBox_Statemark1);
            this.groupBox_Basic.Controls.Add(this.textBox_Statemark0);
            this.groupBox_Basic.Controls.Add(this.textBox_Questmark);
            this.groupBox_Basic.Controls.Add(this.label_Statemark1);
            this.groupBox_Basic.Controls.Add(this.label_Statemark0);
            this.groupBox_Basic.Controls.Add(this.label_Questmark);
            this.groupBox_Basic.Controls.Add(this.txtDeleteItem);
            this.groupBox_Basic.Controls.Add(this.txtRecompense);
            this.groupBox_Basic.Controls.Add(this.btnDeleteRecompense);
            this.groupBox_Basic.Controls.Add(this.btnDeleteUsedItem);
            this.groupBox_Basic.Controls.Add(this.button2);
            this.groupBox_Basic.Controls.Add(this.button1);
            this.groupBox_Basic.Controls.Add(this.label12);
            this.groupBox_Basic.Controls.Add(this.label11);
            this.groupBox_Basic.Controls.Add(this.textBox_QuestDifficulty);
            this.groupBox_Basic.Controls.Add(this.label10);
            this.groupBox_Basic.Controls.Add(this.label_Abstract);
            this.groupBox_Basic.Controls.Add(this.label_StartLevel);
            this.groupBox_Basic.Controls.Add(this.label_Damdang);
            this.groupBox_Basic.Controls.Add(this.textBox_Abstrct);
            this.groupBox_Basic.Controls.Add(this.textBox_StartLevel);
            this.groupBox_Basic.Controls.Add(this.textBox_Damdang);
            this.groupBox_Basic.Controls.Add(this.textBox_QuestID);
            this.groupBox_Basic.Controls.Add(this.textBox_Chapter);
            this.groupBox_Basic.Controls.Add(this.label_QuestID);
            this.groupBox_Basic.Controls.Add(this.label_Chapter);
            this.groupBox_Basic.Controls.Add(this.comboBox_CompleteType);
            this.groupBox_Basic.Controls.Add(this.label_CompleteType);
            this.groupBox_Basic.Controls.Add(this.comboBox_Type);
            this.groupBox_Basic.Controls.Add(this.label_Type);
            this.groupBox_Basic.Controls.Add(this.textBox_EngName);
            this.groupBox_Basic.Controls.Add(this.label_EngName);
            this.groupBox_Basic.Controls.Add(this.textBox_KorName);
            this.groupBox_Basic.Controls.Add(this.label_KorName);
            this.groupBox_Basic.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBox_Basic.Location = new System.Drawing.Point(0, 0);
            this.groupBox_Basic.Name = "groupBox_Basic";
            this.groupBox_Basic.Size = new System.Drawing.Size(477, 739);
            this.groupBox_Basic.TabIndex = 0;
            this.groupBox_Basic.TabStop = false;
            this.groupBox_Basic.Text = "퀘스트 기본정보";
            // 
            // comboBox_UIStyle
            // 
            this.comboBox_UIStyle.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_UIStyle.FormattingEnabled = true;
            this.comboBox_UIStyle.Items.AddRange(new object[] {
            "0 : Basic",
            "1 : Renewal"});
            this.comboBox_UIStyle.Location = new System.Drawing.Point(306, 81);
            this.comboBox_UIStyle.Name = "comboBox_UIStyle";
            this.comboBox_UIStyle.Size = new System.Drawing.Size(157, 20);
            this.comboBox_UIStyle.TabIndex = 48;
            this.comboBox_UIStyle.SelectedIndexChanged += new System.EventHandler(this.comboBox_UIStyle_SelectedIndexChanged);
            // 
            // label_UIStyle
            // 
            this.label_UIStyle.AutoSize = true;
            this.label_UIStyle.Location = new System.Drawing.Point(244, 84);
            this.label_UIStyle.Name = "label_UIStyle";
            this.label_UIStyle.Size = new System.Drawing.Size(56, 12);
            this.label_UIStyle.TabIndex = 47;
            this.label_UIStyle.Text = "UI 스타일";
            // 
            // textBox_Statemark1
            // 
            this.textBox_Statemark1.Location = new System.Drawing.Point(373, 159);
            this.textBox_Statemark1.Name = "textBox_Statemark1";
            this.textBox_Statemark1.Size = new System.Drawing.Size(90, 21);
            this.textBox_Statemark1.TabIndex = 46;
            // 
            // textBox_Statemark0
            // 
            this.textBox_Statemark0.Location = new System.Drawing.Point(373, 132);
            this.textBox_Statemark0.Name = "textBox_Statemark0";
            this.textBox_Statemark0.Size = new System.Drawing.Size(90, 21);
            this.textBox_Statemark0.TabIndex = 45;
            // 
            // textBox_Questmark
            // 
            this.textBox_Questmark.Location = new System.Drawing.Point(373, 106);
            this.textBox_Questmark.Name = "textBox_Questmark";
            this.textBox_Questmark.Size = new System.Drawing.Size(90, 21);
            this.textBox_Questmark.TabIndex = 44;
            // 
            // label_Statemark1
            // 
            this.label_Statemark1.AutoSize = true;
            this.label_Statemark1.Location = new System.Drawing.Point(244, 162);
            this.label_Statemark1.Name = "label_Statemark1";
            this.label_Statemark1.Size = new System.Drawing.Size(115, 12);
            this.label_Statemark1.TabIndex = 43;
            this.label_Statemark1.Text = "스테이트 마크(완료)";
            // 
            // label_Statemark0
            // 
            this.label_Statemark0.AutoSize = true;
            this.label_Statemark0.Location = new System.Drawing.Point(244, 136);
            this.label_Statemark0.Name = "label_Statemark0";
            this.label_Statemark0.Size = new System.Drawing.Size(127, 12);
            this.label_Statemark0.TabIndex = 42;
            this.label_Statemark0.Text = "스테이트 마크(진행중)";
            // 
            // label_Questmark
            // 
            this.label_Questmark.AutoSize = true;
            this.label_Questmark.Location = new System.Drawing.Point(244, 110);
            this.label_Questmark.Name = "label_Questmark";
            this.label_Questmark.Size = new System.Drawing.Size(69, 12);
            this.label_Questmark.TabIndex = 41;
            this.label_Questmark.Text = "퀘스트 마크";
            // 
            // txtDeleteItem
            // 
            this.txtDeleteItem.Location = new System.Drawing.Point(117, 423);
            this.txtDeleteItem.Multiline = true;
            this.txtDeleteItem.Name = "txtDeleteItem";
            this.txtDeleteItem.Size = new System.Drawing.Size(354, 96);
            this.txtDeleteItem.TabIndex = 40;
            // 
            // txtRecompense
            // 
            this.txtRecompense.Location = new System.Drawing.Point(117, 268);
            this.txtRecompense.Multiline = true;
            this.txtRecompense.Name = "txtRecompense";
            this.txtRecompense.Size = new System.Drawing.Size(354, 96);
            this.txtRecompense.TabIndex = 39;
            // 
            // btnDeleteRecompense
            // 
            this.btnDeleteRecompense.Location = new System.Drawing.Point(254, 239);
            this.btnDeleteRecompense.Name = "btnDeleteRecompense";
            this.btnDeleteRecompense.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteRecompense.TabIndex = 38;
            this.btnDeleteRecompense.Text = "삭제";
            this.btnDeleteRecompense.UseVisualStyleBackColor = true;
            // 
            // btnDeleteUsedItem
            // 
            this.btnDeleteUsedItem.Location = new System.Drawing.Point(285, 393);
            this.btnDeleteUsedItem.Name = "btnDeleteUsedItem";
            this.btnDeleteUsedItem.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteUsedItem.TabIndex = 37;
            this.btnDeleteUsedItem.Text = "삭제";
            this.btnDeleteUsedItem.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(204, 393);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 34;
            this.button2.Text = "찾기";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(173, 239);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 33;
            this.button1.Text = "찾기";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(18, 399);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(181, 12);
            this.label12.TabIndex = 32;
            this.label12.Text = "퀘스트 완료, 포기시 삭제 아이템";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(18, 375);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(81, 12);
            this.label11.TabIndex = 30;
            this.label11.Text = "퀘스트 난이도";
            // 
            // textBox_QuestDifficulty
            // 
            this.textBox_QuestDifficulty.Location = new System.Drawing.Point(117, 370);
            this.textBox_QuestDifficulty.Name = "textBox_QuestDifficulty";
            this.textBox_QuestDifficulty.Size = new System.Drawing.Size(50, 21);
            this.textBox_QuestDifficulty.TabIndex = 29;
            this.textBox_QuestDifficulty.Text = "1";
            this.textBox_QuestDifficulty.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.textBox_QuestDifficulty.Validating += new System.ComponentModel.CancelEventHandler(this.textBox_QuestDifficulty_Validating);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(18, 246);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(149, 12);
            this.label10.TabIndex = 28;
            this.label10.Text = "퀘스트 보상 아이템 인덱스";
            // 
            // label_Abstract
            // 
            this.label_Abstract.AutoSize = true;
            this.label_Abstract.Location = new System.Drawing.Point(18, 529);
            this.label_Abstract.Name = "label_Abstract";
            this.label_Abstract.Size = new System.Drawing.Size(69, 12);
            this.label_Abstract.TabIndex = 17;
            this.label_Abstract.Text = "퀘스트 개요";
            // 
            // label_StartLevel
            // 
            this.label_StartLevel.AutoSize = true;
            this.label_StartLevel.Location = new System.Drawing.Point(18, 218);
            this.label_StartLevel.Name = "label_StartLevel";
            this.label_StartLevel.Size = new System.Drawing.Size(93, 12);
            this.label_StartLevel.TabIndex = 16;
            this.label_StartLevel.Text = "퀘스트 시작레벨";
            // 
            // label_Damdang
            // 
            this.label_Damdang.AutoSize = true;
            this.label_Damdang.Location = new System.Drawing.Point(18, 190);
            this.label_Damdang.Name = "label_Damdang";
            this.label_Damdang.Size = new System.Drawing.Size(41, 12);
            this.label_Damdang.TabIndex = 15;
            this.label_Damdang.Text = "담당자";
            // 
            // textBox_Abstrct
            // 
            this.textBox_Abstrct.Location = new System.Drawing.Point(117, 531);
            this.textBox_Abstrct.Multiline = true;
            this.textBox_Abstrct.Name = "textBox_Abstrct";
            this.textBox_Abstrct.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_Abstrct.Size = new System.Drawing.Size(354, 202);
            this.textBox_Abstrct.TabIndex = 14;
            this.textBox_Abstrct.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // textBox_StartLevel
            // 
            this.textBox_StartLevel.Location = new System.Drawing.Point(117, 214);
            this.textBox_StartLevel.Name = "textBox_StartLevel";
            this.textBox_StartLevel.Size = new System.Drawing.Size(121, 21);
            this.textBox_StartLevel.TabIndex = 13;
            this.textBox_StartLevel.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // textBox_Damdang
            // 
            this.textBox_Damdang.Location = new System.Drawing.Point(117, 187);
            this.textBox_Damdang.Name = "textBox_Damdang";
            this.textBox_Damdang.Size = new System.Drawing.Size(121, 21);
            this.textBox_Damdang.TabIndex = 12;
            this.textBox_Damdang.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // textBox_QuestID
            // 
            this.textBox_QuestID.Location = new System.Drawing.Point(117, 160);
            this.textBox_QuestID.Name = "textBox_QuestID";
            this.textBox_QuestID.Size = new System.Drawing.Size(121, 21);
            this.textBox_QuestID.TabIndex = 11;
            this.textBox_QuestID.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // textBox_Chapter
            // 
            this.textBox_Chapter.Location = new System.Drawing.Point(117, 133);
            this.textBox_Chapter.Name = "textBox_Chapter";
            this.textBox_Chapter.Size = new System.Drawing.Size(121, 21);
            this.textBox_Chapter.TabIndex = 10;
            this.textBox_Chapter.Text = "-1";
            this.textBox_Chapter.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // label_QuestID
            // 
            this.label_QuestID.AutoSize = true;
            this.label_QuestID.Location = new System.Drawing.Point(18, 163);
            this.label_QuestID.Name = "label_QuestID";
            this.label_QuestID.Size = new System.Drawing.Size(81, 12);
            this.label_QuestID.TabIndex = 9;
            this.label_QuestID.Text = "퀘스트 아이디";
            // 
            // label_Chapter
            // 
            this.label_Chapter.AutoSize = true;
            this.label_Chapter.Location = new System.Drawing.Point(18, 136);
            this.label_Chapter.Name = "label_Chapter";
            this.label_Chapter.Size = new System.Drawing.Size(29, 12);
            this.label_Chapter.TabIndex = 8;
            this.label_Chapter.Text = "챕터";
            // 
            // comboBox_CompleteType
            // 
            this.comboBox_CompleteType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_CompleteType.FormattingEnabled = true;
            this.comboBox_CompleteType.Items.AddRange(new object[] {
            "1회성",
            "반복성"});
            this.comboBox_CompleteType.Location = new System.Drawing.Point(117, 107);
            this.comboBox_CompleteType.Name = "comboBox_CompleteType";
            this.comboBox_CompleteType.Size = new System.Drawing.Size(121, 20);
            this.comboBox_CompleteType.TabIndex = 7;
            // 
            // label_CompleteType
            // 
            this.label_CompleteType.AutoSize = true;
            this.label_CompleteType.Location = new System.Drawing.Point(18, 110);
            this.label_CompleteType.Name = "label_CompleteType";
            this.label_CompleteType.Size = new System.Drawing.Size(93, 12);
            this.label_CompleteType.TabIndex = 6;
            this.label_CompleteType.Text = "퀘스트 완료타입";
            // 
            // comboBox_Type
            // 
            this.comboBox_Type.FormattingEnabled = true;
            this.comboBox_Type.Items.AddRange(new object[] {
            "메인 퀘스트",
            "서브 퀘스트",
            "모험자 퀘스트",
            "--------------",
            "호감도 퀘스트",
            "일일 퀘스트",
            "커스텀 퀘스트"});
            this.comboBox_Type.Location = new System.Drawing.Point(117, 81);
            this.comboBox_Type.Name = "comboBox_Type";
            this.comboBox_Type.Size = new System.Drawing.Size(121, 20);
            this.comboBox_Type.TabIndex = 5;
            this.comboBox_Type.SelectedIndexChanged += new System.EventHandler(this.comboBox_Type_SelectedIndexChanged);
            // 
            // label_Type
            // 
            this.label_Type.AutoSize = true;
            this.label_Type.Location = new System.Drawing.Point(18, 84);
            this.label_Type.Name = "label_Type";
            this.label_Type.Size = new System.Drawing.Size(69, 12);
            this.label_Type.TabIndex = 4;
            this.label_Type.Text = "퀘스트 타입";
            // 
            // textBox_EngName
            // 
            this.textBox_EngName.Location = new System.Drawing.Point(117, 54);
            this.textBox_EngName.Name = "textBox_EngName";
            this.textBox_EngName.Size = new System.Drawing.Size(302, 21);
            this.textBox_EngName.TabIndex = 3;
            this.textBox_EngName.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // label_EngName
            // 
            this.label_EngName.AutoSize = true;
            this.label_EngName.Location = new System.Drawing.Point(18, 57);
            this.label_EngName.Name = "label_EngName";
            this.label_EngName.Size = new System.Drawing.Size(81, 12);
            this.label_EngName.TabIndex = 2;
            this.label_EngName.Text = "퀘스트 영문명";
            // 
            // textBox_KorName
            // 
            this.textBox_KorName.Location = new System.Drawing.Point(117, 25);
            this.textBox_KorName.Name = "textBox_KorName";
            this.textBox_KorName.Size = new System.Drawing.Size(302, 21);
            this.textBox_KorName.TabIndex = 1;
            this.textBox_KorName.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_KorName_KeyDown);
            // 
            // label_KorName
            // 
            this.label_KorName.AutoSize = true;
            this.label_KorName.Location = new System.Drawing.Point(18, 29);
            this.label_KorName.Name = "label_KorName";
            this.label_KorName.Size = new System.Drawing.Size(81, 12);
            this.label_KorName.TabIndex = 0;
            this.label_KorName.Text = "퀘스트 한글명";
            // 
            // tabPage_Flow
            // 
            this.tabPage_Flow.Controls.Add(this.tabControl_Flow);
            this.tabPage_Flow.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Flow.Name = "tabPage_Flow";
            this.tabPage_Flow.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Flow.Size = new System.Drawing.Size(1182, 745);
            this.tabPage_Flow.TabIndex = 1;
            this.tabPage_Flow.Text = "퀘스트 플로우";
            this.tabPage_Flow.UseVisualStyleBackColor = true;
            // 
            // tabControl_Flow
            // 
            this.tabControl_Flow.Controls.Add(this.tabPage_NoQuest);
            this.tabControl_Flow.Controls.Add(this.tabPage_Accept);
            this.tabControl_Flow.Controls.Add(this.tabPage_General);
            this.tabControl_Flow.Controls.Add(this.tabPage_Complete);
            this.tabControl_Flow.Controls.Add(this.tabPage_Journal);
            this.tabControl_Flow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl_Flow.Location = new System.Drawing.Point(3, 3);
            this.tabControl_Flow.Multiline = true;
            this.tabControl_Flow.Name = "tabControl_Flow";
            this.tabControl_Flow.SelectedIndex = 0;
            this.tabControl_Flow.Size = new System.Drawing.Size(1176, 739);
            this.tabControl_Flow.TabIndex = 0;
            // 
            // tabPage_NoQuest
            // 
            this.tabPage_NoQuest.AutoScroll = true;
            this.tabPage_NoQuest.Controls.Add(this.panel_TabMainNoQuest);
            this.tabPage_NoQuest.Controls.Add(this.panel_TopNoQuest);
            this.tabPage_NoQuest.Location = new System.Drawing.Point(4, 22);
            this.tabPage_NoQuest.Name = "tabPage_NoQuest";
            this.tabPage_NoQuest.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_NoQuest.Size = new System.Drawing.Size(1168, 713);
            this.tabPage_NoQuest.TabIndex = 0;
            this.tabPage_NoQuest.Text = "퀘스트 없을 때 단계";
            this.tabPage_NoQuest.UseVisualStyleBackColor = true;
            // 
            // panel_TabMainNoQuest
            // 
            this.panel_TabMainNoQuest.AutoScroll = true;
            this.panel_TabMainNoQuest.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_TabMainNoQuest.Location = new System.Drawing.Point(3, 37);
            this.panel_TabMainNoQuest.Name = "panel_TabMainNoQuest";
            this.panel_TabMainNoQuest.Size = new System.Drawing.Size(1162, 673);
            this.panel_TabMainNoQuest.TabIndex = 1;
            // 
            // panel_TopNoQuest
            // 
            this.panel_TopNoQuest.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopNoQuest.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopNoQuest.Controls.Add(this.button_NoQuestReset);
            this.panel_TopNoQuest.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopNoQuest.Location = new System.Drawing.Point(3, 3);
            this.panel_TopNoQuest.Name = "panel_TopNoQuest";
            this.panel_TopNoQuest.Size = new System.Drawing.Size(1162, 34);
            this.panel_TopNoQuest.TabIndex = 0;
            // 
            // button_NoQuestReset
            // 
            this.button_NoQuestReset.Location = new System.Drawing.Point(930, 4);
            this.button_NoQuestReset.Name = "button_NoQuestReset";
            this.button_NoQuestReset.Size = new System.Drawing.Size(75, 23);
            this.button_NoQuestReset.TabIndex = 0;
            this.button_NoQuestReset.Text = "단계 리셋";
            this.button_NoQuestReset.UseVisualStyleBackColor = true;
            this.button_NoQuestReset.Click += new System.EventHandler(this.button_NoQuestReset_Click);
            // 
            // tabPage_Accept
            // 
            this.tabPage_Accept.Controls.Add(this.panel_TabMainAccept);
            this.tabPage_Accept.Controls.Add(this.panel_TopAccept);
            this.tabPage_Accept.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Accept.Name = "tabPage_Accept";
            this.tabPage_Accept.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Accept.Size = new System.Drawing.Size(1168, 713);
            this.tabPage_Accept.TabIndex = 1;
            this.tabPage_Accept.Text = "퀘스트 받기 단계";
            this.tabPage_Accept.UseVisualStyleBackColor = true;
            // 
            // panel_TabMainAccept
            // 
            this.panel_TabMainAccept.AutoScroll = true;
            this.panel_TabMainAccept.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_TabMainAccept.Location = new System.Drawing.Point(3, 37);
            this.panel_TabMainAccept.Name = "panel_TabMainAccept";
            this.panel_TabMainAccept.Size = new System.Drawing.Size(1162, 673);
            this.panel_TabMainAccept.TabIndex = 2;
            // 
            // panel_TopAccept
            // 
            this.panel_TopAccept.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopAccept.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopAccept.Controls.Add(this.button_AcceptReset);
            this.panel_TopAccept.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopAccept.Location = new System.Drawing.Point(3, 3);
            this.panel_TopAccept.Name = "panel_TopAccept";
            this.panel_TopAccept.Size = new System.Drawing.Size(1162, 34);
            this.panel_TopAccept.TabIndex = 1;
            // 
            // button_AcceptReset
            // 
            this.button_AcceptReset.Location = new System.Drawing.Point(930, 4);
            this.button_AcceptReset.Name = "button_AcceptReset";
            this.button_AcceptReset.Size = new System.Drawing.Size(75, 23);
            this.button_AcceptReset.TabIndex = 0;
            this.button_AcceptReset.Text = "단계 리셋";
            this.button_AcceptReset.UseVisualStyleBackColor = true;
            this.button_AcceptReset.Click += new System.EventHandler(this.button_AcceptReset_Click);
            // 
            // tabPage_General
            // 
            this.tabPage_General.Controls.Add(this.panel_TabMainGeneral);
            this.tabPage_General.Controls.Add(this.panel_TopGeneral);
            this.tabPage_General.Location = new System.Drawing.Point(4, 22);
            this.tabPage_General.Name = "tabPage_General";
            this.tabPage_General.Size = new System.Drawing.Size(1168, 713);
            this.tabPage_General.TabIndex = 2;
            this.tabPage_General.Text = "퀘스트 일반 단계";
            this.tabPage_General.UseVisualStyleBackColor = true;
            // 
            // panel_TabMainGeneral
            // 
            this.panel_TabMainGeneral.AutoScroll = true;
            this.panel_TabMainGeneral.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_TabMainGeneral.Location = new System.Drawing.Point(0, 34);
            this.panel_TabMainGeneral.Name = "panel_TabMainGeneral";
            this.panel_TabMainGeneral.Size = new System.Drawing.Size(1168, 679);
            this.panel_TabMainGeneral.TabIndex = 3;
            // 
            // panel_TopGeneral
            // 
            this.panel_TopGeneral.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopGeneral.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopGeneral.Controls.Add(this.button_GeneralDel);
            this.panel_TopGeneral.Controls.Add(this.button_GeneralAdd);
            this.panel_TopGeneral.Controls.Add(this.button_GeneralReset);
            this.panel_TopGeneral.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopGeneral.Location = new System.Drawing.Point(0, 0);
            this.panel_TopGeneral.Name = "panel_TopGeneral";
            this.panel_TopGeneral.Size = new System.Drawing.Size(1168, 34);
            this.panel_TopGeneral.TabIndex = 2;
            // 
            // button_GeneralDel
            // 
            this.button_GeneralDel.Location = new System.Drawing.Point(930, 4);
            this.button_GeneralDel.Name = "button_GeneralDel";
            this.button_GeneralDel.Size = new System.Drawing.Size(75, 23);
            this.button_GeneralDel.TabIndex = 2;
            this.button_GeneralDel.Text = "단계 삭제";
            this.button_GeneralDel.UseVisualStyleBackColor = true;
            this.button_GeneralDel.Click += new System.EventHandler(this.button_GeneralDel_Click);
            // 
            // button_GeneralAdd
            // 
            this.button_GeneralAdd.Location = new System.Drawing.Point(849, 4);
            this.button_GeneralAdd.Name = "button_GeneralAdd";
            this.button_GeneralAdd.Size = new System.Drawing.Size(75, 23);
            this.button_GeneralAdd.TabIndex = 1;
            this.button_GeneralAdd.Text = "단계 추가";
            this.button_GeneralAdd.UseVisualStyleBackColor = true;
            this.button_GeneralAdd.Click += new System.EventHandler(this.button_GeneralAdd_Click);
            // 
            // button_GeneralReset
            // 
            this.button_GeneralReset.Location = new System.Drawing.Point(1011, 3);
            this.button_GeneralReset.Name = "button_GeneralReset";
            this.button_GeneralReset.Size = new System.Drawing.Size(75, 23);
            this.button_GeneralReset.TabIndex = 0;
            this.button_GeneralReset.Text = "단계 리셋";
            this.button_GeneralReset.UseVisualStyleBackColor = true;
            this.button_GeneralReset.Click += new System.EventHandler(this.button_GeneralReset_Click);
            // 
            // tabPage_Complete
            // 
            this.tabPage_Complete.Controls.Add(this.panel_TabMainComplete);
            this.tabPage_Complete.Controls.Add(this.panel_TopComplete);
            this.tabPage_Complete.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Complete.Name = "tabPage_Complete";
            this.tabPage_Complete.Size = new System.Drawing.Size(1168, 713);
            this.tabPage_Complete.TabIndex = 3;
            this.tabPage_Complete.Text = "퀘스트 완료 단계";
            this.tabPage_Complete.UseVisualStyleBackColor = true;
            // 
            // panel_TabMainComplete
            // 
            this.panel_TabMainComplete.AutoScroll = true;
            this.panel_TabMainComplete.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_TabMainComplete.Location = new System.Drawing.Point(0, 34);
            this.panel_TabMainComplete.Name = "panel_TabMainComplete";
            this.panel_TabMainComplete.Size = new System.Drawing.Size(1168, 679);
            this.panel_TabMainComplete.TabIndex = 2;
            // 
            // panel_TopComplete
            // 
            this.panel_TopComplete.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopComplete.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopComplete.Controls.Add(this.button_CompleteReset);
            this.panel_TopComplete.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopComplete.Location = new System.Drawing.Point(0, 0);
            this.panel_TopComplete.Name = "panel_TopComplete";
            this.panel_TopComplete.Size = new System.Drawing.Size(1168, 34);
            this.panel_TopComplete.TabIndex = 1;
            // 
            // button_CompleteReset
            // 
            this.button_CompleteReset.Location = new System.Drawing.Point(930, 4);
            this.button_CompleteReset.Name = "button_CompleteReset";
            this.button_CompleteReset.Size = new System.Drawing.Size(75, 23);
            this.button_CompleteReset.TabIndex = 0;
            this.button_CompleteReset.Text = "단계 리셋";
            this.button_CompleteReset.UseVisualStyleBackColor = true;
            this.button_CompleteReset.Click += new System.EventHandler(this.button_CompleteReset_Click);
            // 
            // tabPage_Journal
            // 
            this.tabPage_Journal.Controls.Add(this.panel_TabMainJournal);
            this.tabPage_Journal.Controls.Add(this.panel_TopJournal);
            this.tabPage_Journal.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Journal.Name = "tabPage_Journal";
            this.tabPage_Journal.Size = new System.Drawing.Size(1168, 713);
            this.tabPage_Journal.TabIndex = 4;
            this.tabPage_Journal.Text = "퀘스트 저널";
            this.tabPage_Journal.UseVisualStyleBackColor = true;
            // 
            // panel_TabMainJournal
            // 
            this.panel_TabMainJournal.AutoScroll = true;
            this.panel_TabMainJournal.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_TabMainJournal.Location = new System.Drawing.Point(0, 34);
            this.panel_TabMainJournal.Name = "panel_TabMainJournal";
            this.panel_TabMainJournal.Size = new System.Drawing.Size(1168, 679);
            this.panel_TabMainJournal.TabIndex = 2;
            // 
            // panel_TopJournal
            // 
            this.panel_TopJournal.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopJournal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopJournal.Controls.Add(this.button_JournalCopy);
            this.panel_TopJournal.Controls.Add(this.button_JournalDel);
            this.panel_TopJournal.Controls.Add(this.button_JournalAdd);
            this.panel_TopJournal.Controls.Add(this.button_Journal);
            this.panel_TopJournal.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopJournal.Location = new System.Drawing.Point(0, 0);
            this.panel_TopJournal.Name = "panel_TopJournal";
            this.panel_TopJournal.Size = new System.Drawing.Size(1168, 34);
            this.panel_TopJournal.TabIndex = 1;
            // 
            // button_JournalCopy
            // 
            this.button_JournalCopy.Location = new System.Drawing.Point(849, 4);
            this.button_JournalCopy.Name = "button_JournalCopy";
            this.button_JournalCopy.Size = new System.Drawing.Size(75, 23);
            this.button_JournalCopy.TabIndex = 3;
            this.button_JournalCopy.Text = "저널 복사";
            this.button_JournalCopy.UseVisualStyleBackColor = true;
            this.button_JournalCopy.Click += new System.EventHandler(this.button_JournalCopy_Click);
            // 
            // button_JournalDel
            // 
            this.button_JournalDel.Location = new System.Drawing.Point(930, 4);
            this.button_JournalDel.Name = "button_JournalDel";
            this.button_JournalDel.Size = new System.Drawing.Size(75, 23);
            this.button_JournalDel.TabIndex = 2;
            this.button_JournalDel.Text = "저널 삭제";
            this.button_JournalDel.UseVisualStyleBackColor = true;
            this.button_JournalDel.Click += new System.EventHandler(this.button_JournalDel_Click);
            // 
            // button_JournalAdd
            // 
            this.button_JournalAdd.Location = new System.Drawing.Point(768, 4);
            this.button_JournalAdd.Name = "button_JournalAdd";
            this.button_JournalAdd.Size = new System.Drawing.Size(75, 23);
            this.button_JournalAdd.TabIndex = 1;
            this.button_JournalAdd.Text = "저널 추가";
            this.button_JournalAdd.UseVisualStyleBackColor = true;
            this.button_JournalAdd.Click += new System.EventHandler(this.button_JournalAdd_Click);
            // 
            // button_Journal
            // 
            this.button_Journal.Location = new System.Drawing.Point(1011, 4);
            this.button_Journal.Name = "button_Journal";
            this.button_Journal.Size = new System.Drawing.Size(75, 23);
            this.button_Journal.TabIndex = 0;
            this.button_Journal.Text = "저널 리셋";
            this.button_Journal.UseVisualStyleBackColor = true;
            this.button_Journal.Click += new System.EventHandler(this.button_Journal_Click);
            // 
            // frm_Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(1190, 795);
            this.Controls.Add(this.panel_Main);
            this.Controls.Add(this.menu_Main);
            this.Name = "frm_Main";
            this.Text = "드래곤네스트 퀘스트 에디터";
            this.Load += new System.EventHandler(this.frm_Main_Load);
            this.menu_Main.ResumeLayout(false);
            this.menu_Main.PerformLayout();
            this.panel_Main.ResumeLayout(false);
            this.tabControl_Main.ResumeLayout(false);
            this.tabPage_Basic.ResumeLayout(false);
            this.panel_Basic.ResumeLayout(false);
            this.groupBox_QuestCondition.ResumeLayout(false);
            this.groupBox_QuestCondition.PerformLayout();
            this.groupBox_NPC.ResumeLayout(false);
            this.groupBox_NextQuest.ResumeLayout(false);
            this.groupBox_NextQuest.PerformLayout();
            this.groupBox_Basic.ResumeLayout(false);
            this.groupBox_Basic.PerformLayout();
            this.tabPage_Flow.ResumeLayout(false);
            this.tabControl_Flow.ResumeLayout(false);
            this.tabPage_NoQuest.ResumeLayout(false);
            this.panel_TopNoQuest.ResumeLayout(false);
            this.tabPage_Accept.ResumeLayout(false);
            this.panel_TopAccept.ResumeLayout(false);
            this.tabPage_General.ResumeLayout(false);
            this.panel_TopGeneral.ResumeLayout(false);
            this.tabPage_Complete.ResumeLayout(false);
            this.panel_TopComplete.ResumeLayout(false);
            this.tabPage_Journal.ResumeLayout(false);
            this.panel_TopJournal.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menu_Main;
        private System.Windows.Forms.Panel panel_Main;
        private System.Windows.Forms.TabControl tabControl_Main;
        private System.Windows.Forms.TabPage tabPage_Basic;
        private System.Windows.Forms.TabPage tabPage_Flow;
        private System.Windows.Forms.Panel panel_Basic;
        private System.Windows.Forms.GroupBox groupBox_Basic;
        private System.Windows.Forms.ComboBox comboBox_Type;
        private System.Windows.Forms.Label label_Type;
        private System.Windows.Forms.TextBox textBox_EngName;
        private System.Windows.Forms.Label label_EngName;
        private System.Windows.Forms.TextBox textBox_KorName;
        private System.Windows.Forms.Label label_KorName;
        private System.Windows.Forms.Label label_StartLevel;
        private System.Windows.Forms.Label label_Damdang;
        private System.Windows.Forms.TextBox textBox_Abstrct;
        private System.Windows.Forms.TextBox textBox_StartLevel;
        private System.Windows.Forms.TextBox textBox_QuestID;
        private System.Windows.Forms.TextBox textBox_Chapter;
        private System.Windows.Forms.Label label_QuestID;
        private System.Windows.Forms.Label label_Chapter;
        private System.Windows.Forms.ComboBox comboBox_CompleteType;
        private System.Windows.Forms.Label label_CompleteType;
        private System.Windows.Forms.Label label_Abstract;
        private System.Windows.Forms.GroupBox groupBox_NPC;
        private System.Windows.Forms.Panel panel_splitter;
        private System.Windows.Forms.ListBox listBox_NPC;
        private System.Windows.Forms.Button button_NPCDel;
        private System.Windows.Forms.Button button_NPCAdd;
        private System.Windows.Forms.TabControl tabControl_Flow;
        private System.Windows.Forms.TabPage tabPage_NoQuest;
        private System.Windows.Forms.TabPage tabPage_Accept;
        private System.Windows.Forms.TabPage tabPage_General;
        private System.Windows.Forms.TabPage tabPage_Complete;
        private System.Windows.Forms.TabPage tabPage_Journal;
        private System.Windows.Forms.Panel panel_TopNoQuest;
        private System.Windows.Forms.Button button_NoQuestReset;
        private System.Windows.Forms.Panel panel_TabMainNoQuest;
        private System.Windows.Forms.Panel panel_TopAccept;
        private System.Windows.Forms.Button button_AcceptReset;
        private System.Windows.Forms.Panel panel_TabMainJournal;
        private System.Windows.Forms.Panel panel_TopJournal;
        private System.Windows.Forms.Button button_Journal;
        private System.Windows.Forms.Panel panel_TabMainAccept;
        private System.Windows.Forms.Button button_JournalAdd;
        private System.Windows.Forms.Panel panel_TabMainComplete;
        private System.Windows.Forms.Panel panel_TopComplete;
        private System.Windows.Forms.Button button_CompleteReset;
        private System.Windows.Forms.Panel panel_TabMainGeneral;
        private System.Windows.Forms.Panel panel_TopGeneral;
        private System.Windows.Forms.Button button_GeneralAdd;
        private System.Windows.Forms.Button button_GeneralReset;
        private System.Windows.Forms.ToolStripMenuItem 초기화ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 저장하기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 닫기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 불러오기ToolStripMenuItem;
        private System.Windows.Forms.Button button_GeneralDel;
        private System.Windows.Forms.Button button_JournalDel;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox textBox_QuestDifficulty;
        private System.Windows.Forms.Label label12;
        private GroupBox groupBox_QuestCondition;
        private Label label15;
        private Label label14;
        private TextBox txtPeriodConditionOnDayQuest;
        private TextBox textBox_UserMissionClearCondition;
        private Label label13;
        private TextBox textBox_symbolcondition;
        private TextBox textBox_itemcondition;
        private TextBox textBox_prevquestcondition;
        private TextBox textBox_classcondition;
        private TextBox textBox_worldcondition;
        private TextBox textBox_levelconditon;
        private Label label7;
        private Label label8;
        private Label label9;
        private Label label1;
        private Label label5;
        private Label label6;
        private GroupBox groupBox_NextQuest;
        private Label label2;
        private TextBox textBox_NextNPCEname;
        private Label label4;
        private TextBox textBox_NextID;
        private TextBox textBox_NextEname;
        private Label label3;
        private Label label16;
        private TextBox txtNpc_malice;
        private Label label17;
        private TextBox txtNpc_favor;
        private Button button2;
        private Button button1;
        private TextBox textBox_Damdang;
        private Button btnDelete4NextQuest;
        private CheckBox chkBoxMonday;
        private CheckBox chkBoxSunday;
        private CheckBox chkBoxSaturday;
        private CheckBox chkBoxFriday;
        private CheckBox chkBoxThursday;
        private CheckBox chkBoxWednesDay;
        private CheckBox chkBoxTuesday;
        private Button btnDeleteUsedItem;
        private Button btnDeleteRecompense;
        private TextBox txtRecompense;
        private TextBox txtDeleteItem;
        private TextBox textBox_optionalQuestcondition;
        private Label label18;
        private Label label_Statemark1;
        private Label label_Statemark0;
        private Label label_Questmark;
        private TextBox textBox_Statemark1;
        private TextBox textBox_Statemark0;
        private TextBox textBox_Questmark;
        private Label label19;
        private TextBox textBox_cashitemcondition;
        private ComboBox comboBox_UIStyle;
        private Label label_UIStyle;
        private Button button_JournalCopy;
    }
}

