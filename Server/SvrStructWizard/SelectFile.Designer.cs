namespace SvrStructWizard
{
    partial class SelectFile
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

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.button_ServerStruct = new System.Windows.Forms.Button();
            this.button_ChannelInfo = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // button_ServerStruct
            // 
            this.button_ServerStruct.DialogResult = System.Windows.Forms.DialogResult.Yes;
            this.button_ServerStruct.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_ServerStruct.Location = new System.Drawing.Point(37, 21);
            this.button_ServerStruct.Name = "button_ServerStruct";
            this.button_ServerStruct.Size = new System.Drawing.Size(95, 38);
            this.button_ServerStruct.TabIndex = 0;
            this.button_ServerStruct.Text = "Server Struct";
            this.button_ServerStruct.UseVisualStyleBackColor = true;
            this.button_ServerStruct.Click += new System.EventHandler(this.button_ServerStruct_Click);
            // 
            // button_ChannelInfo
            // 
            this.button_ChannelInfo.DialogResult = System.Windows.Forms.DialogResult.No;
            this.button_ChannelInfo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_ChannelInfo.Location = new System.Drawing.Point(162, 21);
            this.button_ChannelInfo.Name = "button_ChannelInfo";
            this.button_ChannelInfo.Size = new System.Drawing.Size(87, 39);
            this.button_ChannelInfo.TabIndex = 1;
            this.button_ChannelInfo.Text = "ChanneI Info";
            this.button_ChannelInfo.UseVisualStyleBackColor = true;
            this.button_ChannelInfo.Click += new System.EventHandler(this.button_ChannelInfo_Click);
            // 
            // SelectFile
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(287, 81);
            this.Controls.Add(this.button_ChannelInfo);
            this.Controls.Add(this.button_ServerStruct);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SelectFile";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Select File";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button_ServerStruct;
        private System.Windows.Forms.Button button_ChannelInfo;
    }
}