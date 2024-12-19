namespace DNQuest.Executions
{
    partial class Exe_AddDailyQuest
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
            this.txt_ExecuteOnFail = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txt_ExecuteOnSuccess = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txt_QuestNo = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(5, 11);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 56;
            this.label_No.Text = "0";
            // 
            // txt_ExecuteOnFail
            // 
            this.txt_ExecuteOnFail.Location = new System.Drawing.Point(636, 5);
            this.txt_ExecuteOnFail.Name = "txt_ExecuteOnFail";
            this.txt_ExecuteOnFail.Size = new System.Drawing.Size(74, 21);
            this.txt_ExecuteOnFail.TabIndex = 55;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(399, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(235, 12);
            this.label4.TabIndex = 54;
            this.label4.Text = "실패 시 실행(없을 시 기본 알림 대사 출력)";
            // 
            // txt_ExecuteOnSuccess
            // 
            this.txt_ExecuteOnSuccess.Location = new System.Drawing.Point(319, 5);
            this.txt_ExecuteOnSuccess.Name = "txt_ExecuteOnSuccess";
            this.txt_ExecuteOnSuccess.Size = new System.Drawing.Size(73, 21);
            this.txt_ExecuteOnSuccess.TabIndex = 53;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(245, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(73, 12);
            this.label3.TabIndex = 52;
            this.label3.Text = "성공 시 실행";
            // 
            // txt_QuestNo
            // 
            this.txt_QuestNo.Location = new System.Drawing.Point(161, 5);
            this.txt_QuestNo.Name = "txt_QuestNo";
            this.txt_QuestNo.Size = new System.Drawing.Size(76, 21);
            this.txt_QuestNo.TabIndex = 49;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(21, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(137, 12);
            this.label1.TabIndex = 48;
            this.label1.Text = "셋팅할 일일 퀘스트 번호";
            // 
            // Exe_AddDailyQuest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.txt_ExecuteOnFail);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txt_ExecuteOnSuccess);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txt_QuestNo);
            this.Controls.Add(this.label1);
            this.Name = "Exe_AddDailyQuest";
            this.Size = new System.Drawing.Size(719, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        public System.Windows.Forms.TextBox txt_ExecuteOnFail;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox txt_ExecuteOnSuccess;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox txt_QuestNo;
        private System.Windows.Forms.Label label1;
    }
}
