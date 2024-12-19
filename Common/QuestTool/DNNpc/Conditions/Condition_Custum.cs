using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Conditions
{
    public partial class Condition_Custum : UserControl
    {
        public Condition_Custum()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_custum)
        {
            textBox_Custum.Text = p_custum;
        }
    }
}
