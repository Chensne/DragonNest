namespace DNNpc
{
    partial class Quest_Step
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
            this.textBox_StepNo = new System.Windows.Forms.TextBox();
            this.groupBox_Script = new System.Windows.Forms.GroupBox();
            this.textBox_Script = new GvS.Controls.HtmlTextbox();
            this.textBox_Image = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button_SelectAdd = new System.Windows.Forms.Button();
            this.button_SelectDel = new System.Windows.Forms.Button();
            this.groupBox_Step = new System.Windows.Forms.GroupBox();
            this.button_QuestAdd = new System.Windows.Forms.Button();
            this.txtItemIndex = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox_Script.SuspendLayout();
            this.groupBox_Step.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBox_StepNo
            // 
            this.textBox_StepNo.Location = new System.Drawing.Point(7, 40);
            this.textBox_StepNo.Name = "textBox_StepNo";
            this.textBox_StepNo.Size = new System.Drawing.Size(90, 21);
            this.textBox_StepNo.TabIndex = 0;
            // 
            // groupBox_Script
            // 
            this.groupBox_Script.Controls.Add(this.txtItemIndex);
            this.groupBox_Script.Controls.Add(this.label2);
            this.groupBox_Script.Controls.Add(this.textBox_Script);
            this.groupBox_Script.Controls.Add(this.textBox_Image);
            this.groupBox_Script.Controls.Add(this.label1);
            this.groupBox_Script.Location = new System.Drawing.Point(113, 20);
            this.groupBox_Script.Name = "groupBox_Script";
            this.groupBox_Script.Size = new System.Drawing.Size(541, 138);
            this.groupBox_Script.TabIndex = 2;
            this.groupBox_Script.TabStop = false;
            this.groupBox_Script.Text = "대사";
            // 
            // textBox_Script
            // 
            this.textBox_Script.Fonts = new string[] {
        "Corbel",
        "Corbel, Verdana, Arial, Helvetica, sans-serif",
        "Georgia, Times New Roman, Times, serif",
        "Consolas, Courier New, Courier, monospace"};
            this.textBox_Script.IllegalPatterns = new string[] {
        "<script.*?>",
        "<\\w+\\s+.*?(j|java|vb|ecma)script:.*?>",
        "<\\w+(\\s+|\\s+.*?\\s+)on\\w+\\s*=.+?>",
        "</?input.*?>"};
            this.textBox_Script.Location = new System.Drawing.Point(6, 20);
            this.textBox_Script.Name = "textBox_Script";
            this.textBox_Script.Padding = new System.Windows.Forms.Padding(1);
            this.textBox_Script.ShowHtmlSource = false;
            this.textBox_Script.Size = new System.Drawing.Size(529, 76);
            this.textBox_Script.TabIndex = 5;
            // 
            // textBox_Image
            // 
            this.textBox_Image.Location = new System.Drawing.Point(84, 107);
            this.textBox_Image.Name = "textBox_Image";
            this.textBox_Image.Size = new System.Drawing.Size(216, 21);
            this.textBox_Image.TabIndex = 4;
            this.textBox_Image.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_Image_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 112);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "이미지파일명";
            // 
            // button_SelectAdd
            // 
            this.button_SelectAdd.Location = new System.Drawing.Point(7, 67);
            this.button_SelectAdd.Name = "button_SelectAdd";
            this.button_SelectAdd.Size = new System.Drawing.Size(90, 23);
            this.button_SelectAdd.TabIndex = 3;
            this.button_SelectAdd.Text = "선택지 추가";
            this.button_SelectAdd.UseVisualStyleBackColor = true;
            this.button_SelectAdd.Click += new System.EventHandler(this.button_SelectAdd_Click);
            // 
            // button_SelectDel
            // 
            this.button_SelectDel.Location = new System.Drawing.Point(7, 93);
            this.button_SelectDel.Name = "button_SelectDel";
            this.button_SelectDel.Size = new System.Drawing.Size(90, 23);
            this.button_SelectDel.TabIndex = 4;
            this.button_SelectDel.Text = "선택지 삭제";
            this.button_SelectDel.UseVisualStyleBackColor = true;
            this.button_SelectDel.Click += new System.EventHandler(this.button_SelectDel_Click);
            // 
            // groupBox_Step
            // 
            this.groupBox_Step.Controls.Add(this.button_QuestAdd);
            this.groupBox_Step.Controls.Add(this.groupBox_Script);
            this.groupBox_Step.Controls.Add(this.button_SelectDel);
            this.groupBox_Step.Controls.Add(this.textBox_StepNo);
            this.groupBox_Step.Controls.Add(this.button_SelectAdd);
            this.groupBox_Step.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Step.Name = "groupBox_Step";
            this.groupBox_Step.Size = new System.Drawing.Size(657, 164);
            this.groupBox_Step.TabIndex = 5;
            this.groupBox_Step.TabStop = false;
            // 
            // button_QuestAdd
            // 
            this.button_QuestAdd.Location = new System.Drawing.Point(7, 119);
            this.button_QuestAdd.Name = "button_QuestAdd";
            this.button_QuestAdd.Size = new System.Drawing.Size(90, 37);
            this.button_QuestAdd.TabIndex = 5;
            this.button_QuestAdd.Text = "퀘스트 연결 추가";
            this.button_QuestAdd.UseVisualStyleBackColor = true;
            this.button_QuestAdd.Click += new System.EventHandler(this.button_QuestAdd_Click);
            // 
            // txtItemIndex
            // 
            this.txtItemIndex.Location = new System.Drawing.Point(385, 109);
            this.txtItemIndex.Name = "txtItemIndex";
            this.txtItemIndex.Size = new System.Drawing.Size(150, 21);
            this.txtItemIndex.TabIndex = 7;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(306, 114);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 12);
            this.label2.TabIndex = 6;
            this.label2.Text = "아이템인덱스";
            // 
            // Quest_Step
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Step);
            this.Name = "Quest_Step";
            this.Size = new System.Drawing.Size(660, 170);
            this.groupBox_Script.ResumeLayout(false);
            this.groupBox_Script.PerformLayout();
            this.groupBox_Step.ResumeLayout(false);
            this.groupBox_Step.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_StepNo;
        private System.Windows.Forms.GroupBox groupBox_Script;
        private System.Windows.Forms.Button button_SelectAdd;
        private System.Windows.Forms.Button button_SelectDel;
        private System.Windows.Forms.GroupBox groupBox_Step;
        public System.Windows.Forms.TextBox textBox_Image;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_QuestAdd;
        public GvS.Controls.HtmlTextbox textBox_Script;
        public System.Windows.Forms.TextBox txtItemIndex;
        private System.Windows.Forms.Label label2;
    }
}
