using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_AddDailyQuest : Base_UserControl
    {
        public Exe_AddDailyQuest() : base()
        {
            InitializeComponent();

            this.txt_QuestNo.Validating += new System.ComponentModel.CancelEventHandler(Validate);
            this.txt_ExecuteOnFail.Validating += new System.ComponentModel.CancelEventHandler(Validate);
            this.txt_ExecuteOnSuccess.Validating += new System.ComponentModel.CancelEventHandler(Validate);
        }

        internal void Set_Value(string p1, string p2, string p3)
        {
            this.txt_QuestNo.Text = p1;
            this.txt_ExecuteOnSuccess.Text = p2;
            this.txt_ExecuteOnFail.Text = p3;
        }

        public void Validate(object sender, CancelEventArgs e)
        {
            Control ctrl = (Control)sender;

            if (ctrl.Text.Trim().Length < 1)
            {
                errPvdr.SetError(ctrl, "입력이 필요합니다.");
                return;
            }
        }
    }
}
