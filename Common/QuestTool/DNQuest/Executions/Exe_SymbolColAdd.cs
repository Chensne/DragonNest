using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_SymbolColAdd : UserControl
    {
        public Exe_SymbolColAdd()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_id, string p_count, string p_onexe, string p_exe)
        {
            textBox_Count.Text = p_count;
            textBox_Exe.Text = p_exe;
            textBox_OnExe.Text = p_onexe;
            textBox_Symbol.Text = p_id;
        }
    }
}
