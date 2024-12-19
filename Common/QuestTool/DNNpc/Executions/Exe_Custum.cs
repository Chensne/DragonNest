using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Custum : UserControl
    {
        public Exe_Custum()
        {
            InitializeComponent();
        }

        public void Set_Code(string p_code)
        {
            textBox_Code.Text = p_code;
        }
    }
}
