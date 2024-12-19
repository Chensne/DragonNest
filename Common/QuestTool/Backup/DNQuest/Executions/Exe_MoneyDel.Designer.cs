namespace DNQuest.Executions
{
    partial class Exe_MoneyDel
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
            this.label_No = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtLessExecute = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtDeleteMoney = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtMoreExecute = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(4, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 60;
            this.label_No.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(615, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 59;
            this.label3.Text = "실행";
            // 
            // txtLessExecute
            // 
            this.txtLessExecute.Location = new System.Drawing.Point(534, 5);
            this.txtLessExecute.Name = "txtLessExecute";
            this.txtLessExecute.Size = new System.Drawing.Size(75, 21);
            this.txtLessExecute.TabIndex = 58;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(167, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(61, 12);
            this.label2.TabIndex = 57;
            this.label2.Text = "을 차감 후";
            // 
            // txtDeleteMoney
            // 
            this.txtDeleteMoney.Location = new System.Drawing.Point(79, 6);
            this.txtDeleteMoney.Name = "txtDeleteMoney";
            this.txtDeleteMoney.Size = new System.Drawing.Size(82, 21);
            this.txtDeleteMoney.TabIndex = 56;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 55;
            this.label1.Text = "게임머니";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(315, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(213, 12);
            this.label4.TabIndex = 61;
            this.label4.Text = "실행,  설정된 게임머니 보다 작을 경우";
            // 
            // txtMoreExecute
            // 
            this.txtMoreExecute.Location = new System.Drawing.Point(234, 5);
            this.txtMoreExecute.Name = "txtMoreExecute";
            this.txtMoreExecute.Size = new System.Drawing.Size(75, 21);
            this.txtMoreExecute.TabIndex = 62;
            // 
            // Exe_MoneyDel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.txtMoreExecute);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtLessExecute);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtDeleteMoney);
            this.Controls.Add(this.label1);
            this.Name = "Exe_MoneyDel";
            this.Size = new System.Drawing.Size(775, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox txtLessExecute;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox txtDeleteMoney;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox txtMoreExecute;
    }
}
