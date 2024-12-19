namespace DNQuest.Executions
{
    partial class Exe_QuestItemAdd
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
            this.textBox_Count = new System.Windows.Forms.TextBox();
            this.textBox_Item = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 52;
            this.label_No.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(510, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(161, 12);
            this.label3.TabIndex = 51;
            this.label3.Text = "개를 퀘스트 인벤토리에 추가";
            // 
            // textBox_Count
            // 
            this.textBox_Count.Location = new System.Drawing.Point(409, 5);
            this.textBox_Count.Name = "textBox_Count";
            this.textBox_Count.Size = new System.Drawing.Size(101, 21);
            this.textBox_Count.TabIndex = 50;
            // 
            // textBox_Item
            // 
            this.textBox_Item.Location = new System.Drawing.Point(101, 5);
            this.textBox_Item.Name = "textBox_Item";
            this.textBox_Item.Size = new System.Drawing.Size(287, 21);
            this.textBox_Item.TabIndex = 48;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(81, 12);
            this.label1.TabIndex = 47;
            this.label1.Text = "퀘스트 아이템";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(394, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(9, 12);
            this.label2.TabIndex = 49;
            this.label2.Text = ",";
            // 
            // Exe_QuestItemAdd
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_Count);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_Item);
            this.Controls.Add(this.label1);
            this.Name = "Exe_QuestItemAdd";
            this.Size = new System.Drawing.Size(703, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_Count;
        public System.Windows.Forms.TextBox textBox_Item;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}
