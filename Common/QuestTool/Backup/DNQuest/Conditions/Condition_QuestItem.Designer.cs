namespace DNQuest.Conditions
{
    partial class Condition_QuestItem
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
            this.textBox_Value = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_Oper = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_Item = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 10);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 27;
            this.label_No.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(520, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(45, 12);
            this.label3.TabIndex = 26;
            this.label3.Text = "이면 참";
            // 
            // textBox_Value
            // 
            this.textBox_Value.Location = new System.Drawing.Point(473, 3);
            this.textBox_Value.Name = "textBox_Value";
            this.textBox_Value.Size = new System.Drawing.Size(41, 21);
            this.textBox_Value.TabIndex = 25;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(458, 10);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(9, 12);
            this.label4.TabIndex = 24;
            this.label4.Text = ",";
            // 
            // textBox_Oper
            // 
            this.textBox_Oper.Location = new System.Drawing.Point(415, 3);
            this.textBox_Oper.Name = "textBox_Oper";
            this.textBox_Oper.Size = new System.Drawing.Size(41, 21);
            this.textBox_Oper.TabIndex = 23;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(252, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(157, 12);
            this.label2.TabIndex = 22;
            this.label2.Text = "의 보유 수(<,>,=,>=,<=,<>)";
            // 
            // textBox_Item
            // 
            this.textBox_Item.Location = new System.Drawing.Point(168, 3);
            this.textBox_Item.Name = "textBox_Item";
            this.textBox_Item.Size = new System.Drawing.Size(78, 21);
            this.textBox_Item.TabIndex = 21;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(145, 12);
            this.label1.TabIndex = 20;
            this.label1.Text = "퀘스트 인벤토리의 아이템";
            // 
            // Condition_QuestItem
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_Value);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_Oper);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_Item);
            this.Controls.Add(this.label1);
            this.Name = "Condition_QuestItem";
            this.Size = new System.Drawing.Size(573, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_Value;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_Oper;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_Item;
        private System.Windows.Forms.Label label1;
    }
}
