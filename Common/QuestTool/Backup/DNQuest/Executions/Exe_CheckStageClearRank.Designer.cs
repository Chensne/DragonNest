namespace DNQuest.Executions
{
    partial class Exe_CheckStageClearRank
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
            this.txtExecute = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtInequality = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtRank = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtStageIndex = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(4, 11);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 44;
            this.label_No.Text = "0";
            // 
            // txtExecute
            // 
            this.txtExecute.Location = new System.Drawing.Point(667, 5);
            this.txtExecute.Name = "txtExecute";
            this.txtExecute.Size = new System.Drawing.Size(73, 21);
            this.txtExecute.TabIndex = 43;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(523, 11);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(143, 12);
            this.label4.TabIndex = 42;
            this.label4.Text = "(<,>,<=,>=)경우   실행문";
            // 
            // txtInequality
            // 
            this.txtInequality.Location = new System.Drawing.Point(493, 5);
            this.txtInequality.Name = "txtInequality";
            this.txtInequality.Size = new System.Drawing.Size(30, 21);
            this.txtInequality.TabIndex = 41;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(461, 11);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 40;
            this.label2.Text = "보다";
            // 
            // txtRank
            // 
            this.txtRank.Location = new System.Drawing.Point(431, 5);
            this.txtRank.Name = "txtRank";
            this.txtRank.Size = new System.Drawing.Size(28, 21);
            this.txtRank.TabIndex = 39;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(188, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(242, 12);
            this.label3.TabIndex = 38;
            this.label3.Text = "를 랭크(SSS:0/SS:1/S:2/A:3/B:4/C:5/D:6)";
            // 
            // txtStageIndex
            // 
            this.txtStageIndex.Location = new System.Drawing.Point(113, 5);
            this.txtStageIndex.Name = "txtStageIndex";
            this.txtStageIndex.Size = new System.Drawing.Size(75, 21);
            this.txtStageIndex.TabIndex = 37;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(93, 12);
            this.label1.TabIndex = 36;
            this.label1.Text = "스테이지 인덱스";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(742, 11);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(29, 12);
            this.label5.TabIndex = 45;
            this.label5.Text = "실행";
            // 
            // Exe_CheckStageClearRank
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.txtExecute);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txtInequality);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtRank);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtStageIndex);
            this.Controls.Add(this.label1);
            this.Name = "Exe_CheckStageClearRank";
            this.Size = new System.Drawing.Size(794, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        public System.Windows.Forms.TextBox txtExecute;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox txtInequality;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox txtRank;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox txtStageIndex;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label5;
    }
}
