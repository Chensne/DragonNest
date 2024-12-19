namespace DNNpc.Executions
{
    partial class Exe_CutsceneCompleteChangeMap
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
            this.txtGateNo = new System.Windows.Forms.TextBox();
            this.txtStageNo = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label_No = new System.Windows.Forms.Label();
            this.txtCutSceneNo = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // txtGateNo
            // 
            this.txtGateNo.Location = new System.Drawing.Point(367, 6);
            this.txtGateNo.Name = "txtGateNo";
            this.txtGateNo.Size = new System.Drawing.Size(77, 21);
            this.txtGateNo.TabIndex = 73;
            // 
            // txtStageNo
            // 
            this.txtStageNo.Location = new System.Drawing.Point(214, 6);
            this.txtStageNo.Name = "txtStageNo";
            this.txtStageNo.Size = new System.Drawing.Size(77, 21);
            this.txtStageNo.TabIndex = 72;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(446, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 12);
            this.label3.TabIndex = 71;
            this.label3.Text = "게이트로 이동.";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(292, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 12);
            this.label2.TabIndex = 70;
            this.label2.Text = "스테이지의 ";
            // 
            // label_No
            // 
            this.label_No.AutoSize = true;
            this.label_No.Location = new System.Drawing.Point(3, 12);
            this.label_No.Name = "label_No";
            this.label_No.Size = new System.Drawing.Size(11, 12);
            this.label_No.TabIndex = 69;
            this.label_No.Text = "0";
            // 
            // txtCutSceneNo
            // 
            this.txtCutSceneNo.Location = new System.Drawing.Point(20, 6);
            this.txtCutSceneNo.Name = "txtCutSceneNo";
            this.txtCutSceneNo.Size = new System.Drawing.Size(77, 21);
            this.txtCutSceneNo.TabIndex = 68;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(103, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(105, 12);
            this.label1.TabIndex = 67;
            this.label1.Text = "컷씬을 플레이 후, ";
            // 
            // Exe_CutsceneCompleteChangMap
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.txtGateNo);
            this.Controls.Add(this.txtStageNo);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label_No);
            this.Controls.Add(this.txtCutSceneNo);
            this.Controls.Add(this.label1);
            this.Name = "Exe_CutsceneCompleteChangMap";
            this.Size = new System.Drawing.Size(550, 31);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.TextBox txtGateNo;
        public System.Windows.Forms.TextBox txtStageNo;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.Label label_No;
        public System.Windows.Forms.TextBox txtCutSceneNo;
        private System.Windows.Forms.Label label1;

    }
}
