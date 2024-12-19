using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_ProbChoice : Base_UserControl
    {
        public Exe_ProbChoice() : base()
        {
            InitializeComponent();

            this.textBox_Rand.Validating += new CancelEventHandler(Validate);
            this.textBox_Seed.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_rand, string p_seed)
        {
            textBox_Rand.Text = p_rand;
            textBox_Seed.Text = p_seed;
        }
    }
}
