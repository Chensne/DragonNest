using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc
{
    public partial class Quest_Link : UserControl
    {
        public Quest_Link()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_link, string p_xml)
        {
            textBox_Select.Text = p_link;
            textBox_XML.Text = p_xml;
        }

        public string Get_Link()
        {
            string temp = "<![CDATA[<html>" + textBox_Select.Text.Replace("<P>", "").Replace("</P>", "<BR>").Replace("&nbsp;", " ").Replace("<html>", "").Replace("</html>", "") + "</html>]]>";
            return temp.Replace("<BR></html>", "</html>");
        }

        public string Get_Xml()
        {
            return textBox_XML.Text;
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

        private void textBox_XML_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }
    }
}
