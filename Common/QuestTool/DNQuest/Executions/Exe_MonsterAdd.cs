using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_MonsterAdd : Base_UserControl
    {
        public Exe_MonsterAdd() : base()
        {
            InitializeComponent();
            textBox_MonID.Validating += new CancelEventHandler(Validate);
            textBox_Count.Validating += new CancelEventHandler(Validate);
            textBox_Party.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_monID, string p_party, string p_count, string p_onexe, string p_execomp)
        {
            textBox_Count.Text = p_count;
            textBox_ExeComp.Text = p_execomp;
            textBox_MonID.Text = p_monID;
            textBox_OnExe.Text = p_onexe;
            textBox_Party.Text = p_party;
        }
    }
}