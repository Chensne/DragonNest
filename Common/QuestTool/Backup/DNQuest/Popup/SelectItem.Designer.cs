namespace DNQuest
{
    partial class SelectItem
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
            this.txtSearch = new System.Windows.Forms.TextBox();
            this.button_Cancel = new System.Windows.Forms.Button();
            this.listBox_Item = new System.Windows.Forms.ListBox();
            this.button_Select = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnSearch
            // 
            this.btnSearch.Location = new System.Drawing.Point(372, 0);
            this.btnSearch.Name = "btnSearch";
            this.btnSearch.Size = new System.Drawing.Size(75, 23);
            this.btnSearch.TabIndex = 9;
            this.btnSearch.Text = "검색";
            this.btnSearch.UseVisualStyleBackColor = true;
            this.btnSearch.Enter += new System.EventHandler(this.btnSearch_Enter);
            // 
            // txtSearch
            // 
            this.txtSearch.Location = new System.Drawing.Point(6, 2);
            this.txtSearch.Name = "txtSearch";
            this.txtSearch.Size = new System.Drawing.Size(360, 21);
            this.txtSearch.TabIndex = 8;
            // 
            // button_Cancel
            // 
            this.button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button_Cancel.Location = new System.Drawing.Point(371, 279);
            this.button_Cancel.Name = "button_Cancel";
            this.button_Cancel.Size = new System.Drawing.Size(75, 23);
            this.button_Cancel.TabIndex = 7;
            this.button_Cancel.Text = "닫기";
            this.button_Cancel.UseVisualStyleBackColor = true;
            this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
            // 
            // listBox_Item
            // 
            this.listBox_Item.FormattingEnabled = true;
            this.listBox_Item.ItemHeight = 12;
            this.listBox_Item.Location = new System.Drawing.Point(6, 29);
            this.listBox_Item.Name = "listBox_Item";
            this.listBox_Item.Size = new System.Drawing.Size(439, 244);
            this.listBox_Item.TabIndex = 5;
            this.listBox_Item.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listBox_Item_MouseDoubleClick);
            // 
            // button_Select
            // 
            this.button_Select.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button_Select.Location = new System.Drawing.Point(290, 279);
            this.button_Select.Name = "button_Select";
            this.button_Select.Size = new System.Drawing.Size(75, 23);
            this.button_Select.TabIndex = 6;
            this.button_Select.Text = "선택";
            this.button_Select.UseVisualStyleBackColor = true;
            this.button_Select.Click += new System.EventHandler(this.button_Select_Click);
            // 
            // SelectItem
            // 
            this.AcceptButton = this.button_Select;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button_Cancel;
            this.ClientSize = new System.Drawing.Size(450, 305);
            this.Controls.Add(this.btnSearch);
            this.Controls.Add(this.txtSearch);
            this.Controls.Add(this.button_Cancel);
            this.Controls.Add(this.button_Select);
            this.Controls.Add(this.listBox_Item);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "SelectItem";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "SelectItem";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnSearch;
        private System.Windows.Forms.TextBox txtSearch;
        private System.Windows.Forms.Button button_Cancel;
        private System.Windows.Forms.ListBox listBox_Item;
        private System.Windows.Forms.Button button_Select;
    }
}