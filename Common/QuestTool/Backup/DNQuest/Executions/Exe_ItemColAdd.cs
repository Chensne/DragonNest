using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_ItemColAdd : Base_UserControl
    {
        public Exe_ItemColAdd()
        {
            InitializeComponent();
            this.textBox_Item.Validating +=new CancelEventHandler(Validate);
            this.textBox_Count.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_id, string p_count, string p_onexe, string p_exe)
        {
            textBox_Count.Text = p_count;
            textBox_ExeComp.Text = p_exe;
            textBox_Item.Text = p_id;
            textBox_OnExe.Text = p_onexe;
        }
    }
}
