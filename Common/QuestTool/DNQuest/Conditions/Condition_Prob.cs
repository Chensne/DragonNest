using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Conditions
{
    public partial class Condition_Prob : Base_UserControl
    {
        public Condition_Prob() : base()
        {
            InitializeComponent();
            this.textBox_Value.Validating +=new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_value)
        {
            textBox_Value.Text = p_value;
        }
    }
}
