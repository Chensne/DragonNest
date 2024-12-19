namespace DNQuest
{
    partial class Quest_Journal
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
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 구성 요소 디자이너에서 생성한 코드

        /// <summary> 
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox_Journal = new System.Windows.Forms.GroupBox();
            this.comboBox_StatemarkType = new System.Windows.Forms.ComboBox();
            this.textBox_Message = new System.Windows.Forms.TextBox();
            this.label29 = new System.Windows.Forms.Label();
            this.textBox_JournalContents = new GvS.Controls.HtmlTextbox();
            this.label11 = new System.Windows.Forms.Label();
            this.textBox_StartMarkNpc = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.textBox_JournalObjectiveHelper = new System.Windows.Forms.TextBox();
            this.textBox_JournalObjectivePos = new System.Windows.Forms.TextBox();
            this.textBox_JournalObjectiveName = new System.Windows.Forms.TextBox();
            this.textBox_JournalContentsImage = new System.Windows.Forms.TextBox();
            this.textBox_JournalTitleImage = new System.Windows.Forms.TextBox();
            this.textBox_JournalTitle = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox_JournalNo = new System.Windows.Forms.GroupBox();
            this.label_JournalNo = new System.Windows.Forms.Label();
            this.label31 = new System.Windows.Forms.Label();
            this.label30 = new System.Windows.Forms.Label();
            this.textBox_MapToolIndex = new System.Windows.Forms.TextBox();
            this.textBox_TriggerString = new System.Windows.Forms.TextBox();
            this.groupBox_Trigger = new System.Windows.Forms.GroupBox();
            this.groupBox_Journal.SuspendLayout();
            this.groupBox_JournalNo.SuspendLayout();
            this.groupBox_Trigger.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Journal
            // 
            this.groupBox_Journal.Controls.Add(this.groupBox_Trigger);
            this.groupBox_Journal.Controls.Add(this.comboBox_StatemarkType);
            this.groupBox_Journal.Controls.Add(this.textBox_Message);
            this.groupBox_Journal.Controls.Add(this.label29);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalContents);
            this.groupBox_Journal.Controls.Add(this.label11);
            this.groupBox_Journal.Controls.Add(this.textBox_StartMarkNpc);
            this.groupBox_Journal.Controls.Add(this.label10);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalObjectiveHelper);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalObjectivePos);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalObjectiveName);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalContentsImage);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalTitleImage);
            this.groupBox_Journal.Controls.Add(this.textBox_JournalTitle);
            this.groupBox_Journal.Controls.Add(this.label8);
            this.groupBox_Journal.Controls.Add(this.label7);
            this.groupBox_Journal.Controls.Add(this.label6);
            this.groupBox_Journal.Controls.Add(this.label4);
            this.groupBox_Journal.Controls.Add(this.label3);
            this.groupBox_Journal.Controls.Add(this.label2);
            this.groupBox_Journal.Controls.Add(this.label1);
            this.groupBox_Journal.Location = new System.Drawing.Point(144, 3);
            this.groupBox_Journal.Name = "groupBox_Journal";
            this.groupBox_Journal.Size = new System.Drawing.Size(639, 470);
            this.groupBox_Journal.TabIndex = 0;
            this.groupBox_Journal.TabStop = false;
            this.groupBox_Journal.Text = "저널내용";
            // 
            // comboBox_StatemarkType
            // 
            this.comboBox_StatemarkType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_StatemarkType.FormattingEnabled = true;
            this.comboBox_StatemarkType.Items.AddRange(new object[] {
            "진행중",
            "완료"});
            this.comboBox_StatemarkType.Location = new System.Drawing.Point(145, 319);
            this.comboBox_StatemarkType.Name = "comboBox_StatemarkType";
            this.comboBox_StatemarkType.Size = new System.Drawing.Size(213, 20);
            this.comboBox_StatemarkType.TabIndex = 59;
            // 
            // textBox_Message
            // 
            this.textBox_Message.Location = new System.Drawing.Point(100, 155);
            this.textBox_Message.Name = "textBox_Message";
            this.textBox_Message.Size = new System.Drawing.Size(521, 21);
            this.textBox_Message.TabIndex = 58;
            this.textBox_Message.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Location = new System.Drawing.Point(26, 160);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(73, 12);
            this.label29.TabIndex = 57;
            this.label29.Text = "저널 메시지:";
            // 
            // textBox_JournalContents
            // 
            this.textBox_JournalContents.Fonts = new string[] {
        "Corbel",
        "Corbel, Verdana, Arial, Helvetica, sans-serif",
        "Georgia, Times New Roman, Times, serif",
        "Consolas, Courier New, Courier, monospace"};
            this.textBox_JournalContents.IllegalPatterns = new string[] {
        "<script.*?>",
        "<\\w+\\s+.*?(j|java|vb|ecma)script:.*?>",
        "<\\w+(\\s+|\\s+.*?\\s+)on\\w+\\s*=.+?>",
        "</?input.*?>"};
            this.textBox_JournalContents.Location = new System.Drawing.Point(93, 78);
            this.textBox_JournalContents.Name = "textBox_JournalContents";
            this.textBox_JournalContents.Padding = new System.Windows.Forms.Padding(1);
            this.textBox_JournalContents.ShowHtmlSource = false;
            this.textBox_JournalContents.Size = new System.Drawing.Size(528, 71);
            this.textBox_JournalContents.TabIndex = 56;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(26, 324);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(113, 12);
            this.label11.TabIndex = 20;
            this.label11.Text = "퀘스트 아이콘 타입 ";
            // 
            // textBox_StartMarkNpc
            // 
            this.textBox_StartMarkNpc.Location = new System.Drawing.Point(235, 290);
            this.textBox_StartMarkNpc.Name = "textBox_StartMarkNpc";
            this.textBox_StartMarkNpc.Size = new System.Drawing.Size(334, 21);
            this.textBox_StartMarkNpc.TabIndex = 19;
            this.textBox_StartMarkNpc.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(26, 297);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(203, 12);
            this.label10.TabIndex = 18;
            this.label10.Text = "퀘스트 아이콘 출력할 NPC의 아이디";
            // 
            // textBox_JournalObjectiveHelper
            // 
            this.textBox_JournalObjectiveHelper.Location = new System.Drawing.Point(133, 264);
            this.textBox_JournalObjectiveHelper.Name = "textBox_JournalObjectiveHelper";
            this.textBox_JournalObjectiveHelper.Size = new System.Drawing.Size(488, 21);
            this.textBox_JournalObjectiveHelper.TabIndex = 16;
            this.textBox_JournalObjectiveHelper.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_JournalObjectivePos
            // 
            this.textBox_JournalObjectivePos.Location = new System.Drawing.Point(93, 239);
            this.textBox_JournalObjectivePos.Name = "textBox_JournalObjectivePos";
            this.textBox_JournalObjectivePos.Size = new System.Drawing.Size(528, 21);
            this.textBox_JournalObjectivePos.TabIndex = 15;
            this.textBox_JournalObjectivePos.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_JournalObjectiveName
            // 
            this.textBox_JournalObjectiveName.Location = new System.Drawing.Point(93, 210);
            this.textBox_JournalObjectiveName.Name = "textBox_JournalObjectiveName";
            this.textBox_JournalObjectiveName.Size = new System.Drawing.Size(528, 21);
            this.textBox_JournalObjectiveName.TabIndex = 14;
            this.textBox_JournalObjectiveName.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_JournalContentsImage
            // 
            this.textBox_JournalContentsImage.Location = new System.Drawing.Point(201, 183);
            this.textBox_JournalContentsImage.Name = "textBox_JournalContentsImage";
            this.textBox_JournalContentsImage.Size = new System.Drawing.Size(420, 21);
            this.textBox_JournalContentsImage.TabIndex = 12;
            this.textBox_JournalContentsImage.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_JournalTitleImage
            // 
            this.textBox_JournalTitleImage.Location = new System.Drawing.Point(201, 50);
            this.textBox_JournalTitleImage.Name = "textBox_JournalTitleImage";
            this.textBox_JournalTitleImage.Size = new System.Drawing.Size(420, 21);
            this.textBox_JournalTitleImage.TabIndex = 10;
            this.textBox_JournalTitleImage.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_JournalTitle
            // 
            this.textBox_JournalTitle.Location = new System.Drawing.Point(93, 23);
            this.textBox_JournalTitle.Name = "textBox_JournalTitle";
            this.textBox_JournalTitle.Size = new System.Drawing.Size(528, 21);
            this.textBox_JournalTitle.TabIndex = 9;
            this.textBox_JournalTitle.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(26, 269);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(101, 12);
            this.label8.TabIndex = 7;
            this.label8.Text = "목표 알리미 모드:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(26, 243);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(57, 12);
            this.label7.TabIndex = 6;
            this.label7.Text = "목표좌표:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(26, 213);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(57, 12);
            this.label6.TabIndex = 5;
            this.label6.Text = "목표이름:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(26, 186);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(169, 12);
            this.label4.TabIndex = 3;
            this.label4.Text = "저널 내용 칸의 이미지 파일명:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(26, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(61, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "저널 내용:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(26, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(169, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "저널 제목 칸의 이미지 파일명:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(26, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "저널 제목:";
            // 
            // groupBox_JournalNo
            // 
            this.groupBox_JournalNo.Controls.Add(this.label_JournalNo);
            this.groupBox_JournalNo.Location = new System.Drawing.Point(12, 3);
            this.groupBox_JournalNo.Name = "groupBox_JournalNo";
            this.groupBox_JournalNo.Size = new System.Drawing.Size(126, 470);
            this.groupBox_JournalNo.TabIndex = 18;
            this.groupBox_JournalNo.TabStop = false;
            this.groupBox_JournalNo.Text = "저널 번호";
            // 
            // label_JournalNo
            // 
            this.label_JournalNo.AutoSize = true;
            this.label_JournalNo.Location = new System.Drawing.Point(36, 194);
            this.label_JournalNo.Name = "label_JournalNo";
            this.label_JournalNo.Size = new System.Drawing.Size(0, 12);
            this.label_JournalNo.TabIndex = 18;
            this.label_JournalNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(6, 50);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(37, 12);
            this.label31.TabIndex = 62;
            this.label31.Text = "설명 :";
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Location = new System.Drawing.Point(9, 26);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(98, 12);
            this.label30.TabIndex = 60;
            this.label30.Text = "MapTool Index :";
            // 
            // textBox_MapToolIndex
            // 
            this.textBox_MapToolIndex.Location = new System.Drawing.Point(122, 20);
            this.textBox_MapToolIndex.Name = "textBox_MapToolIndex";
            this.textBox_MapToolIndex.Size = new System.Drawing.Size(227, 21);
            this.textBox_MapToolIndex.TabIndex = 61;
            this.textBox_MapToolIndex.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // textBox_TriggerString
            // 
            this.textBox_TriggerString.Location = new System.Drawing.Point(54, 47);
            this.textBox_TriggerString.Multiline = true;
            this.textBox_TriggerString.Name = "textBox_TriggerString";
            this.textBox_TriggerString.Size = new System.Drawing.Size(544, 57);
            this.textBox_TriggerString.TabIndex = 63;
            this.textBox_TriggerString.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_JournalTitle_KeyDown);
            // 
            // groupBox_Trigger
            // 
            this.groupBox_Trigger.Controls.Add(this.textBox_TriggerString);
            this.groupBox_Trigger.Controls.Add(this.textBox_MapToolIndex);
            this.groupBox_Trigger.Controls.Add(this.label30);
            this.groupBox_Trigger.Controls.Add(this.label31);
            this.groupBox_Trigger.Location = new System.Drawing.Point(23, 346);
            this.groupBox_Trigger.Name = "groupBox_Trigger";
            this.groupBox_Trigger.Size = new System.Drawing.Size(607, 116);
            this.groupBox_Trigger.TabIndex = 64;
            this.groupBox_Trigger.TabStop = false;
            this.groupBox_Trigger.Text = "트리거 정보";
            // 
            // Quest_Journal
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_JournalNo);
            this.Controls.Add(this.groupBox_Journal);
            this.Name = "Quest_Journal";
            this.Size = new System.Drawing.Size(794, 483);
            this.Load += new System.EventHandler(this.Quest_Journal_Load);
            this.groupBox_Journal.ResumeLayout(false);
            this.groupBox_Journal.PerformLayout();
            this.groupBox_JournalNo.ResumeLayout(false);
            this.groupBox_JournalNo.PerformLayout();
            this.groupBox_Trigger.ResumeLayout(false);
            this.groupBox_Trigger.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_Journal;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_JournalObjectiveHelper;
        public System.Windows.Forms.TextBox textBox_JournalObjectivePos;
        public System.Windows.Forms.TextBox textBox_JournalObjectiveName;
        public System.Windows.Forms.TextBox textBox_JournalContentsImage;
        public System.Windows.Forms.TextBox textBox_JournalTitleImage;
        public System.Windows.Forms.TextBox textBox_JournalTitle;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox_JournalNo;
        public System.Windows.Forms.Label label_JournalNo;
        private System.Windows.Forms.Label label11;
        public System.Windows.Forms.TextBox textBox_StartMarkNpc;
        private System.Windows.Forms.Label label10;
        public GvS.Controls.HtmlTextbox textBox_JournalContents;
        public System.Windows.Forms.TextBox textBox_Message;
        private System.Windows.Forms.Label label29;
        public System.Windows.Forms.ComboBox comboBox_StatemarkType;
        private System.Windows.Forms.GroupBox groupBox_Trigger;
        public System.Windows.Forms.TextBox textBox_TriggerString;
        public System.Windows.Forms.TextBox textBox_MapToolIndex;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.Label label31;
    }
}
