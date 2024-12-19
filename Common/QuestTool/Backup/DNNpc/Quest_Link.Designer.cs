namespace DNNpc
{
    partial class Quest_Link
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
            this.groupBox_Select = new System.Windows.Forms.GroupBox();
            this.textBox_Select = new GvS.Controls.HtmlTextbox();
            this.label_Select = new System.Windows.Forms.Label();
            this.label_Target = new System.Windows.Forms.Label();
            this.textBox_XML = new System.Windows.Forms.TextBox();
            this.groupBox_Select.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Select
            // 
            this.groupBox_Select.Controls.Add(this.textBox_Select);
            this.groupBox_Select.Controls.Add(this.label_Select);
            this.groupBox_Select.Controls.Add(this.label_Target);
            this.groupBox_Select.Controls.Add(this.textBox_XML);
            this.groupBox_Select.Location = new System.Drawing.Point(5, 3);
            this.groupBox_Select.Name = "groupBox_Select";
            this.groupBox_Select.Size = new System.Drawing.Size(541, 65);
            this.groupBox_Select.TabIndex = 7;
            this.groupBox_Select.TabStop = false;
            this.groupBox_Select.Text = "선택지";
            // 
            // textBox_Select
            // 
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
            this.textBox_Select.Location = new System.Drawing.Point(9, 32);
            this.textBox_Select.Name = "textBox_Select";
            this.textBox_Select.Padding = new System.Windows.Forms.Padding(1);
            this.textBox_Select.ShowHtmlSource = false;
            this.textBox_Select.Size = new System.Drawing.Size(288, 21);
            this.textBox_Select.TabIndex = 7;
            // 
            // label_Select
            // 
            this.label_Select.AutoSize = true;
            this.label_Select.Location = new System.Drawing.Point(7, 18);
            this.label_Select.Name = "label_Select";
            this.label_Select.Size = new System.Drawing.Size(41, 12);
            this.label_Select.TabIndex = 0;
            this.label_Select.Text = "선택지";
            // 
            // label_Target
            // 
            this.label_Target.AutoSize = true;
            this.label_Target.Location = new System.Drawing.Point(301, 18);
            this.label_Target.Name = "label_Target";
            this.label_Target.Size = new System.Drawing.Size(99, 12);
            this.label_Target.TabIndex = 2;
            this.label_Target.Text = "퀘스트 XML 파일";
            // 
            // textBox_XML
            // 
            this.textBox_XML.Location = new System.Drawing.Point(303, 32);
            this.textBox_XML.Name = "textBox_XML";
            this.textBox_XML.Size = new System.Drawing.Size(232, 21);
            this.textBox_XML.TabIndex = 3;
            this.textBox_XML.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_XML_KeyDown);
            // 
            // Quest_Link
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Select);
            this.Name = "Quest_Link";
            this.Size = new System.Drawing.Size(550, 70);
            this.groupBox_Select.ResumeLayout(false);
            this.groupBox_Select.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_Select;
        private System.Windows.Forms.Label label_Select;
        private System.Windows.Forms.Label label_Target;
        private System.Windows.Forms.TextBox textBox_XML;
        public GvS.Controls.HtmlTextbox textBox_Select;
    }
}
