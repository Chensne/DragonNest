namespace QuestTool
{
    partial class AboutForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label_Ver = new System.Windows.Forms.Label();
            this.button_Close = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 72);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(158, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "(c) 2008 Eyedentity Games";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(20, 19);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(133, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "DragonNest QuestTool";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(20, 84);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(106, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "all rights reserved";
            // 
            // label_Ver
            // 
            this.label_Ver.AutoSize = true;
            this.label_Ver.Location = new System.Drawing.Point(20, 43);
            this.label_Ver.Name = "label_Ver";
            this.label_Ver.Size = new System.Drawing.Size(0, 12);
            this.label_Ver.TabIndex = 3;
            // 
            // button_Close
            // 
            this.button_Close.Location = new System.Drawing.Point(68, 121);
            this.button_Close.Name = "button_Close";
            this.button_Close.Size = new System.Drawing.Size(100, 23);
            this.button_Close.TabIndex = 4;
            this.button_Close.Text = "확인";
            this.button_Close.UseVisualStyleBackColor = true;
            this.button_Close.Click += new System.EventHandler(this.button_Close_Click);
            // 
            // AboutForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(247, 156);
            this.ControlBox = false;
            this.Controls.Add(this.button_Close);
            this.Controls.Add(this.label_Ver);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "AboutForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "정보";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label_Ver;
        private System.Windows.Forms.Button button_Close;
    }
}