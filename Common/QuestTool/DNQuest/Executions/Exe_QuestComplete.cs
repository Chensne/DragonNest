using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_QuestComplete : Base_UserControl
    {
        public Exe_QuestComplete() : base()
        {
            InitializeComponent();
            textBox_Quest.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_quest)
        {
            textBox_Quest.Text = p_quest;
        }

        public void Set_CompleteValue(string p_complete)
        {
            textBox_QuestComplete.Text = p_complete;
        }
    }
}
