namespace DNQuest.Executions
{
    partial class Exe_ProbChoice
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
            this.textBox_Rand = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_Seed = new System.Windows.Forms.TextBox();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // textBox_Rand
            // 
            this.textBox_Rand.Location = new System.Drawing.Point(88, 5);
            this.textBox_Rand.Name = "textBox_Rand";
            this.textBox_Rand.Size = new System.Drawing.Size(99, 21);
            this.textBox_Rand.TabIndex = 23;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 12);
            this.label1.TabIndex = 22;
            this.label1.Text = "다음 숫자들";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(190, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(187, 12);
            this.label2.TabIndex = 24;
            this.label2.Text = "의 구간을 랜덤으로 1 택해서 실행";
            // 
            // textBox_Seed
            // 
            this.textBox_Seed.Location = new System.Drawing.Point(378, 4);
            this.textBox_Seed.Name = "textBox_Seed";
            this.textBox_Seed.Size = new System.Drawing.Size(99, 21);
            this.textBox_Seed.TabIndex = 25;
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 41;
            this.label_No.Text = "0";
            // 
            // Exe_ProbChoice
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.textBox_Seed);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_Rand);
            this.Controls.Add(this.label1);
            this.Name = "Exe_ProbChoice";
            this.Size = new System.Drawing.Size(487, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_Rand;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_Seed;
        public System.Windows.Forms.Label label_No;
    }
}
