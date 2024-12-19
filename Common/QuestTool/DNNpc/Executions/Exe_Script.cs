using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Script : UserControl
    {
        public Exe_Script()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_script)
        {
            textBox_Script.Text = p_script;
        }
    }
}
