using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Quest : UserControl
    {
        public Exe_Quest()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_quest)
        {
            textBox_Quest.Text = p_quest;
        }
    }
}
