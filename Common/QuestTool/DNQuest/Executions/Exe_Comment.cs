using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_Comment : UserControl
    {
        public Exe_Comment()
        {
            InitializeComponent();
        }

        public void Set_Code(string p_code)
        {
            textBox_Comment.Text = p_code;
        }

        private void textBox_Comment_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    textBox_Comment.Text = textBox_Comment.Text.Substring(0, textBox_Comment.Text.Length - 1);
                    textBox_Comment.SelectionStart = textBox_Comment.Text.Length;
                }
            }
        }
    }
}
