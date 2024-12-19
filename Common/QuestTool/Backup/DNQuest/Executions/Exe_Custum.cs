using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_Custum : Base_UserControl
    {
        public Exe_Custum() : base()
        {
            InitializeComponent();
            textBox_Code.Validating +=new CancelEventHandler(Validate);
        }

        public void Set_Code(string p_code)
        {
            textBox_Code.Text = p_code;
        }

        private void textBox_Code_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    textBox_Code.Text = textBox_Code.Text.Substring(0, textBox_Code.Text.Length - 1);
                    textBox_Code.SelectionStart = textBox_Code.Text.Length;
                }
            }
        }

        public void validate(object sender, CancelEventArgs e)
        {
            base.Validate();
            if (textBox_Code.Text.IndexOf('<') > -1 || textBox_Code.Text.IndexOf('>') > -1 ) {
                errPvdr.SetError(textBox_Code, "< 혹은 > 는 사용할 수 없습니다!");
                return;
            }
        }
    }
}
