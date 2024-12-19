namespace DNQuest.Executions
{
    partial class Exe_MonsterAdd
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
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_MonID = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_Party = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_OnExe = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_ExeComp = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_Count = new System.Windows.Forms.TextBox();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "몬스터ID";
            // 
            // textBox_MonID
            // 
            this.textBox_MonID.Location = new System.Drawing.Point(73, 4);
            this.textBox_MonID.Name = "textBox_MonID";
            this.textBox_MonID.Size = new System.Drawing.Size(100, 21);
            this.textBox_MonID.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(287, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(241, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "동료 파티원이 잡아주는 것 허용(1), 금지(0)";
            // 
            // textBox_Party
            // 
            this.textBox_Party.Location = new System.Drawing.Point(534, 4);
            this.textBox_Party.Name = "textBox_Party";
            this.textBox_Party.Size = new System.Drawing.Size(36, 21);
            this.textBox_Party.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(584, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(69, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "사냥시 실행";
            // 
            // textBox_OnExe
            // 
            this.textBox_OnExe.Location = new System.Drawing.Point(659, 5);
            this.textBox_OnExe.Name = "textBox_OnExe";
            this.textBox_OnExe.Size = new System.Drawing.Size(100, 21);
            this.textBox_OnExe.TabIndex = 5;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(774, 10);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(97, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "사냥 완료시 실행";
            // 
            // textBox_ExeComp
            // 
            this.textBox_ExeComp.Location = new System.Drawing.Point(877, 5);
            this.textBox_ExeComp.Name = "textBox_ExeComp";
            this.textBox_ExeComp.Size = new System.Drawing.Size(100, 21);
            this.textBox_ExeComp.TabIndex = 7;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(173, 10);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 12);
            this.label5.TabIndex = 8;
            this.label5.Text = ",사냥 마리";
            // 
            // textBox_Count
            // 
            this.textBox_Count.Location = new System.Drawing.Point(232, 4);
            this.textBox_Count.Name = "textBox_Count";
            this.textBox_Count.Size = new System.Drawing.Size(36, 21);
            this.textBox_Count.TabIndex = 9;
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 40;
            this.label_No.Text = "0";
            // 
            // Exe_MonsterAdd
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.textBox_Count);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textBox_ExeComp);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_OnExe);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_Party);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_MonID);
            this.Controls.Add(this.label1);
            this.Name = "Exe_MonsterAdd";
            this.Size = new System.Drawing.Size(1001, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox textBox_MonID;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_Party;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_OnExe;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_ExeComp;
        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.TextBox textBox_Count;
        public System.Windows.Forms.Label label_No;
    }
}
