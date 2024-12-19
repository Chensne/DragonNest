namespace QuestTool
{
    partial class SaveProgress
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
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.label1 = new System.Windows.Forms.Label();
            this.label_Filename = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label_Time = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label_Count = new System.Windows.Forms.Label();
            this.button_Cancel = new System.Windows.Forms.Button();
            this.listBox_Fail = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(12, 94);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(512, 23);
            this.progressBar1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "Filename :";
            // 
            // label_Filename
            // 
            this.label_Filename.AutoSize = true;
            this.label_Filename.Location = new System.Drawing.Point(81, 9);
            this.label_Filename.Name = "label_Filename";
            this.label_Filename.Size = new System.Drawing.Size(57, 12);
            this.label_Filename.TabIndex = 2;
            this.label_Filename.Text = "Unknown";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 37);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(66, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "Time       :";
            // 
            // label_Time
            // 
            this.label_Time.AutoSize = true;
            this.label_Time.Location = new System.Drawing.Point(81, 37);
            this.label_Time.Name = "label_Time";
            this.label_Time.Size = new System.Drawing.Size(49, 12);
            this.label_Time.TabIndex = 4;
            this.label_Time.Text = "00:00:00";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 65);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(66, 12);
            this.label3.TabIndex = 5;
            this.label3.Text = "Count      :";
            // 
            // label_Count
            // 
            this.label_Count.AutoSize = true;
            this.label_Count.Location = new System.Drawing.Point(81, 65);
            this.label_Count.Name = "label_Count";
            this.label_Count.Size = new System.Drawing.Size(11, 12);
            this.label_Count.TabIndex = 6;
            this.label_Count.Text = "0";
            // 
            // button_Cancel
            // 
            this.button_Cancel.Location = new System.Drawing.Point(449, 65);
            this.button_Cancel.Name = "button_Cancel";
            this.button_Cancel.Size = new System.Drawing.Size(75, 23);
            this.button_Cancel.TabIndex = 7;
            this.button_Cancel.Text = "stop";
            this.button_Cancel.UseVisualStyleBackColor = true;
            this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
            // 
            // listBox_Fail
            // 
            this.listBox_Fail.FormattingEnabled = true;
            this.listBox_Fail.ItemHeight = 12;
            this.listBox_Fail.Location = new System.Drawing.Point(11, 147);
            this.listBox_Fail.Name = "listBox_Fail";
            this.listBox_Fail.Size = new System.Drawing.Size(509, 124);
            this.listBox_Fail.TabIndex = 8;
            this.listBox_Fail.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listBox_Fail_MouseDoubleClick);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 132);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 12);
            this.label4.TabIndex = 9;
            this.label4.Text = "Fail List";
            // 
            // SaveProgress
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(534, 285);
            this.ControlBox = false;
            this.Controls.Add(this.label4);
            this.Controls.Add(this.listBox_Fail);
            this.Controls.Add(this.button_Cancel);
            this.Controls.Add(this.label_Count);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label_Time);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label_Filename);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.progressBar1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "SaveProgress";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "SaveProgress";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label_Filename;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label_Time;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label_Count;
        private System.Windows.Forms.Button button_Cancel;
        private System.Windows.Forms.ListBox listBox_Fail;
        private System.Windows.Forms.Label label4;
    }
}