namespace DNQuest
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
            this.textBox_PortraitRight = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_PortraitLeft = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.flowLayoutPanel4SelectControl = new System.Windows.Forms.FlowLayoutPanel();
            this.textBox_Script = new GvS.Controls.HtmlTextbox();
            this.txtItemIndex = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_Image = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button_SelectAdd = new System.Windows.Forms.Button();
            this.button_SelectDel = new System.Windows.Forms.Button();
            this.groupBox_Step = new System.Windows.Forms.GroupBox();
            this.comboBox_Step = new System.Windows.Forms.ComboBox();
            this.button_DelStep = new System.Windows.Forms.Button();
            this.comboBox_Function = new System.Windows.Forms.ComboBox();
            this.button_DelFunction = new System.Windows.Forms.Button();
            this.groupBox_Script.SuspendLayout();
            this.groupBox_Step.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBox_StepNo
            // 
            this.textBox_StepNo.Location = new System.Drawing.Point(7, 18);
            this.textBox_StepNo.Name = "textBox_StepNo";
            this.textBox_StepNo.ReadOnly = true;
            this.textBox_StepNo.Size = new System.Drawing.Size(90, 21);
            this.textBox_StepNo.TabIndex = 0;
            // 
            // groupBox_Script
            // 
            this.groupBox_Script.Controls.Add(this.textBox_PortraitRight);
            this.groupBox_Script.Controls.Add(this.label4);
            this.groupBox_Script.Controls.Add(this.textBox_PortraitLeft);
            this.groupBox_Script.Controls.Add(this.label3);
            this.groupBox_Script.Controls.Add(this.flowLayoutPanel4SelectControl);
            this.groupBox_Script.Controls.Add(this.textBox_Script);
            this.groupBox_Script.Controls.Add(this.txtItemIndex);
            this.groupBox_Script.Controls.Add(this.label2);
            this.groupBox_Script.Controls.Add(this.textBox_Image);
            this.groupBox_Script.Controls.Add(this.label1);
            this.groupBox_Script.Location = new System.Drawing.Point(103, 10);
            this.groupBox_Script.Name = "groupBox_Script";
            this.groupBox_Script.Size = new System.Drawing.Size(663, 160);
            this.groupBox_Script.TabIndex = 2;
            this.groupBox_Script.TabStop = false;
            this.groupBox_Script.Text = "대사";
            // 
            // textBox_PortraitRight
            // 
            this.textBox_PortraitRight.Location = new System.Drawing.Point(426, 130);
            this.textBox_PortraitRight.Name = "textBox_PortraitRight";
            this.textBox_PortraitRight.Size = new System.Drawing.Size(231, 21);
            this.textBox_PortraitRight.TabIndex = 16;
            this.textBox_PortraitRight.Click += new System.EventHandler(this.textBox_PortraitRight_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(339, 135);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(85, 12);
            this.label4.TabIndex = 15;
            this.label4.Text = "R 포트레이트 :";
            // 
            // textBox_PortraitLeft
            // 
            this.textBox_PortraitLeft.Location = new System.Drawing.Point(96, 130);
            this.textBox_PortraitLeft.Name = "textBox_PortraitLeft";
            this.textBox_PortraitLeft.Size = new System.Drawing.Size(223, 21);
            this.textBox_PortraitLeft.TabIndex = 14;
            this.textBox_PortraitLeft.Click += new System.EventHandler(this.textBox_PortraitLeft_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 135);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(84, 12);
            this.label3.TabIndex = 13;
            this.label3.Text = "L 포트레이트 :";
            // 
            // flowLayoutPanel4SelectControl
            // 
            this.flowLayoutPanel4SelectControl.AutoSize = true;
            this.flowLayoutPanel4SelectControl.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.flowLayoutPanel4SelectControl.Location = new System.Drawing.Point(0, 132);
            this.flowLayoutPanel4SelectControl.Name = "flowLayoutPanel4SelectControl";
            this.flowLayoutPanel4SelectControl.Size = new System.Drawing.Size(669, 1);
            this.flowLayoutPanel4SelectControl.TabIndex = 7;
            // 
            // textBox_Script
            // 
            this.textBox_Script.AutoScroll = true;
            this.textBox_Script.AutoScrollMinSize = new System.Drawing.Size(100, 60);
            this.textBox_Script.Cursor = System.Windows.Forms.Cursors.WaitCursor;
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
            this.textBox_Script.Location = new System.Drawing.Point(11, 15);
            this.textBox_Script.Name = "textBox_Script";
            this.textBox_Script.Padding = new System.Windows.Forms.Padding(1);
            this.textBox_Script.ShowHtmlSource = false;
            this.textBox_Script.Size = new System.Drawing.Size(649, 86);
            this.textBox_Script.TabIndex = 8;
            this.textBox_Script.UseWaitCursor = true;
            // 
            // txtItemIndex
            // 
            this.txtItemIndex.Location = new System.Drawing.Point(426, 106);
            this.txtItemIndex.Name = "txtItemIndex";
            this.txtItemIndex.Size = new System.Drawing.Size(231, 21);
            this.txtItemIndex.TabIndex = 5;
            this.txtItemIndex.Leave += new System.EventHandler(this.txtItemIndex_Leave);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(339, 111);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(85, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "아이템 인덱스:";
            // 
            // textBox_Image
            // 
            this.textBox_Image.Location = new System.Drawing.Point(96, 106);
            this.textBox_Image.Name = "textBox_Image";
            this.textBox_Image.Size = new System.Drawing.Size(223, 21);
            this.textBox_Image.TabIndex = 2;
            this.textBox_Image.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_Image_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 111);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "이미지 파일명:";
            // 
            // button_SelectAdd
            // 
            this.button_SelectAdd.Location = new System.Drawing.Point(7, 45);
            this.button_SelectAdd.Name = "button_SelectAdd";
            this.button_SelectAdd.Size = new System.Drawing.Size(90, 23);
            this.button_SelectAdd.TabIndex = 3;
            this.button_SelectAdd.Text = "선택지 추가";
            this.button_SelectAdd.UseVisualStyleBackColor = true;
            this.button_SelectAdd.Click += new System.EventHandler(this.button_SelectAdd_Click);
            // 
            // button_SelectDel
            // 
            this.button_SelectDel.Location = new System.Drawing.Point(7, 69);
            this.button_SelectDel.Name = "button_SelectDel";
            this.button_SelectDel.Size = new System.Drawing.Size(90, 23);
            this.button_SelectDel.TabIndex = 4;
            this.button_SelectDel.Text = "선택지 삭제";
            this.button_SelectDel.UseVisualStyleBackColor = true;
            this.button_SelectDel.Click += new System.EventHandler(this.button_SelectDel_Click);
            // 
            // groupBox_Step
            // 
            this.groupBox_Step.AutoSize = true;
            this.groupBox_Step.Controls.Add(this.comboBox_Step);
            this.groupBox_Step.Controls.Add(this.button_DelStep);
            this.groupBox_Step.Controls.Add(this.groupBox_Script);
            this.groupBox_Step.Controls.Add(this.button_SelectDel);
            this.groupBox_Step.Controls.Add(this.textBox_StepNo);
            this.groupBox_Step.Controls.Add(this.button_SelectAdd);
            this.groupBox_Step.Controls.Add(this.comboBox_Function);
            this.groupBox_Step.Controls.Add(this.button_DelFunction);
            this.groupBox_Step.Location = new System.Drawing.Point(0, 0);
            this.groupBox_Step.Name = "groupBox_Step";
            this.groupBox_Step.Size = new System.Drawing.Size(778, 190);
            this.groupBox_Step.TabIndex = 5;
            this.groupBox_Step.TabStop = false;
            this.groupBox_Step.Text = "서브단계";
            // 
            // comboBox_Step
            // 
            this.comboBox_Step.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Step.FormattingEnabled = true;
            this.comboBox_Step.Items.AddRange(new object[] {
            "아래단계",
            "-------------",
            "추가",
            "복사"});
            this.comboBox_Step.Location = new System.Drawing.Point(8, 94);
            this.comboBox_Step.Name = "comboBox_Step";
            this.comboBox_Step.Size = new System.Drawing.Size(88, 20);
            this.comboBox_Step.TabIndex = 14;
            this.comboBox_Step.SelectedIndexChanged += new System.EventHandler(this.comboBox_Step_SelectedIndexChanged);
            // 
            // button_DelStep
            // 
            this.button_DelStep.Location = new System.Drawing.Point(7, 117);
            this.button_DelStep.Name = "button_DelStep";
            this.button_DelStep.Size = new System.Drawing.Size(90, 23);
            this.button_DelStep.TabIndex = 6;
            this.button_DelStep.Text = "현재단계 삭제";
            this.button_DelStep.UseVisualStyleBackColor = true;
            this.button_DelStep.Click += new System.EventHandler(this.button_DelStep_Click);
            // 
            // comboBox_Function
            // 
            this.comboBox_Function.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Function.FormattingEnabled = true;
            this.comboBox_Function.Items.AddRange(new object[] {
            "버튼 추가",
            "-------------",
            "계속",
            "수락",
            "완료",
            "대화종료",
            "돌아가기"});
            this.comboBox_Function.Location = new System.Drawing.Point(8, 143);
            this.comboBox_Function.Name = "comboBox_Function";
            this.comboBox_Function.Size = new System.Drawing.Size(88, 20);
            this.comboBox_Function.TabIndex = 7;
            this.comboBox_Function.SelectedIndexChanged += new System.EventHandler(this.comboBox_Function_SelectedIndexChanged);
            // 
            // button_DelFunction
            // 
            this.button_DelFunction.Location = new System.Drawing.Point(7, 142);
            this.button_DelFunction.Name = "button_DelFunction";
            this.button_DelFunction.Size = new System.Drawing.Size(90, 23);
            this.button_DelFunction.TabIndex = 13;
            this.button_DelFunction.Text = "버튼 삭제";
            this.button_DelFunction.UseVisualStyleBackColor = true;
            this.button_DelFunction.Visible = false;
            this.button_DelFunction.Click += new System.EventHandler(this.button_DelFunction_Click);
            // 
            // Quest_Step
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.Controls.Add(this.groupBox_Step);
            this.Name = "Quest_Step";
            this.Size = new System.Drawing.Size(783, 194);
            this.groupBox_Script.ResumeLayout(false);
            this.groupBox_Script.PerformLayout();
            this.groupBox_Step.ResumeLayout(false);
            this.groupBox_Step.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox textBox_StepNo;
        private System.Windows.Forms.GroupBox groupBox_Script;
        private System.Windows.Forms.Button button_SelectAdd;
        private System.Windows.Forms.Button button_SelectDel;
        private System.Windows.Forms.GroupBox groupBox_Step;
        public System.Windows.Forms.TextBox textBox_Image;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_DelStep;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox txtItemIndex;
        public GvS.Controls.HtmlTextbox textBox_Script;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel4SelectControl;
        private System.Windows.Forms.ComboBox comboBox_Function;
        private System.Windows.Forms.Button button_DelFunction;
        public System.Windows.Forms.TextBox textBox_PortraitRight;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_PortraitLeft;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox comboBox_Step;
    }
}
