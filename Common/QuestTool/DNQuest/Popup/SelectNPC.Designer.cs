namespace DNQuest
{
    partial class SelectNPC
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
            this.listBox_NPC = new System.Windows.Forms.ListBox();
            this.button_Select = new System.Windows.Forms.Button();
            this.button_Cancel = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnSearch = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // listBox_NPC
            // 
            this.listBox_NPC.FormattingEnabled = true;
            this.listBox_NPC.ItemHeight = 12;
            this.listBox_NPC.Location = new System.Drawing.Point(1, 30);
            this.listBox_NPC.Name = "listBox_NPC";
            this.listBox_NPC.Size = new System.Drawing.Size(449, 280);
            this.listBox_NPC.TabIndex = 0;
            this.listBox_NPC.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listBox_NPC_MouseDoubleClick);
            // 
            // button_Select
            // 
            this.button_Select.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button_Select.Location = new System.Drawing.Point(294, 316);
            this.button_Select.Name = "button_Select";
            this.button_Select.Size = new System.Drawing.Size(75, 23);
            this.button_Select.TabIndex = 1;
            this.button_Select.Text = "선택";
            this.button_Select.UseVisualStyleBackColor = true;
            this.button_Select.Click += new System.EventHandler(this.button_Select_Click);
            // 
            // button_Cancel
            // 
            this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button_Cancel.Location = new System.Drawing.Point(375, 316);
            this.button_Cancel.Name = "button_Cancel";
            this.button_Cancel.Size = new System.Drawing.Size(75, 23);
            this.button_Cancel.TabIndex = 2;
            this.button_Cancel.Text = "닫기";
            this.button_Cancel.UseVisualStyleBackColor = true;
            this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(1, 4);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(368, 21);
            this.textBox1.TabIndex = 3;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // btnSearch
            // 
            this.btnSearch.Location = new System.Drawing.Point(375, 2);
            this.btnSearch.Name = "btnSearch";
            this.btnSearch.Size = new System.Drawing.Size(75, 23);
            this.btnSearch.TabIndex = 4;
            this.btnSearch.Text = "검색";
            this.btnSearch.UseVisualStyleBackColor = true;
            // 
            // SelectNPC
            // 
            this.AcceptButton = this.button_Select;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button_Cancel;
            this.ClientSize = new System.Drawing.Size(452, 343);
            this.Controls.Add(this.btnSearch);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button_Cancel);
            this.Controls.Add(this.button_Select);
            this.Controls.Add(this.listBox_NPC);
            this.Cursor = System.Windows.Forms.Cursors.Default;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "SelectNPC";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "SelectNPC";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox listBox_NPC;
        private System.Windows.Forms.Button button_Select;
        private System.Windows.Forms.Button button_Cancel;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnSearch;
    }
}