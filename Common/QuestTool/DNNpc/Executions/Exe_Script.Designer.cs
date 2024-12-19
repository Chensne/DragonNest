namespace DNNpc.Executions
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
            this.textBox_Script = new System.Windows.Forms.TextBox();
            this.label_No = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(133, 12);
            this.label1.TabIndex = 32;
            this.label1.Text = "출력할 대사창의 인덱스";
            // 
            // textBox_Script
            // 
            this.textBox_Script.Location = new System.Drawing.Point(153, 5);
            this.textBox_Script.Name = "textBox_Script";
            this.textBox_Script.Size = new System.Drawing.Size(209, 21);
            this.textBox_Script.TabIndex = 33;
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 63;
            this.label_No.Text = "0";
            // 
            // Exe_Script
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.textBox_Script);
            this.Controls.Add(this.label1);
            this.Name = "Exe_Script";
            this.Size = new System.Drawing.Size(369, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox textBox_Script;
        public System.Windows.Forms.Label label_No;
    }
}
