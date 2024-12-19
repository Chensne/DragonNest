using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Conditions
{
    public partial class Condition_Quest : UserControl
    {
        public Condition_Quest()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_value)
        {
            textBox_Value.Text = p_value;
        }
    }
}
