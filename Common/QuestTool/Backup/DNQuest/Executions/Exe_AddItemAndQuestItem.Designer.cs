namespace DNQuest.Executions
{
    partial class Exe_AddItemAndQuestItem
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
            this.txt_ItemCount = new System.Windows.Forms.TextBox();
            this.txt_Item = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.txt_QuestItemCount = new System.Windows.Forms.TextBox();
            this.txt_QuestItem = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(4, 8);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 40;
            this.label_No.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(405, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(21, 12);
            this.label3.TabIndex = 39;
            this.label3.Text = "개,";
            // 
            // txt_ItemCount
            // 
            this.txt_ItemCount.Location = new System.Drawing.Point(323, 3);
            this.txt_ItemCount.Name = "txt_ItemCount";
            this.txt_ItemCount.Size = new System.Drawing.Size(80, 21);
            this.txt_ItemCount.TabIndex = 38;
            // 
            // txt_Item
            // 
            this.txt_Item.Location = new System.Drawing.Point(93, 3);
            this.txt_Item.Name = "txt_Item";
            this.txt_Item.Size = new System.Drawing.Size(225, 21);
            this.txt_Item.TabIndex = 36;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(21, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 12);
            this.label1.TabIndex = 35;
            this.label1.Text = "일반아이템:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(797, 8);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(45, 12);
            this.label5.TabIndex = 57;
            this.label5.Text = "개 추가";
            // 
            // txt_QuestItemCount
            // 
            this.txt_QuestItemCount.Location = new System.Drawing.Point(728, 3);
            this.txt_QuestItemCount.Name = "txt_QuestItemCount";
            this.txt_QuestItemCount.Size = new System.Drawing.Size(69, 21);
            this.txt_QuestItemCount.TabIndex = 56;
            // 
            // txt_QuestItem
            // 
            this.txt_QuestItem.Location = new System.Drawing.Point(513, 3);
            this.txt_QuestItem.Name = "txt_QuestItem";
            this.txt_QuestItem.Size = new System.Drawing.Size(211, 21);
            this.txt_QuestItem.TabIndex = 54;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(430, 8);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(81, 12);
            this.label7.TabIndex = 53;
            this.label7.Text = "퀘스트아이템:";
            // 
            // Exe_AddItemAndQuestItem
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txt_QuestItemCount);
            this.Controls.Add(this.txt_QuestItem);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txt_ItemCount);
            this.Controls.Add(this.txt_Item);
            this.Controls.Add(this.label1);
            this.Name = "Exe_AddItemAndQuestItem";
            this.Size = new System.Drawing.Size(849, 29);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label_No;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox txt_ItemCount;
        public System.Windows.Forms.TextBox txt_Item;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.TextBox txt_QuestItemCount;
        public System.Windows.Forms.TextBox txt_QuestItem;
        private System.Windows.Forms.Label label7;
    }
}
