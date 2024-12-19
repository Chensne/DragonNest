using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_Recompense_View : UserControl
    {
        public Exe_Recompense_View()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_index)
        {
            textBox_Index.Text = p_index;            
        }

        private void Exe_Recompense_View_Leave(object sender, EventArgs e)
        {
            if (textBox_Index.Text.Trim().Length < 1)
            {
                if (txtRewardArcher.Text.Length < 1 || txtRewardCleric.Text.Length < 1 || this.txtRewardSorcerer.Text.Length < 1 || this.txtRewardWarrior.Text.Length < 1 || this.txtRewardKally.Text.Length < 1 || this.txtRewardArcademic.Text.Length < 1 || this.txtRewardAssassin.Text.Length < 1)
                {
                    MessageBox.Show("기본 값이 없을 경우, 각 클래스 별 모든 값이 존재해야합니다.");
                    this.txtRewardWarrior.Focus();
                    return;
                }
            }
        }
    }
}
