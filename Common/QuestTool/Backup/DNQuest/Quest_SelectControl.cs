using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_SelectControl : UserControl
    {
        private int m_nSelectType = 0;

        public Quest_SelectControl(int select_type)
        {
            InitializeComponent();

            SetSelectType(select_type);
        }

        public Quest_SelectControl(string p_no, string p_select, string p_target, int select_type)
        {
            InitializeComponent();

            textBox_ExeNo.Text = p_no;
            textBox_Select.Text = p_select;
            cmbTargetIndex.Text = p_target;

            SetSelectType(select_type);
        }

        public void SetSelectType(int select_type)
        {
            m_nSelectType = select_type;

            if (0 == select_type)
                return;

            textBox_Select.Hide();
            label_Target.Location = new Point(18, 22);
            cmbTargetIndex.Location = new Point(96, 19);
            label_ExeNo.Location = new Point(300, 22);
            textBox_ExeNo.Location = new Point(367, 19);

            switch (select_type)
            { 
                case 2: groupBox_Select.Text = "계속 버튼 사용"; break;
                case 3: groupBox_Select.Text = "수락 버튼 사용"; break;
                case 4: groupBox_Select.Text = "완료 버튼 사용"; break;
                case 5: groupBox_Select.Text = "대화종료 버튼 사용"; break;
                case 6: groupBox_Select.Text = "돌아가기 버튼 사용"; break;
            }

            if (5 == select_type)
            {
                label_Target.Text = "대화종료 버튼을 누르면 대화가 종료 됩니다.";

                label_ExeNo.Hide();
                cmbTargetIndex.Hide();
                textBox_ExeNo.Hide();
            }

            if (6 == select_type)
            {
                label_Target.Text = "돌아가기 버튼을 누르면 현재 대화중인 NPC의 초기 대화 상태로 돌아갑니다.";

                label_ExeNo.Hide();
                cmbTargetIndex.Hide();
                textBox_ExeNo.Hide();
            }
        }

        public int GetSelectType()
        {
            return m_nSelectType;
        }

        //public void Set_Value(string p_no, string p_select, string p_target)
        //{
        //    textBox_ExeNo.Text = p_no;
        //    textBox_Select.Text = p_select;
        //    cmbTargetIndex.Text = p_target;
        //}

        //public void Set_ExeNo(string p_no)
        //{
        //    textBox_ExeNo.Text = p_no;
        //}

        public string Get_Select()
        {
            if (0 != m_nSelectType)
                return "";

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

        private void textBox_ExeNo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.Modifiers == Keys.Shift)
                {
                    if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                    {
                        MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        textBox_ExeNo.Text = textBox_ExeNo.Text.Substring(0, textBox_ExeNo.Text.Length - 1);
                        textBox_ExeNo.SelectionStart = textBox_ExeNo.Text.Length;
                    }
                }
            }
        }

        private void cmbTargetIndex_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    cmbTargetIndex.Text = cmbTargetIndex.Text.Substring(0, cmbTargetIndex.Text.Length - 1);
                    cmbTargetIndex.SelectionStart = cmbTargetIndex.Text.Length;
                }
            }
        }
    }
}
