namespace DNQuest
{
    partial class Quest_RemoteComplete
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
            this.textBox_ExecuteNo = new System.Windows.Forms.TextBox();
            this.label_ExeNo = new System.Windows.Forms.Label();
            this.groupBox_NPC.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_NPC
            // 
            this.groupBox_NPC.Controls.Add(this.textBox_ExecuteNo);
            this.groupBox_NPC.Controls.Add(this.label_ExeNo);
            this.groupBox_NPC.Location = new System.Drawing.Point(13, 5);
            this.groupBox_NPC.Name = "groupBox_NPC";
            this.groupBox_NPC.Size = new System.Drawing.Size(958, 55);
            this.groupBox_NPC.TabIndex = 5;
            this.groupBox_NPC.TabStop = false;
            this.groupBox_NPC.Text = "원격완료";
            // 
            // textBox_ExecuteNo
            // 
            this.textBox_ExecuteNo.Location = new System.Drawing.Point(73, 23);
            this.textBox_ExecuteNo.Name = "textBox_ExecuteNo";
            this.textBox_ExecuteNo.Size = new System.Drawing.Size(363, 21);
            this.textBox_ExecuteNo.TabIndex = 3;
            // 
            // label_ExeNo
            // 
            this.label_ExeNo.AutoSize = true;
            this.label_ExeNo.Location = new System.Drawing.Point(6, 26);
            this.label_ExeNo.Name = "label_ExeNo";
            this.label_ExeNo.Size = new System.Drawing.Size(53, 12);
            this.label_ExeNo.TabIndex = 2;
            this.label_ExeNo.Text = "실행번호";
            // 
            // Quest_RemoteComplete
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_NPC);
            this.Name = "Quest_RemoteComplete";
            this.Size = new System.Drawing.Size(982, 75);
            this.groupBox_NPC.ResumeLayout(false);
            this.groupBox_NPC.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.GroupBox groupBox_NPC;
        public System.Windows.Forms.TextBox textBox_ExecuteNo;
        private System.Windows.Forms.Label label_ExeNo;

    }
}
