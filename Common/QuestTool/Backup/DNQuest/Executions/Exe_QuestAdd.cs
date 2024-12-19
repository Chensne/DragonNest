using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_QuestAdd : Base_UserControl
    {
        public Exe_QuestAdd() : base()
        {
            InitializeComponent();
            textBox_Item.Validating +=new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_quest)
        {
            textBox_Item.Text = p_quest;
        }
    }
}
