using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_QuestStepMod : Base_UserControl
    {
        public Exe_QuestStepMod() : base()
        {
            InitializeComponent();

            this.textBox_Step.Validating +=new CancelEventHandler(Validate);
        }

        public void Set_Step(string p_step)
        {
            textBox_Step.Text = p_step;
        }
    }
}
