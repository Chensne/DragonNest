namespace DNNpc
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
            this.groupBox_Condition = new System.Windows.Forms.GroupBox();
            this.button_CondtionDel = new System.Windows.Forms.Button();
            this.button_ConditionAdd = new System.Windows.Forms.Button();
            this.comboBox_Condition = new System.Windows.Forms.ComboBox();
            this.groupBox_Execution = new System.Windows.Forms.GroupBox();
            this.button_ExeDel = new System.Windows.Forms.Button();
            this.comboBox_Execution = new System.Windows.Forms.ComboBox();
            this.button_ExeAdd = new System.Windows.Forms.Button();
            this.groupBox_Condition.SuspendLayout();
            this.groupBox_Execution.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Condition
            // 
            this.groupBox_Condition.Controls.Add(this.button_CondtionDel);
            this.groupBox_Condition.Controls.Add(this.button_ConditionAdd);
            this.groupBox_Condition.Controls.Add(this.comboBox_Condition);
            this.groupBox_Condition.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Condition.Name = "groupBox_Condition";
            this.groupBox_Condition.Size = new System.Drawing.Size(834, 51);
            this.groupBox_Condition.TabIndex = 1;
            this.groupBox_Condition.TabStop = false;
            this.groupBox_Condition.Text = "조건리스트";
            // 
            // button_CondtionDel
            // 
            this.button_CondtionDel.Location = new System.Drawing.Point(193, 17);
            this.button_CondtionDel.Name = "button_CondtionDel";
            this.button_CondtionDel.Size = new System.Drawing.Size(54, 23);
            this.button_CondtionDel.TabIndex = 2;
            this.button_CondtionDel.Text = "삭제";
            this.button_CondtionDel.UseVisualStyleBackColor = true;
            this.button_CondtionDel.Click += new System.EventHandler(this.button_CondtionDel_Click);
            // 
            // button_ConditionAdd
            // 
            this.button_ConditionAdd.Location = new System.Drawing.Point(133, 17);
            this.button_ConditionAdd.Name = "button_ConditionAdd";
            this.button_ConditionAdd.Size = new System.Drawing.Size(54, 23);
            this.button_ConditionAdd.TabIndex = 1;
            this.button_ConditionAdd.Text = "추가";
            this.button_ConditionAdd.UseVisualStyleBackColor = true;
            this.button_ConditionAdd.Click += new System.EventHandler(this.button_ConditionAdd_Click);
            // 
            // comboBox_Condition
            // 
            this.comboBox_Condition.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Condition.FormattingEnabled = true;
            this.comboBox_Condition.Items.AddRange(new object[] {
            "유저레벨체크",
            "일반아이템체크",
            "심볼아이템체크",
            "유저직업체크",
            "확률체크",
            "퀘스트보유체크",
            "완료퀘스트보유체크",
            "인벤토리빈칸체크",
            "커스텀조건 추가"});
            this.comboBox_Condition.Location = new System.Drawing.Point(6, 20);
            this.comboBox_Condition.Name = "comboBox_Condition";
            this.comboBox_Condition.Size = new System.Drawing.Size(121, 20);
            this.comboBox_Condition.TabIndex = 0;
            // 
            // groupBox_Execution
            // 
            this.groupBox_Execution.Controls.Add(this.button_ExeDel);
            this.groupBox_Execution.Controls.Add(this.comboBox_Execution);
            this.groupBox_Execution.Controls.Add(this.button_ExeAdd);
            this.groupBox_Execution.Location = new System.Drawing.Point(3, 60);
            this.groupBox_Execution.Name = "groupBox_Execution";
            this.groupBox_Execution.Size = new System.Drawing.Size(834, 51);
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
            this.comboBox_Execution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Execution.FormattingEnabled = true;
            this.comboBox_Execution.Items.AddRange(new object[] {
            "퀘스트 스크립트로 진입",
            "대사창 출력",
            "메세지 출력",
            "게임무비 출력",
            "조건 실행문",
            "아이템 샾 출력",
            "스킬 샆 출력",
            "무인상점 출력",
            "문장조합창 출력",
            "보옥 합치기 창 출력",
            "커스텀 실행문",
            "컷씬 플레이 이후, 맵이동"});
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
            // Quest_Condition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Execution);
            this.Controls.Add(this.groupBox_Condition);
            this.Name = "Quest_Condition";
            this.Size = new System.Drawing.Size(848, 125);
            this.Load += new System.EventHandler(this.Quest_Condition_Load);
            this.SizeChanged += new System.EventHandler(this.Quest_Condition_SizeChanged);
            this.groupBox_Condition.ResumeLayout(false);
            this.groupBox_Execution.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_Condition;
        private System.Windows.Forms.GroupBox groupBox_Execution;
        private System.Windows.Forms.Button button_CondtionDel;
        private System.Windows.Forms.Button button_ConditionAdd;
        private System.Windows.Forms.ComboBox comboBox_Condition;
        private System.Windows.Forms.Button button_ExeDel;
        private System.Windows.Forms.Button button_ExeAdd;
        private System.Windows.Forms.ComboBox comboBox_Execution;
    }
}
