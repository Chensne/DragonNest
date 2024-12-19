using System.Windows.Forms;
namespace DNNpc
{
    partial class DNNpc
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
            if (NpcAllSaveCheck == true)
            {
                DialogResult result = MessageBox.Show("저장하시겠습니까?", "저장", MessageBoxButtons.YesNoCancel);
                if (result == System.Windows.Forms.DialogResult.Yes)
                {
                    allSave();
                }
                else if (result == System.Windows.Forms.DialogResult.Cancel)
                {
                    return;
                }
            }

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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.초기화ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.불러오기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.저장하기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.닫기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl_Main = new System.Windows.Forms.TabControl();
            this.tabPage_Basic = new System.Windows.Forms.TabPage();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.groupBox_Basic = new System.Windows.Forms.GroupBox();
            this.textBox_click = new System.Windows.Forms.TextBox();
            this.textBox_nid = new System.Windows.Forms.TextBox();
            this.textBox_nename = new System.Windows.Forms.TextBox();
            this.textBox_nkname = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage_Paragraph = new System.Windows.Forms.TabPage();
            this.panel_ScriptMain = new System.Windows.Forms.Panel();
            this.panel_TopScript = new System.Windows.Forms.Panel();
            this.button_ScriptDel = new System.Windows.Forms.Button();
            this.button_ScriptAdd = new System.Windows.Forms.Button();
            this.button_ScriptReset = new System.Windows.Forms.Button();
            this.menuStrip1.SuspendLayout();
            this.tabControl_Main.SuspendLayout();
            this.tabPage_Basic.SuspendLayout();
            this.groupBox_Basic.SuspendLayout();
            this.tabPage_Paragraph.SuspendLayout();
            this.panel_TopScript.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.초기화ToolStripMenuItem,
            this.불러오기ToolStripMenuItem,
            this.저장하기ToolStripMenuItem,
            this.닫기ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1011, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // 초기화ToolStripMenuItem
            // 
            this.초기화ToolStripMenuItem.Name = "초기화ToolStripMenuItem";
            this.초기화ToolStripMenuItem.Size = new System.Drawing.Size(53, 20);
            this.초기화ToolStripMenuItem.Text = "초기화";
            this.초기화ToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // 불러오기ToolStripMenuItem
            // 
            this.불러오기ToolStripMenuItem.Name = "불러오기ToolStripMenuItem";
            this.불러오기ToolStripMenuItem.Size = new System.Drawing.Size(65, 20);
            this.불러오기ToolStripMenuItem.Text = "불러오기";
            this.불러오기ToolStripMenuItem.Click += new System.EventHandler(this.loadToolStripMenuItem_Click);
            // 
            // 저장하기ToolStripMenuItem
            // 
            this.저장하기ToolStripMenuItem.Name = "저장하기ToolStripMenuItem";
            this.저장하기ToolStripMenuItem.Size = new System.Drawing.Size(65, 20);
            this.저장하기ToolStripMenuItem.Text = "저장하기";
            this.저장하기ToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // 닫기ToolStripMenuItem
            // 
            this.닫기ToolStripMenuItem.Name = "닫기ToolStripMenuItem";
            this.닫기ToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
            this.닫기ToolStripMenuItem.Text = "닫기";
            this.닫기ToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
            // 
            // tabControl_Main
            // 
            this.tabControl_Main.Controls.Add(this.tabPage_Basic);
            this.tabControl_Main.Controls.Add(this.tabPage_Paragraph);
            this.tabControl_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl_Main.Location = new System.Drawing.Point(0, 24);
            this.tabControl_Main.Name = "tabControl_Main";
            this.tabControl_Main.SelectedIndex = 0;
            this.tabControl_Main.Size = new System.Drawing.Size(1011, 562);
            this.tabControl_Main.TabIndex = 1;
            // 
            // tabPage_Basic
            // 
            this.tabPage_Basic.AutoScroll = true;
            this.tabPage_Basic.Controls.Add(this.textBox1);
            this.tabPage_Basic.Controls.Add(this.groupBox_Basic);
            this.tabPage_Basic.Location = new System.Drawing.Point(4, 21);
            this.tabPage_Basic.Name = "tabPage_Basic";
            this.tabPage_Basic.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Basic.Size = new System.Drawing.Size(1003, 537);
            this.tabPage_Basic.TabIndex = 0;
            this.tabPage_Basic.Text = "NPC 기본정보";
            this.tabPage_Basic.UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(647, 6);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(348, 157);
            this.textBox1.TabIndex = 1;
            this.textBox1.Visible = false;
            // 
            // groupBox_Basic
            // 
            this.groupBox_Basic.Controls.Add(this.textBox_click);
            this.groupBox_Basic.Controls.Add(this.textBox_nid);
            this.groupBox_Basic.Controls.Add(this.textBox_nename);
            this.groupBox_Basic.Controls.Add(this.textBox_nkname);
            this.groupBox_Basic.Controls.Add(this.label4);
            this.groupBox_Basic.Controls.Add(this.label3);
            this.groupBox_Basic.Controls.Add(this.label2);
            this.groupBox_Basic.Controls.Add(this.label1);
            this.groupBox_Basic.Location = new System.Drawing.Point(8, 6);
            this.groupBox_Basic.Name = "groupBox_Basic";
            this.groupBox_Basic.Size = new System.Drawing.Size(633, 157);
            this.groupBox_Basic.TabIndex = 0;
            this.groupBox_Basic.TabStop = false;
            this.groupBox_Basic.Text = "NPC 기본정보";
            // 
            // textBox_click
            // 
            this.textBox_click.Location = new System.Drawing.Point(112, 114);
            this.textBox_click.Name = "textBox_click";
            this.textBox_click.Size = new System.Drawing.Size(234, 21);
            this.textBox_click.TabIndex = 7;
            this.textBox_click.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_nkname_KeyDown);
            // 
            // textBox_nid
            // 
            this.textBox_nid.Location = new System.Drawing.Point(112, 79);
            this.textBox_nid.Name = "textBox_nid";
            this.textBox_nid.Size = new System.Drawing.Size(234, 21);
            this.textBox_nid.TabIndex = 6;
            this.textBox_nid.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_nkname_KeyDown);
            // 
            // textBox_nename
            // 
            this.textBox_nename.Location = new System.Drawing.Point(112, 52);
            this.textBox_nename.Name = "textBox_nename";
            this.textBox_nename.Size = new System.Drawing.Size(234, 21);
            this.textBox_nename.TabIndex = 5;
            this.textBox_nename.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_nkname_KeyDown);
            // 
            // textBox_nkname
            // 
            this.textBox_nkname.Location = new System.Drawing.Point(112, 24);
            this.textBox_nkname.Name = "textBox_nkname";
            this.textBox_nkname.Size = new System.Drawing.Size(234, 21);
            this.textBox_nkname.TabIndex = 4;
            this.textBox_nkname.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_nkname_KeyDown);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(17, 118);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(81, 12);
            this.label4.TabIndex = 3;
            this.label4.Text = "클릭시 실행문";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 88);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "NPC 아이디";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "NPC 영문명";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "NPC 한글명";
            // 
            // tabPage_Paragraph
            // 
            this.tabPage_Paragraph.Controls.Add(this.panel_ScriptMain);
            this.tabPage_Paragraph.Controls.Add(this.panel_TopScript);
            this.tabPage_Paragraph.Location = new System.Drawing.Point(4, 21);
            this.tabPage_Paragraph.Name = "tabPage_Paragraph";
            this.tabPage_Paragraph.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Paragraph.Size = new System.Drawing.Size(1003, 537);
            this.tabPage_Paragraph.TabIndex = 1;
            this.tabPage_Paragraph.Text = "대사 파라그래프";
            this.tabPage_Paragraph.UseVisualStyleBackColor = true;
            // 
            // panel_ScriptMain
            // 
            this.panel_ScriptMain.AutoScroll = true;
            this.panel_ScriptMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_ScriptMain.Location = new System.Drawing.Point(3, 37);
            this.panel_ScriptMain.Name = "panel_ScriptMain";
            this.panel_ScriptMain.Size = new System.Drawing.Size(997, 497);
            this.panel_ScriptMain.TabIndex = 4;
            // 
            // panel_TopScript
            // 
            this.panel_TopScript.BackColor = System.Drawing.Color.BlanchedAlmond;
            this.panel_TopScript.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_TopScript.Controls.Add(this.button_ScriptDel);
            this.panel_TopScript.Controls.Add(this.button_ScriptAdd);
            this.panel_TopScript.Controls.Add(this.button_ScriptReset);
            this.panel_TopScript.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_TopScript.Location = new System.Drawing.Point(3, 3);
            this.panel_TopScript.Name = "panel_TopScript";
            this.panel_TopScript.Size = new System.Drawing.Size(997, 34);
            this.panel_TopScript.TabIndex = 3;
            // 
            // button_ScriptDel
            // 
            this.button_ScriptDel.Location = new System.Drawing.Point(795, 4);
            this.button_ScriptDel.Name = "button_ScriptDel";
            this.button_ScriptDel.Size = new System.Drawing.Size(75, 23);
            this.button_ScriptDel.TabIndex = 2;
            this.button_ScriptDel.Text = "단계 삭제";
            this.button_ScriptDel.UseVisualStyleBackColor = true;
            this.button_ScriptDel.Click += new System.EventHandler(this.button_ScriptDel_Click);
            // 
            // button_ScriptAdd
            // 
            this.button_ScriptAdd.Location = new System.Drawing.Point(714, 4);
            this.button_ScriptAdd.Name = "button_ScriptAdd";
            this.button_ScriptAdd.Size = new System.Drawing.Size(75, 23);
            this.button_ScriptAdd.TabIndex = 1;
            this.button_ScriptAdd.Text = "단계 추가";
            this.button_ScriptAdd.UseVisualStyleBackColor = true;
            this.button_ScriptAdd.Click += new System.EventHandler(this.button_ScriptAdd_Click);
            // 
            // button_ScriptReset
            // 
            this.button_ScriptReset.Location = new System.Drawing.Point(876, 4);
            this.button_ScriptReset.Name = "button_ScriptReset";
            this.button_ScriptReset.Size = new System.Drawing.Size(75, 23);
            this.button_ScriptReset.TabIndex = 0;
            this.button_ScriptReset.Text = "단계 리셋";
            this.button_ScriptReset.UseVisualStyleBackColor = true;
            this.button_ScriptReset.Click += new System.EventHandler(this.button_ScriptReset_Click);
            // 
            // DNNpc
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1011, 586);
            this.Controls.Add(this.tabControl_Main);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "DNNpc";
            this.Text = "드래곤 네스트 NPC 에디터";
            this.Load += new System.EventHandler(this.DNNpc_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabControl_Main.ResumeLayout(false);
            this.tabPage_Basic.ResumeLayout(false);
            this.tabPage_Basic.PerformLayout();
            this.groupBox_Basic.ResumeLayout(false);
            this.groupBox_Basic.PerformLayout();
            this.tabPage_Paragraph.ResumeLayout(false);
            this.panel_TopScript.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.TabControl tabControl_Main;
        private System.Windows.Forms.TabPage tabPage_Basic;
        private System.Windows.Forms.TabPage tabPage_Paragraph;
        private System.Windows.Forms.GroupBox groupBox_Basic;
        private System.Windows.Forms.TextBox textBox_click;
        private System.Windows.Forms.TextBox textBox_nid;
        private System.Windows.Forms.TextBox textBox_nename;
        private System.Windows.Forms.TextBox textBox_nkname;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel_TopScript;
        private System.Windows.Forms.Button button_ScriptDel;
        private System.Windows.Forms.Button button_ScriptAdd;
        private System.Windows.Forms.Button button_ScriptReset;
        private System.Windows.Forms.Panel panel_ScriptMain;
        private System.Windows.Forms.ToolStripMenuItem 초기화ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 불러오기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 저장하기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 닫기ToolStripMenuItem;
        private System.Windows.Forms.TextBox textBox1;
    }
}

