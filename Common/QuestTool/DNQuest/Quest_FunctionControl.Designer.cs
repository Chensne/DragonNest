namespace DNQuest
{
    partial class Quest_FunctionControl
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
            this.groupBox_Function = new System.Windows.Forms.GroupBox();
            this.cmbTargetIndex = new System.Windows.Forms.ComboBox();
            this.textBox_ExeNo = new System.Windows.Forms.TextBox();
            this.label_ExeNo = new System.Windows.Forms.Label();
            this.label_Target = new System.Windows.Forms.Label();
            this.groupBox_Function.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox_Function
            // 
            this.groupBox_Function.Controls.Add(this.cmbTargetIndex);
            this.groupBox_Function.Controls.Add(this.textBox_ExeNo);
            this.groupBox_Function.Controls.Add(this.label_ExeNo);
            this.groupBox_Function.Controls.Add(this.label_Target);
            this.groupBox_Function.Location = new System.Drawing.Point(3, 3);
            this.groupBox_Function.Name = "groupBox_Function";
            this.groupBox_Function.Size = new System.Drawing.Size(663, 50);
            this.groupBox_Function.TabIndex = 0;
            this.groupBox_Function.TabStop = false;
            this.groupBox_Function.Text = "Function Name";
            // 
            // cmbTargetIndex
            // 
            this.cmbTargetIndex.FormattingEnabled = true;
            this.cmbTargetIndex.Items.AddRange(new object[] {
            "acceptted",
            "!next",
            "class_check"});
            this.cmbTargetIndex.Location = new System.Drawing.Point(96, 19);
            this.cmbTargetIndex.Name = "cmbTargetIndex";
            this.cmbTargetIndex.Size = new System.Drawing.Size(223, 20);
            this.cmbTargetIndex.TabIndex = 12;
            // 
            // textBox_ExeNo
            // 
            this.textBox_ExeNo.Location = new System.Drawing.Point(426, 19);
            this.textBox_ExeNo.Name = "textBox_ExeNo";
            this.textBox_ExeNo.Size = new System.Drawing.Size(231, 21);
            this.textBox_ExeNo.TabIndex = 11;
            // 
            // label_ExeNo
            // 
            this.label_ExeNo.AutoSize = true;
            this.label_ExeNo.Location = new System.Drawing.Point(359, 22);
            this.label_ExeNo.Name = "label_ExeNo";
            this.label_ExeNo.Size = new System.Drawing.Size(65, 12);
            this.label_ExeNo.TabIndex = 10;
            this.label_ExeNo.Text = "실행 번호 :";
            // 
            // label_Target
            // 
            this.label_Target.AutoSize = true;
            this.label_Target.Location = new System.Drawing.Point(18, 22);
            this.label_Target.Name = "label_Target";
            this.label_Target.Size = new System.Drawing.Size(77, 12);
            this.label_Target.TabIndex = 9;
            this.label_Target.Text = "타겟 인덱스 :";
            // 
            // Quest_FunctionControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox_Function);
            this.Name = "Quest_FunctionControl";
            this.Size = new System.Drawing.Size(673, 60);
            this.groupBox_Function.ResumeLayout(false);
            this.groupBox_Function.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_Function;
        private System.Windows.Forms.ComboBox cmbTargetIndex;
        private System.Windows.Forms.TextBox textBox_ExeNo;
        private System.Windows.Forms.Label label_ExeNo;
        private System.Windows.Forms.Label label_Target;


    }
}
