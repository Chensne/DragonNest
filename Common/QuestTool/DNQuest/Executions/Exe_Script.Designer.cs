namespace DNQuest.Executions
{
    partial class Exe_Script
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
            this.textBox_NPC = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_Index = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(31, 12);
            this.label1.TabIndex = 17;
            this.label1.Text = "NPC";
            // 
            // textBox_NPC
            // 
            this.textBox_NPC.Location = new System.Drawing.Point(55, 5);
            this.textBox_NPC.Name = "textBox_NPC";
            this.textBox_NPC.Size = new System.Drawing.Size(99, 21);
            this.textBox_NPC.TabIndex = 18;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(154, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 12);
            this.label3.TabIndex = 19;
            this.label3.Text = ", 인덱스";
            // 
            // textBox_Index
            // 
            this.textBox_Index.AccessibleDescription = "";
            this.textBox_Index.Location = new System.Drawing.Point(203, 5);
            this.textBox_Index.Name = "textBox_Index";
            this.textBox_Index.Size = new System.Drawing.Size(47, 21);
            this.textBox_Index.TabIndex = 20;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(252, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(129, 12);
            this.label2.TabIndex = 21;
            this.label2.Text = ", 의 대사창을 출력한다";
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 45;
            this.label_No.Text = "0";
            // 
            // Exe_Script
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_Index);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_NPC);
            this.Controls.Add(this.label1);
            this.Name = "Exe_Script";
            this.Size = new System.Drawing.Size(389, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox textBox_NPC;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_Index;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.Label label_No;
    }
}
