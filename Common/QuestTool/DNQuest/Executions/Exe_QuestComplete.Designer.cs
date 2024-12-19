namespace DNQuest.Executions
{
    partial class Exe_QuestComplete
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
            this.textBox_Quest = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label_No = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_QuestComplete = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.errPvdr)).BeginInit();
            this.SuspendLayout();
            // 
            // textBox_Quest
            // 
            this.textBox_Quest.Location = new System.Drawing.Point(129, 5);
            this.textBox_Quest.Name = "textBox_Quest";
            this.textBox_Quest.Size = new System.Drawing.Size(99, 21);
            this.textBox_Quest.TabIndex = 14;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(109, 12);
            this.label1.TabIndex = 13;
            this.label1.Text = "완료할 퀘스트 번호";
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 43;
            this.label_No.Text = "0";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(234, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 12);
            this.label2.TabIndex = 44;
            this.label2.Text = "완료시 실행";
            // 
            // textBox_QuestComplete
            // 
            this.textBox_QuestComplete.Location = new System.Drawing.Point(308, 5);
            this.textBox_QuestComplete.Name = "textBox_QuestComplete";
            this.textBox_QuestComplete.Size = new System.Drawing.Size(213, 21);
            this.textBox_QuestComplete.TabIndex = 45;
            // 
            // Exe_QuestComplete
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.textBox_QuestComplete);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.textBox_Quest);
            this.Controls.Add(this.label1);
            this.Name = "Exe_QuestComplete";
            this.Size = new System.Drawing.Size(524, 31);
            ((System.ComponentModel.ISupportInitialize)(this.errPvdr)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_Quest;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.Label label_No;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_QuestComplete;
    }
}
