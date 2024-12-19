using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace QuestTool
{
    public partial class SaveProgress : Form
    {
        private DateTime _time;
        private QuestTool _tool;
        private delegate void _del_ForceCancel();
        private void _ForceCancel()
        {
            _tool.ForceSaveCancel = true;
        }

        public SaveProgress(QuestTool tool)
        {
            _time = DateTime.Now;
            _tool = tool;

            InitializeComponent();

            timer1.Start();
        }

        public void UpdateData(int count, int total, string filename, bool success, string message)
        {
            label_Count.Text = count.ToString() + " / " + total.ToString();
            label_Filename.Text = filename;

            progressBar1.Maximum = total;
            progressBar1.Value = count;

            if (false == success)
                listBox_Fail.Items.Add(filename + message);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            label_Time.Text = (DateTime.Now.ToLocalTime() - _time.ToLocalTime()).ToString();
            label_Time.Text = label_Time.Text.Substring(0, label_Time.Text.LastIndexOf('.'));
        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            if (DialogResult.No == MessageBox.Show("작업을 중단 하시겠습니까?", "", MessageBoxButtons.YesNo, MessageBoxIcon.Question))
            {
                return;
            }

            Invoke(new _del_ForceCancel(_ForceCancel));
            this.Close();
        }

        private void listBox_Fail_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if(null == listBox_Fail.SelectedItem)
                return;

            MessageBox.Show(listBox_Fail.SelectedItem.ToString());
        }
    }
}
