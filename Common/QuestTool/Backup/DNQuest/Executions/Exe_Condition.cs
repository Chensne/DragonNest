using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_Condition : Base_UserControl
    {
        public Exe_Condition() : base()
        {
            InitializeComponent();

            textBox_Condition.Validating += new System.ComponentModel.CancelEventHandler(Validate);
        }

        public void Set_Value(string p_condition, string p_true, string p_false)
        {
            textBox_Condition.Text = p_condition;
            textBox_true.Text = p_true;
            textBox_false.Text = p_false;
        }
    }
}
