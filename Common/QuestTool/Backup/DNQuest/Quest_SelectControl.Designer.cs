namespace DNQuest
{
    partial class Quest_SelectControl
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
            this.label_Target = new System.Windows.Forms.Label();
            this.label_ExeNo = new System.Windows.Forms.Label();
            this.textBox_ExeNo = new System.Windows.Forms.TextBox();
            this.groupBox_Select = new System.Windows.Forms.GroupBox();
            this.cmbTargetIndex = new System.Windows.Forms.ComboBox();
            this.textBox_Select = new GvS.Controls.HtmlTextbox();
            this.groupBox_Select.SuspendLayout();
            this.SuspendLayout();
            // 
            // label_Target
            // 
            this.label_Target.AutoSize = true;
            this.label_Target.Location = new System.Drawing.Point(325, 17);
            this.label_Target.Name = "label_Target";
            this.label_Target.Size = new System.Drawing.Size(69, 12);
            this.label_Target.TabIndex = 2;
            this.label_Target.Text = "타겟인덱스:";
            // 
            // label_ExeNo
            // 
            this.label_ExeNo.AutoSize = true;
            this.label_ExeNo.Location = new System.Drawing.Point(337, 43);
            this.label_ExeNo.Name = "label_ExeNo";
            this.label_ExeNo.Size = new System.Drawing.Size(57, 12);
            this.label_ExeNo.TabIndex = 4;
            this.label_ExeNo.Text = "실행번호:";
            // 
            // textBox_ExeNo
            // 
            this.textBox_ExeNo.Location = new System.Drawing.Point(406, 40);
            this.textBox_ExeNo.Name = "textBox_ExeNo";
            this.textBox_ExeNo.Size = new System.Drawing.Size(185, 21);
            this.textBox_ExeNo.TabIndex = 5;
            this.textBox_ExeNo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_ExeNo_KeyDown);
            // 
            // groupBox_Select
            // 
            this.groupBox_Select.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox_Select.Controls.Add(this.cmbTargetIndex);
            this.groupBox_Select.Controls.Add(this.textBox_Select);
            this.groupBox_Select.Controls.Add(this.textBox_ExeNo);
            this.groupBox_Select.Controls.Add(this.label_ExeNo);
            this.groupBox_Select.Controls.Add(this.label_Target);
            this.groupBox_Select.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Select.Name = "groupBox_Select";
            this.groupBox_Select.Size = new System.Drawing.Size(605, 84);
            this.groupBox_Select.TabIndex = 6;
            this.groupBox_Select.TabStop = false;
            this.groupBox_Select.Text = "선택지";
            // 
            // cmbTargetIndex
            // 
            this.cmbTargetIndex.FormattingEnabled = true;
            this.cmbTargetIndex.Items.AddRange(new object[] {
            "acceptted",
            "!next",
            "class_check"});
            this.cmbTargetIndex.Location = new System.Drawing.Point(407, 14);
            this.cmbTargetIndex.Name = "cmbTargetIndex";
            this.cmbTargetIndex.Size = new System.Drawing.Size(184, 20);
            this.cmbTargetIndex.TabIndex = 8;
            this.cmbTargetIndex.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cmbTargetIndex_KeyDown);
            // 
            // textBox_Select
            // 
            this.textBox_Select.AutoScroll = true;
            this.textBox_Select.AutoScrollMinSize = new System.Drawing.Size(100, 60);
            this.textBox_Select.Cursor = System.Windows.Forms.Cursors.WaitCursor;
            this.textBox_Select.Fonts = new string[] {
        "Corbel",
        "Corbel, Verdana, Arial, Helvetica, sans-serif",
        "Georgia, Times New Roman, Times, serif",
        "Consolas, Courier New, Courier, monospace"};
            this.textBox_Select.IllegalPatterns = new string[] {
        "<script.*?>",
        "<\\w+\\s+.*?(j|java|vb|ecma)script:.*?>",
        "<\\w+(\\s+|\\s+.*?\\s+)on\\w+\\s*=.+?>",
        "</?input.*?>"};
            this.textBox_Select.Location = new System.Drawing.Point(6, 14);
            this.textBox_Select.Name = "textBox_Select";
            this.textBox_Select.Padding = new System.Windows.Forms.Padding(1);
            this.textBox_Select.ShowHtmlSource = false;
            this.textBox_Select.Size = new System.Drawing.Size(313, 64);
            this.textBox_Select.TabIndex = 7;
            this.textBox_Select.UseWaitCursor = true;
            // 
            // Quest_SelectControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Select);
            this.Name = "Quest_SelectControl";
            this.Size = new System.Drawing.Size(611, 90);
            this.groupBox_Select.ResumeLayout(false);
            this.groupBox_Select.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label_Target;
        private System.Windows.Forms.Label label_ExeNo;
        private System.Windows.Forms.TextBox textBox_ExeNo;
        private System.Windows.Forms.GroupBox groupBox_Select;
        public GvS.Controls.HtmlTextbox textBox_Select;
        private System.Windows.Forms.ComboBox cmbTargetIndex;
    }
}
