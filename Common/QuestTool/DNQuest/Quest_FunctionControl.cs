using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_FunctionControl : UserControl
    {
        private int m_nButtonType = 0;

        public Quest_FunctionControl(int nButtonType, string strFunctionName)
        {
            InitializeComponent();

            m_nButtonType = nButtonType;
            groupBox_Function.Text = strFunctionName + " 버튼 사용";

            if ("돌아가기" == strFunctionName)
            {
                label_Target.Hide();
                label_ExeNo.Hide();
                cmbTargetIndex.Hide();
                textBox_ExeNo.Hide();               
            }
        }

        public int GetGroupBoxLocationX()
        {
            return groupBox_Function.Location.X;
        }
    }
}
