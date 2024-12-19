using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_AllHunting : Base_UserControl
    {
        public Exe_AllHunting() : base()
        {
            InitializeComponent();

            textBox_Exe.Validating += new System.ComponentModel.CancelEventHandler(Validate);
        }

        public void Set_Value(string p_id)
        {
            textBox_Exe.Text = p_id;
        }
    }
}
