using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_Journal : UserControl
    {
        // 생성자
        public Quest_Journal()
        {
            InitializeComponent();            
        }

        // 폼 로드 이벤트
        private void Quest_Journal_Load(object sender, EventArgs e)
        {
            label_JournalNo.Text = (Convert.ToInt16(this.Tag) + 1).ToString();
        }

        #region 저널 데이터 리턴 함수

        // 저널 제목
        public string Get_Title()
        {
            return textBox_JournalTitle.Text;
        }

        // 저널 내용
        public string Get_Contents()
        {
            string temp = "<![CDATA[<html>" + this.textBox_JournalContents.Text.Replace("<P>", "").Replace("</P>", "<BR>").Replace("&nbsp;", " ").Replace("<html>", "").Replace("</html>", "") + "</html>]]>";
            return temp.Replace("<BR></html>", "</html>");
        }

        // 저널 이미지
        public string Get_ContentsImage()
        {
            return textBox_JournalContentsImage.Text;
        }

        // 목적지
        public string Get_Destination()
        {
            return textBox_JournalObjectiveName.Text;
        }

        // 목적지 좌표
        public string Get_Destination_Pos()
        {
            return textBox_JournalObjectivePos.Text;
        }

        // 제목 이미지
        public string Get_Title_Image()
        {
            return textBox_JournalTitleImage.Text;
        }

        // NPC 상태 표시
        public string Get_Statemark_NPC()
        {
            return textBox_StartMarkNpc.Text;
        }

        // 상태 표시 타입
        public int Get_Statemark_type()
        {
            return comboBox_StatemarkType.SelectedIndex;
            //if (comboBox_StatemarkType.SelectedIndex == 0)
            //{
            //    return "2"; //진행중
            //}
            //else
            //{
            //    return "3"; //완료.
            //}
        }

        public string Get_Journal_Message()
        {
            return textBox_Message.Text;
        }

        public string Get_MapToolIndex()
        {
            return textBox_MapToolIndex.Text;
        }

        public string Get_TriggerString()
        {
            return textBox_TriggerString.Text;
        }
        #endregion

        public string getJournalObjectiveHelper()
        {
            return textBox_JournalObjectiveHelper.Text;
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

        private void textBox_JournalTitle_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }       
        
    }
}
