using System.Windows.Forms;
namespace QuestTool
{
    partial class QuestTool
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
            DialogResult result = MessageBox.Show("툴 사용을 종료하시겠습니까?", "확인", MessageBoxButtons.YesNo);
            if (result == System.Windows.Forms.DialogResult.Yes)
            {
                DNQuest.frm_Main.QuestAllSaveCheck = false;
                DNNpc.DNNpc.NpcAllSaveCheck = false;
            }
            if (result == System.Windows.Forms.DialogResult.No)
            {
                return;
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
            this.components = new System.ComponentModel.Container();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nPCToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.questToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.testToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.NpcAllSaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.OpenNPCToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.OpenQuestToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.SaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.닫기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.PathToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.questTool종료QToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.환경설정ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.AutoSaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.yesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.noToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.도움말ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.questTool정보ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBox_Culture = new System.Windows.Forms.ComboBox();
            this.groupBox_NPC = new System.Windows.Forms.GroupBox();
            this.listBox_NPC = new System.Windows.Forms.ListBox();
            this.groupBox_QuestList = new System.Windows.Forms.GroupBox();
            this.listBox_Quest = new System.Windows.Forms.ListBox();
            this.panel_Left = new System.Windows.Forms.Panel();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.button1 = new System.Windows.Forms.Button();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.checkBox5 = new System.Windows.Forms.CheckBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.checkBox4 = new System.Windows.Forms.CheckBox();
            this.checkBox3 = new System.Windows.Forms.CheckBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.button14 = new System.Windows.Forms.Button();
            this.button13 = new System.Windows.Forms.Button();
            this.button12 = new System.Windows.Forms.Button();
            this.button11 = new System.Windows.Forms.Button();
            this.button10 = new System.Windows.Forms.Button();
            this.button9 = new System.Windows.Forms.Button();
            this.button8 = new System.Windows.Forms.Button();
            this.button7 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.comboBox4 = new System.Windows.Forms.ComboBox();
            this.comboBox2 = new System.Windows.Forms.ComboBox();
            this.button2 = new System.Windows.Forms.Button();
            this.checkBox6 = new System.Windows.Forms.CheckBox();
            this.checkBox7 = new System.Windows.Forms.CheckBox();
            this.checkBox8 = new System.Windows.Forms.CheckBox();
            this.checkBox9 = new System.Windows.Forms.CheckBox();
            this.checkBox10 = new System.Windows.Forms.CheckBox();
            this.comboBox3 = new System.Windows.Forms.ComboBox();
            this.button3 = new System.Windows.Forms.Button();
            this.checkBox11 = new System.Windows.Forms.CheckBox();
            this.checkBox12 = new System.Windows.Forms.CheckBox();
            this.checkBox13 = new System.Windows.Forms.CheckBox();
            this.checkBox14 = new System.Windows.Forms.CheckBox();
            this.checkBox15 = new System.Windows.Forms.CheckBox();
            this.timer4AutoSave = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1.SuspendLayout();
            this.groupBox_NPC.SuspendLayout();
            this.groupBox_QuestList.SuspendLayout();
            this.panel_Left.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.AllowMerge = false;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.환경설정ToolStripMenuItem,
            this.도움말ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.MdiWindowListItem = this.fileToolStripMenuItem;
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1496, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.Click += new System.EventHandler(this.menuStrip1_Click);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.testToolStripMenuItem,
            this.NpcAllSaveToolStripMenuItem,
            this.toolStripMenuItem2,
            this.열기ToolStripMenuItem,
            this.SaveToolStripMenuItem,
            this.닫기ToolStripMenuItem,
            this.toolStripSeparator1,
            this.PathToolStripMenuItem,
            this.toolStripMenuItem3,
            this.questTool종료QToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(57, 20);
            this.fileToolStripMenuItem.Text = "파일(&F)";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.nPCToolStripMenuItem,
            this.questToolStripMenuItem});
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.newToolStripMenuItem.Text = "새로 만들기";
            // 
            // nPCToolStripMenuItem
            // 
            this.nPCToolStripMenuItem.Name = "nPCToolStripMenuItem";
            this.nPCToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.nPCToolStripMenuItem.Text = "NPC(&n)";
            this.nPCToolStripMenuItem.Click += new System.EventHandler(this.nPCToolStripMenuItem_Click);
            // 
            // questToolStripMenuItem
            // 
            this.questToolStripMenuItem.Name = "questToolStripMenuItem";
            this.questToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.questToolStripMenuItem.Text = "퀘스트(&m)";
            this.questToolStripMenuItem.Click += new System.EventHandler(this.questToolStripMenuItem_Click);
            // 
            // testToolStripMenuItem
            // 
            this.testToolStripMenuItem.Name = "testToolStripMenuItem";
            this.testToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.testToolStripMenuItem.Text = "퀘스트 모두 저장(&U)";
            this.testToolStripMenuItem.Click += new System.EventHandler(this.testToolStripMenuItem_Click);
            // 
            // NpcAllSaveToolStripMenuItem
            // 
            this.NpcAllSaveToolStripMenuItem.Name = "NpcAllSaveToolStripMenuItem";
            this.NpcAllSaveToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.NpcAllSaveToolStripMenuItem.Text = "NPC 모두 저장(&L)";
            this.NpcAllSaveToolStripMenuItem.Click += new System.EventHandler(this.NpcAllSaveToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(188, 6);
            // 
            // 열기ToolStripMenuItem
            // 
            this.열기ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.OpenNPCToolStripMenuItem,
            this.OpenQuestToolStripMenuItem});
            this.열기ToolStripMenuItem.Name = "열기ToolStripMenuItem";
            this.열기ToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.열기ToolStripMenuItem.Text = "열기(&O)";
            // 
            // OpenNPCToolStripMenuItem
            // 
            this.OpenNPCToolStripMenuItem.Name = "OpenNPCToolStripMenuItem";
            this.OpenNPCToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.OpenNPCToolStripMenuItem.Text = "NPC";
            this.OpenNPCToolStripMenuItem.Click += new System.EventHandler(this.OpenNPCToolStripMenuItem_Click);
            // 
            // OpenQuestToolStripMenuItem
            // 
            this.OpenQuestToolStripMenuItem.Name = "OpenQuestToolStripMenuItem";
            this.OpenQuestToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.OpenQuestToolStripMenuItem.Text = "퀘스트";
            this.OpenQuestToolStripMenuItem.Click += new System.EventHandler(this.OpenQuestToolStripMenuItem_Click);
            // 
            // SaveToolStripMenuItem
            // 
            this.SaveToolStripMenuItem.Name = "SaveToolStripMenuItem";
            this.SaveToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.SaveToolStripMenuItem.Text = "저장하기(&S)";
            this.SaveToolStripMenuItem.Click += new System.EventHandler(this.SaveToolStripMenuItem_Click);
            // 
            // 닫기ToolStripMenuItem
            // 
            this.닫기ToolStripMenuItem.Name = "닫기ToolStripMenuItem";
            this.닫기ToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.닫기ToolStripMenuItem.Text = "닫기(&C)";
            this.닫기ToolStripMenuItem.Click += new System.EventHandler(this.닫기ToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(188, 6);
            // 
            // PathToolStripMenuItem
            // 
            this.PathToolStripMenuItem.Name = "PathToolStripMenuItem";
            this.PathToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.PathToolStripMenuItem.Text = "퀘스트/NPC 경로셋팅";
            this.PathToolStripMenuItem.Click += new System.EventHandler(this.PathToolStripMenuItem_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(188, 6);
            // 
            // questTool종료QToolStripMenuItem
            // 
            this.questTool종료QToolStripMenuItem.Name = "questTool종료QToolStripMenuItem";
            this.questTool종료QToolStripMenuItem.Size = new System.Drawing.Size(191, 22);
            this.questTool종료QToolStripMenuItem.Text = "QuestTool 종료(&Q)";
            this.questTool종료QToolStripMenuItem.Click += new System.EventHandler(this.questTool종료QToolStripMenuItem_Click);
            // 
            // 환경설정ToolStripMenuItem
            // 
            this.환경설정ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.AutoSaveToolStripMenuItem});
            this.환경설정ToolStripMenuItem.Name = "환경설정ToolStripMenuItem";
            this.환경설정ToolStripMenuItem.Size = new System.Drawing.Size(67, 20);
            this.환경설정ToolStripMenuItem.Text = "환경설정";
            // 
            // AutoSaveToolStripMenuItem
            // 
            this.AutoSaveToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.yesToolStripMenuItem,
            this.noToolStripMenuItem});
            this.AutoSaveToolStripMenuItem.Name = "AutoSaveToolStripMenuItem";
            this.AutoSaveToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.AutoSaveToolStripMenuItem.Text = "자동저장설정";
            // 
            // yesToolStripMenuItem
            // 
            this.yesToolStripMenuItem.Name = "yesToolStripMenuItem";
            this.yesToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.yesToolStripMenuItem.Text = "Yes";
            this.yesToolStripMenuItem.Click += new System.EventHandler(this.yesToolStripMenuItem_Click);
            // 
            // noToolStripMenuItem
            // 
            this.noToolStripMenuItem.Name = "noToolStripMenuItem";
            this.noToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.noToolStripMenuItem.Text = "No";
            this.noToolStripMenuItem.Click += new System.EventHandler(this.noToolStripMenuItem_Click);
            // 
            // 도움말ToolStripMenuItem
            // 
            this.도움말ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.questTool정보ToolStripMenuItem});
            this.도움말ToolStripMenuItem.Name = "도움말ToolStripMenuItem";
            this.도움말ToolStripMenuItem.Size = new System.Drawing.Size(72, 20);
            this.도움말ToolStripMenuItem.Text = "도움말(&H)";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(169, 6);
            // 
            // questTool정보ToolStripMenuItem
            // 
            this.questTool정보ToolStripMenuItem.Name = "questTool정보ToolStripMenuItem";
            this.questTool정보ToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
            this.questTool정보ToolStripMenuItem.Text = "QuestTool 정보(&A)";
            this.questTool정보ToolStripMenuItem.Click += new System.EventHandler(this.questTool정보ToolStripMenuItem_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "국가 선택";
            // 
            // comboBox_Culture
            // 
            this.comboBox_Culture.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Culture.FormattingEnabled = true;
            this.comboBox_Culture.Items.AddRange(new object[] {
            "공통",
            "한국",
            "중국",
            "일본",
            "미국",
            "대만",
            "싱가포르&말레이시아",
            "태국",
            "인도네시아",
            "러시아",
            "유럽"});
            this.comboBox_Culture.Location = new System.Drawing.Point(73, 10);
            this.comboBox_Culture.Name = "comboBox_Culture";
            this.comboBox_Culture.Size = new System.Drawing.Size(218, 20);
            this.comboBox_Culture.TabIndex = 2;
            this.comboBox_Culture.SelectedIndexChanged += new System.EventHandler(this.comboBox_Culture_SelectedIndexChanged);
            // 
            // groupBox_NPC
            // 
            this.groupBox_NPC.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox_NPC.Controls.Add(this.listBox_NPC);
            this.groupBox_NPC.Location = new System.Drawing.Point(3, 113);
            this.groupBox_NPC.Name = "groupBox_NPC";
            this.groupBox_NPC.Size = new System.Drawing.Size(291, 972);
            this.groupBox_NPC.TabIndex = 1;
            this.groupBox_NPC.TabStop = false;
            this.groupBox_NPC.Text = "NPC 선택";
            // 
            // listBox_NPC
            // 
            this.listBox_NPC.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_NPC.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
            this.listBox_NPC.FormattingEnabled = true;
            this.listBox_NPC.ItemHeight = 20;
            this.listBox_NPC.Location = new System.Drawing.Point(3, 17);
            this.listBox_NPC.Name = "listBox_NPC";
            this.listBox_NPC.Size = new System.Drawing.Size(285, 952);
            this.listBox_NPC.TabIndex = 1;
            this.listBox_NPC.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.listBox_NPC_DrawItem);
            this.listBox_NPC.DoubleClick += new System.EventHandler(this.listBox_NPC_DoubleClick);
            // 
            // groupBox_QuestList
            // 
            this.groupBox_QuestList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox_QuestList.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBox_QuestList.Controls.Add(this.listBox_Quest);
            this.groupBox_QuestList.Location = new System.Drawing.Point(3, 72);
            this.groupBox_QuestList.Name = "groupBox_QuestList";
            this.groupBox_QuestList.Size = new System.Drawing.Size(291, 630);
            this.groupBox_QuestList.TabIndex = 0;
            this.groupBox_QuestList.TabStop = false;
            this.groupBox_QuestList.Text = "선택";
            // 
            // listBox_Quest
            // 
            this.listBox_Quest.BackColor = System.Drawing.SystemColors.HighlightText;
            this.listBox_Quest.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_Quest.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listBox_Quest.FormattingEnabled = true;
            this.listBox_Quest.ItemHeight = 20;
            this.listBox_Quest.Location = new System.Drawing.Point(3, 17);
            this.listBox_Quest.Name = "listBox_Quest";
            this.listBox_Quest.Size = new System.Drawing.Size(285, 604);
            this.listBox_Quest.TabIndex = 0;
            this.listBox_Quest.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.listBox_Quest_DrawItem);
            this.listBox_Quest.DoubleClick += new System.EventHandler(this.listBox_Quest_DoubleClick);
            this.listBox_Quest.MouseDown += new System.Windows.Forms.MouseEventHandler(this.listBox_Quest_MouseDown);
            // 
            // panel_Left
            // 
            this.panel_Left.AllowDrop = true;
            this.panel_Left.Controls.Add(this.comboBox_Culture);
            this.panel_Left.Controls.Add(this.label1);
            this.panel_Left.Controls.Add(this.tabControl1);
            this.panel_Left.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel_Left.Location = new System.Drawing.Point(0, 24);
            this.panel_Left.Name = "panel_Left";
            this.panel_Left.Size = new System.Drawing.Size(305, 772);
            this.panel_Left.TabIndex = 2;
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(0, 36);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(305, 736);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox1);
            this.tabPage1.Controls.Add(this.groupBox_QuestList);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(297, 710);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Quest";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.comboBox1);
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.checkBox1);
            this.groupBox1.Controls.Add(this.checkBox5);
            this.groupBox1.Controls.Add(this.checkBox2);
            this.groupBox1.Controls.Add(this.checkBox4);
            this.groupBox1.Controls.Add(this.checkBox3);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(291, 67);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "검색";
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(3, 40);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(227, 20);
            this.comboBox1.TabIndex = 7;
            this.comboBox1.TextUpdate += new System.EventHandler(this.comboBox1_Enter);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(236, 38);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(49, 23);
            this.button1.TabIndex = 6;
            this.button1.Text = "초기화";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Enter += new System.EventHandler(this.button1_Enter);
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(6, 20);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(48, 16);
            this.checkBox1.TabIndex = 1;
            this.checkBox1.Text = "메인";
            this.checkBox1.UseVisualStyleBackColor = true;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox5
            // 
            this.checkBox5.AutoSize = true;
            this.checkBox5.Location = new System.Drawing.Point(224, 20);
            this.checkBox5.Name = "checkBox5";
            this.checkBox5.Size = new System.Drawing.Size(60, 16);
            this.checkBox5.TabIndex = 5;
            this.checkBox5.Text = "호감도";
            this.checkBox5.UseVisualStyleBackColor = true;
            this.checkBox5.CheckedChanged += new System.EventHandler(this.checkBox5_CheckedChanged);
            // 
            // checkBox2
            // 
            this.checkBox2.AutoSize = true;
            this.checkBox2.Location = new System.Drawing.Point(55, 20);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(48, 16);
            this.checkBox2.TabIndex = 2;
            this.checkBox2.Text = "서브";
            this.checkBox2.UseVisualStyleBackColor = true;
            this.checkBox2.CheckedChanged += new System.EventHandler(this.checkBox2_CheckedChanged);
            // 
            // checkBox4
            // 
            this.checkBox4.AutoSize = true;
            this.checkBox4.Location = new System.Drawing.Point(160, 20);
            this.checkBox4.Name = "checkBox4";
            this.checkBox4.Size = new System.Drawing.Size(60, 16);
            this.checkBox4.TabIndex = 4;
            this.checkBox4.Text = "모험자";
            this.checkBox4.UseVisualStyleBackColor = true;
            this.checkBox4.CheckedChanged += new System.EventHandler(this.checkBox4_CheckedChanged);
            // 
            // checkBox3
            // 
            this.checkBox3.AutoSize = true;
            this.checkBox3.Location = new System.Drawing.Point(104, 20);
            this.checkBox3.Name = "checkBox3";
            this.checkBox3.Size = new System.Drawing.Size(48, 16);
            this.checkBox3.TabIndex = 3;
            this.checkBox3.Text = "일일";
            this.checkBox3.UseVisualStyleBackColor = true;
            this.checkBox3.CheckedChanged += new System.EventHandler(this.checkBox3_CheckedChanged);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox2);
            this.tabPage2.Controls.Add(this.groupBox_NPC);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(297, 710);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "NPC";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.button14);
            this.groupBox2.Controls.Add(this.button13);
            this.groupBox2.Controls.Add(this.button12);
            this.groupBox2.Controls.Add(this.button11);
            this.groupBox2.Controls.Add(this.button10);
            this.groupBox2.Controls.Add(this.button9);
            this.groupBox2.Controls.Add(this.button8);
            this.groupBox2.Controls.Add(this.button7);
            this.groupBox2.Controls.Add(this.button6);
            this.groupBox2.Controls.Add(this.button5);
            this.groupBox2.Controls.Add(this.button4);
            this.groupBox2.Controls.Add(this.comboBox4);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(3, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(291, 108);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "검색";
            // 
            // button14
            // 
            this.button14.Location = new System.Drawing.Point(195, 73);
            this.button14.Name = "button14";
            this.button14.Size = new System.Drawing.Size(42, 23);
            this.button14.TabIndex = 11;
            this.button14.Text = "900";
            this.button14.UseVisualStyleBackColor = true;
            this.button14.Click += new System.EventHandler(this.button14_Click);
            // 
            // button13
            // 
            this.button13.Location = new System.Drawing.Point(229, 20);
            this.button13.Name = "button13";
            this.button13.Size = new System.Drawing.Size(55, 23);
            this.button13.TabIndex = 10;
            this.button13.Text = "초기화";
            this.button13.UseVisualStyleBackColor = true;
            this.button13.Enter += new System.EventHandler(this.button13_Enter);
            // 
            // button12
            // 
            this.button12.Location = new System.Drawing.Point(145, 73);
            this.button12.Name = "button12";
            this.button12.Size = new System.Drawing.Size(42, 23);
            this.button12.TabIndex = 9;
            this.button12.Text = "800";
            this.button12.UseVisualStyleBackColor = true;
            this.button12.Click += new System.EventHandler(this.button12_Click);
            // 
            // button11
            // 
            this.button11.Location = new System.Drawing.Point(99, 73);
            this.button11.Name = "button11";
            this.button11.Size = new System.Drawing.Size(42, 23);
            this.button11.TabIndex = 8;
            this.button11.Text = "700";
            this.button11.UseVisualStyleBackColor = true;
            this.button11.Click += new System.EventHandler(this.button11_Click);
            // 
            // button10
            // 
            this.button10.Location = new System.Drawing.Point(51, 73);
            this.button10.Name = "button10";
            this.button10.Size = new System.Drawing.Size(42, 23);
            this.button10.TabIndex = 7;
            this.button10.Text = "600";
            this.button10.UseVisualStyleBackColor = true;
            this.button10.Click += new System.EventHandler(this.button10_Click);
            // 
            // button9
            // 
            this.button9.Location = new System.Drawing.Point(3, 73);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(42, 23);
            this.button9.TabIndex = 6;
            this.button9.Text = "500";
            this.button9.UseVisualStyleBackColor = true;
            this.button9.Click += new System.EventHandler(this.button9_Click);
            // 
            // button8
            // 
            this.button8.Location = new System.Drawing.Point(195, 46);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(42, 23);
            this.button8.TabIndex = 5;
            this.button8.Text = "400";
            this.button8.UseVisualStyleBackColor = true;
            this.button8.Click += new System.EventHandler(this.button8_Click);
            // 
            // button7
            // 
            this.button7.Location = new System.Drawing.Point(147, 46);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(42, 23);
            this.button7.TabIndex = 4;
            this.button7.Text = "300";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new System.EventHandler(this.button7_Click);
            // 
            // button6
            // 
            this.button6.Location = new System.Drawing.Point(99, 46);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(42, 23);
            this.button6.TabIndex = 3;
            this.button6.Text = "200";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.button6_Click);
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(51, 46);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(42, 23);
            this.button5.TabIndex = 2;
            this.button5.Text = "100";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(3, 46);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(42, 23);
            this.button4.TabIndex = 1;
            this.button4.Text = "000";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // comboBox4
            // 
            this.comboBox4.FormattingEnabled = true;
            this.comboBox4.Location = new System.Drawing.Point(3, 20);
            this.comboBox4.Name = "comboBox4";
            this.comboBox4.Size = new System.Drawing.Size(220, 20);
            this.comboBox4.TabIndex = 0;
            this.comboBox4.TextUpdate += new System.EventHandler(this.comboBox4_Enter);
            // 
            // comboBox2
            // 
            this.comboBox2.FormattingEnabled = true;
            this.comboBox2.Location = new System.Drawing.Point(3, 42);
            this.comboBox2.Name = "comboBox2";
            this.comboBox2.Size = new System.Drawing.Size(211, 20);
            this.comboBox2.TabIndex = 7;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(220, 42);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(49, 23);
            this.button2.TabIndex = 6;
            this.button2.Text = "검색";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // checkBox6
            // 
            this.checkBox6.AutoSize = true;
            this.checkBox6.Location = new System.Drawing.Point(6, 20);
            this.checkBox6.Name = "checkBox6";
            this.checkBox6.Size = new System.Drawing.Size(48, 16);
            this.checkBox6.TabIndex = 1;
            this.checkBox6.Text = "메인";
            this.checkBox6.UseVisualStyleBackColor = true;
            // 
            // checkBox7
            // 
            this.checkBox7.AutoSize = true;
            this.checkBox7.Location = new System.Drawing.Point(212, 20);
            this.checkBox7.Name = "checkBox7";
            this.checkBox7.Size = new System.Drawing.Size(60, 16);
            this.checkBox7.TabIndex = 5;
            this.checkBox7.Text = "호감도";
            this.checkBox7.UseVisualStyleBackColor = true;
            // 
            // checkBox8
            // 
            this.checkBox8.AutoSize = true;
            this.checkBox8.Location = new System.Drawing.Point(55, 20);
            this.checkBox8.Name = "checkBox8";
            this.checkBox8.Size = new System.Drawing.Size(48, 16);
            this.checkBox8.TabIndex = 2;
            this.checkBox8.Text = "서브";
            this.checkBox8.UseVisualStyleBackColor = true;
            // 
            // checkBox9
            // 
            this.checkBox9.AutoSize = true;
            this.checkBox9.Location = new System.Drawing.Point(152, 20);
            this.checkBox9.Name = "checkBox9";
            this.checkBox9.Size = new System.Drawing.Size(60, 16);
            this.checkBox9.TabIndex = 4;
            this.checkBox9.Text = "모험자";
            this.checkBox9.UseVisualStyleBackColor = true;
            // 
            // checkBox10
            // 
            this.checkBox10.AutoSize = true;
            this.checkBox10.Location = new System.Drawing.Point(104, 20);
            this.checkBox10.Name = "checkBox10";
            this.checkBox10.Size = new System.Drawing.Size(48, 16);
            this.checkBox10.TabIndex = 3;
            this.checkBox10.Text = "일일";
            this.checkBox10.UseVisualStyleBackColor = true;
            // 
            // comboBox3
            // 
            this.comboBox3.FormattingEnabled = true;
            this.comboBox3.Location = new System.Drawing.Point(3, 42);
            this.comboBox3.Name = "comboBox3";
            this.comboBox3.Size = new System.Drawing.Size(211, 20);
            this.comboBox3.TabIndex = 7;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(220, 42);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(49, 23);
            this.button3.TabIndex = 6;
            this.button3.Text = "검색";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // checkBox11
            // 
            this.checkBox11.AutoSize = true;
            this.checkBox11.Location = new System.Drawing.Point(6, 20);
            this.checkBox11.Name = "checkBox11";
            this.checkBox11.Size = new System.Drawing.Size(48, 16);
            this.checkBox11.TabIndex = 1;
            this.checkBox11.Text = "메인";
            this.checkBox11.UseVisualStyleBackColor = true;
            // 
            // checkBox12
            // 
            this.checkBox12.AutoSize = true;
            this.checkBox12.Location = new System.Drawing.Point(212, 20);
            this.checkBox12.Name = "checkBox12";
            this.checkBox12.Size = new System.Drawing.Size(60, 16);
            this.checkBox12.TabIndex = 5;
            this.checkBox12.Text = "호감도";
            this.checkBox12.UseVisualStyleBackColor = true;
            // 
            // checkBox13
            // 
            this.checkBox13.AutoSize = true;
            this.checkBox13.Location = new System.Drawing.Point(55, 20);
            this.checkBox13.Name = "checkBox13";
            this.checkBox13.Size = new System.Drawing.Size(48, 16);
            this.checkBox13.TabIndex = 2;
            this.checkBox13.Text = "서브";
            this.checkBox13.UseVisualStyleBackColor = true;
            // 
            // checkBox14
            // 
            this.checkBox14.AutoSize = true;
            this.checkBox14.Location = new System.Drawing.Point(152, 20);
            this.checkBox14.Name = "checkBox14";
            this.checkBox14.Size = new System.Drawing.Size(60, 16);
            this.checkBox14.TabIndex = 4;
            this.checkBox14.Text = "모험자";
            this.checkBox14.UseVisualStyleBackColor = true;
            // 
            // checkBox15
            // 
            this.checkBox15.AutoSize = true;
            this.checkBox15.Location = new System.Drawing.Point(104, 20);
            this.checkBox15.Name = "checkBox15";
            this.checkBox15.Size = new System.Drawing.Size(48, 16);
            this.checkBox15.TabIndex = 3;
            this.checkBox15.Text = "일일";
            this.checkBox15.UseVisualStyleBackColor = true;
            // 
            // QuestTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1496, 796);
            this.Controls.Add(this.panel_Left);
            this.Controls.Add(this.menuStrip1);
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "QuestTool";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Dragon Nest Quest Tool";
            this.Load += new System.EventHandler(this.QuestTool_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox_NPC.ResumeLayout(false);
            this.groupBox_QuestList.ResumeLayout(false);
            this.panel_Left.ResumeLayout(false);
            this.panel_Left.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem nPCToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem questToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox_QuestList;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboBox_Culture;
        private System.Windows.Forms.GroupBox groupBox_NPC;
        private System.Windows.Forms.ListBox listBox_NPC;
        private System.Windows.Forms.ListBox listBox_Quest;
        private System.Windows.Forms.ToolStripMenuItem testToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 도움말ToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem 열기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem OpenNPCToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem OpenQuestToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 닫기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem questTool종료QToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem questTool정보ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem SaveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem NpcAllSaveToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem PathToolStripMenuItem;
        private Panel panel_Left;
        private TabControl tabControl1;
        private TabPage tabPage1;
        private CheckBox checkBox1;
        private TabPage tabPage2;
        private GroupBox groupBox1;
        private CheckBox checkBox5;
        private CheckBox checkBox2;
        private CheckBox checkBox4;
        private CheckBox checkBox3;
        private ComboBox comboBox1;
        private Button button1;
        private GroupBox groupBox2;
        private Button button12;
        private Button button11;
        private Button button10;
        private Button button9;
        private Button button8;
        private Button button7;
        private Button button6;
        private Button button5;
        private Button button4;
        private ComboBox comboBox4;
        private ComboBox comboBox2;
        private Button button2;
        private CheckBox checkBox6;
        private CheckBox checkBox7;
        private CheckBox checkBox8;
        private CheckBox checkBox9;
        private CheckBox checkBox10;
        private ComboBox comboBox3;
        private Button button3;
        private CheckBox checkBox11;
        private CheckBox checkBox12;
        private CheckBox checkBox13;
        private CheckBox checkBox14;
        private CheckBox checkBox15;
        private Button button13;
        private Button button14;
        private ToolStripMenuItem 환경설정ToolStripMenuItem;
        private ToolStripMenuItem AutoSaveToolStripMenuItem;
        private ToolStripMenuItem yesToolStripMenuItem;
        private ToolStripMenuItem noToolStripMenuItem;
        private Timer timer4AutoSave;        
    }
}

