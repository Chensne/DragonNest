using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Message : UserControl
    {
        public Exe_Message()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_index, string p_type, string p_substring)
        {
            textBox_Index.Text = p_index;
            textBox_Substring.Text = p_substring;
            textBox_Type.Text = p_type;
        }
    }
}
