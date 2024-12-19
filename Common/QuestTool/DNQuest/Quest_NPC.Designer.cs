namespace DNQuest
{
    partial class Quest_NPC
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
            this.groupBox_NPC = new System.Windows.Forms.GroupBox();
            this.button_SubDel = new System.Windows.Forms.Button();
            this.button_SubAdd = new System.Windows.Forms.Button();
            this.textBox_ExecuteNo = new System.Windows.Forms.TextBox();
            this.label_ExeNo = new System.Windows.Forms.Label();
            this.textBox_NPCName = new System.Windows.Forms.TextBox();
            this.label_EngName = new System.Windows.Forms.Label();
            this.groupBox_NPC.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_NPC
            // 
            this.groupBox_NPC.Controls.Add(this.button_SubDel);
            this.groupBox_NPC.Controls.Add(this.button_SubAdd);
            this.groupBox_NPC.Controls.Add(this.textBox_ExecuteNo);
            this.groupBox_NPC.Controls.Add(this.label_ExeNo);
            this.groupBox_NPC.Controls.Add(this.textBox_NPCName);
            this.groupBox_NPC.Controls.Add(this.label_EngName);
            this.groupBox_NPC.Location = new System.Drawing.Point(13, 3);
            this.groupBox_NPC.Name = "groupBox_NPC";
            this.groupBox_NPC.Size = new System.Drawing.Size(180, 190);
            this.groupBox_NPC.TabIndex = 4;
            this.groupBox_NPC.TabStop = false;
            this.groupBox_NPC.Text = "NPC";
            // 
            // button_SubDel
            // 
            this.button_SubDel.Location = new System.Drawing.Point(8, 129);
            this.button_SubDel.Name = "button_SubDel";
            this.button_SubDel.Size = new System.Drawing.Size(104, 23);
            this.button_SubDel.TabIndex = 5;
            this.button_SubDel.Text = "서브단계 삭제";
            this.button_SubDel.UseVisualStyleBackColor = true;
            this.button_SubDel.Click += new System.EventHandler(this.button_SubDel_Click);
            // 
            // button_SubAdd
            // 
            this.button_SubAdd.Location = new System.Drawing.Point(8, 100);
            this.button_SubAdd.Name = "button_SubAdd";
            this.button_SubAdd.Size = new System.Drawing.Size(104, 23);
            this.button_SubAdd.TabIndex = 4;
            this.button_SubAdd.Text = "서브단계 추가";
            this.button_SubAdd.UseVisualStyleBackColor = true;
            this.button_SubAdd.Click += new System.EventHandler(this.button_SubAdd_Click);
            // 
            // textBox_ExecuteNo
            // 
            this.textBox_ExecuteNo.Location = new System.Drawing.Point(6, 72);
            this.textBox_ExecuteNo.Name = "textBox_ExecuteNo";
            this.textBox_ExecuteNo.Size = new System.Drawing.Size(166, 21);
            this.textBox_ExecuteNo.TabIndex = 3;
            this.textBox_ExecuteNo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_NPCName_KeyDown);
            // 
            // label_ExeNo
            // 
            this.label_ExeNo.AutoSize = true;
            this.label_ExeNo.Location = new System.Drawing.Point(6, 57);
            this.label_ExeNo.Name = "label_ExeNo";
            this.label_ExeNo.Size = new System.Drawing.Size(53, 12);
            this.label_ExeNo.TabIndex = 2;
            this.label_ExeNo.Text = "실행번호";
            // 
            // textBox_NPCName
            // 
            this.textBox_NPCName.Location = new System.Drawing.Point(8, 33);
            this.textBox_NPCName.Name = "textBox_NPCName";
            this.textBox_NPCName.Size = new System.Drawing.Size(166, 21);
            this.textBox_NPCName.TabIndex = 1;
            this.textBox_NPCName.Click += new System.EventHandler(this.textBox_NPCName_Click);
            this.textBox_NPCName.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_NPCName_KeyDown);
            // 
            // label_EngName
            // 
            this.label_EngName.AutoSize = true;
            this.label_EngName.Location = new System.Drawing.Point(6, 17);
            this.label_EngName.Name = "label_EngName";
            this.label_EngName.Size = new System.Drawing.Size(67, 12);
            this.label_EngName.TabIndex = 0;
            this.label_EngName.Text = "NPC영문명";
            // 
            // Quest_NPC
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_NPC);
            this.Name = "Quest_NPC";
            this.Size = new System.Drawing.Size(1072, 194);
            this.groupBox_NPC.ResumeLayout(false);
            this.groupBox_NPC.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.GroupBox groupBox_NPC;
        private System.Windows.Forms.Button button_SubDel;
        private System.Windows.Forms.Button button_SubAdd;
        public System.Windows.Forms.TextBox textBox_ExecuteNo;
        private System.Windows.Forms.Label label_ExeNo;
        public System.Windows.Forms.TextBox textBox_NPCName;
        private System.Windows.Forms.Label label_EngName;
    }
}
