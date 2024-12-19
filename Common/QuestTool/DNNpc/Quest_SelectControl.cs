using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc
{
    public partial class Quest_SelectControl : UserControl
    {       

        public Quest_SelectControl()
        {
            InitializeComponent();
        }

        public Quest_SelectControl(string p_no, string p_select, string p_target)
        {
            InitializeComponent();

            textBox_ExeNo.Text = p_no;
            textBox_Select.Text = p_select;
            cmbTargetIndex.Text = p_target;
        }

        public void Set_Value(string p_no, string p_select, string p_target)
        {
            textBox_ExeNo.Text = p_no;
            textBox_Select.Text = p_select;
            cmbTargetIndex.Text = p_target;
        }

        public void Set_ExeNo(string p_no)
        {
            textBox_ExeNo.Text = p_no;
        }

        public string Get_Select()
        {            
            string temp = "<![CDATA[<html>" + this.textBox_Select.Text.Replace("<P>", "").Replace("</P>", "<BR>").Replace("&nbsp;", " ").Replace("<html>", "").Replace("</html>", "") + "</html>]]>";
            return temp.Replace("<BR></html>", "</html>");
        }

        public string Get_Target()
        {
            return cmbTargetIndex.Text;
        }

        public string Get_ExeNo()
        {
            return textBox_ExeNo.Text;
        }

        //꺽쇠 체크
        private void Check_GT(TextBox p_txt, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    p_txt.Text = p_txt.Text.Substring(0, p_txt.Text.Length - 1);
                    p_txt.SelectionStart = p_txt.Text.Length;
                }
            }
        }

        private void textBox_TargetIndex_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }
    }
}
