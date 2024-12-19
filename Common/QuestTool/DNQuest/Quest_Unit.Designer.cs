namespace DNQuest
{
    partial class Quest_Unit
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
            this.checkBox_RemoteComplete = new System.Windows.Forms.CheckBox();
            this.button_NPCDel = new System.Windows.Forms.Button();
            this.button_NPCAdd = new System.Windows.Forms.Button();
            this.groupBox_Unit.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Unit
            // 
            this.groupBox_Unit.Controls.Add(this.checkBox_RemoteComplete);
            this.groupBox_Unit.Controls.Add(this.button_NPCDel);
            this.groupBox_Unit.Controls.Add(this.button_NPCAdd);
            this.groupBox_Unit.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Unit.Name = "groupBox_Unit";
            this.groupBox_Unit.Size = new System.Drawing.Size(144, 190);
            this.groupBox_Unit.TabIndex = 0;
            this.groupBox_Unit.TabStop = false;
            this.groupBox_Unit.Text = "퀘스트 단계";
            // 
            // checkBox_RemoteComplete
            // 
            this.checkBox_RemoteComplete.AutoSize = true;
            this.checkBox_RemoteComplete.Location = new System.Drawing.Point(8, 86);
            this.checkBox_RemoteComplete.Name = "checkBox_RemoteComplete";
            this.checkBox_RemoteComplete.Size = new System.Drawing.Size(76, 16);
            this.checkBox_RemoteComplete.TabIndex = 4;
            this.checkBox_RemoteComplete.Text = "원격 완료";
            this.checkBox_RemoteComplete.UseVisualStyleBackColor = true;
            this.checkBox_RemoteComplete.CheckedChanged += new System.EventHandler(this.checkBox_RemoteComplete_CheckedChanged);
            // 
            // button_NPCDel
            // 
            this.button_NPCDel.Location = new System.Drawing.Point(8, 57);
            this.button_NPCDel.Name = "button_NPCDel";
            this.button_NPCDel.Size = new System.Drawing.Size(130, 23);
            this.button_NPCDel.TabIndex = 1;
            this.button_NPCDel.Text = "NPC삭제";
            this.button_NPCDel.UseVisualStyleBackColor = true;
            this.button_NPCDel.Click += new System.EventHandler(this.button_NPCDel_Click);
            // 
            // button_NPCAdd
            // 
            this.button_NPCAdd.Location = new System.Drawing.Point(8, 28);
            this.button_NPCAdd.Name = "button_NPCAdd";
            this.button_NPCAdd.Size = new System.Drawing.Size(130, 23);
            this.button_NPCAdd.TabIndex = 0;
            this.button_NPCAdd.Text = "NPC추가";
            this.button_NPCAdd.UseVisualStyleBackColor = true;
            this.button_NPCAdd.Click += new System.EventHandler(this.button_NPCAdd_Click);
            // 
            // Quest_Unit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Unit);
            this.Name = "Quest_Unit";
            this.Size = new System.Drawing.Size(1128, 194);
            this.SizeChanged += new System.EventHandler(this.Quest_Unit_SizeChanged);
            this.groupBox_Unit.ResumeLayout(false);
            this.groupBox_Unit.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.GroupBox groupBox_Unit;
        private System.Windows.Forms.Button button_NPCDel;
        private System.Windows.Forms.Button button_NPCAdd;
        private System.Windows.Forms.CheckBox checkBox_RemoteComplete;
    }
}
