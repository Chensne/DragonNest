namespace DNQuest
{
    partial class Quest_Condition
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
            this.groupBox_Unit = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox_Condition = new System.Windows.Forms.GroupBox();
            this.button_CondtionDel = new System.Windows.Forms.Button();
            this.button_ConditionAdd = new System.Windows.Forms.Button();
            this.comboBox_Condition = new System.Windows.Forms.ComboBox();
            this.groupBox_Execution = new System.Windows.Forms.GroupBox();
            this.button_ExeDel = new System.Windows.Forms.Button();
            this.comboBox_Execution = new System.Windows.Forms.ComboBox();
            this.button_ExeAdd = new System.Windows.Forms.Button();
            this.groupBox_Detail = new System.Windows.Forms.GroupBox();
            this.textBox_Detail = new System.Windows.Forms.TextBox();
            this.groupBox_Unit.SuspendLayout();
            this.groupBox_Condition.SuspendLayout();
            this.groupBox_Execution.SuspendLayout();
            this.groupBox_Detail.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Unit
            // 
            this.groupBox_Unit.Controls.Add(this.label1);
            this.groupBox_Unit.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Unit.Name = "groupBox_Unit";
            this.groupBox_Unit.Size = new System.Drawing.Size(144, 213);
            this.groupBox_Unit.TabIndex = 0;
            this.groupBox_Unit.TabStop = false;
            this.groupBox_Unit.Text = "조건 실행문";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(23, 92);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(100, 29);
            this.label1.TabIndex = 6;
            this.label1.Text = "조건실행문";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox_Condition
            // 
            this.groupBox_Condition.Controls.Add(this.button_CondtionDel);
            this.groupBox_Condition.Controls.Add(this.button_ConditionAdd);
            this.groupBox_Condition.Controls.Add(this.comboBox_Condition);
            this.groupBox_Condition.Location = new System.Drawing.Point(153, 3);
            this.groupBox_Condition.Name = "groupBox_Condition";
            this.groupBox_Condition.Size = new System.Drawing.Size(978, 51);
            this.groupBox_Condition.TabIndex = 1;
            this.groupBox_Condition.TabStop = false;
            this.groupBox_Condition.Text = "조건리스트";
            // 
            // button_CondtionDel
            // 
            this.button_CondtionDel.Location = new System.Drawing.Point(314, 18);
            this.button_CondtionDel.Name = "button_CondtionDel";
            this.button_CondtionDel.Size = new System.Drawing.Size(54, 23);
            this.button_CondtionDel.TabIndex = 2;
            this.button_CondtionDel.Text = "삭제";
            this.button_CondtionDel.UseVisualStyleBackColor = true;
            this.button_CondtionDel.Click += new System.EventHandler(this.button_CondtionDel_Click);
            // 
            // button_ConditionAdd
            // 
            this.button_ConditionAdd.Location = new System.Drawing.Point(254, 18);
            this.button_ConditionAdd.Name = "button_ConditionAdd";
            this.button_ConditionAdd.Size = new System.Drawing.Size(54, 23);
            this.button_ConditionAdd.TabIndex = 1;
            this.button_ConditionAdd.Text = "추가";
            this.button_ConditionAdd.UseVisualStyleBackColor = true;
            this.button_ConditionAdd.Click += new System.EventHandler(this.button_ConditionAdd_Click);
            // 
            // comboBox_Condition
            // 
            this.comboBox_Condition.DropDownHeight = 120;
            this.comboBox_Condition.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Condition.FormattingEnabled = true;
            this.comboBox_Condition.IntegralHeight = false;
            this.comboBox_Condition.Items.AddRange(new object[] {
            "유저레벨체크",
            "일반아이템체크",
            "퀘스트아이템체크",
            "유저직업체크",
            "확률체크",
            "퀘스트보유체크",
            "완료퀘스트보유체크",
            "인벤토리빈칸체크",
            "커스텀조건",
            "수행 퀘스트 제한개수 체크",
            "스테이지 난이도 체크"});
            this.comboBox_Condition.Location = new System.Drawing.Point(6, 20);
            this.comboBox_Condition.Name = "comboBox_Condition";
            this.comboBox_Condition.Size = new System.Drawing.Size(241, 20);
            this.comboBox_Condition.TabIndex = 0;
            // 
            // groupBox_Execution
            // 
            this.groupBox_Execution.Controls.Add(this.button_ExeDel);
            this.groupBox_Execution.Controls.Add(this.comboBox_Execution);
            this.groupBox_Execution.Controls.Add(this.button_ExeAdd);
            this.groupBox_Execution.Location = new System.Drawing.Point(153, 60);
            this.groupBox_Execution.Name = "groupBox_Execution";
            this.groupBox_Execution.Size = new System.Drawing.Size(978, 51);
            this.groupBox_Execution.TabIndex = 2;
            this.groupBox_Execution.TabStop = false;
            this.groupBox_Execution.Text = "실행리스트";
            // 
            // button_ExeDel
            // 
            this.button_ExeDel.Location = new System.Drawing.Point(314, 19);
            this.button_ExeDel.Name = "button_ExeDel";
            this.button_ExeDel.Size = new System.Drawing.Size(54, 23);
            this.button_ExeDel.TabIndex = 5;
            this.button_ExeDel.Text = "삭제";
            this.button_ExeDel.UseVisualStyleBackColor = true;
            this.button_ExeDel.Click += new System.EventHandler(this.button_ExeDel_Click);
            // 
            // comboBox_Execution
            // 
            this.comboBox_Execution.DropDownHeight = 460;
            this.comboBox_Execution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Execution.DropDownWidth = 260;
            this.comboBox_Execution.FormattingEnabled = true;
            this.comboBox_Execution.IntegralHeight = false;
            this.comboBox_Execution.Items.AddRange(new object[] {
            "---------------------------",
            "[상태] 퀘스트 받기",
            "[상태] 퀘스트 완료",
            "[상태] 퀘스트 스텝 변경",
            "[상태] 퀘스트 저널 변경",
            "[상태] 보상창출력(보여주기)",
            "[상태] 보상창출력(보상지급)",
            "[상태] 일일 퀘스트 받기",
            "---------------------------",
            "[임무] 몬스터사냥추가",
            "[임무] 아이템(일반/퀘스트)수집추가",
            "[임무] 스테이지 클리어 랭크 조건",
            "[임무] 모든 등록된 사냥, 수집 삭제",
            "[임무] 등록 모든 수집 테스트가 완료 됐을 때 실행설정",
            "---------------------------",
            "[실행] 조건체크 실행",
            "[실행] 확률 택일 실행",
            "[실행] 대사창 출력",
            "[실행] 메세지 출력",
            "---------------------------",
            "[아이템] 퀘스트아이템추가",
            "[아이템] 퀘스트아이템삭제",
            "[아이템] 일반아이템추가",
            "[아이템] 일반아이템삭제",
            "[아이템] 유저 돈 소모시키기",
            "[아이템] 일반/퀘스트아이템추가",
            "---------------------------",
            "[기타] 커스텀 코드",
            "[기타] 코멘트",
            "---------------------------"});
            this.comboBox_Execution.Location = new System.Drawing.Point(6, 20);
            this.comboBox_Execution.Name = "comboBox_Execution";
            this.comboBox_Execution.Size = new System.Drawing.Size(241, 20);
            this.comboBox_Execution.TabIndex = 3;
            // 
            // button_ExeAdd
            // 
            this.button_ExeAdd.Location = new System.Drawing.Point(254, 19);
            this.button_ExeAdd.Name = "button_ExeAdd";
            this.button_ExeAdd.Size = new System.Drawing.Size(54, 23);
            this.button_ExeAdd.TabIndex = 4;
            this.button_ExeAdd.Text = "추가";
            this.button_ExeAdd.UseVisualStyleBackColor = true;
            this.button_ExeAdd.Click += new System.EventHandler(this.button_ExeAdd_Click);
            // 
            // groupBox_Detail
            // 
            this.groupBox_Detail.Controls.Add(this.textBox_Detail);
            this.groupBox_Detail.Location = new System.Drawing.Point(153, 117);
            this.groupBox_Detail.Name = "groupBox_Detail";
            this.groupBox_Detail.Size = new System.Drawing.Size(978, 99);
            this.groupBox_Detail.TabIndex = 3;
            this.groupBox_Detail.TabStop = false;
            this.groupBox_Detail.Text = "설명";
            // 
            // textBox_Detail
            // 
            this.textBox_Detail.Location = new System.Drawing.Point(6, 20);
            this.textBox_Detail.Multiline = true;
            this.textBox_Detail.Name = "textBox_Detail";
            this.textBox_Detail.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_Detail.Size = new System.Drawing.Size(966, 73);
            this.textBox_Detail.TabIndex = 0;
            this.textBox_Detail.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_Detail_KeyDown);
            // 
            // Quest_Condition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Detail);
            this.Controls.Add(this.groupBox_Execution);
            this.Controls.Add(this.groupBox_Condition);
            this.Controls.Add(this.groupBox_Unit);
            this.Name = "Quest_Condition";
            this.Size = new System.Drawing.Size(1134, 219);
            this.Load += new System.EventHandler(this.Quest_Condition_Load);
            this.SizeChanged += new System.EventHandler(this.Quest_Condition_SizeChanged);
            this.groupBox_Unit.ResumeLayout(false);
            this.groupBox_Condition.ResumeLayout(false);
            this.groupBox_Execution.ResumeLayout(false);
            this.groupBox_Detail.ResumeLayout(false);
            this.groupBox_Detail.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_Unit;
        private System.Windows.Forms.GroupBox groupBox_Condition;
        private System.Windows.Forms.GroupBox groupBox_Execution;
        private System.Windows.Forms.GroupBox groupBox_Detail;
        private System.Windows.Forms.TextBox textBox_Detail;
        private System.Windows.Forms.Button button_CondtionDel;
        private System.Windows.Forms.Button button_ConditionAdd;
        private System.Windows.Forms.ComboBox comboBox_Condition;
        private System.Windows.Forms.Button button_ExeDel;
        private System.Windows.Forms.Button button_ExeAdd;
        private System.Windows.Forms.ComboBox comboBox_Execution;
        private System.Windows.Forms.Label label1;
    }
}
