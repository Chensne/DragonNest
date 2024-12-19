using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Store : UserControl
    {
        public Exe_Store()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_store)
        {
            textBox_Store.Text = p_store;
        }
    }
}
