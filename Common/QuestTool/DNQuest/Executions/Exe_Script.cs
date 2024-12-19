using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_Script : Base_UserControl
    {
        public Exe_Script() : base()
        {
            InitializeComponent();
            this.textBox_Index.Validating += new CancelEventHandler(Validate);
            this.textBox_NPC.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_npc, string p_index)
        {
            textBox_Index.Text = p_index;
            textBox_NPC.Text = p_npc;
        }
    }
}