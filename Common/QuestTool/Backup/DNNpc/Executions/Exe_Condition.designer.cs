namespace DNNpc.Executions
{
    partial class Exe_Condition
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
            this.textBox_false = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_true = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_Condition = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // textBox_false
            // 
            this.textBox_false.Location = new System.Drawing.Point(508, 4);
            this.textBox_false.Name = "textBox_false";
            this.textBox_false.Size = new System.Drawing.Size(116, 21);
            this.textBox_false.TabIndex = 27;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(413, 8);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 12);
            this.label2.TabIndex = 26;
            this.label2.Text = ", 실행 거짓이면";
            // 
            // textBox_true
            // 
            this.textBox_true.Location = new System.Drawing.Point(301, 4);
            this.textBox_true.Name = "textBox_true";
            this.textBox_true.Size = new System.Drawing.Size(111, 21);
            this.textBox_true.TabIndex = 25;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(251, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 12);
            this.label3.TabIndex = 24;
            this.label3.Text = ", 참이면";
            // 
            // textBox_Condition
            // 
            this.textBox_Condition.Location = new System.Drawing.Point(93, 4);
            this.textBox_Condition.Name = "textBox_Condition";
            this.textBox_Condition.Size = new System.Drawing.Size(157, 21);
            this.textBox_Condition.TabIndex = 23;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 12);
            this.label1.TabIndex = 22;
            this.label1.Text = "다음 조건이";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(630, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 12);
            this.label4.TabIndex = 28;
            this.label4.Text = ", 실행";
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 58;
            this.label_No.Text = "0";
            // 
            // Exe_Condition
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_false);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_true);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_Condition);
            this.Controls.Add(this.label1);
            this.Name = "Exe_Condition";
            this.Size = new System.Drawing.Size(738, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_false;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_true;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_Condition;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.Label label_No;
    }
}
