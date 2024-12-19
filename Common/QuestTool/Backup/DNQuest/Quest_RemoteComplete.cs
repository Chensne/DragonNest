using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_RemoteComplete : UserControl
    {
        public Quest_Unit qu;
        public Quest_RemoteComplete(Quest_Unit p_qu)
        {
            InitializeComponent();
            qu = p_qu;
        }

        public string GetExecuteNo()
        {
            return textBox_ExecuteNo.Text;
        }

        public void SetExecuteNo(string ExecuteNo)
        {
            textBox_ExecuteNo.Text = ExecuteNo;
        }
    }
}
