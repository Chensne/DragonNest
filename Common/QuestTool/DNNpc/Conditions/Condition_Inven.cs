using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Conditions
{
    public partial class Condition_Inven : UserControl
    {
        public Condition_Inven()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_oper, string p_value)
        {
            textBox_Oper.Text = p_oper;
            textBox_Value.Text = p_value;
        }
    }
}
