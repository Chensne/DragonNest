using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace QuestTool
{
    public partial class AboutForm : Form
    {
        public AboutForm(Form opener)
        {
            InitializeComponent();

            label_Ver.Text = "Ver.  " + opener.ProductVersion;
        }

        private void button_Close_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
