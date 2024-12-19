namespace DNQuest.Conditions
{
    partial class Condition_Class
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
            this.textBox_Oper = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_Value = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // textBox_Oper
            // 
            this.textBox_Oper.Location = new System.Drawing.Point(130, 6);
            this.textBox_Oper.Name = "textBox_Oper";
            this.textBox_Oper.Size = new System.Drawing.Size(41, 21);
            this.textBox_Oper.TabIndex = 13;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(105, 12);
            this.label1.TabIndex = 12;
            this.label1.Text = "유저 클래스(=,<>)";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(177, 10);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(85, 12);
            this.label4.TabIndex = 14;
            this.label4.Text = ", 클래스아이디";
            // 
            // textBox_Value
            // 
            this.textBox_Value.Location = new System.Drawing.Point(268, 6);
            this.textBox_Value.Name = "textBox_Value";
            this.textBox_Value.Size = new System.Drawing.Size(41, 21);
            this.textBox_Value.TabIndex = 15;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(315, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(45, 12);
            this.label3.TabIndex = 16;
            this.label3.Text = "이면 참";
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 17;
            this.label_No.Text = "0";
            // 
            // Condition_Class
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_Value);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_Oper);
            this.Controls.Add(this.label1);
            this.Name = "Condition_Class";
            this.Size = new System.Drawing.Size(388, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_Oper;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_Value;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.Label label_No;
    }
}
