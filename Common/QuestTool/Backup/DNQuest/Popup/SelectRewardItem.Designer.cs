namespace DNQuest
{
    partial class SelectRewardItem
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnSearch = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.button_Cancel = new System.Windows.Forms.Button();
            this.listBox_RewardItem = new System.Windows.Forms.ListBox();
            this.button_Select = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnSearch
            // 
            this.btnSearch.Location = new System.Drawing.Point(371, 2);
            this.btnSearch.Name = "btnSearch";
            this.btnSearch.Size = new System.Drawing.Size(75, 23);
            this.btnSearch.TabIndex = 14;
            this.btnSearch.Text = "검색";
            this.btnSearch.UseVisualStyleBackColor = true;
            this.btnSearch.Enter += new System.EventHandler(this.btnSearch_Enter);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(5, 4);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(360, 21);
            this.textBox1.TabIndex = 13;
            // 
            // button_Cancel
            // 
            this.button_Cancel.Location = new System.Drawing.Point(370, 281);
            this.button_Cancel.Name = "button_Cancel";
            this.button_Cancel.Size = new System.Drawing.Size(75, 23);
            this.button_Cancel.TabIndex = 12;
            this.button_Cancel.Text = "닫기";
            this.button_Cancel.UseVisualStyleBackColor = true;
            this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
            // 
            // listBox_RewardItem
            // 
            this.listBox_RewardItem.FormattingEnabled = true;
            this.listBox_RewardItem.ItemHeight = 12;
            this.listBox_RewardItem.Location = new System.Drawing.Point(5, 31);
            this.listBox_RewardItem.Name = "listBox_RewardItem";
            this.listBox_RewardItem.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.listBox_RewardItem.Size = new System.Drawing.Size(439, 244);
            this.listBox_RewardItem.TabIndex = 10;
            this.listBox_RewardItem.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listBox_RewardItem_MouseDoubleClick);
            // 
            // button_Select
            // 
            this.button_Select.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button_Select.Location = new System.Drawing.Point(289, 281);
            this.button_Select.Name = "button_Select";
            this.button_Select.Size = new System.Drawing.Size(75, 23);
            this.button_Select.TabIndex = 11;
            this.button_Select.Text = "선택";
            this.button_Select.UseVisualStyleBackColor = true;
            this.button_Select.Click += new System.EventHandler(this.button_Select_Click);
            // 
            // SelectRewardItem
            // 
            this.AcceptButton = this.button_Select;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(449, 307);
            this.Controls.Add(this.btnSearch);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button_Cancel);
            this.Controls.Add(this.button_Select);
            this.Controls.Add(this.listBox_RewardItem);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "SelectRewardItem";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "SelectRewardItem";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnSearch;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button_Cancel;
        private System.Windows.Forms.ListBox listBox_RewardItem;
        private System.Windows.Forms.Button button_Select;
    }
}