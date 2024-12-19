using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_SkillStore : UserControl
    {
        public Exe_SkillStore()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_skill)
        {
            textBox_Skill.Text = p_skill;
        }
    }
}
